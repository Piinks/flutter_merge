// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flow/layers/clip_rrect_layer.h"

namespace flow {

ClipRRectLayer::ClipRRectLayer() {
}

ClipRRectLayer::~ClipRRectLayer() {
}

void ClipRRectLayer::Preroll(PrerollContext* context, const SkMatrix& matrix) {
  PrerollChildren(context, matrix);
  if (!context->child_paint_bounds.intersect(clip_rrect_.getBounds()))
    context->child_paint_bounds.setEmpty();
  set_paint_bounds(context->child_paint_bounds);
}

void ClipRRectLayer::Paint(PaintContext::ScopedFrame& frame) {
  SkCanvas& canvas = frame.canvas();
  SkAutoCanvasRestore save(&canvas, false);
  canvas.saveLayer(&paint_bounds(), nullptr);
  canvas.clipRRect(clip_rrect_);
  PaintChildren(frame);
}

}  // namespace flow
