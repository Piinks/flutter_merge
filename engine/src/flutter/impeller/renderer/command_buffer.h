// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <functional>
#include <memory>

#include "flutter/fml/macros.h"

namespace impeller {

class Context;
class RenderPass;
class RenderPassDescriptor;

//------------------------------------------------------------------------------
/// @brief      A collection of encoded commands to be submitted to the GPU for
///             execution. A command buffer is obtained from a graphics
///             `Context`.
///
///             To submit commands to the GPU, acquire a `RenderPass` from the
///             command buffer and record `Command`s into that pass. A
///             `RenderPass` describes the configuration of the various
///             attachments when the command is submitted.
///
///             A command buffer is only meant to be used on a single thread.
///
class CommandBuffer {
 public:
  enum class Status {
    kPending,
    kError,
    kCompleted,
  };

  using CompletionCallback = std::function<void(Status)>;

  virtual ~CommandBuffer();

  virtual bool IsValid() const = 0;

  //----------------------------------------------------------------------------
  /// @brief      Schedule the command encoded by render passes within this
  ///             command buffer on the GPU.
  ///
  ///             A command buffer may only be committed once.
  ///
  /// @param[in]  callback  The completion callback.
  ///
  virtual void SubmitCommands(CompletionCallback callback) = 0;

  //----------------------------------------------------------------------------
  /// @brief      Create a render pass to record render commands into.
  ///
  /// @param[in]  desc  The description of the render pass.
  ///
  /// @return     A valid render pass or null.
  ///
  virtual std::shared_ptr<RenderPass> CreateRenderPass(
      const RenderPassDescriptor& desc) const = 0;

 protected:
  CommandBuffer();

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(CommandBuffer);
};

}  // namespace impeller
