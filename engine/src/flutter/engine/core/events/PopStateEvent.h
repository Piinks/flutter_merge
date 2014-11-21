/*
 * Copyright (C) 2009 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE, INC. ``AS IS'' AND ANY
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

#ifndef SKY_ENGINE_CORE_EVENTS_POPSTATEEVENT_H_
#define SKY_ENGINE_CORE_EVENTS_POPSTATEEVENT_H_

#include "sky/engine/core/events/Event.h"
#include "sky/engine/platform/heap/Handle.h"

namespace blink {

class History;
class SerializedScriptValue;

typedef EventInit PopStateEventInit;

class PopStateEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    virtual ~PopStateEvent();
    static PassRefPtr<PopStateEvent> create();
    static PassRefPtr<PopStateEvent> create(PassRefPtr<SerializedScriptValue>, PassRefPtr<History>);
    static PassRefPtr<PopStateEvent> create(const AtomicString&, const PopStateEventInit&);

    SerializedScriptValue* serializedState() const { return m_serializedState.get(); }
    void setSerializedState(PassRefPtr<SerializedScriptValue> state)
    {
        ASSERT(!m_serializedState);
        m_serializedState = state;
    }
    History* history() const { return m_history.get(); }

    virtual const AtomicString& interfaceName() const override;

private:
    PopStateEvent();
    PopStateEvent(const AtomicString&, const PopStateEventInit&);
    PopStateEvent(PassRefPtr<SerializedScriptValue>, PassRefPtr<History>);

    RefPtr<SerializedScriptValue> m_serializedState;
    RefPtr<History> m_history;
};

} // namespace blink

#endif  // SKY_ENGINE_CORE_EVENTS_POPSTATEEVENT_H_
