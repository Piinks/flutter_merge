/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef SKY_ENGINE_CORE_DOM_MUTATIONOBSERVER_H_
#define SKY_ENGINE_CORE_DOM_MUTATIONOBSERVER_H_

#include "sky/engine/tonic/dart_wrappable.h"
#include "sky/engine/platform/heap/Handle.h"
#include "sky/engine/wtf/HashSet.h"
#include "sky/engine/wtf/PassOwnPtr.h"
#include "sky/engine/wtf/PassRefPtr.h"
#include "sky/engine/wtf/RefCounted.h"
#include "sky/engine/wtf/RefPtr.h"
#include "sky/engine/wtf/Vector.h"

namespace blink {

class ExceptionState;
class MutationCallback;
class MutationObserver;
class MutationObserverRegistration;
class MutationRecord;
class Node;

typedef unsigned char MutationObserverOptions;
typedef unsigned char MutationRecordDeliveryOptions;

typedef HashSet<RefPtr<MutationObserver> > MutationObserverSet;
typedef HashSet<RawPtr<MutationObserverRegistration> > MutationObserverRegistrationSet;
typedef Vector<RefPtr<MutationObserver> > MutationObserverVector;
typedef Vector<RefPtr<MutationRecord> > MutationRecordVector;

class MutationObserver final : public RefCounted<MutationObserver>, public DartWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    enum MutationType {
        ChildList = 1 << 0,
        Attributes = 1 << 1,
        CharacterData = 1 << 2,

        AllMutationTypes = ChildList | Attributes | CharacterData
    };

    enum ObservationFlags  {
        Subtree = 1 << 3,
        AttributeFilter = 1 << 4
    };

    enum DeliveryFlags {
        AttributeOldValue = 1 << 5,
        CharacterDataOldValue = 1 << 6,
    };

    static PassRefPtr<MutationObserver> create(PassOwnPtr<MutationCallback>);
    static void resumeSuspendedObservers();
    static void deliverMutations();

    ~MutationObserver();

    void observe(Node*, ExceptionState&);
    Vector<RefPtr<MutationRecord> > takeRecords();
    void disconnect();
    void observationStarted(MutationObserverRegistration*);
    void observationEnded(MutationObserverRegistration*);
    void enqueueMutationRecord(PassRefPtr<MutationRecord>);
    void setHasTransientRegistration();
    bool canDeliver();

    HashSet<RawPtr<Node> > getObservedNodes() const;

private:
    struct ObserverLessThan;

    explicit MutationObserver(PassOwnPtr<MutationCallback>);
    void deliver();

    OwnPtr<MutationCallback> m_callback;
    MutationRecordVector m_records;
    MutationObserverRegistrationSet m_registrations;
    unsigned m_priority;
};

} // namespace blink

#endif  // SKY_ENGINE_CORE_DOM_MUTATIONOBSERVER_H_
