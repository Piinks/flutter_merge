// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MISSING_CTOR_H_
#define MISSING_CTOR_H_

struct MyString {
  MyString();
  ~MyString();
  MyString(const MyString&);
  MyString(MyString&&);
};

template <class T>
struct MyVector {
  MyVector();
  ~MyVector();
  MyVector(const MyVector&);
  MyVector(MyVector&&);
};

// Note: this should warn for an implicit copy constructor too, but currently
// doesn't, due to a plugin bug.
class MissingCtorsArentOKInHeader {
 public:

 private:
  MyVector<int> one_;
  MyVector<MyString> two_;
};

// Inline move ctors shouldn't be warned about. Similar to the previous test
// case, this also incorrectly fails to warn for the implicit copy ctor.
class InlineImplicitMoveCtorOK {
 public:
  InlineImplicitMoveCtorOK();

 private:
  // ctor weight = 12, dtor weight = 9.
  MyString one_;
  MyString two_;
  MyString three_;
  int four_;
  int five_;
  int six_;
};

class ExplicitlyDefaultedInlineAlsoWarns {
 public:
  ExplicitlyDefaultedInlineAlsoWarns() = default;
  ~ExplicitlyDefaultedInlineAlsoWarns() = default;
  ExplicitlyDefaultedInlineAlsoWarns(
      const ExplicitlyDefaultedInlineAlsoWarns&) = default;

 private:
  MyVector<int> one_;
  MyVector<MyString> two_;

};

#endif  // MISSING_CTOR_H_
