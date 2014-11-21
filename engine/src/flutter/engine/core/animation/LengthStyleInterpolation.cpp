// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/config.h"
#include "sky/engine/core/animation/LengthStyleInterpolation.h"

#include "sky/engine/core/css/CSSCalculationValue.h"
#include "sky/engine/core/css/resolver/StyleBuilder.h"

namespace blink {

bool LengthStyleInterpolation::canCreateFrom(const CSSValue& value)
{
    if (value.isPrimitiveValue()) {
        const CSSPrimitiveValue& primitiveValue = blink::toCSSPrimitiveValue(value);
        if (primitiveValue.cssCalcValue())
            return true;

        CSSPrimitiveValue::LengthUnitType type;
        // Only returns true if the type is a primitive length unit.
        return CSSPrimitiveValue::unitTypeToLengthUnitType(primitiveValue.primitiveType(), type);
    }
    return value.isCalcValue();
}

PassOwnPtr<InterpolableValue> LengthStyleInterpolation::lengthToInterpolableValue(CSSValue* value)
{
    OwnPtr<InterpolableList> result = InterpolableList::create(CSSPrimitiveValue::LengthUnitTypeCount);
    CSSPrimitiveValue* primitive = toCSSPrimitiveValue(value);

    CSSLengthArray array;
    for (size_t i = 0; i < CSSPrimitiveValue::LengthUnitTypeCount; i++)
        array.append(0);
    primitive->accumulateLengthArray(array);

    for (size_t i = 0; i < CSSPrimitiveValue::LengthUnitTypeCount; i++)
        result->set(i, InterpolableNumber::create(array.at(i)));

    return result.release();
}

namespace {

static CSSPrimitiveValue::UnitType toUnitType(int lengthUnitType)
{
    return static_cast<CSSPrimitiveValue::UnitType>(CSSPrimitiveValue::lengthUnitTypeToUnitType(static_cast<CSSPrimitiveValue::LengthUnitType>(lengthUnitType)));
}

static PassRefPtr<CSSCalcExpressionNode> constructCalcExpression(PassRefPtr<CSSCalcExpressionNode> previous, InterpolableList* list, size_t position)
{
    while (position != CSSPrimitiveValue::LengthUnitTypeCount) {
        const InterpolableNumber *subValue = toInterpolableNumber(list->get(position));
        if (subValue->value()) {
            RefPtr<CSSCalcExpressionNode> next;
            if (previous)
                next = CSSCalcValue::createExpressionNode(previous, CSSCalcValue::createExpressionNode(CSSPrimitiveValue::create(subValue->value(), toUnitType(position))), CalcAdd);
            else
                next = CSSCalcValue::createExpressionNode(CSSPrimitiveValue::create(subValue->value(), toUnitType(position)));
            return constructCalcExpression(next, list, position + 1);
        }
        position++;
    }
    return previous;
}

}

PassRefPtr<CSSValue> LengthStyleInterpolation::interpolableValueToLength(InterpolableValue* value, ValueRange range)
{
    InterpolableList* listValue = toInterpolableList(value);
    unsigned unitCount = 0;
    for (size_t i = 0; i < CSSPrimitiveValue::LengthUnitTypeCount; i++) {
        const InterpolableNumber* subValue = toInterpolableNumber(listValue->get(i));
        if (subValue->value()) {
            unitCount++;
        }
    }

    switch (unitCount) {
    case 0:
        return CSSPrimitiveValue::create(0, CSSPrimitiveValue::CSS_PX);
    case 1:
        for (size_t i = 0; i < CSSPrimitiveValue::LengthUnitTypeCount; i++) {
            const InterpolableNumber* subValue = toInterpolableNumber(listValue->get(i));
            double value = subValue->value();
            if (value) {
                if (range == ValueRangeNonNegative && value < 0)
                    value = 0;
                return CSSPrimitiveValue::create(value, toUnitType(i));
            }
        }
        ASSERT_NOT_REACHED();
    default:
        return CSSPrimitiveValue::create(CSSCalcValue::create(constructCalcExpression(nullptr, listValue, 0), range));
    }
}

void LengthStyleInterpolation::apply(StyleResolverState& state) const
{
    StyleBuilder::applyProperty(m_id, state, interpolableValueToLength(m_cachedValue.get(), m_range).get());
}

}
