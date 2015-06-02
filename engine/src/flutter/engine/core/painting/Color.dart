// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

part of dart.sky;

class Color {
  final int _value;
  int get value => _value;

  const Color(this._value);
  const Color.fromARGB(int a, int r, int g, int b) :
    _value = (((a & 0xff) << 24) |
              ((r & 0xff) << 16) |
              ((g & 0xff) << 8) |
              ((b & 0xff) << 0));

}
