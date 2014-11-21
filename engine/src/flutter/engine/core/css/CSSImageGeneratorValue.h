/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
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

#ifndef SKY_ENGINE_CORE_CSS_CSSIMAGEGENERATORVALUE_H_
#define SKY_ENGINE_CORE_CSS_CSSIMAGEGENERATORVALUE_H_

#include "sky/engine/core/css/CSSValue.h"
#include "sky/engine/platform/geometry/IntSizeHash.h"
#include "sky/engine/wtf/HashCountedSet.h"
#include "sky/engine/wtf/RefPtr.h"

namespace blink {

class ResourceFetcher;
class Image;
class RenderObject;
class StyleResolver;

struct SizeAndCount {
    SizeAndCount(IntSize newSize = IntSize(), int newCount = 0)
        : size(newSize)
        , count(newCount)
    {
    }

    IntSize size;
    int count;
};

typedef HashMap<const RenderObject*, SizeAndCount> RenderObjectSizeCountMap;

class CSSImageGeneratorValue : public CSSValue {
public:
    ~CSSImageGeneratorValue();

    void addClient(RenderObject*, const IntSize&);
    void removeClient(RenderObject*);
    PassRefPtr<Image> image(RenderObject*, const IntSize&);

    bool isFixedSize() const;
    IntSize fixedSize(const RenderObject*);

    bool isPending() const;
    bool knownToBeOpaque(const RenderObject*) const;

    void loadSubimages(ResourceFetcher*);

protected:
    explicit CSSImageGeneratorValue(ClassType);

    Image* getImage(RenderObject*, const IntSize&);
    void putImage(const IntSize&, PassRefPtr<Image>);
    const RenderObjectSizeCountMap& clients() const { return m_clients; }

    HashCountedSet<IntSize> m_sizes; // A count of how many times a given image size is in use.
    RenderObjectSizeCountMap m_clients; // A map from RenderObjects (with entry count) to image sizes.
    HashMap<IntSize, RefPtr<Image> > m_images; // A cache of Image objects by image size.
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSImageGeneratorValue, isImageGeneratorValue());

} // namespace blink

#endif  // SKY_ENGINE_CORE_CSS_CSSIMAGEGENERATORVALUE_H_
