/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
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

#ifndef SKY_ENGINE_WTF_ARRAYBUFFERDEALLOCATIONOBSERVER_H_
#define SKY_ENGINE_WTF_ARRAYBUFFERDEALLOCATIONOBSERVER_H_

namespace WTF {

class ArrayBufferContents;

// The current implementation assumes that the instance of this class is a
// singleton living for the entire process's lifetime.
class ArrayBufferDeallocationObserver {
public:
    virtual void arrayBufferDeallocated(unsigned sizeInBytes) = 0;

protected:
    // Called when Blink-allocated ArrayBuffer becomes available to JavaScript.
    // Not called for JavaScript-allocated ArrayBuffers.
    virtual void blinkAllocatedMemory(unsigned sizeInBytes) = 0;

    friend class ArrayBufferContents;
};

} // namespace WTF

#endif  // SKY_ENGINE_WTF_ARRAYBUFFERDEALLOCATIONOBSERVER_H_
