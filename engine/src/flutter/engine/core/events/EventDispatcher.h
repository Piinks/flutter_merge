/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef SKY_ENGINE_CORE_EVENTS_EVENTDISPATCHER_H_
#define SKY_ENGINE_CORE_EVENTS_EVENTDISPATCHER_H_

#include "sky/engine/core/dom/SimulatedClickOptions.h"
#include "sky/engine/platform/heap/Handle.h"
#include "sky/engine/wtf/PassRefPtr.h"
#include "sky/engine/wtf/RefPtr.h"

namespace blink {

class Event;
class EventDispatchMediator;
class FrameView;
class Node;
class NodeEventContext;
class WindowEventContext;

enum EventDispatchContinuation {
    ContinueDispatching,
    DoneDispatching
};

class EventDispatcher {
    STACK_ALLOCATED();
public:
    static bool dispatchEvent(Node*, PassRefPtr<EventDispatchMediator>);
    static void dispatchScopedEvent(Node*, PassRefPtr<EventDispatchMediator>);

    static void dispatchSimulatedClick(Node*, Event* underlyingEvent, SimulatedClickMouseEventOptions);

    bool dispatch();
    Node* node() const { return m_node.get(); }
    Event* event() const { return m_event.get(); }

private:
    EventDispatcher(Node*, PassRefPtr<Event>);
    const NodeEventContext* topNodeEventContext();

    EventDispatchContinuation dispatchEventPreProcess(void*& preDispatchEventHandlerResult);
    EventDispatchContinuation dispatchEventAtCapturing(WindowEventContext&);
    EventDispatchContinuation dispatchEventAtTarget();
    void dispatchEventAtBubbling(WindowEventContext&);
    void dispatchEventPostProcess(void* preDispatchEventHandlerResult);

    RefPtr<Node> m_node;
    RefPtr<Event> m_event;
    RefPtr<FrameView> m_view;
#if ENABLE(ASSERT)
    bool m_eventDispatched;
#endif
};

}

#endif  // SKY_ENGINE_CORE_EVENTS_EVENTDISPATCHER_H_
