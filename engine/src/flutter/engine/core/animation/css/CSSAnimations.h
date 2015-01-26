/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SKY_ENGINE_CORE_ANIMATION_CSS_CSSANIMATIONS_H_
#define SKY_ENGINE_CORE_ANIMATION_CSS_CSSANIMATIONS_H_

#include "sky/engine/core/animation/Animation.h"
#include "sky/engine/core/animation/AnimationPlayer.h"
#include "sky/engine/core/animation/InertAnimation.h"
#include "sky/engine/core/animation/Interpolation.h"
#include "sky/engine/core/css/StylePropertySet.h"
#include "sky/engine/core/dom/Document.h"
#include "sky/engine/core/rendering/style/RenderStyleConstants.h"
#include "sky/engine/wtf/HashMap.h"
#include "sky/engine/wtf/Vector.h"
#include "sky/engine/wtf/text/AtomicString.h"

namespace blink {

class CSSTransitionData;
class Element;
class StylePropertyShorthand;

// This class stores the CSS Animations/Transitions information we use during a style recalc.
// This includes updates to animations/transitions as well as the Interpolations to be applied.
class CSSAnimationUpdate final {
public:
    void startAnimation(AtomicString& animationName, PassRefPtr<InertAnimation> animation)
    {
        NewAnimation newAnimation;
        newAnimation.name = animationName;
        newAnimation.animation = animation;
        m_newAnimations.append(newAnimation);
    }
    // Returns whether player has been cancelled and should be filtered during style application.
    bool isCancelledAnimation(const AnimationPlayer* player) const { return m_cancelledAnimationPlayers.contains(player); }
    void cancelAnimation(const AtomicString& name, AnimationPlayer& player)
    {
        m_cancelledAnimationNames.append(name);
        m_cancelledAnimationPlayers.add(&player);
    }
    void toggleAnimationPaused(const AtomicString& name)
    {
        m_animationsWithPauseToggled.append(name);
    }

    void startTransition(CSSPropertyID id, CSSPropertyID eventId, const AnimatableValue* from, const AnimatableValue* to, PassRefPtr<InertAnimation> animation)
    {
        NewTransition newTransition;
        newTransition.id = id;
        newTransition.eventId = eventId;
        newTransition.from = from;
        newTransition.to = to;
        newTransition.animation = animation;
        m_newTransitions.set(id, newTransition);
    }
    bool isCancelledTransition(CSSPropertyID id) const { return m_cancelledTransitions.contains(id); }
    void cancelTransition(CSSPropertyID id) { m_cancelledTransitions.add(id); }

    struct NewAnimation {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        AtomicString name;
        RefPtr<InertAnimation> animation;
    };
    const Vector<NewAnimation>& newAnimations() const { return m_newAnimations; }
    const Vector<AtomicString>& cancelledAnimationNames() const { return m_cancelledAnimationNames; }
    const HashSet<RawPtr<const AnimationPlayer> >& cancelledAnimationAnimationPlayers() const { return m_cancelledAnimationPlayers; }
    const Vector<AtomicString>& animationsWithPauseToggled() const { return m_animationsWithPauseToggled; }

    struct NewTransition {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        CSSPropertyID id;
        CSSPropertyID eventId;
        RawPtr<const AnimatableValue> from;
        RawPtr<const AnimatableValue> to;
        RefPtr<InertAnimation> animation;
    };
    typedef HashMap<CSSPropertyID, NewTransition> NewTransitionMap;
    const NewTransitionMap& newTransitions() const { return m_newTransitions; }
    const HashSet<CSSPropertyID>& cancelledTransitions() const { return m_cancelledTransitions; }

    void adoptActiveInterpolationsForAnimations(HashMap<CSSPropertyID, RefPtr<Interpolation> >& newMap) { newMap.swap(m_activeInterpolationsForAnimations); }
    void adoptActiveInterpolationsForTransitions(HashMap<CSSPropertyID, RefPtr<Interpolation> >& newMap) { newMap.swap(m_activeInterpolationsForTransitions); }
    const HashMap<CSSPropertyID, RefPtr<Interpolation> >& activeInterpolationsForAnimations() const { return m_activeInterpolationsForAnimations; }
    const HashMap<CSSPropertyID, RefPtr<Interpolation> >& activeInterpolationsForTransitions() const { return m_activeInterpolationsForTransitions; }
    HashMap<CSSPropertyID, RefPtr<Interpolation> >& activeInterpolationsForAnimations() { return m_activeInterpolationsForAnimations; }

    bool isEmpty() const
    {
        return m_newAnimations.isEmpty()
            && m_cancelledAnimationNames.isEmpty()
            && m_cancelledAnimationPlayers.isEmpty()
            && m_animationsWithPauseToggled.isEmpty()
            && m_newTransitions.isEmpty()
            && m_cancelledTransitions.isEmpty()
            && m_activeInterpolationsForAnimations.isEmpty()
            && m_activeInterpolationsForTransitions.isEmpty();
    }

private:
    // Order is significant since it defines the order in which new animations
    // will be started. Note that there may be multiple animations present
    // with the same name, due to the way in which we split up animations with
    // incomplete keyframes.
    Vector<NewAnimation> m_newAnimations;
    Vector<AtomicString> m_cancelledAnimationNames;
    HashSet<RawPtr<const AnimationPlayer> > m_cancelledAnimationPlayers;
    Vector<AtomicString> m_animationsWithPauseToggled;

    NewTransitionMap m_newTransitions;
    HashSet<CSSPropertyID> m_cancelledTransitions;

    HashMap<CSSPropertyID, RefPtr<Interpolation> > m_activeInterpolationsForAnimations;
    HashMap<CSSPropertyID, RefPtr<Interpolation> > m_activeInterpolationsForTransitions;
};

class CSSAnimations final {
    WTF_MAKE_NONCOPYABLE(CSSAnimations);
    DISALLOW_ALLOCATION();
public:
    CSSAnimations();

    static const StylePropertyShorthand& animatableProperties();
    static bool isAllowedAnimation(CSSPropertyID);
    // FIXME: We should change the Element* to a const Element*
    static PassOwnPtr<CSSAnimationUpdate> calculateUpdate(Element*, const Element& parentElement, const RenderStyle&, RenderStyle* parentStyle);

    void setPendingUpdate(PassOwnPtr<CSSAnimationUpdate> update) { m_pendingUpdate = update; }
    void maybeApplyPendingUpdate(Element*);
    bool isEmpty() const { return m_animations.isEmpty() && m_transitions.isEmpty() && !m_pendingUpdate; }
    void cancel();

private:
    struct RunningTransition {
        ALLOW_ONLY_INLINE_ALLOCATION();
    public:
        RefPtr<AnimationPlayer> player;
        RawPtr<const AnimatableValue> from;
        RawPtr<const AnimatableValue> to;
    };

    typedef HashMap<AtomicString, RefPtr<AnimationPlayer> > AnimationMap;
    AnimationMap m_animations;

    typedef HashMap<CSSPropertyID, RunningTransition> TransitionMap;
    TransitionMap m_transitions;

    OwnPtr<CSSAnimationUpdate> m_pendingUpdate;

    HashMap<CSSPropertyID, RefPtr<Interpolation> > m_previousActiveInterpolationsForAnimations;

    static void calculateTransitionUpdate(CSSAnimationUpdate*, const Element*, const RenderStyle&);
    static void calculateTransitionUpdateForProperty(CSSPropertyID, CSSPropertyID eventId, const CSSTransitionData&, size_t transitionIndex, const RenderStyle& oldStyle, const RenderStyle&, const TransitionMap* activeTransitions, CSSAnimationUpdate*, const Element*);

    static void calculateAnimationActiveInterpolations(CSSAnimationUpdate*, const Element*, double timelineCurrentTime);
    static void calculateTransitionActiveInterpolations(CSSAnimationUpdate*, const Element*, double timelineCurrentTime);

    class AnimationEventDelegate final : public AnimationNode::EventDelegate {
    public:
        AnimationEventDelegate(Element* target, const AtomicString& name)
            : m_target(target)
            , m_name(name)
            , m_previousPhase(AnimationNode::PhaseNone)
            , m_previousIteration(nullValue())
        {
        }
        virtual void onEventCondition(const AnimationNode*) override;

    private:
        void maybeDispatch(Document::ListenerType, const AtomicString& eventName, double elapsedTime);
        RawPtr<Element> m_target;
        const AtomicString m_name;
        AnimationNode::Phase m_previousPhase;
        double m_previousIteration;
    };

    class TransitionEventDelegate final : public AnimationNode::EventDelegate {
    public:
        TransitionEventDelegate(Element* target, CSSPropertyID property)
            : m_target(target)
            , m_property(property)
            , m_previousPhase(AnimationNode::PhaseNone)
        {
        }
        virtual void onEventCondition(const AnimationNode*) override;

    private:
        RawPtr<Element> m_target;
        const CSSPropertyID m_property;
        AnimationNode::Phase m_previousPhase;
    };
};

} // namespace blink

WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::CSSAnimationUpdate::NewAnimation);

#endif  // SKY_ENGINE_CORE_ANIMATION_CSS_CSSANIMATIONS_H_
