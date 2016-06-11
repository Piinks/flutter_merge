// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_ENGINE_CORE_PAINTING_PAINT_H_
#define SKY_ENGINE_CORE_PAINTING_PAINT_H_

#include "flutter/tonic/dart_wrappable.h"
#include "third_party/skia/include/core/SkPaint.h"

namespace blink {

class Paint {
 public:
  SkPaint sk_paint;
  bool is_null;

  const SkPaint* paint() const { return is_null ? nullptr : &sk_paint; }
};

template <>
struct DartConverter<Paint> {
  static Paint FromDart(Dart_Handle handle);
  static Paint FromArguments(Dart_NativeArguments args,
                             int index,
                             Dart_Handle& exception);
};

}  // namespace blink

#endif  // SKY_ENGINE_CORE_PAINTING_PAINT_H_
