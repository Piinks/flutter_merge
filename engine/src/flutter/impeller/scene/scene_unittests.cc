// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cmath>
#include <memory>

#include "flutter/testing/testing.h"
#include "impeller/geometry/color.h"
#include "impeller/geometry/constants.h"
#include "impeller/geometry/matrix.h"
#include "impeller/geometry/quaternion.h"
#include "impeller/geometry/vector.h"
#include "impeller/playground/playground.h"
#include "impeller/playground/playground_test.h"
#include "impeller/renderer/formats.h"
#include "impeller/scene/camera.h"
#include "impeller/scene/geometry.h"
#include "impeller/scene/importer/scene_flatbuffers.h"
#include "impeller/scene/material.h"
#include "impeller/scene/mesh.h"
#include "impeller/scene/scene.h"
#include "third_party/flatbuffers/include/flatbuffers/verifier.h"
#include "third_party/imgui/imgui.h"

// #include "third_party/tinygltf/tiny_gltf.h"

namespace impeller {
namespace scene {
namespace testing {

using SceneTest = PlaygroundTest;
INSTANTIATE_PLAYGROUND_SUITE(SceneTest);

TEST_P(SceneTest, CuboidUnlit) {
  auto scene_context = std::make_shared<SceneContext>(GetContext());

  Renderer::RenderCallback callback = [&](RenderTarget& render_target) {
    auto allocator = GetContext()->GetResourceAllocator();
    auto scene = Scene(scene_context);

    {
      Mesh mesh;

      auto material = Material::MakeUnlit();
      material->SetColor(Color::Red());

      Vector3 size(1, 1, 0);
      mesh.AddPrimitive({Geometry::MakeCuboid(size), std::move(material)});

      Node& root = scene.GetRoot();
      root.SetLocalTransform(Matrix::MakeTranslation(-size / 2));
      root.SetMesh(std::move(mesh));
    }

    // Face towards the +Z direction (+X right, +Y up).
    auto camera = Camera::MakePerspective(
                      /* fov */ Radians(kPiOver4),
                      /* position */ {2, 2, -5})
                      .LookAt(
                          /* target */ Vector3(),
                          /* up */ {0, 1, 0});

    scene.Render(render_target, camera);
    return true;
  };

  OpenPlaygroundHere(callback);
}

TEST_P(SceneTest, FlutterLogo) {
  auto allocator = GetContext()->GetResourceAllocator();

  auto mapping =
      flutter::testing::OpenFixtureAsMapping("flutter_logo.glb.ipscene");
  ASSERT_NE(mapping, nullptr);

  std::shared_ptr<Node> gltf_scene =
      Node::MakeFromFlatbuffer(*mapping, *allocator);
  ASSERT_NE(gltf_scene, nullptr);

  std::shared_ptr<UnlitMaterial> material = Material::MakeUnlit();
  auto color_baked = CreateTextureForFixture("flutter_logo_baked.png");
  material->SetColorTexture(color_baked);
  material->SetVertexColorWeight(0);

  ASSERT_EQ(gltf_scene->GetChildren().size(), 1u);
  ASSERT_EQ(gltf_scene->GetChildren()[0]->GetMesh().GetPrimitives().size(), 1u);
  gltf_scene->GetChildren()[0]->GetMesh().GetPrimitives()[0].material =
      material;

  auto scene_context = std::make_shared<SceneContext>(GetContext());
  auto scene = Scene(scene_context);
  scene.GetRoot().AddChild(std::move(gltf_scene));
  scene.GetRoot().SetLocalTransform(Matrix::MakeScale({3, 3, 3}));

  Renderer::RenderCallback callback = [&](RenderTarget& render_target) {
    Quaternion rotation({0, 1, 0}, -GetSecondsElapsed() * 0.5);
    Vector3 start_position(-1, -1.5, -5);

    // Face towards the +Z direction (+X right, +Y up).
    auto camera = Camera::MakePerspective(
                      /* fov */ Degrees(60),
                      /* position */ rotation * start_position)
                      .LookAt(
                          /* target */ Vector3(),
                          /* up */ {0, 1, 0});

    scene.Render(render_target, camera);
    return true;
  };

  OpenPlaygroundHere(callback);
}

TEST_P(SceneTest, TwoTriangles) {
  auto allocator = GetContext()->GetResourceAllocator();

  auto mapping =
      flutter::testing::OpenFixtureAsMapping("two_triangles.glb.ipscene");
  ASSERT_NE(mapping, nullptr);

  std::shared_ptr<Node> gltf_scene =
      Node::MakeFromFlatbuffer(*mapping, *allocator);
  ASSERT_NE(gltf_scene, nullptr);

  auto scene_context = std::make_shared<SceneContext>(GetContext());
  auto scene = Scene(scene_context);
  scene.GetRoot().AddChild(std::move(gltf_scene));

  Renderer::RenderCallback callback = [&](RenderTarget& render_target) {
    Node& node = *scene.GetRoot().GetChildren()[0];
    node.SetLocalTransform(node.GetLocalTransform() *
                           Matrix::MakeRotation(0.02, {0, 1, 0, 0}));

    static ImVec2 mouse_pos_prev = ImGui::GetMousePos();
    ImVec2 mouse_pos = ImGui::GetMousePos();
    Vector2 mouse_diff =
        Vector2(mouse_pos.x - mouse_pos_prev.x, mouse_pos.y - mouse_pos_prev.y);

    static Vector3 position(0, 1, -5);
    static Vector3 cam_position = position;
    auto strafe =
        Vector3(ImGui::IsKeyDown(ImGuiKey_D) - ImGui::IsKeyDown(ImGuiKey_A),
                ImGui::IsKeyDown(ImGuiKey_E) - ImGui::IsKeyDown(ImGuiKey_Q),
                ImGui::IsKeyDown(ImGuiKey_W) - ImGui::IsKeyDown(ImGuiKey_S));
    position += strafe * 0.5;
    cam_position = cam_position.Lerp(position, 0.02);

    // Face towards the +Z direction (+X right, +Y up).
    auto camera = Camera::MakePerspective(
                      /* fov */ Degrees(60),
                      /* position */ cam_position)
                      .LookAt(
                          /* target */ cam_position + Vector3(0, 0, 1),
                          /* up */ {0, 1, 0});

    scene.Render(render_target, camera);
    return true;
  };

  OpenPlaygroundHere(callback);
}

}  // namespace testing
}  // namespace scene
}  // namespace impeller
