// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_VIEWER_DOCUMENT_VIEW_H_
#define SKY_VIEWER_DOCUMENT_VIEW_H_

#include "base/compiler_specific.h"
#include "base/memory/weak_ptr.h"
#include "mojo/public/cpp/application/lazy_interface_ptr.h"
#include "mojo/public/cpp/application/service_provider_impl.h"
#include "mojo/public/cpp/bindings/interface_impl.h"
#include "mojo/public/interfaces/application/application.mojom.h"
#include "mojo/services/public/cpp/view_manager/view_manager_client_factory.h"
#include "mojo/services/public/cpp/view_manager/view_manager_delegate.h"
#include "mojo/services/public/cpp/view_manager/view_observer.h"
#include "mojo/services/public/interfaces/content_handler/content_handler.mojom.h"
#include "mojo/services/public/interfaces/navigation/navigation.mojom.h"
#include "mojo/services/public/interfaces/network/url_loader.mojom.h"
#include "sky/engine/public/web/WebFrameClient.h"
#include "sky/engine/public/web/WebViewClient.h"
#include "sky/viewer/services/inspector_impl.h"

namespace base {
class MessageLoopProxy;
}

namespace mojo {
class ViewManager;
class View;
}

namespace sky {
class ScriptRunner;
class WebLayerTreeViewImpl;

class DocumentView : public mojo::InterfaceImpl<mojo::Application>,
                     public blink::WebViewClient,
                     public blink::WebFrameClient,
                     public mojo::ViewManagerDelegate,
                     public mojo::ViewObserver {
 public:
  // Load a new HTMLDocument with |response|.
  //
  // |shell| is the Shell connection for this mojo::Application.
  DocumentView(mojo::URLResponsePtr response,
               mojo::ShellPtr shell,
               scoped_refptr<base::MessageLoopProxy> compositor_thread);
  virtual ~DocumentView();

  base::WeakPtr<DocumentView> GetWeakPtr();

  blink::WebView* web_view() const { return web_view_; }
  mojo::ServiceProvider* imported_services() const {
    return imported_services_.get();
  }

  mojo::Shell* shell() const { return shell_.get(); }

 private:
  // Application methods:
  void AcceptConnection(const mojo::String& requestor_url,
                        mojo::ServiceProviderPtr provider) override;
  void Initialize(mojo::Array<mojo::String> args) override;

  // WebWidgetClient methods:
  virtual blink::WebLayerTreeView* initializeLayerTreeView();

  // WebFrameClient methods:
  virtual void frameDetached(blink::WebFrame*);
  virtual blink::WebNavigationPolicy decidePolicyForNavigation(
    const blink::WebFrameClient::NavigationPolicyInfo& info);
  virtual void didAddMessageToConsole(
      const blink::WebConsoleMessage& message,
      const blink::WebString& source_name,
      unsigned source_line,
      const blink::WebString& stack_trace);
  virtual void didCreateScriptContext(
      blink::WebLocalFrame*, v8::Handle<v8::Context>, int extensionGroup, int worldId);

  // ViewManagerDelegate methods:
  virtual void OnEmbed(
      mojo::ViewManager* view_manager,
      mojo::View* root,
      mojo::ServiceProviderImpl* exported_services,
      scoped_ptr<mojo::ServiceProvider> imported_services) override;
  virtual void OnViewManagerDisconnected(mojo::ViewManager* view_manager) override;

  // ViewObserver methods:
  virtual void OnViewBoundsChanged(mojo::View* view,
                                   const mojo::Rect& old_bounds,
                                   const mojo::Rect& new_bounds) override;
  virtual void OnViewDestroyed(mojo::View* view) override;
  virtual void OnViewInputEvent(mojo::View* view, const mojo::EventPtr& event) override;

  void Load(mojo::URLResponsePtr response);

  mojo::URLResponsePtr response_;
  mojo::ServiceProviderImpl exported_services_;
  scoped_ptr<mojo::ServiceProvider> imported_services_;
  mojo::ShellPtr shell_;
  mojo::LazyInterfacePtr<mojo::NavigatorHost> navigator_host_;
  blink::WebView* web_view_;
  mojo::View* root_;
  mojo::ViewManagerClientFactory view_manager_client_factory_;
  InspectorServiceFactory inspector_service_factory_;
  scoped_ptr<WebLayerTreeViewImpl> web_layer_tree_view_impl_;
  scoped_refptr<base::MessageLoopProxy> compositor_thread_;
  scoped_ptr<ScriptRunner> script_runner_;

  base::WeakPtrFactory<DocumentView> weak_factory_;
  DISALLOW_COPY_AND_ASSIGN(DocumentView);
};

}  // namespace sky

#endif  // SKY_VIEWER_DOCUMENT_VIEW_H_
