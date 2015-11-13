// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

part of internal;

// The MojoHandleWatcher sends a stream of events to application isolates that
// register Mojo handles with it. Application isolates make the following calls:
//
// add(handle, port, signals) - Instructs the MojoHandleWatcher isolate to add
//     'handle' to the set of handles it watches, and to notify the calling
//     isolate only for the events specified by 'signals' using the send port
//     'port'
//
// remove(handle) - Instructs the MojoHandleWatcher isolate to remove 'handle'
//     from the set of handles it watches. This allows the application isolate
//     to, e.g., pause the stream of events.
//
// close(handle) - Notifies the HandleWatcherIsolate that a handle it is
//     watching should be removed from its set and closed.
class MojoHandleWatcher {
  // Control commands.
  static const int ADD = 0;
  static const int REMOVE = 1;
  static const int CLOSE = 2;
  static const int TIMER = 3;
  static const int SHUTDOWN = 4;

  static const int kMojoHandleInvalid = 0;
  static const int kDeadlineIndefinite = -1;

  static const int kMojoResultOk = 0;
  static const int kMojoResultInvalidArgument = 3;
  static const int kMojoResultDeadlineExceeded = 4;
  static const int kMojoResultFailedPrecondition = 9;

  static const int kMojoSignalsReadable = 0x1;
  static const int kMojoSignalsWritable = 0x2;
  static const int kMojoSignalsPeerClosed = 0x4;
  static const int kMojoSignalsAll = 0x7;

  static int _encodeCommand(int cmd, [int signals = 0]) =>
      (cmd << 3) | (signals & kMojoSignalsAll);
  static int _decodeCommand(int cmd) {
    assert(kMojoSignalsAll < 1 << 3);
    return cmd >> 3;
  }

  static int _decodeSignals(int cmd) {
    return cmd & kMojoSignalsAll;
  }

  // The Mojo handle over which control messages are sent.
  int _controlHandle;

  // Whether the handle watcher should shut down.
  bool _shutdown;

  // External TypedData arrays for the call to MojoWaitMany.
  WaitManyState _waitManyState;

  // Priority queue of timers registered with the watcher.
  TimerQueue _timerQueue;

  MojoHandleWatcher(this._controlHandle)
      : _shutdown = false,
        _waitManyState = new WaitManyState(),
        _timerQueue = new TimerQueue() {
    // Setup control handle.
    _waitManyState.add(_controlHandle, kMojoSignalsReadable, null);
  }

  static void _handleWatcherIsolate(int consumerHandle) {
    MojoHandleWatcher watcher = new MojoHandleWatcher(consumerHandle);
    while (!watcher._shutdown) {
      int deadline = watcher._processTimerDeadlines();
      watcher._waitManyState.waitMany(deadline);
      watcher._processWaitManyState();
    }
  }

  // Look at the signals that are satisified for each handle. If a signal is
  // of interest to the listener, route the event to it. Control messages are
  // processed last, after signaled handles are removed.
  void _processWaitManyState() {
    int mojoResult = _waitManyState.mojoResult;
    int idx = _waitManyState.outIndex;
    if (mojoResult == kMojoResultOk) {
      List<int> toRemove = [];
      for (int i = _waitManyState.length - 1; i >= 0; i--) {
        if (i == 0) {
          toRemove.forEach(_removeHandleIdx);
        }

        int signals = _waitManyState.signals[i];
        int outSignals = _waitManyState.outSignals(i);
        int eventSignals = outSignals & signals;
        if (eventSignals != 0) {
          if (i == 0) {
            _handleControlMessage();
          } else {
            _waitManyState.ports[i].send([signals, eventSignals]);
            toRemove.add(i);
          }
        }
      }
    } else if (mojoResult != kMojoResultDeadlineExceeded) {
      if (mojoResult == kMojoResultInvalidArgument) {
        // If there was an invalid argument, then outSignals has no meaning.
        _pruneClosedHandles(false);
      } else {
        // Some handle was closed, but not by us.
        // Find it and close it on our side.
        _pruneClosedHandles(true);
      }
    }
  }

  void _handleControlMessage() {
    List result = MojoHandleWatcherNatives.recvControlData(_controlHandle);
    // result[0] = mojo handle if any, or a timer deadline in milliseconds.
    // result[1] = SendPort if any.
    // result[2] = command << 2 | WRITABLE | READABLE

    var signals = _decodeSignals(result[2]);
    int command = _decodeCommand(result[2]);
    switch (command) {
      case ADD:
        _addHandle(result[0], result[1], signals);
        break;
      case REMOVE:
        _removeHandle(result[0]);
        break;
      case CLOSE:
        _close(result[0], result[1]);
        break;
      case TIMER:
        _timer(result[1], result[0]);
        break;
      case SHUTDOWN:
        _shutdownHandleWatcher(result[1]);
        break;
      default:
        throw "Invalid Command: $command";
        break;
    }
  }

  void _addHandle(int mojoHandle, SendPort port, int signals) {
    int idx = _waitManyState.handleIndices[mojoHandle];
    if (idx == null) {
      _waitManyState.add(mojoHandle, signals, port);
    } else {
      assert(_waitManyState.ports[idx] == port);
      assert(_waitManyState.handles[idx] == mojoHandle);
      _waitManyState.signals[idx] |= signals;
    }
  }

  void _removeHandle(int mojoHandle) {
    int idx = _waitManyState.handleIndices[mojoHandle];
    if (idx == null) {
      throw "Remove on a non-existent handle: idx = $idx.";
    }
    if (idx == 0) {
      throw "The control handle (idx = 0) cannot be removed.";
    }
    _removeHandleIdx(idx);
  }

  // Assumes idx is a valid handle index.
  void _removeHandleIdx(int idx) {
    _waitManyState.remove(idx);
  }

  void _close(int mojoHandle, SendPort port, {bool pruning: false}) {
    assert(!pruning || (port == null));
    int idx = _waitManyState.handleIndices[mojoHandle];
    if (idx == null) {
      // An app isolate may request that the handle watcher close a handle that
      // has already been pruned. This happens when the app isolate has not yet
      // received the PEER_CLOSED event. The app isolate will not close the
      // handle, so we must do so here.
      MojoHandleNatives.close(mojoHandle);
      if (port != null) port.send(null); // Notify that close is done.
      return;
    }
    if (idx == 0) {
      throw "The control handle (idx = 0) cannot be closed.";
    }
    MojoHandleNatives.close(_waitManyState.handles[idx]);
    if (port != null) port.send(null); // Notify that close is done.
    if (pruning) {
      // If this handle is being pruned, notify the application isolate
      // by sending MojoHandleSignals.PEER_CLOSED.
      _waitManyState.ports[idx]
          .send([_waitManyState.signals[idx], kMojoSignalsPeerClosed]);
    }
    _removeHandle(mojoHandle);
  }

  // Returns the next timer deadline in units of microseconds from 'now'.
  int _processTimerDeadlines() {
    int now = (new DateTime.now()).millisecondsSinceEpoch;
    while (_timerQueue.hasTimer && (now >= _timerQueue.currentTimeout)) {
      _timerQueue.currentPort.send(null);
      _timerQueue.removeCurrent();
      now = (new DateTime.now()).millisecondsSinceEpoch;
    }
    return _timerQueue.hasTimer
        ? (_timerQueue.currentTimeout - now) * 1000
        : kDeadlineIndefinite;
  }

  void _timer(SendPort port, int deadline) {
    _timerQueue.updateTimer(port, deadline);
  }

  void _pruneClosedHandles(bool gotStates) {
    List<int> closed = new List();
    for (var i = 0; i < _waitManyState.length; i++) {
      if (gotStates) {
        int signals = _waitManyState.outSignals(i);
        if ((signals & kMojoSignalsPeerClosed) != 0) {
          closed.add(_waitManyState.handles[i]);
        }
      } else {
        List mwr = MojoHandleNatives.wait(
            _waitManyState.handles[i], kMojoSignalsAll, 0);
        if ((mwr[0] != kMojoResultOk) &&
            (mwr[0] != kMojoResultDeadlineExceeded)) {
          closed.add(_waitManyState.handles[i]);
        }
      }
    }
    closed.forEach((int h) => _close(h, null, pruning: true));
    // '_close' updated the '_handles' array, so at this point the '_handles'
    // array and the caller's 'states' array are mismatched.
  }

  void _shutdownHandleWatcher(SendPort shutdownSendPort) {
    _shutdown = true;
    MojoHandleNatives.close(_controlHandle);
    shutdownSendPort.send(null);
  }

  static int _sendControlData(int rawHandle, SendPort port, int data) {
    int controlHandle = MojoHandleWatcherNatives.getControlHandle();
    if (controlHandle == kMojoHandleInvalid) {
      return kMojoResultFailedPrecondition;
    }

    var result = MojoHandleWatcherNatives.sendControlData(
        controlHandle, rawHandle, port, data);
    return result;
  }

  // Starts up the MojoHandleWatcher isolate. Should be called only once
  // per VM process.
  static Future<Isolate> _start() {
    // Make a control message pipe,
    List pipeEndpoints = MojoMessagePipeNatives.MojoCreateMessagePipe(0);
    assert(pipeEndpoints != null);
    assert((pipeEndpoints is List) && (pipeEndpoints.length == 3));
    assert(pipeEndpoints[0] == kMojoResultOk);

    int consumerHandle = pipeEndpoints[1];
    int producerHandle = pipeEndpoints[2];

    // Call setControlHandle with the other end.
    assert(producerHandle != kMojoHandleInvalid);
    MojoHandleWatcherNatives.setControlHandle(producerHandle);

    // Spawn the handle watcher isolate with the MojoHandleWatcher,
    return Isolate.spawn(_handleWatcherIsolate, consumerHandle);
  }

  // Causes the MojoHandleWatcher isolate to exit. Should be called only
  // once per VM process.
  static void _stop() {
    // Create a port for notification that the handle watcher has shutdown.
    var shutdownReceivePort = new ReceivePort();
    var shutdownSendPort = shutdownReceivePort.sendPort;

    // Send the shutdown command.
    _sendControlData(
        kMojoHandleInvalid, shutdownSendPort, _encodeCommand(SHUTDOWN));

    // Close the control handle.
    int controlHandle = MojoHandleWatcherNatives.getControlHandle();
    MojoHandleNatives.close(controlHandle);

    // Invalidate the control handle.
    MojoHandleWatcherNatives.setControlHandle(kMojoHandleInvalid);

    // Wait for the handle watcher isolate to exit.
    shutdownReceivePort.first.then((_) {
      shutdownReceivePort.close();
    });
  }

  // If wait is true, returns a future that resolves only after the handle
  // has actually been closed by the handle watcher. Otherwise, returns a
  // future that resolves immediately.
  static Future<int> close(int mojoHandle, {bool wait: false}) {
    if (!wait) {
      return new Future.value(
          _sendControlData(mojoHandle, null, _encodeCommand(CLOSE)));
    }
    int result;
    var completer = new Completer();
    var rawPort = new RawReceivePort((_) {
      completer.complete(result);
    });
    result =
        _sendControlData(mojoHandle, rawPort.sendPort, _encodeCommand(CLOSE));
    return completer.future.then((r) {
      rawPort.close();
      return r;
    });
  }

  static int add(int mojoHandle, SendPort port, int signals) {
    return _sendControlData(mojoHandle, port, _encodeCommand(ADD, signals));
  }

  static int remove(int mojoHandle) {
    return _sendControlData(mojoHandle, null, _encodeCommand(REMOVE));
  }

  static int timer(Object ignored, SendPort port, int deadline) {
    // The deadline will be unwrapped before sending to the handle watcher.
    return _sendControlData(deadline, port, _encodeCommand(TIMER));
  }
}
