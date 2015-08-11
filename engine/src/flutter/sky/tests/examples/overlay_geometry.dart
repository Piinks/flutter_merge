// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:async';

import 'package:sky/widgets/framework.dart';

import '../../../examples/widgets/overlay_geometry.dart';
import '../resources/display_list.dart';

main() async {
  TestRenderView testRenderView = new TestRenderView();
  OverlayGeometryApp app = new OverlayGeometryApp();
  runApp(app, renderViewOverride: testRenderView);
  await testRenderView.checkFrame();
  testRenderView.endTest();
}
