// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_ENGINE_CORE_RENDERING_COMPOSITING_COMPOSITINGSTATE_H_
#define SKY_ENGINE_CORE_RENDERING_COMPOSITING_COMPOSITINGSTATE_H_

namespace blink {

enum CompositingState {
    // The layer paints into its enclosing composited ancestor.
    NotComposited = 0,

    // The layer is composited, but its contents still paint into enclosing composited ancestor.
    // In this state, paint invalidations must be sent to the enclosing composited ancestor.
    // Typically this happens when a layer's properties need to be represented in the compositor
    // output data structures, but it doesn't actually have any other reasons to be composited.
    HasOwnBackingButPaintsIntoAncestor = 1,

    PaintsIntoOwnBacking = 2,

    // In this state, the RenderLayer subtree paints into a backing that is shared by
    // several RenderLayer subtrees.
    PaintsIntoGroupedBacking = 3
};

} // namespace blink

#endif  // SKY_ENGINE_CORE_RENDERING_COMPOSITING_COMPOSITINGSTATE_H_
