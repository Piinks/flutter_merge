/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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
 *
 */

#ifndef SKY_ENGINE_CORE_DOM_CONTEXTLIFECYCLEOBSERVER_H_
#define SKY_ENGINE_CORE_DOM_CONTEXTLIFECYCLEOBSERVER_H_

#include "sky/engine/platform/LifecycleContext.h"

namespace blink {

class ExecutionContext;

template<> void observerContext(ExecutionContext*, LifecycleObserver<ExecutionContext>*);
template<> void unobserverContext(ExecutionContext*, LifecycleObserver<ExecutionContext>*);

class ContextLifecycleObserver : public LifecycleObserver<ExecutionContext> {
public:
    explicit ContextLifecycleObserver(ExecutionContext*, Type = GenericType);
    ExecutionContext* executionContext() const { return lifecycleContext(); }
protected:
    virtual ~ContextLifecycleObserver();
};

} // namespace blink

#endif  // SKY_ENGINE_CORE_DOM_CONTEXTLIFECYCLEOBSERVER_H_
