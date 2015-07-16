/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
 */

#ifndef SKY_ENGINE_PLATFORM_GRAPHICS_FILTERS_FEFLOOD_H_
#define SKY_ENGINE_PLATFORM_GRAPHICS_FILTERS_FEFLOOD_H_

#include "sky/engine/platform/graphics/Color.h"
#include "sky/engine/platform/graphics/filters/Filter.h"
#include "sky/engine/platform/graphics/filters/FilterEffect.h"

namespace blink {

class PLATFORM_EXPORT FEFlood : public FilterEffect {
public:
    static PassRefPtr<FEFlood> create(Filter* filter, const Color&, float);

    Color floodColor() const;
    bool setFloodColor(const Color &);

    float floodOpacity() const;
    bool setFloodOpacity(float);

    // feFlood does not perform color interpolation of any kind, so the result is always in the current
    // color space regardless of the value of color-interpolation-filters.
    virtual void setOperatingColorSpace(ColorSpace) override { }
    virtual void setResultColorSpace(ColorSpace) override { }

    virtual PassRefPtr<SkImageFilter> createImageFilter(SkiaImageFilterBuilder*) override;

    virtual TextStream& externalRepresentation(TextStream&, int indention) const override;

private:
    FEFlood(Filter*, const Color&, float);

    virtual void applySoftware() override;

    Color m_floodColor;
    float m_floodOpacity;
};

} // namespace blink

#endif  // SKY_ENGINE_PLATFORM_GRAPHICS_FILTERS_FEFLOOD_H_
