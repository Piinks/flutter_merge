/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 *           (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
 *
 */

#ifndef SKY_ENGINE_CORE_DOM_TREESCOPESTYLESHEETCOLLECTION_H_
#define SKY_ENGINE_CORE_DOM_TREESCOPESTYLESHEETCOLLECTION_H_

#include "sky/engine/core/dom/Document.h"
#include "sky/engine/core/dom/DocumentOrderedList.h"
#include "sky/engine/core/dom/StyleSheetCollection.h"
#include "sky/engine/core/dom/TreeScope.h"
#include "sky/engine/wtf/FastAllocBase.h"
#include "sky/engine/wtf/HashMap.h"
#include "sky/engine/wtf/ListHashSet.h"
#include "sky/engine/wtf/RefPtr.h"
#include "sky/engine/wtf/Vector.h"
#include "sky/engine/wtf/text/WTFString.h"

namespace blink {

class ContainerNode;
class Node;
class StyleSheetContents;
class StyleRuleFontFace;

class TreeScopeStyleSheetCollection : public StyleSheetCollection {
public:
    void addStyleSheetCandidateNode(Node*, bool createdByParser);
    void removeStyleSheetCandidateNode(Node*, ContainerNode* scopingNode);
    bool hasStyleSheetCandidateNodes() const { return !m_styleSheetCandidateNodes.isEmpty(); }

    DocumentOrderedList& styleSheetCandidateNodes() { return m_styleSheetCandidateNodes; }

protected:
    explicit TreeScopeStyleSheetCollection(TreeScope&);

    Document& document() const { return treeScope().document(); }
    TreeScope& treeScope() const { return *m_treeScope; }

    enum StyleResolverUpdateType {
        Reconstruct,
        Reset,
        Additive
    };

protected:
    RawPtr<TreeScope> m_treeScope;

    DocumentOrderedList m_styleSheetCandidateNodes;
};

}

#endif  // SKY_ENGINE_CORE_DOM_TREESCOPESTYLESHEETCOLLECTION_H_
