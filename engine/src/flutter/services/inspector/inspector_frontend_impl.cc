// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/services/inspector/inspector_frontend_impl.h"

#include "base/lazy_instance.h"
#include "net/server/http_server.h"
#include "net/socket/tcp_server_socket.h"

namespace sky {
namespace inspector {
namespace {
const int kNotConnected = -1;
static base::LazyInstance<std::map<int, InspectorFrontendImpl*>> g_servers =
    LAZY_INSTANCE_INITIALIZER;
}

InspectorFrontendImpl::InspectorFrontendImpl()
  : connection_id_(kNotConnected) {
}

InspectorFrontendImpl::~InspectorFrontendImpl() {
  StopListening();
}

void InspectorFrontendImpl::OnConnect(int connection_id) {
}

void InspectorFrontendImpl::OnHttpRequest(
    int connection_id, const net::HttpServerRequestInfo& info) {
  web_server_->Send500(connection_id, "websockets protocol only");
}

void InspectorFrontendImpl::OnWebSocketRequest(
    int connection_id, const net::HttpServerRequestInfo& info) {
  if (connection_id_ != kNotConnected) {
    web_server_->Close(connection_id);
    return;
  }
  web_server_->AcceptWebSocket(connection_id, info);
  connection_id_ = connection_id;
  client()->OnConnect();
}

void InspectorFrontendImpl::OnWebSocketMessage(
    int connection_id, const std::string& data) {
  DCHECK_EQ(connection_id, connection_id_);
  client()->OnMessage(data);
}

void InspectorFrontendImpl::OnClose(int connection_id) {
  if (connection_id != connection_id_)
    return;
  connection_id_ = kNotConnected;
  client()->OnDisconnect();
}

void InspectorFrontendImpl::Listen(int32_t port) {
  Register(port);
  scoped_ptr<net::ServerSocket> server_socket(
      new net::TCPServerSocket(NULL, net::NetLog::Source()));
  server_socket->ListenWithAddressAndPort("0.0.0.0", port, 1);
  web_server_.reset(new net::HttpServer(server_socket.Pass(), this));
}

void InspectorFrontendImpl::StopListening() {
  if (!web_server_)
    return;
  web_server_.reset();
  Unregister();
}

void InspectorFrontendImpl::Register(int port) {
  auto& servers = g_servers.Get();
  auto iter = servers.find(port);
  if (iter != servers.end())
    iter->second->StopListening();
  DCHECK(servers.find(port) == servers.end());
  servers[port] = this;
  port_ = port;
}

void InspectorFrontendImpl::Unregister() {
  DCHECK(g_servers.Get().find(port_)->second == this);
  g_servers.Get().erase(port_);
  port_ = kNotConnected;
}

void InspectorFrontendImpl::SendMessage(const mojo::String& message) {
  if (connection_id_ == kNotConnected)
    return;
  web_server_->SendOverWebSocket(connection_id_, message);
}

}  // namespace inspector
}  // namespace sky
