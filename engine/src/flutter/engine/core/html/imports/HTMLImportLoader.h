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

#ifndef HTMLImportLoader_h
#define HTMLImportLoader_h

#include "core/fetch/RawResource.h"
#include "core/fetch/ResourceOwner.h"
#include "platform/heap/Handle.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class CustomElementSyncMicrotaskQueue;
class Document;
class DocumentWriter;
class HTMLImportChild;
class HTMLImportsController;


//
// Owning imported Document lifetime. It also implements ResourceClient through ResourceOwner
// to feed fetched bytes to the DocumentWriter of the imported document.
// HTMLImportLoader is owned by HTMLImportsController.
//
//
class HTMLImportLoader FINAL : public NoBaseWillBeGarbageCollectedFinalized<HTMLImportLoader>, public ResourceOwner<RawResource> {
public:
    enum State {
        StateLoading,
        StateWritten,
        StateParsed,
        StateLoaded,
        StateError
    };

    static PassOwnPtrWillBeRawPtr<HTMLImportLoader> create(HTMLImportsController* controller)
    {
        return adoptPtrWillBeNoop(new HTMLImportLoader(controller));
    }

    virtual ~HTMLImportLoader();

    Document* document() const { return m_document.get(); }
    void addImport(HTMLImportChild*);
#if !ENABLE(OILPAN)
    void removeImport(HTMLImportChild*);
#endif
    void moveToFirst(HTMLImportChild*);
    HTMLImportChild* firstImport() const { return m_imports[0]; }
    bool isFirstImport(const HTMLImportChild* child) const { return m_imports.size() ? firstImport() == child : false; }

    bool isDone() const { return m_state == StateLoaded || m_state == StateError; }
    bool hasError() const { return m_state == StateError; }
    bool shouldBlockScriptExecution() const;

#if !ENABLE(OILPAN)
    void importDestroyed();
#endif
    void startLoading(const ResourcePtr<RawResource>&);

    // Tells the loader that the parser is done with this import.
    // Called by Document::finishedParsing, after DOMContentLoaded was dispatched.
    void didFinishParsing();
    // Tells the loader that all of the import's stylesheets finished
    // loading.
    // Called by Document::didRemoveAllPendingStylesheet.
    void didRemoveAllPendingStylesheet();

    PassRefPtrWillBeRawPtr<CustomElementSyncMicrotaskQueue> microtaskQueue() const;

    virtual void trace(Visitor*);

private:
    HTMLImportLoader(HTMLImportsController*);

    // RawResourceClient
    virtual void responseReceived(Resource*, const ResourceResponse&) OVERRIDE;
    virtual void dataReceived(Resource*, const char* data, int length) OVERRIDE;
    virtual void notifyFinished(Resource*) OVERRIDE;

    State startWritingAndParsing(const ResourceResponse&);
    State finishWriting();
    State finishParsing();
    State finishLoading();

    void setState(State);
    void didFinishLoading();
#if !ENABLE(OILPAN)
    void clear();
#endif

    RawPtrWillBeMember<HTMLImportsController> m_controller;
    WillBeHeapVector<RawPtrWillBeMember<HTMLImportChild> > m_imports;
    State m_state;
    RefPtrWillBeMember<Document> m_document;
    RefPtrWillBeMember<DocumentWriter> m_writer;
    RefPtrWillBeMember<CustomElementSyncMicrotaskQueue> m_microtaskQueue;
};

} // namespace blink

#endif // HTMLImportLoader_h
