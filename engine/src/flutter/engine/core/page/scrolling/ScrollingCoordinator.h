/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ScrollingCoordinator_h
#define ScrollingCoordinator_h

#include "core/rendering/RenderObject.h"
#include "platform/PlatformWheelEvent.h"
#include "platform/geometry/IntRect.h"
#include "wtf/text/WTFString.h"

namespace blink {
class WebLayer;
class WebScrollbarLayer;
}

namespace blink {

class Document;
class LocalFrame;
class FrameView;
class GraphicsLayer;
class Page;
class Region;
class ScrollableArea;

class ScrollingCoordinator {
public:
    ~ScrollingCoordinator();

    static PassOwnPtr<ScrollingCoordinator> create(Page*);

    void willBeDestroyed();

#if OS(MACOSX)
    // Dispatched by the scrolling tree during handleWheelEvent. This is required as long as scrollbars are painted on the main thread.
    void handleWheelEventPhase(PlatformWheelEventPhase);
#endif

    PassOwnPtr<blink::WebScrollbarLayer> createSolidColorScrollbarLayer(ScrollbarOrientation, int thumbThickness, int trackStart, bool isLeftSideVerticalScrollbar);

    void willDestroyScrollableArea(ScrollableArea*);
    // Returns true if the coordinator handled this change.
    bool scrollableAreaScrollLayerDidChange(ScrollableArea*);
    void scrollableAreaScrollbarLayerDidChange(ScrollableArea*, ScrollbarOrientation);
    void willDestroyRenderLayer(RenderLayer*);

    void updateScrollParentForGraphicsLayer(GraphicsLayer* child, RenderLayer* parent);
    void updateClipParentForGraphicsLayer(GraphicsLayer* child, RenderLayer* parent);

    // For testing purposes only. This ScrollingCoordinator is reused between layout test, and must be reset
    // for the results to be valid.
    void reset();

protected:
    explicit ScrollingCoordinator(Page*);

    bool isForMainFrame(ScrollableArea*) const;

    Page* m_page;

private:
    blink::WebScrollbarLayer* addWebScrollbarLayer(ScrollableArea*, ScrollbarOrientation, PassOwnPtr<blink::WebScrollbarLayer>);
    blink::WebScrollbarLayer* getWebScrollbarLayer(ScrollableArea*, ScrollbarOrientation);
    void removeWebScrollbarLayer(ScrollableArea*, ScrollbarOrientation);

    typedef HashMap<ScrollableArea*, OwnPtr<blink::WebScrollbarLayer> > ScrollbarMap;
    ScrollbarMap m_horizontalScrollbars;
    ScrollbarMap m_verticalScrollbars;
    HashSet<const RenderLayer*> m_layersWithTouchRects;
};

} // namespace blink

#endif // ScrollingCoordinator_h
