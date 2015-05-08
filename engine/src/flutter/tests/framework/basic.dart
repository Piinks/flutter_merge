import 'dart:sky';

import 'package:sky/framework/fn.dart';

class TestApp extends App {
  UINode build() {
    return new Container(
      inlineStyle: 'background-color: green',
      children: [
        new Text('I am Text'),
        new Image(src: 'resources/united.jpg')
      ]
    );
  }
}
