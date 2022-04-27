// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <initializer_list>
#include <map>
#include <vector>

#include "flutter/fml/macros.h"
#include "impeller/base/base.h"
#include "impeller/compositor/allocator.h"
#include "impeller/compositor/device_buffer.h"
#include "impeller/compositor/formats.h"
#include "impeller/compositor/host_buffer.h"
#include "impeller/compositor/vertex_buffer.h"
#include "impeller/geometry/vector.h"

namespace impeller {

template <class VertexType_, class IndexType_ = uint32_t>
class VertexBufferBuilder {
 public:
  using VertexType = VertexType_;
  using IndexType = IndexType_;

  VertexBufferBuilder() = default;

  ~VertexBufferBuilder() = default;

  void SetLabel(std::string label) { label_ = std::move(label); }

  void Reserve(size_t count) { return vertices_.reserve(count); }

  VertexBufferBuilder& AppendVertex(VertexType_ vertex) {
    vertices_.emplace_back(std::move(vertex));
    return *this;
  }

  VertexBufferBuilder& AddVertices(
      std::initializer_list<VertexType_> vertices) {
    vertices_.reserve(vertices.size());
    for (auto& vertex : vertices) {
      vertices_.emplace_back(std::move(vertex));
    }
    return *this;
  }

  VertexBuffer CreateVertexBuffer(HostBuffer& host_buffer) const {
    VertexBuffer buffer;
    buffer.vertex_buffer = CreateVertexBufferView(host_buffer);
    buffer.index_buffer = CreateIndexBufferView(host_buffer);
    buffer.index_count = GetIndexCount();
    return buffer;
  };

  VertexBuffer CreateVertexBuffer(Allocator& device_allocator) const {
    VertexBuffer buffer;
    // This can be merged into a single allocation.
    buffer.vertex_buffer = CreateVertexBufferView(device_allocator);
    buffer.index_buffer = CreateIndexBufferView(device_allocator);
    buffer.index_count = GetIndexCount();
    return buffer;
  };

 private:
  // This is a placeholder till vertex de-duplication can be implemented. The
  // current implementation is a very dumb placeholder.
  std::vector<VertexType> vertices_;
  std::string label_;

  BufferView CreateVertexBufferView(HostBuffer& buffer) const {
    auto view =
        buffer.Emplace(vertices_.data(), vertices_.size() * sizeof(VertexType),
                       alignof(VertexType));
    if (!label_.empty()) {
      view.SetLabel(SPrintF("%s Vertices"), label_.c_str());
    }
    return view;
  }

  BufferView CreateVertexBufferView(Allocator& allocator) const {
    auto buffer = allocator.CreateBufferWithCopy(
        reinterpret_cast<const uint8_t*>(vertices_.data()),
        vertices_.size() * sizeof(VertexType));
    if (!buffer) {
      return {};
    }
    if (!label_.empty()) {
      buffer->SetLabel(SPrintF("%s Vertices", label_.c_str()));
    }
    return buffer->AsBufferView();
  }

  std::vector<IndexType> CreateIndexBuffer() const {
    // So dumb! We don't actually need an index buffer right now. But we will
    // once de-duplication is done. So assume this is always done.
    std::vector<IndexType> index_buffer;
    for (size_t i = 0; i < vertices_.size(); i++) {
      index_buffer.push_back(i);
    }
    return index_buffer;
  }

  BufferView CreateIndexBufferView(HostBuffer& buffer) const {
    const auto index_buffer = CreateIndexBuffer();
    auto view = buffer.Emplace(index_buffer.data(),
                               index_buffer.size() * sizeof(IndexType),
                               alignof(IndexType));
    if (!label_.empty()) {
      view.SetLabel(SPrintF("%s Indices"), label_);
    }
    return view;
  }

  BufferView CreateIndexBufferView(Allocator& allocator) const {
    const auto index_buffer = CreateIndexBuffer();
    auto buffer = allocator.CreateBufferWithCopy(
        reinterpret_cast<const uint8_t*>(index_buffer.data()),
        index_buffer.size() * sizeof(IndexType));
    if (!buffer) {
      return {};
    }
    if (!label_.empty()) {
      buffer->SetLabel(SPrintF("%s Indices", label_.c_str()));
    }
    return buffer->AsBufferView();
  }

  size_t GetIndexCount() const { return vertices_.size(); }
};

}  // namespace impeller
