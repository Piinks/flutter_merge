// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_VIEWER_CONTENT_HANDLER_IMPL_H_
#define SKY_VIEWER_CONTENT_HANDLER_IMPL_H_

#include "base/message_loop/message_loop.h"
#include "mojo/public/cpp/bindings/strong_binding.h"
#include "mojo/public/interfaces/application/shell.mojom.h"
#include "mojo/services/content_handler/public/interfaces/content_handler.mojom.h"

namespace sky {
class DocumentView;

class ContentHandlerImpl : public mojo::ContentHandler {
 public:
  explicit ContentHandlerImpl(
      mojo::InterfaceRequest<mojo::ContentHandler> request);
  ~ContentHandlerImpl() override;

 private:
  // Overridden from ContentHandler:
  void StartApplication(mojo::InterfaceRequest<mojo::Application> application,
                        mojo::URLResponsePtr response) override;

  mojo::StrongBinding<mojo::ContentHandler> binding_;
  DISALLOW_COPY_AND_ASSIGN(ContentHandlerImpl);
};

}  // namespace sky

#endif  // SKY_VIEWER_CONTENT_HANDLER_IMPL_H_
