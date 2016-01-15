// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

part of bindings;

class ProxyError {
  final String message;
  ProxyError(this.message);
  String toString() => "ProxyError: $message";
}

abstract class Proxy extends core.MojoEventHandler {
  HashMap<int, Completer> _completerMap = new HashMap<int, Completer>();
  Completer _errorCompleter = new Completer();
  Set<Completer> _errorCompleters;
  int _nextId = 0;
  int _version = 0;
  int _pendingCount = 0;

  Proxy.fromEndpoint(core.MojoMessagePipeEndpoint endpoint)
      : super.fromEndpoint(endpoint);

  Proxy.fromHandle(core.MojoHandle handle) : super.fromHandle(handle);

  Proxy.unbound() : super.unbound();

  void handleResponse(ServiceMessage reader);

  /// If there is an error in using this proxy, this future completes with
  /// a ProxyError.
  Future get errorFuture => _errorCompleter.future;

  /// Version of this interface that the remote side supports. Updated when a
  /// call to [queryVersion] or [requireVersion] is made.
  int get version => _version;

  void handleRead() {
    var result = endpoint.queryAndRead();
    if ((result.data == null) || (result.dataLength == 0)) {
      proxyError("Read from message pipe endpoint failed");
      return;
    }
    try {
      var message = new ServiceMessage.fromMessage(new Message(result.data,
          result.handles, result.dataLength, result.handlesLength));
      _pendingCount--;
      if (ControlMessageHandler.isControlMessage(message)) {
        _handleControlMessageResponse(message);
        return;
      }
      handleResponse(message);
    } on MojoCodecError catch (e) {
      proxyError(e.toString());
      close(immediate: true);
    }
  }

  void handleWrite() {
    proxyError("Unexpected writable signal");
  }

  @override
  Future close({bool immediate: false}) {
    // Drop the completers for outstanding calls. The Futures will never
    // complete.
    _completerMap.clear();

    // Signal to any pending calls that the Proxy is closed.
    if (_pendingCount > 0) {
      proxyError("The Proxy is closed.");
    }

    return super.close(immediate: immediate);
  }

  void sendMessage(Struct message, int name) {
    if (!isBound) {
      proxyError("The Proxy is closed.");
      return;
    }
    if (!isOpen) {
      beginHandlingEvents();
    }
    var header = new MessageHeader(name);
    var serviceMessage = message.serializeWithHeader(header);
    endpoint.write(serviceMessage.buffer, serviceMessage.buffer.lengthInBytes,
        serviceMessage.handles);
    if (endpoint.status != core.MojoResult.kOk) {
      proxyError("Write to message pipe endpoint failed.");
    }
  }

  Future sendMessageWithRequestId(Struct message, int name, int id, int flags) {
    var completer = new Completer();
    if (!isBound) {
      proxyError("The Proxy is closed.");
      return completer.future;
    }
    if (!isOpen) {
      beginHandlingEvents();
    }
    if (id == -1) {
      id = _nextId++;
    }

    var header = new MessageHeader.withRequestId(name, flags, id);
    var serviceMessage = message.serializeWithHeader(header);
    endpoint.write(serviceMessage.buffer, serviceMessage.buffer.lengthInBytes,
        serviceMessage.handles);

    if (endpoint.status == core.MojoResult.kOk) {
      _completerMap[id] = completer;
      _pendingCount++;
    } else {
      proxyError("Write to message pipe endpoint failed: ${endpoint}");
    }
    return completer.future;
  }

  // Need a getter for this for access in subclasses.
  HashMap<int, Completer> get completerMap => _completerMap;

  String toString() {
    var superString = super.toString();
    return "Proxy(${superString})";
  }

  /// Queries the max version that the remote side supports.
  /// Updates [version].
  Future<int> queryVersion() async {
    var params = new icm.RunMessageParams();
    params.reserved0 = 16;
    params.reserved1 = 0;
    params.queryVersion = new icm.QueryVersion();
    var response = await sendMessageWithRequestId(
        params, icm.kRunMessageId, -1, MessageHeader.kMessageExpectsResponse);
    _version = response.queryVersionResult.version;
    return _version;
  }

  /// If the remote side doesn't support the [requiredVersion], it will close
  /// its end of the message pipe asynchronously. This does nothing if it's
  /// already known that the remote side supports [requiredVersion].
  /// Updates [version].
  void requireVersion(int requiredVersion) {
    if (requiredVersion <= _version) {
      // Already supported.
      return;
    }

    // If the remote end doesn't close the pipe, we know that it supports
    // required version.
    _version = requiredVersion;

    var params = new icm.RunOrClosePipeMessageParams();
    params.reserved0 = 16;
    params.reserved1 = 0;
    params.requireVersion = new icm.RequireVersion();
    params.requireVersion.version = requiredVersion;
    // TODO(johnmccutchan): We've set _version above but if this sendMessage
    // fails we may not have sent the RunOrClose message. Should
    // we reset _version in that case?
    sendMessage(params, icm.kRunOrClosePipeMessageId);
  }

  void proxyError(String msg) {
    if (!_errorCompleter.isCompleted) {
      errorFuture.whenComplete(() {
        _errorCompleter = new Completer();
      });
      _errorCompleter.complete(new ProxyError(msg));
    }
  }

  /// [responseOrError] returns a [Future] that completes to whatever [f]
  /// completes to unless [errorFuture] completes first. When [errorFuture]
  /// completes first, the [Future] returned by [responseOrError] completes with
  /// an error using the object that [errorFuture] completed with.
  ///
  /// Example usage:
  ///
  /// try {
  ///   result = await MyProxy.responseOrError(MyProxy.ptr.call(a,b,c));
  /// } catch (e) {
  ///   ...
  /// }
  Future responseOrError(Future f) {
    assert(f != null);
    if (_errorCompleters == null) {
      _errorCompleters = new Set<Completer>();
      errorFuture.then((e) {
        for (var completer in _errorCompleters) {
          assert(!completer.isCompleted);
          completer.completeError(e);
        }
        _errorCompleters.clear();
        _errorCompleters = null;
      });
    }

    Completer callCompleter = new Completer();
    f.then((callResult) {
      if (!callCompleter.isCompleted) {
        _errorCompleters.remove(callCompleter);
        callCompleter.complete(callResult);
      }
    });
    _errorCompleters.add(callCompleter);
    return callCompleter.future;
  }

  _handleControlMessageResponse(ServiceMessage message) {
    // We only expect to see Run messages.
    if (message.header.type != icm.kRunMessageId) {
      proxyError("Unexpected header type in control message response: "
          "${message.header.type}");
      return;
    }

    var response = icm.RunResponseMessageParams.deserialize(message.payload);
    if (!message.header.hasRequestId) {
      proxyError("Expected a message with a valid request Id.");
      return;
    }
    Completer c = completerMap[message.header.requestId];
    if (c == null) {
      proxyError("Message had unknown request Id: ${message.header.requestId}");
      return;
    }
    completerMap.remove(message.header.requestId);
    if (c.isCompleted) {
      proxyError("Control message response completer already completed");
      return;
    }
    c.complete(response);
  }
}

/// Generated Proxy classes implement this interface.
abstract class ProxyBase {
  final Proxy impl = null;
  final String serviceName = null;
}

/// Generated Proxy classes have a factory Proxy.connectToService which takes
/// a ServiceConnector, a url, and optionally a service name and returns a
/// bound Proxy. For example, every class extending the Application base class
/// in package:mojo/application.dart inherits and implementation of the
/// ServiceConnector interface.
abstract class ServiceConnector {
  /// Connects [proxy] to the service called [serviceName] that lives at [url].
  void connectToService(String url, ProxyBase proxy, [String serviceName]);
}
