/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SKY_ENGINE_PUBLIC_WEB_WEBACTIVEWHEELFLINGPARAMETERS_H_
#define SKY_ENGINE_PUBLIC_WEB_WEBACTIVEWHEELFLINGPARAMETERS_H_

#include "../platform/WebCommon.h"
#include "../platform/WebFloatPoint.h"
#include "../platform/WebGestureDevice.h"
#include "../platform/WebInputEvent.h"
#include "../platform/WebPoint.h"
#include "../platform/WebSize.h"

namespace blink {

struct WebActiveWheelFlingParameters {
    WebFloatPoint delta;
    WebPoint point;
    WebPoint globalPoint;
    int modifiers;
    WebGestureDevice sourceDevice;
    WebSize cumulativeScroll;
    double startTime;

    WebActiveWheelFlingParameters()
        : modifiers(0)
        , sourceDevice(WebGestureDeviceTouchpad)
        , startTime(0)
    {
    }
};

}

#endif  // SKY_ENGINE_PUBLIC_WEB_WEBACTIVEWHEELFLINGPARAMETERS_H_
