// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/fml/message_loop.h"

#include <utility>

#include "flutter/fml/message_loop_impl.h"
#include "flutter/fml/task_runner.h"
#include "flutter/fml/thread_local.h"
#include "lib/ftl/memory/ref_counted.h"
#include "lib/ftl/memory/ref_ptr.h"

namespace fml {

FML_THREAD_LOCAL ThreadLocal tls_message_loop([](intptr_t value) {
  delete reinterpret_cast<MessageLoop*>(value);
});

MessageLoop& MessageLoop::GetCurrent() {
  auto loop = reinterpret_cast<MessageLoop*>(tls_message_loop.Get());
  FTL_CHECK(loop != nullptr)
      << "MessageLoop::EnsureInitializedForCurrentThread was not called on "
         "this thread prior to message loop use.";
  return *loop;
}

void MessageLoop::EnsureInitializedForCurrentThread() {
  if (tls_message_loop.Get() != 0) {
    // Already initialized.
    return;
  }
  tls_message_loop.Set(reinterpret_cast<intptr_t>(new MessageLoop()));
}

bool MessageLoop::IsInitializedForCurrentThread() {
  return tls_message_loop.Get() != 0;
}

MessageLoop::MessageLoop()
    : loop_(MessageLoopImpl::Create()),
      task_runner_(ftl::MakeRefCounted<fml::TaskRunner>(loop_)) {
  FTL_CHECK(loop_);
  FTL_CHECK(task_runner_);
}

MessageLoop::~MessageLoop() = default;

void MessageLoop::Run() {
  loop_->DoRun();
}

void MessageLoop::Terminate() {
  loop_->DoTerminate();
}

ftl::RefPtr<ftl::TaskRunner> MessageLoop::GetTaskRunner() const {
  return task_runner_;
}

ftl::RefPtr<MessageLoopImpl> MessageLoop::GetLoopImpl() const {
  return loop_;
}

void MessageLoop::SetTaskObserver(TaskObserver observer) {
  loop_->SetTaskObserver(std::move(observer));
}

}  // namespace fml
