// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/lib/ui/semantics/semantics_update_builder.h"

#include "flutter/lib/ui/ui_dart_state.h"
#include "third_party/skia/include/core/SkScalar.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

namespace flutter {

void pushStringAttributes(
    StringAttributes& destination,
    const std::vector<NativeStringAttribute*>& native_attributes) {
  for (const auto& native_attribute : native_attributes) {
    destination.push_back(native_attribute->GetAttribute());
  }
}

IMPLEMENT_WRAPPERTYPEINFO(ui, SemanticsUpdateBuilder);

SemanticsUpdateBuilder::SemanticsUpdateBuilder() = default;

SemanticsUpdateBuilder::~SemanticsUpdateBuilder() = default;

void SemanticsUpdateBuilder::updateNode(
    int id,
    int flags,
    int actions,
    int maxValueLength,
    int currentValueLength,
    int textSelectionBase,
    int textSelectionExtent,
    int platformViewId,
    int scrollChildren,
    int scrollIndex,
    double scrollPosition,
    double scrollExtentMax,
    double scrollExtentMin,
    double left,
    double top,
    double right,
    double bottom,
    double elevation,
    double thickness,
    std::string label,
    std::vector<NativeStringAttribute*> labelAttributes,
    std::string value,
    std::vector<NativeStringAttribute*> valueAttributes,
    std::string increasedValue,
    std::vector<NativeStringAttribute*> increasedValueAttributes,
    std::string decreasedValue,
    std::vector<NativeStringAttribute*> decreasedValueAttributes,
    std::string hint,
    std::vector<NativeStringAttribute*> hintAttributes,
    std::string tooltip,
    int textDirection,
    const tonic::Float64List& transform,
    const tonic::Int32List& childrenInTraversalOrder,
    const tonic::Int32List& childrenInHitTestOrder,
    const tonic::Int32List& localContextActions) {
  FML_CHECK(transform.data() && SkScalarsAreFinite(*transform.data(), 9))
      << "Semantics update transform was not set or not finite.";
  FML_CHECK(scrollChildren == 0 ||
            (scrollChildren > 0 && childrenInHitTestOrder.data()))
      << "Semantics update contained scrollChildren but did not have "
         "childrenInHitTestOrder";
  SemanticsNode node;
  node.id = id;
  node.flags = flags;
  node.actions = actions;
  node.maxValueLength = maxValueLength;
  node.currentValueLength = currentValueLength;
  node.textSelectionBase = textSelectionBase;
  node.textSelectionExtent = textSelectionExtent;
  node.platformViewId = platformViewId;
  node.scrollChildren = scrollChildren;
  node.scrollIndex = scrollIndex;
  node.scrollPosition = scrollPosition;
  node.scrollExtentMax = scrollExtentMax;
  node.scrollExtentMin = scrollExtentMin;
  node.rect = SkRect::MakeLTRB(left, top, right, bottom);
  node.elevation = elevation;
  node.thickness = thickness;
  node.label = label;
  pushStringAttributes(node.labelAttributes, labelAttributes);
  node.value = value;
  pushStringAttributes(node.valueAttributes, valueAttributes);
  node.increasedValue = increasedValue;
  pushStringAttributes(node.increasedValueAttributes, increasedValueAttributes);
  node.decreasedValue = decreasedValue;
  pushStringAttributes(node.decreasedValueAttributes, decreasedValueAttributes);
  node.hint = hint;
  pushStringAttributes(node.hintAttributes, hintAttributes);
  node.tooltip = tooltip;
  node.textDirection = textDirection;
  SkScalar scalarTransform[16];
  for (int i = 0; i < 16; ++i) {
    scalarTransform[i] = transform.data()[i];
  }
  node.transform = SkM44::ColMajor(scalarTransform);
  node.childrenInTraversalOrder =
      std::vector<int32_t>(childrenInTraversalOrder.data(),
                           childrenInTraversalOrder.data() +
                               childrenInTraversalOrder.num_elements());
  node.childrenInHitTestOrder = std::vector<int32_t>(
      childrenInHitTestOrder.data(),
      childrenInHitTestOrder.data() + childrenInHitTestOrder.num_elements());
  node.customAccessibilityActions = std::vector<int32_t>(
      localContextActions.data(),
      localContextActions.data() + localContextActions.num_elements());
  nodes_[id] = node;
}

void SemanticsUpdateBuilder::updateCustomAction(int id,
                                                std::string label,
                                                std::string hint,
                                                int overrideId) {
  CustomAccessibilityAction action;
  action.id = id;
  action.overrideId = overrideId;
  action.label = label;
  action.hint = hint;
  actions_[id] = action;
}

void SemanticsUpdateBuilder::build(Dart_Handle semantics_update_handle) {
  SemanticsUpdate::create(semantics_update_handle, std::move(nodes_),
                          std::move(actions_));
}

}  // namespace flutter
