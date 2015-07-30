// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include "base/at_exit.h"
#include "base/basictypes.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/i18n/icu_util.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "sky/engine/public/web/WebRuntimeFeatures.h"
#include "sky/shell/platform_view.h"
#include "sky/shell/service_provider.h"
#include "sky/shell/shell.h"
#include "sky/shell/shell_view.h"
#include "sky/shell/switches.h"
#include "sky/shell/testing/test_runner.h"

namespace sky {
namespace shell {
namespace {

void Init() {
  base::CommandLine& command_line = *base::CommandLine::ForCurrentProcess();
  blink::WebRuntimeFeatures::enableObservatory(
      !command_line.HasSwitch(switches::kNonInteractive));

  // Explicitly boot the shared test runner.
  TestRunner& runner = TestRunner::Shared();

  std::string package_root =
      command_line.GetSwitchValueASCII(switches::kPackageRoot);
  runner.set_package_root(package_root);

  scoped_ptr<TestRunner::SingleTest> single_test;
  if (command_line.HasSwitch(switches::kSnapshot)) {
    single_test.reset(new TestRunner::SingleTest);
    single_test->path = command_line.GetSwitchValueASCII(switches::kSnapshot);
    single_test->is_snapshot = true;
  } else {
    auto args = command_line.GetArgs();
    if (!args.empty()) {
      single_test.reset(new TestRunner::SingleTest);
      single_test->path = args[0];
    }
  }

  runner.Start(single_test.Pass());
}

}  // namespace
}  // namespace shell
}  // namespace sky

int main(int argc, const char* argv[]) {
  base::AtExitManager exit_manager;
  base::CommandLine::Init(argc, argv);

  base::CommandLine& command_line = *base::CommandLine::ForCurrentProcess();

  if (command_line.HasSwitch(sky::shell::switches::kHelp) ||
      (!command_line.HasSwitch(sky::shell::switches::kPackageRoot) &&
       !command_line.HasSwitch(sky::shell::switches::kSnapshot))) {
    sky::shell::switches::PrintUsage("sky_shell");
    return 0;
  }

  base::MessageLoop message_loop;

  sky::shell::Shell::Init(make_scoped_ptr(
      new sky::shell::ServiceProviderContext(message_loop.task_runner())));

  message_loop.PostTask(FROM_HERE, base::Bind(&sky::shell::Init));
  message_loop.Run();

  return 0;
}
