/*
 * Copyright (c) 2013, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sky/engine/platform/graphics/skia/GaneshUtils.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/gpu/SkGrPixelRef.h"

namespace blink {

bool ensureTextureBackedSkBitmap(GrContext* gr, SkBitmap& bitmap, const IntSize& size, GrSurfaceOrigin origin, GrPixelConfig config)
{
    if (!bitmap.getTexture() || bitmap.width() != size.width() || bitmap.height() != size.height()) {
        if (!gr)
            return false;
        GrTextureDesc desc;
        desc.fConfig = config;
        desc.fFlags = kRenderTarget_GrSurfaceFlag;
        desc.fSampleCnt = 0;
        desc.fOrigin = origin;
        desc.fWidth = size.width();
        desc.fHeight = size.height();
        SkAutoTUnref<GrTexture> texture(gr->textureProvider()->createTexture(desc, false, 0, 0));
        if (!texture.get())
            return false;

        SkImageInfo info = SkImageInfo::MakeN32Premul(desc.fWidth, desc.fHeight);
        SkGrPixelRef* pixelRef = SkNEW_ARGS(SkGrPixelRef, (info, texture.get()));
        if (!pixelRef)
            return false;
        bitmap.setInfo(info);
        bitmap.setPixelRef(pixelRef)->unref();
    }

    return true;
}

} // namespace blink
