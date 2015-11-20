// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mojo/edk/base_edk/platform_task_runner_impl.h"

#include <utility>

#include "base/location.h"
#include "base/logging.h"

namespace base_edk {

PlatformTaskRunnerImpl::PlatformTaskRunnerImpl(
    scoped_refptr<base::TaskRunner>&& base_task_runner)
    : base_task_runner_(std::move(base_task_runner)) {
  DCHECK(base_task_runner_);
}

PlatformTaskRunnerImpl::~PlatformTaskRunnerImpl() {}

void PlatformTaskRunnerImpl::PostTask(const base::Closure& task) {
  bool result = base_task_runner_->PostTask(tracked_objects::Location(), task);
  DCHECK(result);
}

bool PlatformTaskRunnerImpl::RunsTasksOnCurrentThread() const {
  return base_task_runner_->RunsTasksOnCurrentThread();
}

}  // namespace base_edk
