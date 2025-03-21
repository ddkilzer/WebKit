/*
 * Copyright (C) 2016 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "WTFTestUtilities.h"

#include <wtf/Seconds.h>

namespace TestWebKitAPI::Util {

// Runs a platform runloop until the 'done' flag is true.
void run(bool* done);

// Runs a platform runloop until the 'done' flag is true, or until the amount of seconds has passed.
// Returns true if exiting due to the 'done' flag becoming true, or false if exiting due to a timeout.
bool runFor(bool* done, Seconds duration);

// Runs a platform runloop until the amount of seconds has passed.
void runFor(Seconds duration);

// Runs a platform runloop `count` number of spins.
void spinRunLoop(uint64_t count = 1);

// Waits for 'c' to return true by repeatedly running the platform runloop for 100ms, up to 'maxTries'.
template <typename Callable>
bool waitFor(Callable&& c, size_t maxTries = 100)
{
    size_t tries = 0;
    do {
        if (c())
            return true;
        TestWebKitAPI::Util::runFor(0.1_s);
    } while (++tries <= maxTries);
    return false;
}

} // namespace TestWebKitAPI::Util
