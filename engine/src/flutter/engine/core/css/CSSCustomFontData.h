/*
 * Copyright (C) 2013 Google, Inc.
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

#ifndef SKY_ENGINE_CORE_CSS_CSSCUSTOMFONTDATA_H_
#define SKY_ENGINE_CORE_CSS_CSSCUSTOMFONTDATA_H_

#include "sky/engine/core/css/CSSFontFaceSource.h"
#include "sky/engine/platform/fonts/CustomFontData.h"

namespace blink {

class CSSCustomFontData final : public CustomFontData {
public:
    enum FallbackVisibility { InvisibleFallback, VisibleFallback };

    static PassRefPtr<CSSCustomFontData> create(RemoteFontFaceSource* source, FallbackVisibility visibility)
    {
        return adoptRef(new CSSCustomFontData(source, visibility));
    }

    virtual ~CSSCustomFontData() { }

    virtual bool shouldSkipDrawing() const override
    {
        if (m_fontFaceSource)
            m_fontFaceSource->paintRequested();
        return m_fallbackVisibility == InvisibleFallback && m_isLoading;
    }

    virtual void beginLoadIfNeeded() const override
    {
        if (!m_isLoading && m_fontFaceSource) {
            m_isLoading = true;
            m_fontFaceSource->beginLoadIfNeeded();
        }
    }

    virtual bool isLoading() const override { return m_isLoading; }
    virtual bool isLoadingFallback() const override { return true; }
    virtual void clearFontFaceSource() override { m_fontFaceSource = 0; }

private:
    CSSCustomFontData(RemoteFontFaceSource* source, FallbackVisibility visibility)
        : m_fontFaceSource(source)
        , m_fallbackVisibility(visibility)
        , m_isLoading(false)
    {
        if (source)
            m_isLoading = source->isLoading();
    }

    RemoteFontFaceSource* m_fontFaceSource;
    FallbackVisibility m_fallbackVisibility;
    mutable bool m_isLoading;
};

}

#endif  // SKY_ENGINE_CORE_CSS_CSSCUSTOMFONTDATA_H_
