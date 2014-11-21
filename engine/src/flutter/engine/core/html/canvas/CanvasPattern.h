/*
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
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

#ifndef SKY_ENGINE_CORE_HTML_CANVAS_CANVASPATTERN_H_
#define SKY_ENGINE_CORE_HTML_CANVAS_CANVASPATTERN_H_

#include "sky/engine/bindings/core/v8/ScriptWrappable.h"
#include "sky/engine/platform/graphics/Pattern.h"
#include "sky/engine/wtf/Forward.h"
#include "sky/engine/wtf/PassRefPtr.h"
#include "sky/engine/wtf/RefCounted.h"

namespace blink {

class ExceptionState;
class Image;

class CanvasPattern final : public RefCounted<CanvasPattern>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static Pattern::RepeatMode parseRepetitionType(const String&, ExceptionState&);

    static PassRefPtr<CanvasPattern> create(PassRefPtr<Image> image, Pattern::RepeatMode repeat)
    {
        return adoptRef(new CanvasPattern(image, repeat));
    }

    Pattern* pattern() const { return m_pattern.get(); }

private:
    CanvasPattern(PassRefPtr<Image>, Pattern::RepeatMode);

    RefPtr<Pattern> m_pattern;
};

} // namespace blink

#endif  // SKY_ENGINE_CORE_HTML_CANVAS_CANVASPATTERN_H_
