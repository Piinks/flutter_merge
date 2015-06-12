// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:sky';

import 'package:mojom/intents/intents.mojom.dart';
import 'package:sky/framework/shell.dart' as shell;
import 'package:sky/framework/theme2/colors.dart' as colors;
import 'package:sky/framework/theme2/edges.dart';
import 'package:sky/framework/theme2/typography.dart' as typography;
import 'package:sky/framework/widgets/material.dart';
import 'package:sky/framework/widgets/raised_button.dart';
import 'package:sky/framework/widgets/scaffold.dart';
import 'package:sky/framework/widgets/tool_bar.dart';
import 'package:sky/framework/widgets/wrappers.dart';

void launch(String relativeUrl) {
  Uri url = Uri.base.resolve(relativeUrl);
  url = url.replace(scheme: 'sky');

  ActivityManagerProxy activityManager = new ActivityManagerProxy.unbound();
  Intent intent = new Intent()
    ..action = 'android.intent.action.VIEW'
    ..url = url.toString();
  shell.requestService(null, activityManager);
  activityManager.ptr.startActivity(intent);
}

class SkyDemo extends Component {
  String text;
  String href;

  SkyDemo(String text, this.href) : this.text = text, super(key: text);

  void _handlePress() {
    launch(href);
  }

  UINode build() {
    return new RaisedButton(
      child: new Text(text),
      onPressed: _handlePress
    );
  }
}

class SkyHome extends App {
  UINode build() {
    List<UINode> children = [
      new SkyDemo('Stocks2 App', 'examples/stocks2/lib/stock_app.dart'),
      new SkyDemo('Box2D Game', 'examples/game/main.dart'),
      new SkyDemo('Interactive Flex', 'examples/rendering/interactive_flex.dart'),
      new SkyDemo('Sector Layout', 'examples/rendering/sector_layout.dart'),
      new SkyDemo('Touch Demo', 'examples/rendering/touch_demo.dart'),

      // TODO(eseidel): We could use to separate these groups?
      new SkyDemo('Stocks App (Old)', 'examples/stocks/main.sky'),
      new SkyDemo('Touch Demo (Old)', 'examples/raw/touch-demo.sky'),
      new SkyDemo('Spinning Square (Old)', 'examples/raw/spinning-square.sky'),

      new SkyDemo('Licences (Old)', 'LICENSES.sky'),
    ];

    return new Scaffold(
      toolbar: new ToolBar(
          center: new Text('Sky Demos', style: typography.white.title),
          backgroundColor: colors.Blue[500]),
      body: new Material(
        edge: MaterialEdge.canvas,
        child: new Flex(
          children,
          direction: FlexDirection.vertical,
          justifyContent: FlexJustifyContent.spaceAround
        )
      )
    );
  }
}

void main() {
  new SkyHome();
}
