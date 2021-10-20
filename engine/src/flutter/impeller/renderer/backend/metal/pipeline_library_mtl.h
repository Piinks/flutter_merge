// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "flutter/fml/macros.h"
#include "impeller/renderer/pipeline_library.h"

namespace impeller {

class PipelineLibraryMTL final : public PipelineLibrary {
 public:
  PipelineLibraryMTL();

  ~PipelineLibraryMTL() override;

 private:
  FML_DISALLOW_COPY_AND_ASSIGN(PipelineLibraryMTL);
};

}  // namespace impeller
