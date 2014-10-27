/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2011 Research In Motion Limited. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/css/PropertySetCSSStyleDeclaration.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/HTMLNames.h"
#include "core/css/parser/BisonCSSParser.h"
#include "core/css/CSSStyleSheet.h"
#include "core/css/StylePropertySet.h"
#include "core/dom/Element.h"
#include "core/dom/MutationObserverInterestGroup.h"
#include "core/dom/MutationRecord.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

namespace {

class StyleAttributeMutationScope {
    WTF_MAKE_NONCOPYABLE(StyleAttributeMutationScope);
    STACK_ALLOCATED();
public:
    StyleAttributeMutationScope(AbstractPropertySetCSSStyleDeclaration* decl)
    {
        ++s_scopeCount;

        if (s_scopeCount != 1) {
            ASSERT(s_currentDecl == decl);
            return;
        }

        ASSERT(!s_currentDecl);
        s_currentDecl = decl;

        if (!s_currentDecl->parentElement())
            return;

        bool shouldReadOldValue = false;

        m_mutationRecipients = MutationObserverInterestGroup::createForAttributesMutation(*s_currentDecl->parentElement(), HTMLNames::styleAttr);
        if (m_mutationRecipients && m_mutationRecipients->isOldValueRequested())
            shouldReadOldValue = true;

        AtomicString oldValue;
        if (shouldReadOldValue)
            oldValue = s_currentDecl->parentElement()->getAttribute(HTMLNames::styleAttr);

        if (m_mutationRecipients) {
            AtomicString requestedOldValue = m_mutationRecipients->isOldValueRequested() ? oldValue : nullAtom;
            m_mutation = MutationRecord::createAttributes(s_currentDecl->parentElement(), HTMLNames::styleAttr, requestedOldValue);
        }
    }

    ~StyleAttributeMutationScope()
    {
        --s_scopeCount;
        if (s_scopeCount)
            return;

        if (m_mutation && s_shouldDeliver)
            m_mutationRecipients->enqueueMutationRecord(m_mutation);

        s_shouldDeliver = false;
        s_currentDecl = 0;
    }

    void enqueueMutationRecord()
    {
        s_shouldDeliver = true;
    }

    void didInvalidateStyleAttr()
    {
    }

private:
    static unsigned s_scopeCount;
    static AbstractPropertySetCSSStyleDeclaration* s_currentDecl;
    static bool s_shouldDeliver;

    OwnPtr<MutationObserverInterestGroup> m_mutationRecipients;
    RefPtr<MutationRecord> m_mutation;
};

unsigned StyleAttributeMutationScope::s_scopeCount = 0;
AbstractPropertySetCSSStyleDeclaration* StyleAttributeMutationScope::s_currentDecl = 0;
bool StyleAttributeMutationScope::s_shouldDeliver = false;

} // namespace

#if !ENABLE(OILPAN)
void PropertySetCSSStyleDeclaration::ref()
{
    m_propertySet->ref();
}

void PropertySetCSSStyleDeclaration::deref()
{
    m_propertySet->deref();
}
#endif

void PropertySetCSSStyleDeclaration::trace(Visitor* visitor)
{
    visitor->trace(m_propertySet);
    AbstractPropertySetCSSStyleDeclaration::trace(visitor);
}

unsigned AbstractPropertySetCSSStyleDeclaration::length() const
{
    return propertySet().propertyCount();
}

String AbstractPropertySetCSSStyleDeclaration::item(unsigned i) const
{
    if (i >= propertySet().propertyCount())
        return "";
    return propertySet().propertyAt(i).cssName();
}

String AbstractPropertySetCSSStyleDeclaration::cssText() const
{
    return propertySet().asText();
}

void AbstractPropertySetCSSStyleDeclaration::setCSSText(const String& text, ExceptionState& exceptionState)
{
    StyleAttributeMutationScope mutationScope(this);
    willMutate();

    // FIXME: Detect syntax errors and set exceptionState.
    propertySet().parseDeclaration(text, contextStyleSheet());

    didMutate(PropertyChanged);

    mutationScope.enqueueMutationRecord();
}

PassRefPtr<CSSValue> AbstractPropertySetCSSStyleDeclaration::getPropertyCSSValue(const String& propertyName)
{
    CSSPropertyID propertyID = cssPropertyID(propertyName);
    if (!propertyID)
        return nullptr;
    return cloneAndCacheForCSSOM(propertySet().getPropertyCSSValue(propertyID).get());
}

String AbstractPropertySetCSSStyleDeclaration::getPropertyValue(const String &propertyName)
{
    CSSPropertyID propertyID = cssPropertyID(propertyName);
    if (!propertyID)
        return String();
    return propertySet().getPropertyValue(propertyID);
}

String AbstractPropertySetCSSStyleDeclaration::getPropertyPriority(const String& propertyName)
{
    CSSPropertyID propertyID = cssPropertyID(propertyName);
    if (!propertyID)
        return String();
    return propertySet().propertyIsImportant(propertyID) ? "important" : "";
}

String AbstractPropertySetCSSStyleDeclaration::getPropertyShorthand(const String& propertyName)
{
    CSSPropertyID propertyID = cssPropertyID(propertyName);
    if (!propertyID)
        return String();
    CSSPropertyID shorthandID = propertySet().getPropertyShorthand(propertyID);
    if (!shorthandID)
        return String();
    return getPropertyNameString(shorthandID);
}

bool AbstractPropertySetCSSStyleDeclaration::isPropertyImplicit(const String& propertyName)
{
    CSSPropertyID propertyID = cssPropertyID(propertyName);
    if (!propertyID)
        return false;
    return propertySet().isPropertyImplicit(propertyID);
}

void AbstractPropertySetCSSStyleDeclaration::setProperty(const String& propertyName, const String& value, const String& priority, ExceptionState& exceptionState)
{
    CSSPropertyID propertyID = cssPropertyID(propertyName);
    if (!propertyID)
        return;

    bool important = equalIgnoringCase(priority, "important");
    if (!important && !priority.isEmpty())
        return;

    setPropertyInternal(propertyID, value, important, exceptionState);
}

String AbstractPropertySetCSSStyleDeclaration::removeProperty(const String& propertyName, ExceptionState& exceptionState)
{
    StyleAttributeMutationScope mutationScope(this);
    CSSPropertyID propertyID = cssPropertyID(propertyName);
    if (!propertyID)
        return String();

    willMutate();

    String result;
    bool changed = propertySet().removeProperty(propertyID, &result);

    didMutate(changed ? PropertyChanged : NoChanges);

    if (changed)
        mutationScope.enqueueMutationRecord();
    return result;
}

PassRefPtr<CSSValue> AbstractPropertySetCSSStyleDeclaration::getPropertyCSSValueInternal(CSSPropertyID propertyID)
{
    return propertySet().getPropertyCSSValue(propertyID);
}

String AbstractPropertySetCSSStyleDeclaration::getPropertyValueInternal(CSSPropertyID propertyID)
{
    return propertySet().getPropertyValue(propertyID);
}

void AbstractPropertySetCSSStyleDeclaration::setPropertyInternal(CSSPropertyID propertyID, const String& value, bool important, ExceptionState&)
{
    StyleAttributeMutationScope mutationScope(this);
    willMutate();

    bool changed = propertySet().setProperty(propertyID, value, important, contextStyleSheet());

    didMutate(changed ? PropertyChanged : NoChanges);

    if (changed)
        mutationScope.enqueueMutationRecord();
}

CSSValue* AbstractPropertySetCSSStyleDeclaration::cloneAndCacheForCSSOM(CSSValue* internalValue)
{
    if (!internalValue)
        return 0;

    // The map is here to maintain the object identity of the CSSValues over multiple invocations.
    // FIXME: It is likely that the identity is not important for web compatibility and this code should be removed.
    if (!m_cssomCSSValueClones)
        m_cssomCSSValueClones = adoptPtr(new HashMap<RawPtr<CSSValue>, RefPtr<CSSValue> >);

    RefPtr<CSSValue>& clonedValue = m_cssomCSSValueClones->add(internalValue, RefPtr<CSSValue>()).storedValue->value;
    if (!clonedValue)
        clonedValue = internalValue->cloneForCSSOM();
    return clonedValue.get();
}

StyleSheetContents* AbstractPropertySetCSSStyleDeclaration::contextStyleSheet() const
{
    CSSStyleSheet* cssStyleSheet = parentStyleSheet();
    return cssStyleSheet ? cssStyleSheet->contents() : 0;
}

PassRefPtr<MutableStylePropertySet> AbstractPropertySetCSSStyleDeclaration::copyProperties() const
{
    return propertySet().mutableCopy();
}

bool AbstractPropertySetCSSStyleDeclaration::cssPropertyMatches(CSSPropertyID propertyID, const CSSValue* propertyValue) const
{
    return propertySet().propertyMatches(propertyID, propertyValue);
}

void AbstractPropertySetCSSStyleDeclaration::trace(Visitor* visitor)
{
#if ENABLE(OILPAN)
    visitor->trace(m_cssomCSSValueClones);
#endif
    CSSStyleDeclaration::trace(visitor);
}

StyleRuleCSSStyleDeclaration::StyleRuleCSSStyleDeclaration(MutableStylePropertySet& propertySetArg, CSSRule* parentRule)
    : PropertySetCSSStyleDeclaration(propertySetArg)
#if !ENABLE(OILPAN)
    , m_refCount(1)
#endif
    , m_parentRule(parentRule)
{
#if !ENABLE(OILPAN)
    m_propertySet->ref();
#endif
}

StyleRuleCSSStyleDeclaration::~StyleRuleCSSStyleDeclaration()
{
#if !ENABLE(OILPAN)
    m_propertySet->deref();
#endif
}

#if !ENABLE(OILPAN)
void StyleRuleCSSStyleDeclaration::ref()
{
    ++m_refCount;
}

void StyleRuleCSSStyleDeclaration::deref()
{
    ASSERT(m_refCount);
    if (!--m_refCount)
        delete this;
}
#endif

void StyleRuleCSSStyleDeclaration::willMutate()
{
    if (m_parentRule && m_parentRule->parentStyleSheet())
        m_parentRule->parentStyleSheet()->willMutateRules();
}

void StyleRuleCSSStyleDeclaration::didMutate(MutationType type)
{
    if (type == PropertyChanged)
        m_cssomCSSValueClones.clear();

    // Style sheet mutation needs to be signaled even if the change failed. willMutateRules/didMutateRules must pair.
    if (m_parentRule && m_parentRule->parentStyleSheet())
        m_parentRule->parentStyleSheet()->didMutateRules();
}

CSSStyleSheet* StyleRuleCSSStyleDeclaration::parentStyleSheet() const
{
    return m_parentRule ? m_parentRule->parentStyleSheet() : 0;
}

void StyleRuleCSSStyleDeclaration::reattach(MutableStylePropertySet& propertySet)
{
#if !ENABLE(OILPAN)
    m_propertySet->deref();
#endif
    m_propertySet = &propertySet;
#if !ENABLE(OILPAN)
    m_propertySet->ref();
#endif
}

void StyleRuleCSSStyleDeclaration::trace(Visitor* visitor)
{
    visitor->trace(m_parentRule);
    PropertySetCSSStyleDeclaration::trace(visitor);
}

MutableStylePropertySet& InlineCSSStyleDeclaration::propertySet() const
{
    return m_parentElement->ensureMutableInlineStyle();
}

void InlineCSSStyleDeclaration::didMutate(MutationType type)
{
    if (type == NoChanges)
        return;

    m_cssomCSSValueClones.clear();

    if (!m_parentElement)
        return;

    m_parentElement->clearMutableInlineStyleIfEmpty();
    m_parentElement->setNeedsStyleRecalc(LocalStyleChange);
    m_parentElement->invalidateStyleAttribute();
    StyleAttributeMutationScope(this).didInvalidateStyleAttr();
}

CSSStyleSheet* InlineCSSStyleDeclaration::parentStyleSheet() const
{
    return m_parentElement ? &m_parentElement->document().elementSheet() : 0;
}

#if !ENABLE(OILPAN)
void InlineCSSStyleDeclaration::ref()
{
    m_parentElement->ref();
}

void InlineCSSStyleDeclaration::deref()
{
    m_parentElement->deref();
}
#endif

void InlineCSSStyleDeclaration::trace(Visitor* visitor)
{
    visitor->trace(m_parentElement);
    AbstractPropertySetCSSStyleDeclaration::trace(visitor);
}

} // namespace blink
