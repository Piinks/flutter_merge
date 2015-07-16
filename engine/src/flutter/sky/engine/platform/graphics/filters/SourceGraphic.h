/*
 * Copyright (C) 2008 Alex Mathews <possessedpenguinbob@gmail.com>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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

#ifndef SKY_ENGINE_PLATFORM_GRAPHICS_FILTERS_SOURCEGRAPHIC_H_
#define SKY_ENGINE_PLATFORM_GRAPHICS_FILTERS_SOURCEGRAPHIC_H_

#include "sky/engine/platform/graphics/DisplayList.h"
#include "sky/engine/platform/graphics/filters/Filter.h"
#include "sky/engine/platform/graphics/filters/FilterEffect.h"

namespace blink {

class PLATFORM_EXPORT SourceGraphic : public FilterEffect {
public:
    static PassRefPtr<SourceGraphic> create(Filter*);

    static const AtomicString& effectName();

    virtual FloatRect determineAbsolutePaintRect(const FloatRect& requestedRect) override;

    virtual FilterEffectType filterEffectType() const override { return FilterEffectTypeSourceInput; }

    virtual TextStream& externalRepresentation(TextStream&, int indention) const override;
    PassRefPtr<SkImageFilter> createImageFilter(SkiaImageFilterBuilder*) override;

private:
    SourceGraphic(Filter* filter)
        : FilterEffect(filter)
    {
        setOperatingColorSpace(ColorSpaceDeviceRGB);
    }

    virtual void applySoftware() override;
    // TODO(sky): This class appears to be broken...
};

} //namespace blink

#endif  // SKY_ENGINE_PLATFORM_GRAPHICS_FILTERS_SOURCEGRAPHIC_H_
