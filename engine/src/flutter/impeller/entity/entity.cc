// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "impeller/entity/entity.h"

namespace impeller {

Entity::Entity() = default;

Entity::~Entity() = default;

const Matrix& Entity::GetTransformation() const {
  return transformation_;
}

void Entity::SetTransformation(const Matrix& transformation) {
  transformation_ = transformation;
}

const Color& Entity::GetBackgroundColor() const {
  return background_color_;
}

void Entity::SetBackgroundColor(const Color& backgroundColor) {
  background_color_ = backgroundColor;
}

const Color& Entity::GetStrokeColor() const {
  return stroke_color_;
}

void Entity::SetStrokeColor(const Color& strokeColor) {
  stroke_color_ = strokeColor;
}

double Entity::GetStrokeSize() const {
  return stroke_size_;
}

void Entity::SetStrokeSize(double strokeSize) {
  stroke_size_ = std::max(strokeSize, 0.0);
}

const Path& Entity::GetPath() const {
  return path_;
}

void Entity::SetPath(Path path) {
  path_ = std::move(path);
}

void Entity::SetIsClip(bool is_clip) {
  is_clip_ = is_clip;
}

bool Entity::IsClip() const {
  return is_clip_;
}

void Entity::SetContents(std::shared_ptr<Contents> contents) {
  contents_ = std::move(contents);
}

const std::shared_ptr<Contents>& Entity::GetContents() const {
  return contents_;
}

bool Entity::HasStroke() const {
  return stroke_size_ > 0.0 && !stroke_color_.IsTransparent();
}

bool Entity::HasContents() const {
  if (contents_) {
    return true;
  }
  return !background_color_.IsTransparent();
}

bool Entity::HasRenderableContents() const {
  const bool has_empty_path = path_.GetBoundingBox().IsZero();

  if (has_empty_path) {
    return false;
  }

  if (IsClip()) {
    return true;
  }

  if (HasStroke()) {
    return true;
  }

  if (HasContents()) {
    return true;
  }

  return false;
}

}  // namespace impeller
