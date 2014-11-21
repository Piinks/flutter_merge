/*
* Copyright (C) 2012 Google Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1.  Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sky/engine/config.h"
#include "sky/engine/public/web/WebHitTestResult.h"

#include "sky/engine/core/dom/Element.h"
#include "sky/engine/core/dom/Node.h"
#include "sky/engine/core/editing/VisiblePosition.h"
#include "sky/engine/core/rendering/HitTestResult.h"
#include "sky/engine/core/rendering/RenderObject.h"
#include "sky/engine/platform/weborigin/KURL.h"
#include "sky/engine/public/platform/WebPoint.h"
#include "sky/engine/public/platform/WebURL.h"
#include "sky/engine/public/web/WebElement.h"
#include "sky/engine/public/web/WebNode.h"

namespace blink {

class WebHitTestResultPrivate : public RefCounted<WebHitTestResultPrivate> {
public:
    static PassRefPtr<WebHitTestResultPrivate> create(const HitTestResult&);
    static PassRefPtr<WebHitTestResultPrivate> create(const WebHitTestResultPrivate&);
    const HitTestResult& result() const { return m_result; }

private:
    WebHitTestResultPrivate(const HitTestResult&);
    WebHitTestResultPrivate(const WebHitTestResultPrivate&);

    HitTestResult m_result;
};

inline WebHitTestResultPrivate::WebHitTestResultPrivate(const HitTestResult& result)
    : m_result(result)
{
}

inline WebHitTestResultPrivate::WebHitTestResultPrivate(const WebHitTestResultPrivate& result)
    : m_result(result.m_result)
{
}

PassRefPtr<WebHitTestResultPrivate> WebHitTestResultPrivate::create(const HitTestResult& result)
{
    return adoptRef(new WebHitTestResultPrivate(result));
}

PassRefPtr<WebHitTestResultPrivate> WebHitTestResultPrivate::create(const WebHitTestResultPrivate& result)
{
    return adoptRef(new WebHitTestResultPrivate(result));
}

WebNode WebHitTestResult::node() const
{
    return WebNode(m_private->result().innerNode());
}

WebPoint WebHitTestResult::localPoint() const
{
    return roundedIntPoint(m_private->result().localPoint());
}

WebElement WebHitTestResult::urlElement() const
{
    return WebElement(m_private->result().URLElement());
}

WebURL WebHitTestResult::absoluteImageURL() const
{
    return m_private->result().absoluteImageURL();
}

WebURL WebHitTestResult::absoluteLinkURL() const
{
    return m_private->result().absoluteLinkURL();
}

bool WebHitTestResult::isContentEditable() const
{
    return m_private->result().isContentEditable();
}

WebHitTestResult::WebHitTestResult(const HitTestResult& result)
    : m_private(WebHitTestResultPrivate::create(result))
{
}

WebHitTestResult& WebHitTestResult::operator=(const HitTestResult& result)
{
    m_private = WebHitTestResultPrivate::create(result);
    return *this;
}

bool WebHitTestResult::isNull() const
{
    return !m_private.get();
}

void WebHitTestResult::assign(const WebHitTestResult& info)
{
    if (info.isNull())
        m_private.reset();
    else
        m_private = WebHitTestResultPrivate::create(*info.m_private.get());
}

void WebHitTestResult::reset()
{
    m_private.reset();
}

} // namespace blink
