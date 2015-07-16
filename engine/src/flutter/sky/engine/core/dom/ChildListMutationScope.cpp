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

#include "sky/engine/core/dom/ChildListMutationScope.h"

#include "sky/engine/core/dom/MutationObserverInterestGroup.h"
#include "sky/engine/core/dom/MutationRecord.h"
#include "sky/engine/core/dom/StaticNodeList.h"
#include "sky/engine/wtf/HashMap.h"
#include "sky/engine/wtf/StdLibExtras.h"

namespace blink {

// The accumulator map is used to make sure that there is only one mutation
// accumulator for a given node even if there are multiple ChildListMutationScopes
// on the stack. The map is always empty when there are no ChildListMutationScopes
// on the stack.
typedef HashMap<RawPtr<Node>, RawPtr<ChildListMutationAccumulator> > AccumulatorMap;

static AccumulatorMap& accumulatorMap()
{
    DEFINE_STATIC_LOCAL(OwnPtr<AccumulatorMap>, map, (adoptPtr(new AccumulatorMap())));
    return *map;
}

ChildListMutationAccumulator::ChildListMutationAccumulator(PassRefPtr<Node> target, PassOwnPtr<MutationObserverInterestGroup> observers)
    : m_target(target)
    , m_lastAdded(nullptr)
    , m_observers(observers)
    , m_mutationScopes(0)
{
}

void ChildListMutationAccumulator::leaveMutationScope()
{
    ASSERT(m_mutationScopes > 0);
    if (!--m_mutationScopes) {
        if (!isEmpty())
            enqueueMutationRecord();
        accumulatorMap().remove(m_target.get());
    }
}

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(ChildListMutationAccumulator);

PassRefPtr<ChildListMutationAccumulator> ChildListMutationAccumulator::getOrCreate(Node& target)
{
    AccumulatorMap::AddResult result = accumulatorMap().add(&target, nullptr);
    RefPtr<ChildListMutationAccumulator> accumulator;
    if (!result.isNewEntry)
        accumulator = result.storedValue->value;
    else {
        accumulator = adoptRef(new ChildListMutationAccumulator(PassRefPtr<Node>(target), MutationObserverInterestGroup::createForChildListMutation(target)));
        result.storedValue->value = accumulator.get();
    }
    return accumulator.release();
}

inline bool ChildListMutationAccumulator::isAddedNodeInOrder(Node* child)
{
    return isEmpty() || (m_lastAdded == child->previousSibling() && m_nextSibling == child->nextSibling());
}

void ChildListMutationAccumulator::childAdded(PassRefPtr<Node> prpChild)
{
    ASSERT(hasObservers());

    RefPtr<Node> child = prpChild;

    if (!isAddedNodeInOrder(child.get()))
        enqueueMutationRecord();

    if (isEmpty()) {
        m_previousSibling = child->previousSibling();
        m_nextSibling = child->nextSibling();
    }

    m_lastAdded = child.get();
    m_addedNodes.append(child.release());
}

inline bool ChildListMutationAccumulator::isRemovedNodeInOrder(Node* child)
{
    return isEmpty() || m_nextSibling == child;
}

void ChildListMutationAccumulator::willRemoveChild(PassRefPtr<Node> prpChild)
{
    ASSERT(hasObservers());

    RefPtr<Node> child = prpChild;

    if (!m_addedNodes.isEmpty() || !isRemovedNodeInOrder(child.get()))
        enqueueMutationRecord();

    if (isEmpty()) {
        m_previousSibling = child->previousSibling();
        m_nextSibling = child->nextSibling();
        m_lastAdded = child->previousSibling();
    } else
        m_nextSibling = child->nextSibling();

    m_removedNodes.append(child.release());
}

void ChildListMutationAccumulator::enqueueMutationRecord()
{
    ASSERT(hasObservers());
    ASSERT(!isEmpty());

    RefPtr<StaticNodeList> addedNodes = StaticNodeList::adopt(m_addedNodes);
    RefPtr<StaticNodeList> removedNodes = StaticNodeList::adopt(m_removedNodes);
    RefPtr<MutationRecord> record = MutationRecord::createChildList(m_target, addedNodes.release(), removedNodes.release(), m_previousSibling.release(), m_nextSibling.release());
    m_observers->enqueueMutationRecord(record.release());
    m_lastAdded = nullptr;
    ASSERT(isEmpty());
}

bool ChildListMutationAccumulator::isEmpty()
{
    bool result = m_removedNodes.isEmpty() && m_addedNodes.isEmpty();
#if ENABLE(ASSERT)
    if (result) {
        ASSERT(!m_previousSibling);
        ASSERT(!m_nextSibling);
        ASSERT(!m_lastAdded);
    }
#endif
    return result;
}

} // namespace blink
