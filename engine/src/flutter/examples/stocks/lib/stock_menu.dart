// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

part of stocks;

enum _MenuItems { add, remove, autorefresh }

Future showStockMenu(NavigatorState navigator, { bool autorefresh, ValueChanged onAutorefreshChanged }) async {
  switch (await showMenu(
    navigator: navigator,
    position: new MenuPosition(
      right: sky.view.paddingRight,
      top: sky.view.paddingTop
    ),
    builder: (NavigatorState navigator) {
      return <PopupMenuItem>[
        new PopupMenuItem(
          value: _MenuItems.add,
          child: new Text('Add stock')
        ),
        new PopupMenuItem(
          value: _MenuItems.remove,
          child: new Text('Remove stock')
        ),
        new PopupMenuItem(
          value: _MenuItems.autorefresh,
          child: new Row([
              new Flexible(child: new Text('Autorefresh')),
              new Checkbox(
                value: autorefresh,
                onChanged: onAutorefreshChanged
              )
            ]
          )
        ),
      ];
    }
  )) {
    case _MenuItems.autorefresh:
      onAutorefreshChanged(!autorefresh);
      break;
    case _MenuItems.add:
    case _MenuItems.remove:
      await showDialog(navigator, (NavigatorState navigator) {
        return new Dialog(
          title: new Text('Not Implemented'),
          content: new Text('This feature has not yet been implemented.'),
          actions: [
            new FlatButton(
              child: new Text('OH WELL'),
              onPressed: () {
                navigator.pop(false);
              }
            ),
          ]
        );
      });
      break;
    default:
      // menu was canceled.
  }
}