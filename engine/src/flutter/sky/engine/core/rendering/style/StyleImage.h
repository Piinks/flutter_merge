/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef SKY_ENGINE_CORE_RENDERING_STYLE_STYLEIMAGE_H_
#define SKY_ENGINE_CORE_RENDERING_STYLE_STYLEIMAGE_H_

#include "sky/engine/platform/geometry/IntSize.h"
#include "sky/engine/platform/geometry/LayoutSize.h"
#include "sky/engine/platform/graphics/Image.h"
#include "sky/engine/wtf/PassRefPtr.h"
#include "sky/engine/wtf/RefCounted.h"
#include "sky/engine/wtf/RefPtr.h"

namespace blink {

class RenderObject;

typedef void* WrappedImagePtr;

class StyleImage : public RefCounted<StyleImage> {
public:
    virtual ~StyleImage() { }

    bool operator==(const StyleImage& other) const
    {
        return data() == other.data();
    }

    virtual bool canRender(const RenderObject&) const { return true; }
    virtual bool isLoaded() const { return true; }
    virtual bool errorOccurred() const { return false; }
    virtual LayoutSize imageSize(const RenderObject*) const = 0;
    virtual void computeIntrinsicDimensions(const RenderObject*, Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio) = 0;
    virtual bool imageHasRelativeWidth() const = 0;
    virtual bool imageHasRelativeHeight() const = 0;
    virtual bool usesImageContainerSize() const = 0;
    virtual void setContainerSizeForRenderer(const RenderObject*, const IntSize&) = 0;
    virtual void addClient(RenderObject*) = 0;
    virtual void removeClient(RenderObject*) = 0;
    virtual PassRefPtr<Image> image(RenderObject*, const IntSize&) const = 0;
    virtual WrappedImagePtr data() const = 0;
    virtual float imageScaleFactor() const { return 1; }
    virtual bool knownToBeOpaque(const RenderObject*) const = 0;

    ALWAYS_INLINE bool isPendingImage() const { return m_isPendingImage; }
    ALWAYS_INLINE bool isGeneratedImage() const { return m_isGeneratedImage; }

protected:
    StyleImage()
        : m_isPendingImage(false)
        , m_isGeneratedImage(false)
    {
    }
    bool m_isPendingImage:1;
    bool m_isGeneratedImage:1;
};

#define DEFINE_STYLE_IMAGE_TYPE_CASTS(thisType, function) \
    DEFINE_TYPE_CASTS(thisType, StyleImage, styleImage, styleImage->function, styleImage.function); \
    inline thisType* to##thisType(const RefPtr<StyleImage>& styleImage) { return to##thisType(styleImage.get()); } \
    typedef int NeedsSemiColonAfterDefineStyleImageTypeCasts

}
#endif  // SKY_ENGINE_CORE_RENDERING_STYLE_STYLEIMAGE_H_
