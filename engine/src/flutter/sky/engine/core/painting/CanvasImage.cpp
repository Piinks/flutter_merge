// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/core/painting/CanvasImage.h"

#include "sky/engine/tonic/dart_args.h"
#include "sky/engine/tonic/dart_binding_macros.h"
#include "sky/engine/tonic/dart_converter.h"
#include "sky/engine/tonic/dart_library_natives.h"

namespace blink {

typedef CanvasImage Image;

IMPLEMENT_WRAPPERTYPEINFO(Image);

#define FOR_EACH_BINDING(V) \
  V(Image, width) \
  V(Image, height) \
  V(Image, dispose)

FOR_EACH_BINDING(DART_NATIVE_CALLBACK)

void CanvasImage::RegisterNatives(DartLibraryNatives* natives) {
  natives->Register({
FOR_EACH_BINDING(DART_REGISTER_NATIVE)
  });
}

CanvasImage::CanvasImage() {
}

CanvasImage::~CanvasImage() {
}

int CanvasImage::width() {
  return image_->width();
}

int CanvasImage::height() {
  return image_->height();
}

void CanvasImage::dispose() {
  ClearDartWrapper();
}

}  // namespace blink
