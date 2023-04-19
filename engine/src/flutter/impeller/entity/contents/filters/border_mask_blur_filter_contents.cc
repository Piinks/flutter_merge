// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/entity/contents/filters/border_mask_blur_filter_contents.h"
#include "impeller/entity/contents/content_context.h"

#include "impeller/entity/contents/anonymous_contents.h"
#include "impeller/entity/contents/contents.h"
#include "impeller/renderer/render_pass.h"
#include "impeller/renderer/sampler_library.h"

namespace impeller {

BorderMaskBlurFilterContents::BorderMaskBlurFilterContents() = default;

BorderMaskBlurFilterContents::~BorderMaskBlurFilterContents() = default;

void BorderMaskBlurFilterContents::SetSigma(Sigma sigma_x, Sigma sigma_y) {
  sigma_x_ = sigma_x;
  sigma_y_ = sigma_y;
}

void BorderMaskBlurFilterContents::SetBlurStyle(BlurStyle blur_style) {
  blur_style_ = blur_style;

  switch (blur_style) {
    case FilterContents::BlurStyle::kNormal:
      src_color_factor_ = false;
      inner_blur_factor_ = true;
      outer_blur_factor_ = true;
      break;
    case FilterContents::BlurStyle::kSolid:
      src_color_factor_ = true;
      inner_blur_factor_ = false;
      outer_blur_factor_ = true;
      break;
    case FilterContents::BlurStyle::kOuter:
      src_color_factor_ = false;
      inner_blur_factor_ = false;
      outer_blur_factor_ = true;
      break;
    case FilterContents::BlurStyle::kInner:
      src_color_factor_ = false;
      inner_blur_factor_ = true;
      outer_blur_factor_ = false;
      break;
  }
}

std::optional<Entity> BorderMaskBlurFilterContents::RenderFilter(
    const FilterInput::Vector& inputs,
    const ContentContext& renderer,
    const Entity& entity,
    const Matrix& effect_transform,
    const Rect& coverage) const {
  using VS = BorderMaskBlurPipeline::VertexShader;
  using FS = BorderMaskBlurPipeline::FragmentShader;

  //----------------------------------------------------------------------------
  /// Handle inputs.
  ///

  if (inputs.empty()) {
    return std::nullopt;
  }

  auto input_snapshot = inputs[0]->GetSnapshot(renderer, entity);
  if (!input_snapshot.has_value()) {
    return std::nullopt;
  }

  auto maybe_input_uvs = input_snapshot->GetCoverageUVs(coverage);
  if (!maybe_input_uvs.has_value()) {
    return std::nullopt;
  }
  auto input_uvs = maybe_input_uvs.value();

  //----------------------------------------------------------------------------
  /// Create AnonymousContents for rendering.
  ///

  auto sigma = effect_transform * Vector2(sigma_x_.sigma, sigma_y_.sigma);
  RenderProc render_proc = [coverage, input_snapshot, input_uvs = input_uvs,
                            src_color_factor = src_color_factor_,
                            inner_blur_factor = inner_blur_factor_,
                            outer_blur_factor = outer_blur_factor_, sigma](
                               const ContentContext& renderer,
                               const Entity& entity, RenderPass& pass) -> bool {
    auto& host_buffer = pass.GetTransientsBuffer();

    VertexBufferBuilder<VS::PerVertexData> vtx_builder;
    vtx_builder.AddVertices({
        {coverage.origin, input_uvs[0]},
        {{coverage.origin.x + coverage.size.width, coverage.origin.y},
         input_uvs[1]},
        {{coverage.origin.x + coverage.size.width,
          coverage.origin.y + coverage.size.height},
         input_uvs[3]},
        {coverage.origin, input_uvs[0]},
        {{coverage.origin.x + coverage.size.width,
          coverage.origin.y + coverage.size.height},
         input_uvs[3]},
        {{coverage.origin.x, coverage.origin.y + coverage.size.height},
         input_uvs[2]},
    });
    auto vtx_buffer = vtx_builder.CreateVertexBuffer(host_buffer);

    Command cmd;
    cmd.label = "Border Mask Blur Filter";
    auto options = OptionsFromPassAndEntity(pass, entity);

    cmd.pipeline = renderer.GetBorderMaskBlurPipeline(options);
    cmd.BindVertices(vtx_buffer);
    cmd.stencil_reference = entity.GetStencilDepth();

    VS::FrameInfo frame_info;
    frame_info.mvp = Matrix::MakeOrthographic(pass.GetRenderTargetSize()) *
                     entity.GetTransformation();
    frame_info.texture_sampler_y_coord_scale =
        input_snapshot->texture->GetYCoordScale();

    FS::FragInfo frag_info;
    frag_info.sigma_uv = sigma.Abs() / input_snapshot->texture->GetSize();
    frag_info.src_factor = src_color_factor;
    frag_info.inner_blur_factor = inner_blur_factor;
    frag_info.outer_blur_factor = outer_blur_factor;

    FS::BindFragInfo(cmd, host_buffer.EmplaceUniform(frag_info));
    VS::BindFrameInfo(cmd, host_buffer.EmplaceUniform(frame_info));

    auto sampler = renderer.GetContext()->GetSamplerLibrary()->GetSampler({});
    FS::BindTextureSampler(cmd, input_snapshot->texture, sampler);

    return pass.AddCommand(std::move(cmd));
  };

  CoverageProc coverage_proc =
      [coverage](const Entity& entity) -> std::optional<Rect> {
    return coverage.TransformBounds(entity.GetTransformation());
  };

  auto contents = AnonymousContents::Make(render_proc, coverage_proc);

  Entity sub_entity;
  sub_entity.SetContents(std::move(contents));
  sub_entity.SetStencilDepth(entity.GetStencilDepth());
  sub_entity.SetBlendMode(entity.GetBlendMode());
  return sub_entity;
}

std::optional<Rect> BorderMaskBlurFilterContents::GetFilterCoverage(
    const FilterInput::Vector& inputs,
    const Entity& entity,
    const Matrix& effect_transform) const {
  if (inputs.empty()) {
    return std::nullopt;
  }

  auto coverage = inputs[0]->GetCoverage(entity);
  if (!coverage.has_value()) {
    return std::nullopt;
  }
  auto transform = inputs[0]->GetTransform(entity) * effect_transform;
  auto transformed_blur_vector =
      transform.TransformDirection(Vector2(Radius{sigma_x_}.radius, 0)).Abs() +
      transform.TransformDirection(Vector2(0, Radius{sigma_y_}.radius)).Abs();
  auto extent = coverage->size + transformed_blur_vector * 2;
  return Rect(coverage->origin - transformed_blur_vector,
              Size(extent.x, extent.y));
}

}  // namespace impeller
