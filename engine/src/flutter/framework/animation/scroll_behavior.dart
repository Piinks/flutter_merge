// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:math' as math;
import 'mechanics.dart';
import 'generators.dart';

const double _kScrollFriction = 0.005;
const double _kOverscrollFriction = 0.075;
const double _kBounceSlopeAngle = math.PI / 512.0; // radians

abstract class ScrollBehavior {
  Simulation release(Particle particle) => null;

  // Returns the new scroll offset.
  double applyCurve(double scrollOffset, double scrollDelta);
}

class BoundedScrollBehavior extends ScrollBehavior {
  double minOffset;
  double maxOffset;

  BoundedScrollBehavior({this.minOffset: 0.0, this.maxOffset});

  double applyCurve(double scrollOffset, double scrollDelta) {
    double newScrollOffset = scrollOffset + scrollDelta;
    if (minOffset != null)
      newScrollOffset = math.max(minOffset, newScrollOffset);
    if (maxOffset != null)
      newScrollOffset = math.min(maxOffset, newScrollOffset);
    return newScrollOffset;
  }
}

class OverscrollBehavior extends ScrollBehavior {
  Simulation release(Particle particle) {
    System system;
    if (particle.position >= 0.0) {
      if (particle.velocity == 0.0)
        return null;
      System slowdownSystem = new ParticleInBoxWithFriction(
        particle: particle,
        friction: _kScrollFriction,
        box: new GeofenceBox(min: 0.0, onEscape: () {
          (system as Multisystem).transitionToSystem(new ParticleInBoxWithFriction(
            particle: particle,
            friction: _kOverscrollFriction,
            box: new ClosedBox(),
            onStop: () => (system as Multisystem).transitionToSystem(getBounceBackSystem(particle))
          ));
        }));
      system = new Multisystem(particle: particle, system: slowdownSystem);
    } else {
      system = getBounceBackSystem(particle);
    }
    return new Simulation(system, terminationCondition: () => particle.position == 0.0);
  }

  System getBounceBackSystem(Particle particle) {
    return new ParticleClimbingRamp(
        particle: particle,
        box: new ClosedBox(max: 0.0),
        theta: _kBounceSlopeAngle,
        targetPosition: 0.0);
  }

  double applyCurve(double scrollOffset, double scrollDelta) {
    double newScrollOffset = scrollOffset + scrollDelta;
    if (newScrollOffset < 0.0) {
      // If we're overscrolling, we want move the scroll offset 2x slower than
      // we would otherwise. Therefore, we "rewind" the newScrollOffset by half
      // the amount that we moved it above. Notice that we clap the "old" value
      // to 0.0 so that we only reduce the portion of scrollDelta that's applied
      // beyond 0.0.
      newScrollOffset -= (newScrollOffset - math.min(0.0, scrollOffset)) / 2.0;
    }
    return newScrollOffset;
  }
}
