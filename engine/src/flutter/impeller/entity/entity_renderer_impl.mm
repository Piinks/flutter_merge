// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/entity/entity_renderer_impl.h"

#include "flutter/fml/trace_event.h"
#include "impeller/renderer/tessellator.h"
#include "impeller/renderer/vertex_buffer_builder.h"

namespace impeller {

EntityRendererImpl::EntityRendererImpl(std::shared_ptr<Context> context)
    : context_(std::move(context)) {
  if (!context_ || !context_->IsValid()) {
    return;
  }

  content_renderer_ = std::make_unique<ContentRenderer>(context_);
  if (!content_renderer_->IsValid()) {
    return;
  }

  solid_fill_pipeline_ = std::make_unique<SolidFillPipeline>(*context_);

  is_valid_ = true;
}

EntityRendererImpl::~EntityRendererImpl() = default;

bool EntityRendererImpl::IsValid() const {
  return is_valid_;
}

EntityRendererImpl::RenderResult EntityRendererImpl::RenderEntity(
    const Surface& surface,
    RenderPass& pass,
    const Entity& entity) {
  if (!entity.HasRenderableContents()) {
    return RenderResult::kSkipped;
  }

  if (entity.HasContents() && !entity.IsClip() && !entity.GetContents()) {
    using VS = SolidFillPipeline::VertexShader;

    Command cmd;
    cmd.label = "SolidFill";
    cmd.pipeline = solid_fill_pipeline_->WaitAndGet();
    if (cmd.pipeline == nullptr) {
      return RenderResult::kFailure;
    }

    VertexBufferBuilder<VS::PerVertexData> vtx_builder;
    {
      auto tesselation_result = Tessellator{}.Tessellate(
          entity.GetPath().SubdivideAdaptively(), [&vtx_builder](auto point) {
            VS::PerVertexData vtx;
            vtx.vertices = point;
            vtx_builder.AppendVertex(vtx);
          });
      if (!tesselation_result) {
        return RenderResult::kFailure;
      }
    }

    cmd.BindVertices(
        vtx_builder.CreateVertexBuffer(*context_->GetPermanentsAllocator()));

    VS::FrameInfo frame_info;
    frame_info.mvp = Matrix::MakeOrthographic(surface.GetSize()) *
                     entity.GetTransformation();
    frame_info.color = entity.GetBackgroundColor();
    VS::BindFrameInfo(cmd,
                      pass.GetTransientsBuffer().EmplaceUniform(frame_info));

    cmd.primitive_type = PrimitiveType::kTriangle;

    if (!pass.AddCommand(std::move(cmd))) {
      return RenderResult::kFailure;
    }
  } else if (entity.GetContents()) {
    auto result =
        entity.GetContents()->Render(*content_renderer_, entity, surface, pass);
    if (!result) {
      return RenderResult::kFailure;
    }
  }

  return RenderResult::kSuccess;
}

}  // namespace impeller
