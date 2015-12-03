// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_EDK_EMBEDDER_PLATFORM_CHANNEL_UTILS_H_
#define MOJO_EDK_EMBEDDER_PLATFORM_CHANNEL_UTILS_H_

#include <stddef.h>
#include <sys/types.h>  // For |ssize_t|.

#include <deque>

#include "mojo/edk/platform/platform_handle.h"
#include "mojo/edk/platform/scoped_platform_handle.h"

struct iovec;  // Declared in <sys/uio.h>.

namespace mojo {
namespace embedder {

// The maximum number of handles that can be sent "at once" using
// |PlatformChannelSendmsgWithHandles()|.
// TODO(vtl): This number is taken from ipc/ipc_message_attachment_set.h:
// |IPC::MessageAttachmentSet::kMaxDescriptorsPerMessage|.
const size_t kPlatformChannelMaxNumHandles = 128;

// Use these to write to a socket created using |PlatformChannelPair| (or
// equivalent). These are like |write()| and |writev()|, but handle |EINTR| and
// never raise |SIGPIPE|. (Note: On Mac, the suppression of |SIGPIPE| is set up
// by |PlatformChannelPair|.)
ssize_t PlatformChannelWrite(platform::PlatformHandle h,
                             const void* bytes,
                             size_t num_bytes);
ssize_t PlatformChannelWritev(platform::PlatformHandle h,
                              struct iovec* iov,
                              size_t num_iov);

// Writes data, and the given set of |PlatformHandle|s (i.e., file descriptors)
// over the Unix domain socket given by |h| (e.g., created using
// |PlatformChannelPair()|). All the handles must be valid, and there must be at
// least one and at most |kPlatformChannelMaxNumHandles| handles. The return
// value is as for |sendmsg()|, namely -1 on failure and otherwise the number of
// bytes of data sent on success (note that this may not be all the data
// specified by |iov|). (The handles are not closed, regardless of success or
// failure.)
ssize_t PlatformChannelSendmsgWithHandles(
    platform::PlatformHandle h,
    struct iovec* iov,
    size_t num_iov,
    platform::PlatformHandle* platform_handles,
    size_t num_platform_handles);

// Wrapper around |recvmsg()|, which will extract any attached file descriptors
// (in the control message) to |ScopedPlatformHandle|s (and append them to
// |platform_handles|). (This also handles |EINTR|.)
ssize_t PlatformChannelRecvmsg(
    platform::PlatformHandle h,
    void* buf,
    size_t num_bytes,
    std::deque<platform::ScopedPlatformHandle>* platform_handles);

}  // namespace embedder
}  // namespace mojo

#endif  // MOJO_EDK_EMBEDDER_PLATFORM_CHANNEL_UTILS_H_
