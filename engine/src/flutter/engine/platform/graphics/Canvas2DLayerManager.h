/*
Copyright (C) 2012 Google Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SKY_ENGINE_PLATFORM_GRAPHICS_CANVAS2DLAYERMANAGER_H_
#define SKY_ENGINE_PLATFORM_GRAPHICS_CANVAS2DLAYERMANAGER_H_

#include "sky/engine/platform/Timer.h"
#include "sky/engine/platform/graphics/Canvas2DLayerBridge.h"

class Canvas2DLayerManagerTest;

namespace blink {

class PLATFORM_EXPORT Canvas2DLayerManager {
public:
    static Canvas2DLayerManager& get();

    void init(size_t maxBytesAllocated, size_t targetBytesAllocated);
    virtual ~Canvas2DLayerManager();

    void layerTransientResourceAllocationChanged(Canvas2DLayerBridge*, intptr_t deltaBytes = 0);
    void layerDidDraw(Canvas2DLayerBridge*);

    bool isInList(Canvas2DLayerBridge*) const;
private:
    Canvas2DLayerManager();

    // internal methods
    void freeMemoryIfNecessary();
    void addLayerToList(Canvas2DLayerBridge*);

    void limitPendingFramesTimerFired(Timer<Canvas2DLayerManager>*);

    size_t m_bytesAllocated;
    size_t m_maxBytesAllocated;
    size_t m_targetBytesAllocated;
    Timer<Canvas2DLayerManager> m_limitPendingFramesTimer;
    DoublyLinkedList<Canvas2DLayerBridge> m_layerList;

    friend class ::Canvas2DLayerManagerTest; // for unit testing
};

} // namespace blink

#endif  // SKY_ENGINE_PLATFORM_GRAPHICS_CANVAS2DLAYERMANAGER_H_

