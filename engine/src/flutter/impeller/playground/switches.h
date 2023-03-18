// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <chrono>
#include <optional>

#include "flutter/fml/command_line.h"
#include "flutter/fml/macros.h"

namespace impeller {

struct PlaygroundSwitches {
  // If specified, the playgrounds will render for at least the duration
  // specified in the timeout. If the timeout is zero, exactly one frame will be
  // rendered in the playground.
  std::optional<std::chrono::milliseconds> timeout;

  explicit PlaygroundSwitches(const fml::CommandLine& args);
};

}  // namespace impeller
