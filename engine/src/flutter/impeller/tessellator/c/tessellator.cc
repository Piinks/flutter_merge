// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tessellator.h"

#include <vector>

namespace impeller {
PathBuilder* CreatePathBuilder() {
  return new PathBuilder();
}

void DestroyPathBuilder(PathBuilder* builder) {
  delete builder;
}

void MoveTo(PathBuilder* builder, Scalar x, Scalar y) {
  builder->MoveTo(Point(x, y));
}

void LineTo(PathBuilder* builder, Scalar x, Scalar y) {
  builder->LineTo(Point(x, y));
}

void CubicTo(PathBuilder* builder,
             Scalar x1,
             Scalar y1,
             Scalar x2,
             Scalar y2,
             Scalar x3,
             Scalar y3) {
  builder->CubicCurveTo(Point(x1, y1), Point(x2, y2), Point(x3, y3));
}

void Close(PathBuilder* builder) {
  builder->Close();
}

struct Vertices* Tessellate(PathBuilder* builder,
                            int fill_type,
                            Scalar scale,
                            Scalar angle_tolerance,
                            Scalar cusp_limit) {
  auto path = builder->CopyPath(static_cast<FillType>(fill_type));
  auto smoothing = SmoothingApproximation(scale, angle_tolerance, cusp_limit);
  auto polyline = path.CreatePolyline(smoothing);
  std::vector<float> points;
  if (Tessellator{}.Tessellate(
          path.GetFillType(), polyline,
          [&points](const float* vertices, size_t vertices_size,
                    const uint16_t* indices, size_t indices_size) {
            // Results are expected to be re-duplicated.
            std::vector<Point> raw_points;
            for (auto i = 0u; i < vertices_size; i += 2) {
              raw_points.emplace_back(Point{vertices[i], vertices[i + 1]});
            }
            for (auto i = 0u; i < indices_size; i++) {
              auto point = raw_points[indices[i]];
              points.push_back(point.x);
              points.push_back(point.y);
            }
            return true;
          }) != Tessellator::Result::kSuccess) {
    return nullptr;
  }

  Vertices* vertices = new Vertices();
  vertices->points = new float[points.size()];
  if (!vertices->points) {
    return nullptr;
  }
  vertices->length = points.size();
  std::copy(points.begin(), points.end(), vertices->points);
  return vertices;
}

void DestroyVertices(Vertices* vertices) {
  delete vertices->points;
  delete vertices;
}

}  // namespace impeller
