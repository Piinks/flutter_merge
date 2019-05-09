// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:html' as html;
import 'dart:typed_data';

import 'package:ui/src/engine.dart';
import 'package:ui/ui.dart';

/// Contains method name that was called on [MockEngineCanvas] and arguments
/// that were passed.
class MockCanvasCall {
  MockCanvasCall._({
    this.methodName,
    this.arguments,
  });

  final String methodName;
  final dynamic arguments;

  @override
  String toString() {
    return '$MockCanvasCall($methodName, $arguments)';
  }
}

/// A fake implementation of [EngineCanvas] that logs calls to its methods but
/// doesn't actually paint anything.
///
/// Useful for testing interactions between upper layers of the system with
/// canvases.
class MockEngineCanvas implements EngineCanvas {
  final List<MockCanvasCall> methodCallLog = <MockCanvasCall>[];

  @override
  html.Element get rootElement => null;

  void _called(String methodName, {dynamic arguments}) {
    methodCallLog.add(MockCanvasCall._(
      methodName: methodName,
      arguments: arguments,
    ));
  }

  @override
  void dispose() {
    _called('dispose');
  }

  @override
  void clear() {
    _called('clear');
  }

  @override
  void save() {
    _called('save');
  }

  @override
  void restore() {
    _called('restore');
  }

  @override
  void translate(double dx, double dy) {
    _called('translate', arguments: {
      'dx': dx,
      'dy': dy,
    });
  }

  @override
  void scale(double sx, double sy) {
    _called('scale', arguments: {
      'sx': sx,
      'sy': sy,
    });
  }

  @override
  void rotate(double radians) {
    _called('rotate', arguments: radians);
  }

  @override
  void skew(double sx, double sy) {
    _called('skew', arguments: {
      'sx': sx,
      'sy': sy,
    });
  }

  @override
  void transform(Float64List matrix4) {
    _called('transform', arguments: matrix4);
  }

  @override
  void clipRect(Rect rect) {
    _called('clipRect', arguments: rect);
  }

  @override
  void clipRRect(RRect rrect) {
    _called('clipRRect', arguments: rrect);
  }

  @override
  void clipPath(Path path) {
    _called('clipPath', arguments: path);
  }

  @override
  void drawColor(Color color, BlendMode blendMode) {
    _called('drawColor', arguments: {
      'color': color,
      'blendMode': blendMode,
    });
  }

  @override
  void drawLine(Offset p1, Offset p2, PaintData paint) {
    _called('drawLine', arguments: {
      'p1': p1,
      'p2': p2,
      'paint': paint,
    });
  }

  @override
  void drawPaint(PaintData paint) {
    _called('drawPaint', arguments: paint);
  }

  @override
  void drawRect(Rect rect, PaintData paint) {
    _called('drawRect', arguments: paint);
  }

  @override
  void drawRRect(RRect rrect, PaintData paint) {
    _called('drawRRect', arguments: {
      'rrect': rrect,
      'paint': paint,
    });
  }

  @override
  void drawDRRect(RRect outer, RRect inner, PaintData paint) {
    _called('drawDRRect', arguments: {
      'outer': outer,
      'inner': inner,
      'paint': paint,
    });
  }

  @override
  void drawOval(Rect rect, PaintData paint) {
    _called('drawOval', arguments: {
      'rect': rect,
      'paint': paint,
    });
  }

  @override
  void drawCircle(Offset c, double radius, PaintData paint) {
    _called('drawCircle', arguments: {
      'c': c,
      'radius': radius,
      'paint': paint,
    });
  }

  @override
  void drawPath(Path path, PaintData paint) {
    _called('drawPath', arguments: {
      'path': path,
      'paint': paint,
    });
  }

  @override
  void drawShadow(
      Path path, Color color, double elevation, bool transparentOccluder) {
    _called('drawShadow', arguments: {
      'path': path,
      'color': color,
      'elevation': elevation,
      'transparentOccluder': transparentOccluder,
    });
  }

  @override
  void drawImage(Image image, Offset p, PaintData paint) {
    _called('drawImage', arguments: {
      'image': image,
      'p': p,
      'paint': paint,
    });
  }

  @override
  void drawImageRect(Image image, Rect src, Rect dst, PaintData paint) {
    _called('drawImageRect', arguments: {
      'image': image,
      'src': src,
      'dst': dst,
      'paint': paint,
    });
  }

  @override
  void drawParagraph(Paragraph paragraph, Offset offset) {
    _called('drawParagraph', arguments: {
      'paragraph': paragraph,
      'offset': offset,
    });
  }
}
