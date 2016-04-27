// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_ENGINE_CORE_PAINTING_COLORFILTER_H_
#define SKY_ENGINE_CORE_PAINTING_COLORFILTER_H_

#include "sky/engine/core/painting/CanvasColor.h"
#include "sky/engine/core/painting/TransferMode.h"
#include "sky/engine/tonic/dart_wrappable.h"
#include "sky/engine/wtf/PassRefPtr.h"
#include "sky/engine/wtf/ThreadSafeRefCounted.h"
#include "third_party/skia/include/core/SkColorFilter.h"

namespace blink {
class DartLibraryNatives;

class ColorFilter : public ThreadSafeRefCounted<ColorFilter>, public DartWrappable {
  DEFINE_WRAPPERTYPEINFO();
 public:
  ~ColorFilter() override;
  static PassRefPtr<ColorFilter> create(CanvasColor color,
                                        TransferMode transfer_mode);

  sk_sp<SkColorFilter> filter() { return filter_; }

  static void RegisterNatives(DartLibraryNatives* natives);

 private:
  ColorFilter(sk_sp<SkColorFilter> filter);

  sk_sp<SkColorFilter> filter_;
};

} // namespace blink

#endif  // SKY_ENGINE_CORE_PAINTING_COLORFILTER_H_
