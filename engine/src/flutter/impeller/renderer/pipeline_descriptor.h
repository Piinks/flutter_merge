// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include "flutter/fml/hash_combine.h"
#include "flutter/fml/macros.h"
#include "impeller/renderer/comparable.h"
#include "impeller/renderer/formats.h"
#include "impeller/renderer/shader_types.h"

namespace impeller {

class ShaderFunction;
class VertexDescriptor;

class PipelineDescriptor final : public Comparable<PipelineDescriptor> {
 public:
  PipelineDescriptor();

  ~PipelineDescriptor();

  PipelineDescriptor& SetLabel(std::string label);

  const std::string& GetLabel() const { return label_; }

  PipelineDescriptor& SetSampleCount(size_t samples);

  size_t GetSampleCount() const { return sample_count_; }

  PipelineDescriptor& AddStageEntrypoint(
      std::shared_ptr<const ShaderFunction> function);

  const std::map<ShaderStage, std::shared_ptr<const ShaderFunction>>&
  GetStageEntrypoints() const {
    return entrypoints_;
  }

  PipelineDescriptor& SetVertexDescriptor(
      std::shared_ptr<VertexDescriptor> vertex_descriptor);

  const std::shared_ptr<VertexDescriptor>& GetVertexDescriptor() const {
    return vertex_descriptor_;
  }

  PipelineDescriptor& SetColorAttachmentDescriptor(
      size_t index,
      ColorAttachmentDescriptor desc);

  const ColorAttachmentDescriptor* GetColorAttachmentDescriptor(
      size_t index) const;

  const std::map<size_t /* index */, ColorAttachmentDescriptor>
  GetColorAttachmentDescriptors() const {
    return color_attachment_descriptors_;
  }

  PipelineDescriptor& SetDepthStencilAttachmentDescriptor(
      DepthAttachmentDescriptor desc);

  std::optional<DepthAttachmentDescriptor> GetDepthStencilAttachmentDescriptor()
      const {
    return depth_attachment_descriptor_;
  }

  PipelineDescriptor& SetStencilAttachmentDescriptors(
      StencilAttachmentDescriptor front_and_back);

  PipelineDescriptor& SetStencilAttachmentDescriptors(
      StencilAttachmentDescriptor front,
      StencilAttachmentDescriptor back);

  std::optional<StencilAttachmentDescriptor>
  GetFrontStencilAttachmentDescriptor() const {
    return front_stencil_attachment_descriptor_;
  }

  std::optional<StencilAttachmentDescriptor>
  GetBackStencilAttachmentDescriptor() const {
    return back_stencil_attachment_descriptor_;
  }

  PipelineDescriptor& SetDepthPixelFormat(PixelFormat format);

  PixelFormat GetDepthPixelFormat() const { return depth_pixel_format_; }

  PipelineDescriptor& SetStencilPixelFormat(PixelFormat format);

  PixelFormat GetStencilPixelFormat() const { return stencil_pixel_format_; }

  // Comparable<PipelineDescriptor>
  std::size_t GetHash() const override;

  // Comparable<PipelineDescriptor>
  bool IsEqual(const PipelineDescriptor& other) const override;

 private:
  std::string label_;
  size_t sample_count_ = 1;
  std::map<ShaderStage, std::shared_ptr<const ShaderFunction>> entrypoints_;
  std::map<size_t /* index */, ColorAttachmentDescriptor>
      color_attachment_descriptors_;
  std::shared_ptr<VertexDescriptor> vertex_descriptor_;
  PixelFormat depth_pixel_format_ = PixelFormat::kUnknown;
  PixelFormat stencil_pixel_format_ = PixelFormat::kUnknown;
  std::optional<DepthAttachmentDescriptor> depth_attachment_descriptor_;
  std::optional<StencilAttachmentDescriptor>
      front_stencil_attachment_descriptor_;
  std::optional<StencilAttachmentDescriptor>
      back_stencil_attachment_descriptor_;
};

}  // namespace impeller
