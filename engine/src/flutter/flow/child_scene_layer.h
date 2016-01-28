// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLOW_CHILD_SCENE_LAYER_H_
#define FLOW_CHILD_SCENE_LAYER_H_

#include "flow/layer.h"
#include "mojo/services/gfx/composition/interfaces/scenes.mojom.h"

namespace flow {

class ChildSceneLayer : public Layer {
 public:
  ChildSceneLayer();
  ~ChildSceneLayer() override;

  void set_offset(const SkPoint& offset) { offset_ = offset; }

  void set_physical_size(const SkISize& physical_size) {
    physical_size_ = physical_size;
  }

  void set_scene_token(mojo::gfx::composition::SceneTokenPtr scene_token) {
    scene_token_ = scene_token.Pass();
  }

  void Paint(PaintContext::ScopedFrame& frame) override;

 private:
  SkPoint offset_;
  SkISize physical_size_;
  mojo::gfx::composition::SceneTokenPtr scene_token_;

  DISALLOW_COPY_AND_ASSIGN(ChildSceneLayer);
};

}  // namespace flow

#endif  // FLOW_CHILD_SCENE_LAYER_H_
