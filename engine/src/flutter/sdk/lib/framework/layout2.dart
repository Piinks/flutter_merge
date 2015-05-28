// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'node.dart';
import 'dart:sky' as sky;

// ABSTRACT LAYOUT

class ParentData {
  void detach() {
    detachSiblings();
  }
  void detachSiblings() { } // workaround for lack of inter-class mixins in Dart
  void merge(ParentData other) {
    // override this in subclasses to merge in data from other into this
    assert(other.runtimeType == this.runtimeType);
  }
}

const kLayoutDirections = 4;

double clamp({double min: 0.0, double value: 0.0, double max: double.INFINITY}) {
  assert(min != null);
  assert(value != null);
  assert(max != null);

  if (value > max)
    value = max;
  if (value < min)
    value = min;
  return value;
}

class RenderNodeDisplayList extends sky.PictureRecorder {
  RenderNodeDisplayList(double width, double height) : super(width, height);
  void paintChild(RenderNode child, sky.Point position) {
    save();
    translate(position.x, position.y);
    child.paint(this);
    restore();
  }
}

abstract class RenderNode extends AbstractNode {

  // LAYOUT

  // parentData is only for use by the RenderNode that actually lays this
  // node out, and any other nodes who happen to know exactly what
  // kind of node that is.
  ParentData parentData;
  void setParentData(RenderNode child) {
    // override this to setup .parentData correctly for your class
    if (child.parentData is! ParentData)
      child.parentData = new ParentData();
  }

  void adoptChild(RenderNode child) { // only for use by subclasses
    // call this whenever you decide a node is a child
    assert(child != null);
    setParentData(child);
    super.adoptChild(child);
  }
  void dropChild(RenderNode child) { // only for use by subclasses
    assert(child != null);
    assert(child.parentData != null);
    child.parentData.detach();
    super.dropChild(child);
  }

  static List<RenderNode> _nodesNeedingLayout = new List<RenderNode>();
  static bool _debugDoingLayout = false;
  bool _needsLayout = true;
  bool get needsLayout => _needsLayout;
  RenderNode _relayoutSubtreeRoot;
  dynamic _constraints;
  dynamic get constraints => _constraints;
  bool debugAncestorsAlreadyMarkedNeedsLayout() {
    if (_relayoutSubtreeRoot == null)
      return true; // we haven't yet done layout even once, so there's nothing for us to do
    RenderNode node = this;
    while (node != _relayoutSubtreeRoot) {
      assert(node._relayoutSubtreeRoot == _relayoutSubtreeRoot);
      assert(node.parent != null);
      node = node.parent as RenderNode;
      if (!node._needsLayout)
        return false;
    }
    assert(node._relayoutSubtreeRoot == node);
    return true;
  }
  void markNeedsLayout() {
    assert(!_debugDoingLayout);
    assert(!_debugDoingPaint);
    if (_needsLayout) {
      assert(debugAncestorsAlreadyMarkedNeedsLayout());
      return;
    }
    _needsLayout = true;
    assert(_relayoutSubtreeRoot != null);
    if (_relayoutSubtreeRoot != this) {
      assert(parent is RenderNode);
      parent.markNeedsLayout();
    } else {
      _nodesNeedingLayout.add(this);
    }
  }
  static void flushLayout() {
    _debugDoingLayout = true;
    List<RenderNode> dirtyNodes = _nodesNeedingLayout;
    _nodesNeedingLayout = new List<RenderNode>();
    dirtyNodes..sort((a, b) => a.depth - b.depth)..forEach((node) {
      if (node._needsLayout && node.attached)
        node._doLayout();
    });
    _debugDoingLayout = false;
  }
  void _doLayout() {
    try {
      assert(_relayoutSubtreeRoot == this);
      performLayout();
    } catch (e, stack) {
      print('Exception raised during layout of ${this}: ${e}');
      print(stack);
      return;
    }
    _needsLayout = false;
  }
  void layout(dynamic constraints, { bool parentUsesSize: false }) {
    RenderNode relayoutSubtreeRoot;
    if (!parentUsesSize || sizedByParent || parent is! RenderNode)
      relayoutSubtreeRoot = this;
    else
      relayoutSubtreeRoot = parent._relayoutSubtreeRoot;
    if (!needsLayout && constraints == _constraints && relayoutSubtreeRoot == _relayoutSubtreeRoot)
      return;
    _constraints = constraints;
    _relayoutSubtreeRoot = relayoutSubtreeRoot;
    if (sizedByParent)
      performResize();
    performLayout();
    _needsLayout = false;
    markNeedsPaint();
  }
  bool get sizedByParent => false; // return true if the constraints are the only input to the sizing algorithm (in particular, child nodes have no impact)
  void performResize(); // set the local dimensions, using only the constraints (only called if sizedByParent is true)
  void performLayout();
    // Override this to perform relayout without your parent's
    // involvement.
    //
    // This is called during layout. If sizedByParent is true, then
    // performLayout() should not change your dimensions, only do that
    // in performResize(). If sizedByParent is false, then set both
    // your dimensions and do your children's layout here.
    //
    // When calling layout() on your children, pass in
    // "parentUsesSize: true" if your size or layout is dependent on
    // your child's size.

  // when the parent has rotated (e.g. when the screen has been turned
  // 90 degrees), immediately prior to layout() being called for the
  // new dimensions, rotate() is called with the old and new angles.
  // The next time paint() is called, the coordinate space will have
  // been rotated N quarter-turns clockwise, where:
  //    N = newAngle-oldAngle
  // ...but the rendering is expected to remain the same, pixel for
  // pixel, on the output device. Then, the layout() method or
  // equivalent will be invoked.

  void rotate({
    int oldAngle, // 0..3
    int newAngle, // 0..3
    Duration time
  }) { }


  // PAINTING

  static bool _debugDoingPaint = false;
  void markNeedsPaint() {
    assert(!_debugDoingPaint);
    // TODO(abarth): It's very redundant to call this for every node in the
    // render tree during layout. We should instead compute a summary bit and
    // call it once at the end of layout.
    sky.view.scheduleFrame();
  }
  void paint(RenderNodeDisplayList canvas) { }


  // HIT TESTING

  void handlePointer(sky.PointerEvent event) {
    // override this if you have a client, to hand it to the client
    // override this if you want to do anything with the pointer event
  }

  // RenderNode subclasses are expected to have a method like the
  // following (with the signature being whatever passes for coordinates
  // for this particular class):
  // bool hitTest(HitTestResult result, { sky.Point position }) {
  //   // If (x,y) is not inside this node, then return false. (You
  //   // can assume that the given coordinate is inside your
  //   // dimensions. You only need to check this if you're an
  //   // irregular shape, e.g. if you have a hole.)
  //   // Otherwise:
  //   // For each child that intersects x,y, in z-order starting from the top,
  //   // call hitTest() for that child, passing it /result/, and the coordinates
  //   // converted to the child's coordinate origin, and stop at the first child
  //   // that returns true.
  //   // Then, add yourself to /result/, and return true.
  // }
  // You must not add yourself to /result/ if you return false.

}

class HitTestResult {
  final List<RenderNode> path = new List<RenderNode>();

  RenderNode get result => path.first;

  void add(RenderNode node) {
    path.add(node);
  }
}


// GENERIC MIXIN FOR RENDER NODES WITH ONE CHILD

abstract class RenderNodeWithChildMixin<ChildType extends RenderNode> {
  ChildType _child;
  ChildType get child => _child;
  void set child (ChildType value) {
    if (_child != null)
      dropChild(_child);
    _child = value;
    if (_child != null)
      adoptChild(_child);
    markNeedsLayout();
  }
}


// GENERIC MIXIN FOR RENDER NODES WITH A LIST OF CHILDREN

abstract class ContainerParentDataMixin<ChildType extends RenderNode> {
  ChildType previousSibling;
  ChildType nextSibling;
  void detachSiblings() {
    if (previousSibling != null) {
      assert(previousSibling.parentData is ContainerParentDataMixin<ChildType>);
      assert(previousSibling != this);
      assert(previousSibling.parentData.nextSibling == this);
      previousSibling.parentData.nextSibling = nextSibling;
    }
    if (nextSibling != null) {
      assert(nextSibling.parentData is ContainerParentDataMixin<ChildType>);
      assert(nextSibling != this);
      assert(nextSibling.parentData.previousSibling == this);
      nextSibling.parentData.previousSibling = previousSibling;
    }
    previousSibling = null;
    nextSibling = null;
  }
}

abstract class ContainerRenderNodeMixin<ChildType extends RenderNode, ParentDataType extends ContainerParentDataMixin<ChildType>> implements RenderNode {
  // abstract class that has only InlineNode children

  bool _debugUltimatePreviousSiblingOf(ChildType child, { ChildType equals }) {
    assert(child.parentData is ParentDataType);
    while (child.parentData.previousSibling != null) {
      assert(child.parentData.previousSibling != child);
      child = child.parentData.previousSibling;
      assert(child.parentData is ParentDataType);
    }
    return child == equals;
  }
  bool _debugUltimateNextSiblingOf(ChildType child, { ChildType equals }) {
    assert(child.parentData is ParentDataType);
    while (child.parentData.nextSibling != null) {
      assert(child.parentData.nextSibling != child);
      child = child.parentData.nextSibling;
      assert(child.parentData is ParentDataType);
    }
    return child == equals;
  }

  ChildType _firstChild;
  ChildType _lastChild;
  void add(ChildType child, { ChildType before }) {
    assert(child != this);
    assert(before != this);
    assert(child != before);
    assert(child != _firstChild);
    assert(child != _lastChild);
    adoptChild(child);
    assert(child.parentData is ParentDataType);
    assert(child.parentData.nextSibling == null);
    assert(child.parentData.previousSibling == null);
    if (before == null) {
      // append at the end (_lastChild)
      child.parentData.previousSibling = _lastChild;
      if (_lastChild != null) {
        assert(_lastChild.parentData is ParentDataType);
        _lastChild.parentData.nextSibling = child;
      }
      _lastChild = child;
      if (_firstChild == null)
        _firstChild = child;
    } else {
      assert(_firstChild != null);
      assert(_lastChild != null);
      assert(_debugUltimatePreviousSiblingOf(before, equals: _firstChild));
      assert(_debugUltimateNextSiblingOf(before, equals: _lastChild));
      assert(before.parentData is ParentDataType);
      if (before.parentData.previousSibling == null) {
        // insert at the start (_firstChild); we'll end up with two or more children
        assert(before == _firstChild);
        child.parentData.nextSibling = before;
        before.parentData.previousSibling = child;
        _firstChild = child;
      } else {
        // insert in the middle; we'll end up with three or more children
        // set up links from child to siblings
        child.parentData.previousSibling = before.parentData.previousSibling;
        child.parentData.nextSibling = before;
        // set up links from siblings to child
        assert(child.parentData.previousSibling.parentData is ParentDataType);
        assert(child.parentData.nextSibling.parentData is ParentDataType);
        child.parentData.previousSibling.parentData.nextSibling = child;
        child.parentData.nextSibling.parentData.previousSibling = child;
        assert(before.parentData.previousSibling == child);
      }
    }
    markNeedsLayout();
  }
  void remove(ChildType child) {
    assert(child.parentData is ParentDataType);
    assert(_debugUltimatePreviousSiblingOf(child, equals: _firstChild));
    assert(_debugUltimateNextSiblingOf(child, equals: _lastChild));
    if (child.parentData.previousSibling == null) {
      assert(_firstChild == child);
      _firstChild = child.parentData.nextSibling;
    } else {
      assert(child.parentData.previousSibling.parentData is ParentDataType);
      child.parentData.previousSibling.parentData.nextSibling = child.parentData.nextSibling;
    }
    if (child.parentData.nextSibling == null) {
      assert(_lastChild == child);
      _lastChild = child.parentData.previousSibling;
    } else {
      assert(child.parentData.nextSibling.parentData is ParentDataType);
      child.parentData.nextSibling.parentData.previousSibling = child.parentData.previousSibling;
    }
    child.parentData.previousSibling = null;
    child.parentData.nextSibling = null;
    dropChild(child);
    markNeedsLayout();
  }
  void redepthChildren() {
    ChildType child = _firstChild;
    while (child != null) {
      redepthChild(child);
      assert(child.parentData is ParentDataType);
      child = child.parentData.nextSibling;
    }
  }
  void attachChildren() {
    ChildType child = _firstChild;
    while (child != null) {
      child.attach();
      assert(child.parentData is ParentDataType);
      child = child.parentData.nextSibling;
    }
  }
  void detachChildren() {
    ChildType child = _firstChild;
    while (child != null) {
      child.detach();
      assert(child.parentData is ParentDataType);
      child = child.parentData.nextSibling;
    }
  }

  ChildType get firstChild => _firstChild;
  ChildType get lastChild => _lastChild;
  ChildType childAfter(ChildType child) {
    assert(child.parentData is ParentDataType);
    return child.parentData.nextSibling;
  }

}


// GENERIC BOX RENDERING
// Anything that has a concept of x, y, width, height is going to derive from this

class EdgeDims {
  // used for e.g. padding
  const EdgeDims(this.top, this.right, this.bottom, this.left);
  final double top;
  final double right;
  final double bottom;
  final double left;
  operator ==(EdgeDims other) => (top == other.top) ||
                                 (right == other.right) ||
                                 (bottom == other.bottom) ||
                                 (left == other.left);
}

class BoxConstraints {
  const BoxConstraints({
    this.minWidth: 0.0,
    this.maxWidth: double.INFINITY,
    this.minHeight: 0.0,
    this.maxHeight: double.INFINITY});

  BoxConstraints.tight(sky.Size size)
    : minWidth = size.width,
      maxWidth = size.width,
      minHeight = size.height,
      maxHeight = size.height;

  BoxConstraints deflate(EdgeDims edges) {
    assert(edges != null);
    return new BoxConstraints(
      minWidth: minWidth,
      maxWidth: maxWidth - (edges.left + edges.right),
      minHeight: minHeight,
      maxHeight: maxHeight - (edges.top + edges.bottom)
    );
  }

  final double minWidth;
  final double maxWidth;
  final double minHeight;
  final double maxHeight;

  double constrainWidth(double width) {
    return clamp(min: minWidth, max: maxWidth, value: width);
  }

  double constrainHeight(double height) {
    return clamp(min: minHeight, max: maxHeight, value: height);
  }

  sky.Size constrain(sky.Size size) {
    return new sky.Size(constrainWidth(size.width), constrainHeight(size.height));
  }

  bool get isInfinite => maxWidth >= double.INFINITY || maxHeight >= double.INFINITY;
}

class BoxParentData extends ParentData {
  sky.Point position = new sky.Point(0.0, 0.0);
}

abstract class RenderBox extends RenderNode {

  void setParentData(RenderNode child) {
    if (child.parentData is! BoxParentData)
      child.parentData = new BoxParentData();
  }

  // override this to report what dimensions you would have if you
  // were laid out with the given constraints this can walk the tree
  // if it must, but it should be as cheap as possible; just get the
  // dimensions and nothing else (e.g. don't calculate hypothetical
  // child positions if they're not needed to determine dimensions)
  sky.Size getIntrinsicDimensions(BoxConstraints constraints) {
    return constraints.constrain(new sky.Size(0.0, 0.0));
  }

  BoxConstraints get constraints => super.constraints as BoxConstraints;
  void performResize() {
    // default behaviour for subclasses that have sizedByParent = true
    size = constraints.constrain(new sky.Size(0.0, 0.0));
    assert(size.height < double.INFINITY);
    assert(size.width < double.INFINITY);
  }
  void performLayout() {
    // descendants have to either override performLayout() to set both
    // width and height and lay out children, or, set sizedByParent to
    // true so that performResize()'s logic above does its thing.
    assert(sizedByParent);
  }

  bool hitTest(HitTestResult result, { sky.Point position }) {
    hitTestChildren(result, position: position);
    result.add(this);
    return true;
  }
  void hitTestChildren(HitTestResult result, { sky.Point position }) { }

  sky.Size size = new sky.Size(0.0, 0.0);
}

abstract class RenderProxyBox extends RenderBox with RenderNodeWithChildMixin<RenderBox> {
  RenderProxyBox(RenderBox child) {
    this.child = child;
  }

  sky.Size getIntrinsicDimensions(BoxConstraints constraints) {
    if (child != null)
      return child.getIntrinsicDimensions(constraints);
    return super.getIntrinsicDimensions(constraints);
  }

  void performLayout() {
    if (child != null) {
      child.layout(constraints, parentUsesSize: true);
      size = child.size;
    } else {
      performResize();
    }
  }

  void hitTestChildren(HitTestResult result, { sky.Point position }) {
    if (child != null)
      child.hitTest(result, position: position);
    else
      super.hitTestChildren(result, position: position);
  }

  void paint(RenderNodeDisplayList canvas) {
    if (child != null)
      child.paint(canvas);
  }
}

class RenderSizedBox extends RenderProxyBox {
  final sky.Size desiredSize;

  RenderSizedBox({
    RenderBox child, 
    this.desiredSize: const sky.Size.infinite()
  }) : super(child);

  sky.Size getIntrinsicDimensions(BoxConstraints constraints) {
    return constraints.constrain(desiredSize);
  }

  void performLayout() {
    size = constraints.constrain(desiredSize);
    child.layout(new BoxConstraints.tight(size));
  }
}

class RenderPadding extends RenderBox with RenderNodeWithChildMixin<RenderBox> {

  RenderPadding(EdgeDims padding, RenderBox child) {
    assert(padding != null);
    this.padding = padding;
    this.child = child;
  }

  EdgeDims _padding;
  EdgeDims get padding => _padding;
  void set padding (EdgeDims value) {
    assert(value != null);
    if (_padding != value) {
      _padding = value;
      markNeedsLayout();
    }
  }

  sky.Size getIntrinsicDimensions(BoxConstraints constraints) {
    assert(padding != null);
    constraints = constraints.deflate(padding);
    if (child == null)
      return super.getIntrinsicDimensions(constraints);
    return child.getIntrinsicDimensions(constraints);
  }

  void performLayout() {
    assert(padding != null);
    BoxConstraints innerConstraints = constraints.deflate(padding);
    if (child == null) {
      size = innerConstraints.constrain(
          new sky.Size(padding.left + padding.right, padding.top + padding.bottom));
      return;
    }
    child.layout(innerConstraints, parentUsesSize: true);
    assert(child.parentData is BoxParentData);
    child.parentData.position = new sky.Point(padding.left, padding.top);
    size = constraints.constrain(new sky.Size(padding.left + child.size.width + padding.right,
                                              padding.top + child.size.height + padding.bottom));
  }

  void paint(RenderNodeDisplayList canvas) {
    if (child != null)
      canvas.paintChild(child, child.parentData.position);
  }

  void hitTestChildren(HitTestResult result, { sky.Point position }) {
    if (child != null) {
      assert(child.parentData is BoxParentData);
      sky.Rect childBounds = new sky.Rect.fromPointAndSize(child.parentData.position, child.size);
      if (childBounds.contains(position)) {
        child.hitTest(result, position: new sky.Point(position.x - child.parentData.position.x,
                                                      position.y - child.parentData.position.y));
      }
    }
  }

}

// This must be immutable, because we won't notice when it changes
class BoxDecoration {
  const BoxDecoration({
    this.backgroundColor
  });

  final int backgroundColor;
}

class RenderDecoratedBox extends RenderProxyBox {

  RenderDecoratedBox({
    BoxDecoration decoration,
    RenderBox child
  }) : _decoration = decoration, super(child);

  BoxDecoration _decoration;
  BoxDecoration get decoration => _decoration;
  void set decoration (BoxDecoration value) {
    if (value == _decoration)
      return;
    _decoration = value;
    markNeedsPaint();
  }

  void paint(RenderNodeDisplayList canvas) {
    assert(size.width != null);
    assert(size.height != null);

    if (_decoration == null)
      return;

    if (_decoration.backgroundColor != null) {
      sky.Paint paint = new sky.Paint()..color = _decoration.backgroundColor;
      canvas.drawRect(new sky.Rect.fromLTRB(0.0, 0.0, size.width, size.height), paint);
    }
    super.paint(canvas);
  }

}


// RENDER VIEW LAYOUT MANAGER

class ViewConstraints {

  const ViewConstraints({
    this.width: 0.0, this.height: 0.0, this.orientation: null
  });

  final double width;
  final double height;
  final int orientation;

}

class RenderView extends RenderNode with RenderNodeWithChildMixin<RenderBox> {

  RenderView({
    RenderBox child,
    this.timeForRotation: const Duration(microseconds: 83333)
  }) {
    this.child = child;
  }

  sky.Size _size = new sky.Size(0.0, 0.0);
  double get width => _size.width;
  double get height => _size.height;

  int _orientation; // 0..3
  int get orientation => _orientation;
  Duration timeForRotation;

  ViewConstraints get constraints => super.constraints as ViewConstraints;
  bool get sizedByParent => true;
  void performResize() {
    if (constraints.orientation != _orientation) {
      if (_orientation != null && child != null)
        child.rotate(oldAngle: _orientation, newAngle: constraints.orientation, time: timeForRotation);
      _orientation = constraints.orientation;
    }
    _size = new sky.Size(constraints.width, constraints.height);
    assert(_size.height < double.INFINITY);
    assert(_size.width < double.INFINITY);
  }
  void performLayout() {
    if (child != null) {
      child.layout(new BoxConstraints.tight(_size));
      assert(child.size.width == width);
      assert(child.size.height == height);
    }
  }

  void rotate({ int oldAngle, int newAngle, Duration time }) {
    assert(false); // nobody tells the screen to rotate, the whole rotate() dance is started from our performResize()
  }

  bool hitTest(HitTestResult result, { sky.Point position }) {
    if (child != null) {
      sky.Rect childBounds = new sky.Rect.fromSize(child.size);
      if (childBounds.contains(position))
        child.hitTest(result, position: position);
    }
    result.add(this);
    return true;
  }

  void paint(RenderNodeDisplayList canvas) {
    if (child != null)
      canvas.paintChild(child, new sky.Point(0.0, 0.0));
  }

  void paintFrame() {
    RenderNode._debugDoingPaint = true;
    var canvas = new RenderNodeDisplayList(sky.view.width, sky.view.height);
    paint(canvas);
    sky.view.picture = canvas.endRecording();
    RenderNode._debugDoingPaint = false;
  }

}

// DEFAULT BEHAVIORS FOR RENDERBOX CONTAINERS
abstract class RenderBoxContainerDefaultsMixin<ChildType extends RenderBox, ParentDataType extends ContainerParentDataMixin<ChildType>> implements ContainerRenderNodeMixin<ChildType, ParentDataType> {

  void defaultHitTestChildren(HitTestResult result, { sky.Point position }) {
    // the x, y parameters have the top left of the node's box as the origin
    ChildType child = lastChild;
    while (child != null) {
      assert(child.parentData is BoxParentData);
      sky.Rect childBounds = new sky.Rect.fromPointAndSize(child.parentData.position, child.size);
      if (childBounds.contains(position)) {
        if (child.hitTest(result, position: new sky.Point(position.x - child.parentData.position.x,
                                                          position.y - child.parentData.position.y)))
          break;
      }
      child = child.parentData.previousSibling;
    }
  }

  void defaultPaint(RenderNodeDisplayList canvas) {
    RenderBox child = firstChild;
    while (child != null) {
      assert(child.parentData is BoxParentData);
      canvas.paintChild(child, child.parentData.position);
      child = child.parentData.nextSibling;
    }
  }
}

// BLOCK LAYOUT MANAGER

class BlockParentData extends BoxParentData with ContainerParentDataMixin<RenderBox> { }

class RenderBlock extends RenderBox with ContainerRenderNodeMixin<RenderBox, BlockParentData>,
                                         RenderBoxContainerDefaultsMixin<RenderBox, BlockParentData> {
  // lays out RenderBox children in a vertical stack
  // uses the maximum width provided by the parent
  // sizes itself to the height of its child stack

  void setParentData(RenderBox child) {
    if (child.parentData is! BlockParentData)
      child.parentData = new BlockParentData();
  }

  // override this to report what dimensions you would have if you
  // were laid out with the given constraints this can walk the tree
  // if it must, but it should be as cheap as possible; just get the
  // dimensions and nothing else (e.g. don't calculate hypothetical
  // child positions if they're not needed to determine dimensions)
  sky.Size getIntrinsicDimensions(BoxConstraints constraints) {
    double height = 0.0;
    double width = constraints.constrainWidth(constraints.maxWidth);
    assert(width < double.INFINITY);
    RenderBox child = firstChild;
    BoxConstraints innerConstraints = new BoxConstraints(minWidth: width,
                                                         maxWidth: width);
    while (child != null) {
      height += child.getIntrinsicDimensions(innerConstraints).height;
      assert(child.parentData is BlockParentData);
      child = child.parentData.nextSibling;
    }

    return new sky.Size(width, constraints.constrainHeight(height));
  }

  void performLayout() {
    assert(constraints is BoxConstraints);
    double width = constraints.constrainWidth(constraints.maxWidth);
    double y = 0.0;
    RenderBox child = firstChild;
    while (child != null) {
      child.layout(new BoxConstraints(minWidth: width, maxWidth: width), parentUsesSize: true);
      assert(child.parentData is BlockParentData);
      child.parentData.position = new sky.Point(0.0, y);
      y += child.size.height;
      child = child.parentData.nextSibling;
    }
    size = new sky.Size(width, constraints.constrainHeight(y));
    assert(size.width < double.INFINITY);
    assert(size.height < double.INFINITY);
  }

  void hitTestChildren(HitTestResult result, { sky.Point position }) {
    defaultHitTestChildren(result, position: position);
  }

  void paint(RenderNodeDisplayList canvas) {
    defaultPaint(canvas);
  }

}

// FLEXBOX LAYOUT MANAGER

class FlexBoxParentData extends BoxParentData with ContainerParentDataMixin<RenderBox> {
  int flex;
  void merge(FlexBoxParentData other) {
    if (other.flex != null)
      flex = other.flex;
    super.merge(other);
  }
}

enum FlexDirection { Horizontal, Vertical }

class RenderFlex extends RenderBox with ContainerRenderNodeMixin<RenderBox, FlexBoxParentData>,
                                        RenderBoxContainerDefaultsMixin<RenderBox, BlockParentData> {
  // lays out RenderBox children using flexible layout

  RenderFlex({
    FlexDirection direction: FlexDirection.Horizontal
  }) : _direction = direction;

  FlexDirection _direction;
  FlexDirection get direction => _direction;
  void set direction (FlexDirection value) {
    if (_direction != value) {
      _direction = value;
      markNeedsLayout();
    }
  }

  void setParentData(RenderBox child) {
    if (child.parentData is! FlexBoxParentData)
      child.parentData = new FlexBoxParentData();
  }

  bool get sizedByParent => true;
  void performResize() {
    size = _constraints.constrain(new sky.Size(_constraints.maxWidth, _constraints.maxHeight));
    assert(size.height < double.INFINITY);
    assert(size.width < double.INFINITY);
  }

  int _getFlex(RenderBox child) {
    assert(child.parentData is FlexBoxParentData);
    return child.parentData.flex != null ? child.parentData.flex : 0;
  }

  void performLayout() {
    // Based on http://www.w3.org/TR/css-flexbox-1/ Section 9.7 Resolving Flexible Lengths
    // Steps 1-3. Determine used flex factor, size inflexible items, calculate free space
    int totalFlex = 0;
    assert(constraints != null);
    double freeSpace = (_direction == FlexDirection.Horizontal) ? constraints.maxWidth : constraints.maxHeight;
    RenderBox child = firstChild;
    while (child != null) {
      int flex = _getFlex(child);
      if (flex > 0) {
        totalFlex += child.parentData.flex;
      } else {
        BoxConstraints innerConstraints = new BoxConstraints(maxHeight: constraints.maxHeight,
                                                             maxWidth: constraints.maxWidth);
        child.layout(innerConstraints, parentUsesSize: true);
        freeSpace -= (_direction == FlexDirection.Horizontal) ? child.size.width : child.size.height;
      }
      child = child.parentData.nextSibling;
    }

    // Steps 4-5. Distribute remaining space to flexible children.
    double spacePerFlex = totalFlex > 0 ? (freeSpace / totalFlex) : 0.0;
    double usedSpace = 0.0;
    child = firstChild;
    while (child != null) {
      int flex = _getFlex(child);
      if (flex > 0) {
        double spaceForChild = spacePerFlex * flex;
        BoxConstraints innerConstraints;
        switch (_direction) {
          case FlexDirection.Horizontal:
            innerConstraints = new BoxConstraints(maxHeight: constraints.maxHeight,
                                                  minWidth: spaceForChild,
                                                  maxWidth: spaceForChild);
            break;
          case FlexDirection.Vertical:
            innerConstraints = new BoxConstraints(minHeight: spaceForChild,
                                                  maxHeight: spaceForChild,
                                                  maxWidth: constraints.maxWidth);
            break;
        }
        child.layout(innerConstraints, parentUsesSize: true);
      }

      // For now, center the flex items in the cross direction
      switch (_direction) {
        case FlexDirection.Horizontal:
          child.parentData.position = new sky.Point(usedSpace, size.height / 2.0 - child.size.height / 2.0);
          usedSpace += child.size.width;
          break;
        case FlexDirection.Vertical:
          child.parentData.position = new sky.Point(size.width / 2.0 - child.size.width / 2.0, usedSpace);
          usedSpace += child.size.height;
          break;
      }
      child = child.parentData.nextSibling;
    }
  }

  void hitTestChildren(HitTestResult result, { sky.Point position }) {
    defaultHitTestChildren(result, position: position);
  }

  void paint(RenderNodeDisplayList canvas) {
    defaultPaint(canvas);
  }
}

class RenderInline extends RenderNode {
  String data;

  RenderInline(this.data);
}

class RenderParagraph extends RenderBox {

  RenderParagraph({
    String text,
    int color
  }) : _color = color {
    _layoutRoot.rootElement = _document.createElement('p');
    this.text = text;
  }

  final sky.Document _document = new sky.Document();
  final sky.LayoutRoot _layoutRoot = new sky.LayoutRoot();

  String get text => (_layoutRoot.rootElement.firstChild as sky.Text).data;
  void set text (String value) {
    _layoutRoot.rootElement.setChild(_document.createText(value));
    markNeedsLayout();
  }

  int _color = 0xFF000000;
  int get color => _color;
  void set color (int value) {
    if (_color != value) {
      _color = value;
      markNeedsPaint();
    }
  }

  sky.Size getIntrinsicDimensions(BoxConstraints constraints) {
    assert(false);
    return null;
    // we don't currently support this for RenderParagraph
  }

  void performLayout() {
    _layoutRoot.maxWidth = constraints.maxWidth;
    _layoutRoot.minWidth = constraints.minWidth;
    _layoutRoot.minHeight = constraints.minHeight;
    _layoutRoot.maxHeight = constraints.maxHeight;
    _layoutRoot.layout();
    size = constraints.constrain(new sky.Size(_layoutRoot.rootElement.width, _layoutRoot.rootElement.height));
  }

  void paint(RenderNodeDisplayList canvas) {
    // _layoutRoot.rootElement.style['color'] = 'rgba(' + ...color... + ')';
    _layoutRoot.paint(canvas);
  }

  // we should probably expose a way to do precise (inter-glpyh) hit testing

}
