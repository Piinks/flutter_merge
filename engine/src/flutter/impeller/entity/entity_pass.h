// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "flutter/fml/macros.h"
#include "impeller/entity/contents/contents.h"
#include "impeller/entity/entity.h"
#include "impeller/entity/entity_pass_delegate.h"
#include "impeller/renderer/render_target.h"
#include "impeller/typographer/lazy_glyph_atlas.h"

namespace impeller {

class ContentContext;

class EntityPass {
 public:
  using Entities = std::vector<Entity>;
  using Subpasses = std::vector<std::unique_ptr<EntityPass>>;

  EntityPass();

  ~EntityPass();

  void SetDelegate(std::unique_ptr<EntityPassDelegate> delgate);

  size_t GetSubpassesDepth() const;

  std::unique_ptr<EntityPass> Clone() const;

  void AddEntity(Entity entity);

  void SetEntities(Entities entities);

  const std::vector<Entity>& GetEntities() const;

  const Subpasses& GetSubpasses() const;

  const std::shared_ptr<LazyGlyphAtlas>& GetLazyGlyphAtlas() const;

  EntityPass* AddSubpass(std::unique_ptr<EntityPass> pass);

  EntityPass* GetSuperpass() const;

  bool Render(ContentContext& renderer,
              RenderPass& parent_pass,
              Point position = Point()) const;

  void IterateAllEntities(std::function<bool(Entity&)> iterator);

  void SetTransformation(Matrix xformation);

  void SetStencilDepth(size_t stencil_depth);

  void SetBlendMode(Entity::BlendMode blend_mode);

 private:
  Entities entities_;
  Subpasses subpasses_;
  EntityPass* superpass_ = nullptr;
  Matrix xformation_;
  size_t stencil_depth_ = 0u;
  Entity::BlendMode blend_mode_ = Entity::BlendMode::kSourceOver;
  std::unique_ptr<EntityPassDelegate> delegate_ =
      EntityPassDelegate::MakeDefault();
  std::shared_ptr<LazyGlyphAtlas> lazy_glyph_atlas_ =
      std::make_shared<LazyGlyphAtlas>();

  std::optional<Rect> GetSubpassCoverage(const EntityPass& subpass) const;

  std::optional<Rect> GetEntitiesCoverage() const;

  FML_DISALLOW_COPY_AND_ASSIGN(EntityPass);
};

struct CanvasStackEntry {
  Matrix xformation;
  size_t stencil_depth = 0u;
  bool is_subpass = false;
  bool contains_clips = false;
};

}  // namespace impeller
