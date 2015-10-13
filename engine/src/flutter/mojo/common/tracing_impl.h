// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_COMMON_TRACING_IMPL_H_
#define MOJO_COMMON_TRACING_IMPL_H_

#include "base/macros.h"
#include "mojo/common/trace_provider_impl.h"
#include "mojo/public/cpp/application/interface_factory.h"
#include "mojo/services/tracing/interfaces/tracing.mojom.h"

namespace mojo {

class ApplicationImpl;

class TracingImpl : public InterfaceFactory<tracing::TraceProvider> {
 public:
  TracingImpl();
  ~TracingImpl() override;

  // This connects to the tracing service and registers ourselves to provide
  // tracing data on demand.
  void Initialize(ApplicationImpl* app);

 private:
  // InterfaceFactory<tracing::TraceProvider> implementation.
  void Create(ApplicationConnection* connection,
              InterfaceRequest<tracing::TraceProvider> request) override;

  TraceProviderImpl provider_impl_;

  DISALLOW_COPY_AND_ASSIGN(TracingImpl);
};

}  // namespace mojo

#endif  // MOJO_COMMON_TRACING_IMPL_H_
