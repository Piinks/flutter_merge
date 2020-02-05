// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:html' as html;

import 'package:ui/src/engine.dart';
import 'package:ui/ui.dart';
import 'package:test/test.dart';

import 'package:web_engine_tester/golden_tester.dart';

import 'scuba.dart';

void main() async {
  final Rect region = Rect.fromLTWH(0, 0, 500, 100);

  BitmapCanvas canvas;

  void appendToScene() {
    // Create a <flt-scene> element to make sure our CSS reset applies correctly.
    final html.Element testScene = html.Element.tag('flt-scene');
    testScene.append(canvas.rootElement);
    html.document.querySelector('flt-scene-host').append(testScene);
  }

  setUpStableTestFonts();

  tearDown(() {
    html.document.querySelector('flt-scene').remove();
  });

  /// Draws several lines, some aligned precisely with the pixel grid, and some
  /// that are offset by 0.5 vertically or horizontally.
  ///
  /// The produced picture stresses the antialiasing generated by the browser
  /// when positioning and rasterizing `<canvas>` tags. Aliasing artifacts can
  /// be seen depending on pixel alignment and whether antialiasing happens
  /// before or after rasterization.
  void drawMisalignedLines(BitmapCanvas canvas) {
    final SurfacePaintData linePaint = (SurfacePaint()
          ..style = PaintingStyle.stroke
          ..strokeWidth = 1)
        .paintData;

    final SurfacePaintData fillPaint =
        (SurfacePaint()..style = PaintingStyle.fill).paintData;

    canvas.translate(10, 10);

    canvas.drawRect(
      const Rect.fromLTWH(0, 0, 40, 40),
      linePaint,
    );

    canvas.drawLine(
      const Offset(10, 0),
      const Offset(10, 40),
      linePaint,
    );

    canvas.drawLine(
      const Offset(20.5, 0),
      const Offset(20, 40),
      linePaint,
    );

    canvas.drawCircle(const Offset(30, 10), 3, fillPaint);
    canvas.drawCircle(const Offset(30.5, 30), 3, fillPaint);
  }

  test('renders pixels that are not aligned inside the canvas', () async {
    canvas = BitmapCanvas(const Rect.fromLTWH(0, 0, 60, 60));

    drawMisalignedLines(canvas);

    appendToScene();

    await matchGoldenFile('misaligned_pixels_in_canvas_test.png', region: region);
  }, timeout: const Timeout(Duration(seconds: 10)));

  test('compensates for misalignment of the canvas', () async {
    // Notice the 0.5 offset in the bounds rectangle. It's what causes the
    // misalignment of canvas relative to the pixel grid. BitmapCanvas will
    // shift its position back to 0.0 and at the same time it will it will
    // compensate by shifting the contents of the canvas in the opposite
    // direction.
    canvas = BitmapCanvas(const Rect.fromLTWH(0.5, 0.5, 60, 60));

    drawMisalignedLines(canvas);

    appendToScene();

    await matchGoldenFile('misaligned_canvas_test.png', region: region);
  }, timeout: const Timeout(Duration(seconds: 10)));

  test('fill the whole canvas with color even when transformed', () async {
    canvas = BitmapCanvas(const Rect.fromLTWH(0, 0, 50, 50));

    canvas.translate(25, 25);
    canvas.drawColor(const Color.fromRGBO(0, 255, 0, 1.0), BlendMode.src);

    appendToScene();

    await matchGoldenFile('bitmap_canvas_fills_color_when_transformed.png', region: region);
  }, timeout: const Timeout(Duration(seconds: 10)));

  test('fill the whole canvas with paint even when transformed', () async {
    canvas = BitmapCanvas(const Rect.fromLTWH(0, 0, 50, 50));

    canvas.translate(25, 25);
    canvas.drawPaint(SurfacePaintData()
      ..color = const Color.fromRGBO(0, 255, 0, 1.0)
      ..style = PaintingStyle.fill);

    appendToScene();

    await matchGoldenFile('bitmap_canvas_fills_paint_when_transformed.png', region: region);
  }, timeout: const Timeout(Duration(seconds: 10)));

  // This test reproduces text blurriness when two pieces of text appear inside
  // two nested clips:
  //
  //   ┌───────────────────────┐
  //   │   text in outer clip  │
  //   │ ┌────────────────────┐│
  //   │ │ text in inner clip ││
  //   │ └────────────────────┘│
  //   └───────────────────────┘
  //
  // This test clips using canvas. See a similar test in `compositing_golden_test.dart`,
  // which clips using layers.
  //
  // More details: https://github.com/flutter/flutter/issues/32274
  test('renders clipped DOM text with high quality', () async {
    final Paragraph paragraph =
        (ParagraphBuilder(ParagraphStyle(fontFamily: 'Roboto'))..addText('Am I blurry?')).build();
    paragraph.layout(const ParagraphConstraints(width: 1000));

    final Rect canvasSize = Rect.fromLTRB(
      0,
      0,
      paragraph.maxIntrinsicWidth + 16,
      2 * paragraph.height + 32,
    );
    final Rect outerClip =
        Rect.fromLTRB(0.5, 0.5, canvasSize.right, canvasSize.bottom);
    final Rect innerClip = Rect.fromLTRB(0.5, canvasSize.bottom / 2 + 0.5,
        canvasSize.right, canvasSize.bottom);

    canvas = BitmapCanvas(canvasSize);
    canvas.debugChildOverdraw = true;
    canvas.clipRect(outerClip);
    canvas.drawParagraph(paragraph, const Offset(8.5, 8.5));
    canvas.clipRect(innerClip);
    canvas.drawParagraph(paragraph, Offset(8.5, 8.5 + innerClip.top));

    expect(
      canvas.rootElement.querySelectorAll('p').map<String>((e) => e.innerText).toList(),
      <String>['Am I blurry?', 'Am I blurry?'],
      reason: 'Expected to render text using HTML',
    );

    appendToScene();

    await matchGoldenFile(
      'bitmap_canvas_draws_high_quality_text.png',
      region: canvasSize,
      maxDiffRatePercent: 0.0,
      pixelComparison: PixelComparison.precise,
    );
  }, timeout: const Timeout(Duration(seconds: 10)), testOn: 'chrome');
}
