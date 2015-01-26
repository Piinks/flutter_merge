/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "sky/engine/config.h"

#include "sky/engine/platform/graphics/filters/SkiaImageFilterBuilder.h"

#include "sky/engine/platform/graphics/ImageBuffer.h"
#include "sky/engine/platform/graphics/filters/FilterEffect.h"
#include "sky/engine/platform/graphics/filters/FilterOperations.h"
#include "sky/engine/platform/graphics/filters/SourceGraphic.h"
#include "sky/engine/platform/graphics/skia/SkiaUtils.h"
#include "sky/engine/public/platform/WebPoint.h"
#include "third_party/skia/include/effects/SkBlurImageFilter.h"
#include "third_party/skia/include/effects/SkColorFilterImageFilter.h"
#include "third_party/skia/include/effects/SkColorMatrixFilter.h"
#include "third_party/skia/include/effects/SkDropShadowImageFilter.h"
#include "third_party/skia/include/effects/SkMatrixImageFilter.h"
#include "third_party/skia/include/effects/SkTableColorFilter.h"

namespace blink {

SkiaImageFilterBuilder::SkiaImageFilterBuilder()
    : m_context(0)
    , m_sourceGraphic(0)
{
}

SkiaImageFilterBuilder::SkiaImageFilterBuilder(GraphicsContext* context)
    : m_context(context)
    , m_sourceGraphic(0)
{
}

SkiaImageFilterBuilder::~SkiaImageFilterBuilder()
{
}

PassRefPtr<SkImageFilter> SkiaImageFilterBuilder::build(FilterEffect* effect, ColorSpace colorSpace, bool destinationRequiresValidPreMultipliedPixels)
{
    if (!effect)
        return nullptr;

    bool requiresPMColorValidation = effect->mayProduceInvalidPreMultipliedPixels() && destinationRequiresValidPreMultipliedPixels;

    if (SkImageFilter* filter = effect->getImageFilter(colorSpace, requiresPMColorValidation))
        return filter;

    // Note that we may still need the color transform even if the filter is null
    RefPtr<SkImageFilter> origFilter = requiresPMColorValidation ? effect->createImageFilter(this) : effect->createImageFilterWithoutValidation(this);
    RefPtr<SkImageFilter> filter = transformColorSpace(origFilter.get(), effect->operatingColorSpace(), colorSpace);
    effect->setImageFilter(colorSpace, requiresPMColorValidation, filter.get());
    if (filter.get() != origFilter.get())
        effect->setImageFilter(effect->operatingColorSpace(), requiresPMColorValidation, origFilter.get());
    return filter.release();
}

PassRefPtr<SkImageFilter> SkiaImageFilterBuilder::transformColorSpace(
    SkImageFilter* input, ColorSpace srcColorSpace, ColorSpace dstColorSpace) {

    RefPtr<SkColorFilter> colorFilter = ImageBuffer::createColorSpaceFilter(srcColorSpace, dstColorSpace);
    if (!colorFilter)
        return input;

    return adoptRef(SkColorFilterImageFilter::Create(colorFilter.get(), input));
}

PassRefPtr<SkImageFilter> SkiaImageFilterBuilder::buildTransform(const AffineTransform& transform, SkImageFilter* input)
{
    return adoptRef(SkMatrixImageFilter::Create(affineTransformToSkMatrix(transform), SkPaint::kHigh_FilterLevel, input));
}

} // namespace blink
