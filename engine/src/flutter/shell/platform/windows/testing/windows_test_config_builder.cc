// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/windows/testing/windows_test_config_builder.h"

#include <combaseapi.h>

#include <string>
#include <string_view>
#include <vector>

#include "flutter/fml/logging.h"
#include "flutter/shell/platform/windows/public/flutter_windows.h"
#include "flutter/shell/platform/windows/testing/windows_test_context.h"

namespace flutter {
namespace testing {

WindowsConfigBuilder::WindowsConfigBuilder(WindowsTestContext& context)
    : context_(context) {}

WindowsConfigBuilder::~WindowsConfigBuilder() = default;

void WindowsConfigBuilder::AddDartEntrypointArgument(std::string_view arg) {
  if (arg.empty()) {
    return;
  }

  dart_entrypoint_arguments_.emplace_back(std::move(arg));
}

FlutterDesktopEngineProperties WindowsConfigBuilder::GetEngineProperties()
    const {
  FlutterDesktopEngineProperties engine_properties = {};
  engine_properties.assets_path = context_.GetAssetsPath().c_str();
  engine_properties.icu_data_path = context_.GetIcuDataPath().c_str();

  // Set Dart entrypoint argc, argv.
  std::vector<const char*> dart_args;
  dart_args.reserve(dart_entrypoint_arguments_.size());
  for (const auto& arg : dart_entrypoint_arguments_) {
    dart_args.push_back(arg.c_str());
  }
  if (!dart_args.empty()) {
    engine_properties.dart_entrypoint_argv = dart_args.data();
    engine_properties.dart_entrypoint_argc = dart_args.size();
  } else {
    // Clear this out in case this is not the first engine launch from the
    // embedder config builder.
    engine_properties.dart_entrypoint_argv = nullptr;
    engine_properties.dart_entrypoint_argc = 0;
  }

  return engine_properties;
}

ViewControllerPtr WindowsConfigBuilder::LaunchEngine() const {
  InitializeCOM();

  EnginePtr engine = InitializeEngine();
  if (!engine) {
    return {};
  }

  int width = 600;
  int height = 400;
  ViewControllerPtr controller(
      FlutterDesktopViewControllerCreate(width, height, engine.release()));
  if (!controller) {
    return {};
  }

  return controller;
}

void WindowsConfigBuilder::InitializeCOM() const {
  FML_CHECK(SUCCEEDED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED)));
}

EnginePtr WindowsConfigBuilder::InitializeEngine() const {
  FlutterDesktopEngineProperties engine_properties = GetEngineProperties();
  return EnginePtr(FlutterDesktopEngineCreate(&engine_properties));
}

}  // namespace testing
}  // namespace flutter
