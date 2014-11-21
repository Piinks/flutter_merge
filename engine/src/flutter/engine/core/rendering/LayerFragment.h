/*
 * Copyright (C) 2003, 2009, 2012 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SKY_ENGINE_CORE_RENDERING_LAYERFRAGMENT_H_
#define SKY_ENGINE_CORE_RENDERING_LAYERFRAGMENT_H_

#include "sky/engine/core/rendering/ClipRect.h"
#include "sky/engine/wtf/Vector.h"

namespace blink {

struct LayerFragment {
public:
    LayerFragment()
        : shouldPaintContent(false)
    {
    }

    void setRects(const LayoutRect& bounds, const ClipRect& background, const ClipRect& foreground, const ClipRect& outline)
    {
        layerBounds = bounds;
        backgroundRect = background;
        foregroundRect = foreground;
        outlineRect = outline;
    }

    void moveBy(const LayoutPoint& offset)
    {
        layerBounds.moveBy(offset);
        backgroundRect.moveBy(offset);
        foregroundRect.moveBy(offset);
        outlineRect.moveBy(offset);
    }

    void intersect(const LayoutRect& rect)
    {
        backgroundRect.intersect(rect);
        foregroundRect.intersect(rect);
        outlineRect.intersect(rect);
    }

    bool shouldPaintContent;
    LayoutRect layerBounds;
    ClipRect backgroundRect;
    ClipRect foregroundRect;
    ClipRect outlineRect;
};

typedef Vector<LayerFragment, 1> LayerFragments;

} // namespace blink

#endif  // SKY_ENGINE_CORE_RENDERING_LAYERFRAGMENT_H_
