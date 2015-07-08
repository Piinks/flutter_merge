// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'box.dart';
import 'object.dart';
import 'package:cassowary/cassowary.dart' as al;

/// Hosts the edge parameters and vends useful methods to construct expressions
/// for constraints. Also sets up and manages implicit constraints and edit
/// variables. Used as a mixin by layout containers and parent data instances
/// of render boxes taking part in auto layout
abstract class _AutoLayoutParamMixin {
  // Ideally, the edges would all be final, but then they would have to be
  // initialized before the constructor. Not sure how to do that using a Mixin
  al.Param _leftEdge;
  al.Param _rightEdge;
  al.Param _topEdge;
  al.Param _bottomEdge;

  List<al.Constraint> _implicitConstraints;

  al.Param get leftEdge => _leftEdge;
  al.Param get rightEdge => _rightEdge;
  al.Param get topEdge => _topEdge;
  al.Param get bottomEdge => _bottomEdge;

  al.Expression get width => _rightEdge - _leftEdge;
  al.Expression get height => _bottomEdge - _topEdge;

  al.Expression get horizontalCenter => (_leftEdge + _rightEdge) / al.CM(2.0);
  al.Expression get verticalCenter => (_topEdge + _bottomEdge) / al.CM(2.0);

  void _setupLayoutParameters(dynamic context) {
    _leftEdge = new al.Param.withContext(context);
    _rightEdge = new al.Param.withContext(context);
    _topEdge = new al.Param.withContext(context);
    _bottomEdge = new al.Param.withContext(context);
  }

  void _setupEditVariablesInSolver(al.Solver solver, double priority) {
    solver.addEditVariables([
        _leftEdge.variable,
        _rightEdge.variable,
        _topEdge.variable,
        _bottomEdge.variable], priority);
  }

  void _applyEditsAtSize(al.Solver solver, Size size) {
    solver.suggestValueForVariable(_leftEdge.variable, 0.0);
    solver.suggestValueForVariable(_topEdge.variable, 0.0);
    solver.suggestValueForVariable(_bottomEdge.variable, size.height);
    solver.suggestValueForVariable(_rightEdge.variable, size.width);
  }

  void _applyAutolayoutParameterUpdates();
  List<al.Constraint> _constructImplicitConstraints();

  void _setupImplicitConstraints(al.Solver solver) {
    List<al.Constraint> implicit = _constructImplicitConstraints();

    if (implicit == null || implicit.length == 0) {
      return;
    }

    al.Result result = solver.addConstraints(implicit);
    assert(result == al.Result.success);

    _implicitConstraints = implicit;
  }

  void _collectImplicitConstraints(al.Solver solver) {
    if (_implicitConstraints == null || _implicitConstraints.length == 0) {
      return;
    }

    al.Result result = solver.removeConstraints(_implicitConstraints);
    assert(result == al.Result.success);

    _implicitConstraints = null;
  }
}

class AutoLayoutParentData extends BoxParentData
    with ContainerParentDataMixin<RenderBox>, _AutoLayoutParamMixin {

  final RenderBox _renderBox;

  AutoLayoutParentData(this._renderBox) {
    _setupLayoutParameters(this);
  }

  @override
  void _applyAutolayoutParameterUpdates() {
    BoxConstraints box = new BoxConstraints.tightFor(
        width: _rightEdge.value - _leftEdge.value,
        height: _bottomEdge.value - _topEdge.value);

    _renderBox.layout(box, parentUsesSize: false);
    position = new Point(_leftEdge.value, _topEdge.value);
  }

  @override
  List<al.Constraint> _constructImplicitConstraints() {
    return [
      // The left edge must be positive
      _leftEdge >= al.CM(0.0),

      // Width must be positive
      _rightEdge >= _leftEdge,
    ];
  }
}

class RenderAutoLayout extends RenderBox
    with ContainerRenderObjectMixin<RenderBox, AutoLayoutParentData>,
         RenderBoxContainerDefaultsMixin<RenderBox, AutoLayoutParentData>,
         _AutoLayoutParamMixin {

  final al.Solver _solver = new al.Solver();
  List<al.Constraint> _explicitConstraints = new List<al.Constraint>();

  RenderAutoLayout({List<RenderBox> children}) {
    _setupLayoutParameters(this);
    _setupEditVariablesInSolver(_solver, al.Priority.required - 1);

    addAll(children);
  }

  /// Adds all the given constraints to the solver. Either all constraints are
  /// added or none
  al.Result addConstraints(List<al.Constraint> constraints) {
    al.Result result = _solver.addConstraints(constraints);

    if (result == al.Result.success) {
      markNeedsLayout();
      _explicitConstraints.addAll(constraints);
    }

    return result;
  }

  /// Add the given constraint to the solver.
  al.Result addConstraint(al.Constraint constraint) {
    al.Result result = _solver.addConstraint(constraint);

    if (result == al.Result.success) {
      markNeedsLayout();
      _explicitConstraints.add(constraint);
    }

    return result;
  }

  /// Removes all explicitly added constraints.
  al.Result clearAllConstraints() {
    al.Result result = _solver.removeConstraints(_explicitConstraints);

    if (result == al.Result.success) {
      markNeedsLayout();
      _explicitConstraints = new List<al.Constraint>();
    }

    return result;
  }

  @override
  void setupParentData(RenderObject child) {
    if (child.parentData is! AutoLayoutParentData) {
      child.parentData = new AutoLayoutParentData(child);
    }
  }

  @override
  void performLayout() {
    // Step 1: Update dimensions of self
    size = constraints.biggest;
    _applyEditsAtSize(_solver, size);

    // Step 2: Resolve solver updates and flush parameters

    // We don't iterate over the children, instead, we ask the solver to tell
    // us the updated parameters. Attached to the parameters (via the context)
    // are the _AutoLayoutParamMixin instances.
    for (_AutoLayoutParamMixin update in _solver.flushUpdates()) {
      update._applyAutolayoutParameterUpdates();
    }
  }

  @override
  void _applyAutolayoutParameterUpdates() {
    // Nothing to do since the size update has already been presented to the
    // solver as an edit variable modification. The invokation of this method
    // only indicates that the value has been flushed to the variable.
  }

  @override
  void hitTestChildren(HitTestResult result, {Point position}) =>
      defaultHitTestChildren(result, position: position);

  @override
  void paint(PaintingCanvas canvas, Offset offset) =>
      defaultPaint(canvas, offset);

  @override
  void adoptChild(RenderObject child) {
    // Make sure to call super first to setup the parent data
    super.adoptChild(child);
    child.parentData._setupImplicitConstraints(_solver);
  }

  @override
  void dropChild(RenderObject child) {
    child.parentData._collectImplicitConstraints(_solver);

    // Call super last as this collects parent data
    super.dropChild(child);
  }

  @override
  List<al.Constraint> _constructImplicitConstraints() {
    // Only edits are present on layout containers
    return null;
  }
}
