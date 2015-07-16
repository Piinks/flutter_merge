/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.
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

#ifndef SKY_ENGINE_CORE_CSS_CSSPROPERTY_H_
#define SKY_ENGINE_CORE_CSS_CSSPROPERTY_H_

#include "gen/sky/core/CSSPropertyNames.h"
#include "gen/sky/platform/RuntimeEnabledFeatures.h"
#include "sky/engine/core/css/CSSPropertyMetadata.h"
#include "sky/engine/core/css/CSSValue.h"
#include "sky/engine/platform/text/TextDirection.h"
#include "sky/engine/wtf/PassRefPtr.h"
#include "sky/engine/wtf/RefPtr.h"

namespace blink {

struct StylePropertyMetadata {
    StylePropertyMetadata(CSSPropertyID propertyID, bool isSetFromShorthand, int indexInShorthandsVector, bool implicit, bool inherited)
        : m_propertyID(propertyID)
        , m_isSetFromShorthand(isSetFromShorthand)
        , m_indexInShorthandsVector(indexInShorthandsVector)
        , m_implicit(implicit)
        , m_inherited(inherited)
    {
    }

    CSSPropertyID shorthandID() const;

    uint16_t m_propertyID : 10;
    uint16_t m_isSetFromShorthand : 1;
    uint16_t m_indexInShorthandsVector : 2; // If this property was set as part of an ambiguous shorthand, gives the index in the shorthands vector.
    uint16_t m_implicit : 1; // Whether or not the property was set implicitly as the result of a shorthand.
    uint16_t m_inherited : 1;
};

class CSSProperty {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    CSSProperty(CSSPropertyID propertyID, PassRefPtr<CSSValue> value, bool isSetFromShorthand = false, int indexInShorthandsVector = 0, bool implicit = false)
        : m_metadata(propertyID, isSetFromShorthand, indexInShorthandsVector, implicit, CSSPropertyMetadata::isInheritedProperty(propertyID))
        , m_value(value)
    {
    }

    // FIXME: Remove this.
    CSSProperty(StylePropertyMetadata metadata, CSSValue* value)
        : m_metadata(metadata)
        , m_value(value)
    {
    }

    CSSPropertyID id() const { return static_cast<CSSPropertyID>(m_metadata.m_propertyID); }
    bool isSetFromShorthand() const { return m_metadata.m_isSetFromShorthand; };
    CSSPropertyID shorthandID() const { return m_metadata.shorthandID(); };

    CSSValue* value() const { return m_value.get(); }

    void wrapValueInCommaSeparatedList();

    static CSSPropertyID resolveDirectionAwareProperty(CSSPropertyID, TextDirection);

    const StylePropertyMetadata& metadata() const { return m_metadata; }

private:
    StylePropertyMetadata m_metadata;
    RefPtr<CSSValue> m_value;
};

} // namespace blink

WTF_ALLOW_MOVE_AND_INIT_WITH_MEM_FUNCTIONS(blink::CSSProperty);

#endif  // SKY_ENGINE_CORE_CSS_CSSPROPERTY_H_
