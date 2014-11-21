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

#include "sky/engine/config.h"
#include "sky/engine/core/animation/AnimationTimeline.h"

#include "sky/engine/core/animation/ActiveAnimations.h"
#include "sky/engine/core/animation/AnimationClock.h"
#include "sky/engine/core/dom/Document.h"
#include "sky/engine/core/frame/FrameView.h"
#include "sky/engine/core/page/Page.h"
#include "sky/engine/platform/TraceEvent.h"

namespace blink {

namespace {

bool compareAnimationPlayers(const RefPtr<blink::AnimationPlayer>& left, const RefPtr<blink::AnimationPlayer>& right)
{
    return AnimationPlayer::hasLowerPriority(left.get(), right.get());
}

}

// This value represents 1 frame at 30Hz plus a little bit of wiggle room.
// TODO: Plumb a nominal framerate through and derive this value from that.
const double AnimationTimeline::s_minimumDelay = 0.04;


PassRefPtr<AnimationTimeline> AnimationTimeline::create(Document* document, PassOwnPtr<PlatformTiming> timing)
{
    return adoptRef(new AnimationTimeline(document, timing));
}

AnimationTimeline::AnimationTimeline(Document* document, PassOwnPtr<PlatformTiming> timing)
    : m_document(document)
{
    if (!timing)
        m_timing = adoptPtr(new AnimationTimelineTiming(this));
    else
        m_timing = timing;

    ASSERT(document);
}

AnimationTimeline::~AnimationTimeline()
{
#if !ENABLE(OILPAN)
    for (HashSet<RawPtr<AnimationPlayer> >::iterator it = m_players.begin(); it != m_players.end(); ++it)
        (*it)->timelineDestroyed();
#endif
}

AnimationPlayer* AnimationTimeline::createAnimationPlayer(AnimationNode* child)
{
    RefPtr<AnimationPlayer> player = AnimationPlayer::create(m_document->contextDocument().get(), *this, child);
    AnimationPlayer* result = player.get();
    m_players.add(result);
    setOutdatedAnimationPlayer(result);
    return result;
}

AnimationPlayer* AnimationTimeline::play(AnimationNode* child)
{
    if (!m_document)
        return 0;
    AnimationPlayer* player = createAnimationPlayer(child);
    return player;
}

Vector<RefPtr<AnimationPlayer> > AnimationTimeline::getAnimationPlayers()
{
    Vector<RefPtr<AnimationPlayer> > animationPlayers;
    for (HashSet<RawPtr<AnimationPlayer> >::iterator it = m_players.begin(); it != m_players.end(); ++it) {
        if ((*it)->source() && (*it)->source()->isCurrent()) {
            animationPlayers.append(*it);
        }
    }
    std::sort(animationPlayers.begin(), animationPlayers.end(), compareAnimationPlayers);
    return animationPlayers;
}

void AnimationTimeline::wake()
{
    m_timing->serviceOnNextFrame();
}

void AnimationTimeline::serviceAnimations(TimingUpdateReason reason)
{
    TRACE_EVENT0("blink", "AnimationTimeline::serviceAnimations");

    m_timing->cancelWake();

    double timeToNextEffect = std::numeric_limits<double>::infinity();

    Vector<RawPtr<AnimationPlayer> > players;
    players.reserveInitialCapacity(m_playersNeedingUpdate.size());
    for (HashSet<RefPtr<AnimationPlayer> >::iterator it = m_playersNeedingUpdate.begin(); it != m_playersNeedingUpdate.end(); ++it)
        players.append(it->get());

    std::sort(players.begin(), players.end(), AnimationPlayer::hasLowerPriority);

    for (size_t i = 0; i < players.size(); ++i) {
        AnimationPlayer* player = players[i];
        if (player->update(reason))
            timeToNextEffect = std::min(timeToNextEffect, player->timeToEffectChange());
        else
            m_playersNeedingUpdate.remove(player);
    }

    if (timeToNextEffect < s_minimumDelay)
        m_timing->serviceOnNextFrame();
    else if (timeToNextEffect != std::numeric_limits<double>::infinity())
        m_timing->wakeAfter(timeToNextEffect - s_minimumDelay);

    ASSERT(!hasOutdatedAnimationPlayer());
}

void AnimationTimeline::AnimationTimelineTiming::wakeAfter(double duration)
{
    m_timer.startOneShot(duration, FROM_HERE);
}

void AnimationTimeline::AnimationTimelineTiming::cancelWake()
{
    m_timer.stop();
}

void AnimationTimeline::AnimationTimelineTiming::serviceOnNextFrame()
{
    if (m_timeline->m_document && m_timeline->m_document->view())
        m_timeline->m_document->view()->scheduleAnimation();
}

double AnimationTimeline::currentTime(bool& isNull)
{
    return currentTimeInternal(isNull) * 1000;
}

double AnimationTimeline::currentTimeInternal(bool& isNull)
{
    if (!m_document) {
        isNull = true;
        return std::numeric_limits<double>::quiet_NaN();
    }
    double result = m_document->animationClock().currentTime() - zeroTime();
    isNull = std::isnan(result);
    return result;
}

double AnimationTimeline::currentTime()
{
    return currentTimeInternal() * 1000;
}

double AnimationTimeline::currentTimeInternal()
{
    bool isNull;
    return currentTimeInternal(isNull);
}

double AnimationTimeline::effectiveTime()
{
    double time = currentTimeInternal();
    return std::isnan(time) ? 0 : time;
}

void AnimationTimeline::pauseAnimationsForTesting(double pauseTime)
{
    for (HashSet<RefPtr<AnimationPlayer> >::iterator it = m_playersNeedingUpdate.begin(); it != m_playersNeedingUpdate.end(); ++it)
        (*it)->pauseForTesting(pauseTime);
    serviceAnimations(TimingUpdateOnDemand);
}

bool AnimationTimeline::hasOutdatedAnimationPlayer() const
{
    for (HashSet<RefPtr<AnimationPlayer> >::iterator it = m_playersNeedingUpdate.begin(); it != m_playersNeedingUpdate.end(); ++it) {
        if ((*it)->outdated())
            return true;
    }
    return false;
}

void AnimationTimeline::setOutdatedAnimationPlayer(AnimationPlayer* player)
{
    ASSERT(player->outdated());
    m_playersNeedingUpdate.add(player);
    if (m_document && m_document->page() && !m_document->page()->animator().isServicingAnimations())
        m_timing->serviceOnNextFrame();
}

#if !ENABLE(OILPAN)
void AnimationTimeline::detachFromDocument()
{
    // FIXME: AnimationTimeline should keep Document alive.
    m_document = nullptr;
}
#endif

} // namespace
