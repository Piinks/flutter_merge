import 'dart:ui' as ui;

/// Base class for input events.
class InputEvent {

  const InputEvent({ this.type, this.timeStamp: 0.0 });

  factory InputEvent.fromUiEvent(ui.Event event) {
    if (event is ui.PointerEvent)
      return new PointerInputEvent.fromUiEvent(event);

    // Default event
    return new InputEvent(
      type: event.type,
      timeStamp: event.timeStamp
    );
  }

  final String type;
  final double timeStamp;

}

/// Input event
class PointerInputEvent extends InputEvent {

  // Map actual input pointer value to a unique value
  // Since events are serialized we can just use a counter
  static Map<int, int> _pointerMap = new Map<int, int>();
  static int _pointerCount = 0;

  const PointerInputEvent({
    String type,
    double timeStamp: 0.0,
    this.pointer,
    this.kind,
    this.x,
    this.y,
    this.dx,
    this.dy,
    this.buttons,
    this.down,
    this.primary,
    this.obscured,
    this.pressure,
    this.pressureMin,
    this.pressureMax,
    this.distance,
    this.distanceMin,
    this.distanceMax,
    this.radiusMajor,
    this.radiusMinor,
    this.radiusMin,
    this.radiusMax,
    this.orientation,
    this.tilt
  }) : super(type: type, timeStamp: timeStamp);

  factory PointerInputEvent.fromUiEvent(ui.PointerEvent event) {
     PointerInputEvent result = new PointerInputEvent(
        type: event.type,
        timeStamp: event.timeStamp,
        pointer: (event.type == 'pointerDown') ? _pointerCount : _pointerMap[event.pointer],
        kind: event.kind,
        x: event.x,
        y: event.y,
        dx: event.dx,
        dy: event.dy,
        buttons: event.buttons,
        down: event.down,
        primary: event.primary,
        obscured: event.obscured,
        pressure: event.pressure,
        pressureMin: event.pressureMin,
        pressureMax: event.pressureMax,
        distance: event.distance,
        distanceMin: event.distanceMin,
        distanceMax: event.distanceMax,
        radiusMajor: event.radiusMajor,
        radiusMinor: event.radiusMinor,
        radiusMin: event.radiusMin,
        radiusMax: event.radiusMax,
        orientation: event.orientation,
        tilt: event.tilt
      );
      if (event.type == 'pointerdown') {
        _pointerMap[event.pointer] = _pointerCount;
        _pointerCount++;
      }
      return result;
  }

  final int pointer;
  final String kind;
  final double x;
  final double y;
  final double dx;
  final double dy;
  final int buttons;
  final bool down;
  final bool primary;
  final bool obscured;
  final double pressure;
  final double pressureMin;
  final double pressureMax;
  final double distance;
  final double distanceMin;
  final double distanceMax;
  final double radiusMajor;
  final double radiusMinor;
  final double radiusMin;
  final double radiusMax;
  final double orientation;
  final double tilt;

}
