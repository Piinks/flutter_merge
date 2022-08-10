// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/entity/contents/filters/color_matrix_filter_contents.h"

#include <optional>

#include "impeller/entity/contents/content_context.h"
#include "impeller/entity/contents/contents.h"
#include "impeller/geometry/point.h"
#include "impeller/geometry/vector.h"
#include "impeller/renderer/render_pass.h"
#include "impeller/renderer/sampler_library.h"

namespace impeller {

ColorMatrixFilterContents::ColorMatrixFilterContents() = default;

ColorMatrixFilterContents::~ColorMatrixFilterContents() = default;

void ColorMatrixFilterContents::SetMatrix(const ColorMatrix& matrix) {
  matrix_ = matrix;
}

bool ColorMatrixFilterContents::RenderFilter(const FilterInput::Vector& inputs,
                                             const ContentContext& renderer,
                                             const Entity& entity,
                                             RenderPass& pass,
                                             const Rect& coverage) const {
  if (inputs.empty()) {
    return true;
  }

  using VS = ColorMatrixColorFilterPipeline::VertexShader;
  using FS = ColorMatrixColorFilterPipeline::FragmentShader;

  auto input_snapshot = inputs[0]->GetSnapshot(renderer, entity);
  if (!input_snapshot.has_value()) {
    return true;
  }

  Command cmd;
  cmd.label = "Color Matrix Filter";

  auto options = OptionsFromPass(pass);
  options.blend_mode = Entity::BlendMode::kSource;
  cmd.pipeline = renderer.GetColorMatrixColorFilterPipeline(options);

  VertexBufferBuilder<VS::PerVertexData> vtx_builder;
  vtx_builder.AddVertices({
      {Point(0, 0)},
      {Point(1, 0)},
      {Point(1, 1)},
      {Point(0, 0)},
      {Point(1, 1)},
      {Point(0, 1)},
  });
  auto& host_buffer = pass.GetTransientsBuffer();
  auto vtx_buffer = vtx_builder.CreateVertexBuffer(host_buffer);
  cmd.BindVertices(vtx_buffer);

  VS::FrameInfo frame_info;
  frame_info.mvp = Matrix::MakeOrthographic(ISize(1, 1));

  FS::FragInfo frag_info;
  const float* matrix = matrix_.array;
  frag_info.color_v = Vector4(matrix[4], matrix[9], matrix[14], matrix[19]);
  // clang-format off
  frag_info.color_m =
      Matrix(
        matrix[ 0], matrix[ 1], matrix[ 2], matrix[ 3],
        matrix[ 5], matrix[ 6], matrix[ 7], matrix[ 8],
        matrix[10], matrix[11], matrix[12], matrix[13],
        matrix[15], matrix[16], matrix[17], matrix[18]
      );
  // clang-format on

  auto sampler = renderer.GetContext()->GetSamplerLibrary()->GetSampler({});
  FS::BindInputTexture(cmd, input_snapshot->texture, sampler);
  FS::BindFragInfo(cmd, host_buffer.EmplaceUniform(frag_info));

  VS::BindFrameInfo(cmd, host_buffer.EmplaceUniform(frame_info));

  return pass.AddCommand(std::move(cmd));
}

}  // namespace impeller
