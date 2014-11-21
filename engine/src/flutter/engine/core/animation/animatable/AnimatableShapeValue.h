/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef SKY_ENGINE_CORE_ANIMATION_ANIMATABLE_ANIMATABLESHAPEVALUE_H_
#define SKY_ENGINE_CORE_ANIMATION_ANIMATABLE_ANIMATABLESHAPEVALUE_H_

#include "sky/engine/core/animation/animatable/AnimatableValue.h"
#include "sky/engine/core/rendering/style/ShapeValue.h"

namespace blink {

class AnimatableShapeValue final : public AnimatableValue {
public:
    virtual ~AnimatableShapeValue() { }
    static PassRefPtr<AnimatableShapeValue> create(ShapeValue* shape)
    {
        return adoptRef(new AnimatableShapeValue(shape));
    }
    ShapeValue* shapeValue() const { return m_shape.get(); }

protected:
    virtual PassRefPtr<AnimatableValue> interpolateTo(const AnimatableValue*, double fraction) const override;
    virtual bool usesDefaultInterpolationWith(const AnimatableValue*) const override;

private:
    AnimatableShapeValue(ShapeValue* shape)
        : m_shape(shape)
    {
        ASSERT(m_shape);
    }
    virtual AnimatableType type() const override { return TypeShapeValue; }
    virtual bool equalTo(const AnimatableValue*) const override;

    RefPtr<ShapeValue> m_shape;
};

DEFINE_ANIMATABLE_VALUE_TYPE_CASTS(AnimatableShapeValue, isShapeValue());

} // namespace blink

#endif  // SKY_ENGINE_CORE_ANIMATION_ANIMATABLE_ANIMATABLESHAPEVALUE_H_
