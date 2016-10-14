// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/lib/ui/window/window.h"

#include "flutter/lib/ui/compositing/scene.h"
#include "flutter/lib/ui/ui_dart_state.h"
#include "flutter/lib/ui/window/platform_message_response_dart.h"
#include "lib/tonic/converter/dart_converter.h"
#include "lib/tonic/dart_args.h"
#include "lib/tonic/dart_library_natives.h"
#include "lib/tonic/logging/dart_invoke.h"
#include "lib/tonic/typed_data/dart_byte_data.h"

using tonic::DartInvokeField;
using tonic::DartState;
using tonic::StdStringToDart;
using tonic::ToDart;

namespace blink {
namespace {

Dart_Handle ToByteData(const std::vector<char> buffer) {
  if (buffer.empty())
    return Dart_Null();

  Dart_Handle data_handle =
      Dart_NewTypedData(Dart_TypedData_kByteData, buffer.size());
  if (Dart_IsError(data_handle))
    return data_handle;

  Dart_TypedData_Type type;
  void* data = nullptr;
  intptr_t num_bytes = 0;
  FTL_CHECK(!Dart_IsError(
      Dart_TypedDataAcquireData(data_handle, &type, &data, &num_bytes)));

  memcpy(data, buffer.data(), num_bytes);
  Dart_TypedDataReleaseData(data_handle);
  return data_handle;
}

void ScheduleFrame(Dart_NativeArguments args) {
  UIDartState::Current()->window()->client()->ScheduleFrame();
}

void Render(Dart_NativeArguments args) {
  Dart_Handle exception = nullptr;
  Scene* scene =
      tonic::DartConverter<Scene*>::FromArguments(args, 1, exception);
  if (exception) {
    Dart_ThrowException(exception);
    return;
  }
  UIDartState::Current()->window()->client()->Render(scene);
}

void UpdateSemantics(Dart_NativeArguments args) {
  Dart_Handle exception = nullptr;
  SemanticsUpdate* update =
      tonic::DartConverter<SemanticsUpdate*>::FromArguments(args, 1, exception);
  if (exception) {
    Dart_ThrowException(exception);
    return;
  }
  UIDartState::Current()->window()->client()->UpdateSemantics(update);
}

void SendPlatformMessage(Dart_Handle window,
                         const std::string& name,
                         Dart_Handle callback,
                         const tonic::DartByteData& data) {
  UIDartState* dart_state = UIDartState::Current();
  const char* buffer = static_cast<const char*>(data.data());

  ftl::RefPtr<PlatformMessageResponse> response;
  if (!Dart_IsNull(callback)) {
    response = ftl::MakeRefCounted<PlatformMessageResponseDart>(
        tonic::DartPersistentValue(dart_state, callback));
  }

  UIDartState::Current()->window()->client()->HandlePlatformMessage(
      ftl::MakeRefCounted<PlatformMessage>(
          name, std::vector<char>(buffer, buffer + data.length_in_bytes()),
          response));
}

void _SendPlatformMessage(Dart_NativeArguments args) {
  tonic::DartCallStatic(&SendPlatformMessage, args);
}

void RespondToPlatformMessage(Dart_Handle window,
                              int response_id,
                              const tonic::DartByteData& data) {
  const char* buffer = static_cast<const char*>(data.data());
  UIDartState::Current()->window()->CompletePlatformMessageResponse(
      response_id, std::vector<char>(buffer, buffer + data.length_in_bytes()));
}

void _RespondToPlatformMessage(Dart_NativeArguments args) {
  tonic::DartCallStatic(&RespondToPlatformMessage, args);
}

}  // namespace

WindowClient::~WindowClient() {}

Window::Window(WindowClient* client) : client_(client) {}

Window::~Window() {}

void Window::DidCreateIsolate() {
  library_.Set(DartState::Current(), Dart_LookupLibrary(ToDart("dart:ui")));
}

void Window::UpdateWindowMetrics(const sky::ViewportMetricsPtr& metrics) {
  tonic::DartState* dart_state = library_.dart_state().get();
  if (!dart_state)
    return;
  tonic::DartState::Scope scope(dart_state);
  DartInvokeField(
      library_.value(), "_updateWindowMetrics",
      {
          ToDart(metrics->device_pixel_ratio),
          ToDart(static_cast<double>(metrics->physical_width)),
          ToDart(static_cast<double>(metrics->physical_height)),
          ToDart(static_cast<double>(metrics->physical_padding_top)),
          ToDart(static_cast<double>(metrics->physical_padding_right)),
          ToDart(static_cast<double>(metrics->physical_padding_bottom)),
          ToDart(static_cast<double>(metrics->physical_padding_left)),
      });
}

void Window::UpdateLocale(const std::string& language_code,
                          const std::string& country_code) {
  tonic::DartState* dart_state = library_.dart_state().get();
  if (!dart_state)
    return;
  tonic::DartState::Scope scope(dart_state);

  DartInvokeField(
      library_.value(), "_updateLocale",
      {
          StdStringToDart(language_code), StdStringToDart(country_code),
      });
}

void Window::UpdateSemanticsEnabled(bool enabled) {
  tonic::DartState* dart_state = library_.dart_state().get();
  if (!dart_state)
    return;
  tonic::DartState::Scope scope(dart_state);

  DartInvokeField(library_.value(), "_updateSemanticsEnabled",
                  {ToDart(enabled)});
}

void Window::PushRoute(const std::string& route) {
  tonic::DartState* dart_state = library_.dart_state().get();
  if (!dart_state)
    return;
  tonic::DartState::Scope scope(dart_state);

  DartInvokeField(library_.value(), "_pushRoute", {
                                                      StdStringToDart(route),
                                                  });
}

void Window::PopRoute() {
  tonic::DartState* dart_state = library_.dart_state().get();
  if (!dart_state)
    return;
  tonic::DartState::Scope scope(dart_state);

  DartInvokeField(library_.value(), "_popRoute", {});
}

void Window::DispatchPlatformMessage(ftl::RefPtr<PlatformMessage> message) {
  tonic::DartState* dart_state = library_.dart_state().get();
  if (!dart_state)
    return;
  tonic::DartState::Scope scope(dart_state);

  Dart_Handle data_handle = ToByteData(message->data());
  if (Dart_IsError(data_handle))
    return;

  int response_id = 0;
  if (auto response = message->response()) {
    response_id = next_response_id_++;
    pending_responses_[response_id] = response;
  }

  DartInvokeField(library_.value(), "_dispatchPlatformMessage",
                  {ToDart(message->name()), data_handle, ToDart(response_id)});
}

void Window::DispatchPointerDataPacket(const PointerDataPacket& packet) {
  tonic::DartState* dart_state = library_.dart_state().get();
  if (!dart_state)
    return;
  tonic::DartState::Scope scope(dart_state);

  Dart_Handle data_handle = ToByteData(packet.data());
  if (Dart_IsError(data_handle))
    return;
  DartInvokeField(library_.value(), "_dispatchPointerDataPacket",
                  {data_handle});
}

void Window::DispatchSemanticsAction(int32_t id, SemanticsAction action) {
  tonic::DartState* dart_state = library_.dart_state().get();
  if (!dart_state)
    return;
  tonic::DartState::Scope scope(dart_state);

  DartInvokeField(library_.value(), "_dispatchSemanticsAction",
                  {ToDart(id), ToDart(static_cast<int32_t>(action))});
}

void Window::BeginFrame(ftl::TimePoint frameTime) {
  tonic::DartState* dart_state = library_.dart_state().get();
  if (!dart_state)
    return;
  tonic::DartState::Scope scope(dart_state);

  int64_t microseconds = (frameTime - ftl::TimePoint()).ToMicroseconds();

  DartInvokeField(library_.value(), "_beginFrame",
                  {
                      Dart_NewInteger(microseconds),
                  });
}

void Window::OnAppLifecycleStateChanged(sky::AppLifecycleState state) {
  tonic::DartState* dart_state = library_.dart_state().get();
  if (!dart_state)
    return;
  tonic::DartState::Scope scope(dart_state);

  DartInvokeField(library_.value(), "_onAppLifecycleStateChanged",
                  {ToDart(static_cast<int>(state))});
}

void Window::CompletePlatformMessageResponse(int response_id,
                                             std::vector<char> data) {
  if (!response_id)
    return;
  auto it = pending_responses_.find(response_id);
  if (it == pending_responses_.end())
    return;
  auto response = std::move(it->second);
  pending_responses_.erase(it);
  response->Complete(std::move(data));
}

void Window::RegisterNatives(tonic::DartLibraryNatives* natives) {
  natives->Register({
      {"Window_scheduleFrame", ScheduleFrame, 1, true},
      {"Window_sendPlatformMessage", _SendPlatformMessage, 4, true},
      {"Window_respondToPlatformMessage", _RespondToPlatformMessage, 3, true},
      {"Window_render", Render, 2, true},
      {"Window_updateSemantics", UpdateSemantics, 2, true},
  });
}

}  // namespace blink
