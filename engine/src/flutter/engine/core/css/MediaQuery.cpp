/*
 * CSS Media Query
 *
 * Copyright (C) 2005, 2006 Kimmo Kinnunen <kimmo.t.kinnunen@nokia.com>.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sky/engine/config.h"
#include "sky/engine/core/css/MediaQuery.h"

#include "gen/sky/core/MediaTypeNames.h"
#include "sky/engine/core/css/MediaQueryExp.h"
#include "sky/engine/core/html/parser/HTMLParserIdioms.h"
#include "sky/engine/wtf/NonCopyingSort.h"
#include "sky/engine/wtf/text/StringBuilder.h"

namespace blink {

// http://dev.w3.org/csswg/cssom/#serialize-a-media-query
String MediaQuery::serialize() const
{
    StringBuilder result;
    switch (m_restrictor) {
    case MediaQuery::Only:
        result.appendLiteral("only ");
        break;
    case MediaQuery::Not:
        result.appendLiteral("not ");
        break;
    case MediaQuery::None:
        break;
    }

    if (m_expressions->isEmpty()) {
        result.append(m_mediaType);
        return result.toString();
    }

    if (m_mediaType != MediaTypeNames::all || m_restrictor != None) {
        result.append(m_mediaType);
        result.appendLiteral(" and ");
    }

    result.append(m_expressions->at(0)->serialize());
    for (size_t i = 1; i < m_expressions->size(); ++i) {
        result.appendLiteral(" and ");
        result.append(m_expressions->at(i)->serialize());
    }
    return result.toString();
}

static bool expressionCompare(const OwnPtr<MediaQueryExp>& a, const OwnPtr<MediaQueryExp>& b)
{
    return codePointCompare(a->serialize(), b->serialize()) < 0;
}

PassOwnPtr<MediaQuery> MediaQuery::createNotAll()
{
    return adoptPtr(new MediaQuery(MediaQuery::Not, MediaTypeNames::all, nullptr));
}

MediaQuery::MediaQuery(Restrictor r, const String& mediaType, PassOwnPtr<ExpressionHeapVector> expressions)
    : m_restrictor(r)
    , m_mediaType(attemptStaticStringCreation(mediaType.lower()))
    , m_expressions(expressions)
{
    if (!m_expressions) {
        m_expressions = adoptPtr(new ExpressionHeapVector);
        return;
    }

    nonCopyingSort(m_expressions->begin(), m_expressions->end(), expressionCompare);

    // Remove all duplicated expressions.
    MediaQueryExp* key = 0;
    for (int i = m_expressions->size() - 1; i >= 0; --i) {
        MediaQueryExp* exp = m_expressions->at(i).get();

        if (key && *exp == *key)
            m_expressions->remove(i);
        else
            key = exp;
    }
}

MediaQuery::MediaQuery(const MediaQuery& o)
    : m_restrictor(o.m_restrictor)
    , m_mediaType(o.m_mediaType)
    , m_expressions(adoptPtr(new ExpressionHeapVector(o.m_expressions->size())))
    , m_serializationCache(o.m_serializationCache)
{
    for (unsigned i = 0; i < m_expressions->size(); ++i)
        (*m_expressions)[i] = o.m_expressions->at(i)->copy();
}

MediaQuery::~MediaQuery()
{
}

// http://dev.w3.org/csswg/cssom/#compare-media-queries
bool MediaQuery::operator==(const MediaQuery& other) const
{
    return cssText() == other.cssText();
}

// http://dev.w3.org/csswg/cssom/#serialize-a-list-of-media-queries
String MediaQuery::cssText() const
{
    if (m_serializationCache.isNull())
        const_cast<MediaQuery*>(this)->m_serializationCache = serialize();

    return m_serializationCache;
}

}
