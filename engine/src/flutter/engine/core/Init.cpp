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

#include "config.h"
#include "Init.h"

#include "core/EventNames.h"
#include "core/EventTargetNames.h"
#include "core/EventTypeNames.h"
#include "core/FetchInitiatorTypeNames.h"
#include "core/HTMLNames.h"
#include "core/MediaFeatureNames.h"
#include "core/MediaTypeNames.h"
#include "core/dom/Document.h"
#include "core/events/EventFactory.h"
#include "core/html/parser/HTMLParserThread.h"
#include "platform/EventTracer.h"
#include "platform/FontFamilyNames.h"
#include "platform/Partitions.h"
#include "platform/PlatformThreadData.h"
#include "wtf/text/StringStatics.h"

namespace blink {

void CoreInitializer::registerEventFactory()
{
    static bool isRegistered = false;
    if (isRegistered)
        return;
    isRegistered = true;

    Document::registerEventFactory(EventFactory::create());
}

void CoreInitializer::init()
{
    ASSERT(!m_isInited);
    m_isInited = true;

    HTMLNames::init();

    EventNames::init();
    EventTargetNames::init();
    EventTypeNames::init();
    FetchInitiatorTypeNames::init();
    FontFamilyNames::init();
    MediaFeatureNames::init();
    MediaTypeNames::init();

    // It would make logical sense to do this in WTF::initialize() but there are
    // ordering dependencies, e.g. about "xmlns".
    WTF::StringStatics::init();

    QualifiedName::init();
    Partitions::init();
    EventTracer::initialize();

    registerEventFactory();

    // Ensure that the main thread's thread-local data is initialized before
    // starting any worker threads.
    PlatformThreadData::current();

    StringImpl::freezeStaticStrings();

    HTMLParserThread::start();
}

void CoreInitializer::shutdown()
{
    HTMLParserThread::stop();
    Partitions::shutdown();
}

} // namespace blink
