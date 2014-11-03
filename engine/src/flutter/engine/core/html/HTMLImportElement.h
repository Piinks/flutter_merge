// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLImportElement_h
#define HTMLImportElement_h

#include "core/html/HTMLElement.h"
#include "core/html/imports/HTMLImportChildClient.h"

namespace blink {

class HTMLImportElement final : public HTMLElement, public HTMLImportChildClient {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtr<HTMLImportElement> create(Document&);

private:
    explicit HTMLImportElement(Document&);
    ~HTMLImportElement();

    bool shouldLoad() const;
    void load();

    // From HTMLElement
    InsertionNotificationRequest insertedInto(ContainerNode*) override;

    // From HTMLImportChildClient
    void didFinish() override;
    void importChildWasDestroyed(HTMLImportChild*) override;
    bool isSync() const override;
    Element* link() override;

    HTMLImportChild* m_child;
};

} // namespace blink

#endif // HTMLImportElement_h
