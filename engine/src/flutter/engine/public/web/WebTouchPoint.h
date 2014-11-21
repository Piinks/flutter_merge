/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef SKY_ENGINE_PUBLIC_WEB_WEBTOUCHPOINT_H_
#define SKY_ENGINE_PUBLIC_WEB_WEBTOUCHPOINT_H_

#include "../platform/WebCommon.h"
#include "../platform/WebFloatPoint.h"

namespace blink {

class WebTouchPoint {
public:
    WebTouchPoint()
        : id(0)
        , state(StateUndefined)
        , radiusX(0)
        , radiusY(0)
        , rotationAngle(0)
        , force(0)
    {
    }

    enum State {
        StateUndefined,
        StateReleased,
        StatePressed,
        StateMoved,
        StateStationary,
        StateCancelled,
    };

    int id;
    State state;
    WebFloatPoint screenPosition;
    WebFloatPoint position;

    float radiusX;
    float radiusY;
    float rotationAngle;
    float force;
};

} // namespace blink

#endif  // SKY_ENGINE_PUBLIC_WEB_WEBTOUCHPOINT_H_
