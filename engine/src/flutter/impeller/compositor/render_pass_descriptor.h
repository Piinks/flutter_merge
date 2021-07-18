// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <map>
#include <optional>

#include <Metal/Metal.h>

#include "flutter/fml/macros.h"
#include "impeller/compositor/formats.h"
#include "impeller/geometry/size.h"

namespace impeller {

class RenderPassDescriptor {
 public:
  RenderPassDescriptor();

  ~RenderPassDescriptor();

  bool HasColorAttachment(size_t index) const;

  std::optional<ISize> GetColorAttachmentSize(size_t index) const;

  RenderPassDescriptor& SetColorAttachment(RenderPassColorAttachment attachment,
                                           size_t index);

  RenderPassDescriptor& SetDepthAttachment(
      RenderPassDepthAttachment attachment);

  RenderPassDescriptor& SetStencilAttachment(
      RenderPassStencilAttachment attachment);

  MTLRenderPassDescriptor* ToMTLRenderPassDescriptor() const;

 private:
  std::map<size_t, RenderPassColorAttachment> colors_;
  std::optional<RenderPassDepthAttachment> depth_;
  std::optional<RenderPassStencilAttachment> stencil_;
};

}  // namespace impeller
