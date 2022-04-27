// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <memory>
#include <optional>
#include <stack>

#include "flutter/fml/macros.h"
#include "impeller/aiks/paint.h"
#include "impeller/aiks/picture.h"
#include "impeller/geometry/matrix.h"
#include "impeller/geometry/path.h"
#include "impeller/geometry/vector.h"

namespace impeller {

class Canvas {
 public:
  Canvas();

  ~Canvas();

  void Save();

  void SaveLayer(const Paint& paint, std::optional<Rect> bounds = std::nullopt);

  bool Restore();

  size_t GetSaveCount() const;

  const Matrix& GetCurrentTransformation() const;

  void Concat(const Matrix& xformation);

  void Translate(const Vector3& offset);

  void Scale(const Vector3& scale);

  void Rotate(Radians radians);

  void DrawPath(Path path, Paint paint);

  void ClipPath(Path path);

  void DrawShadow(Path path, Color color, Scalar elevation);

  void DrawPicture(std::shared_ptr<Picture> picture);

 private:
  std::stack<Matrix> xformation_stack_;

  FML_DISALLOW_COPY_AND_ASSIGN(Canvas);
};

}  // namespace impeller
