// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/compositor/texture.h"

namespace impeller {

Texture::Texture(id<MTLTexture> texture) : texture_(texture) {}

Texture::~Texture() = default;

id<MTLTexture> Texture::GetMTLTexture() const {
  return texture_;
}

}  // namespace impeller
