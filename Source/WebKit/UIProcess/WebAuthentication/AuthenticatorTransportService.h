/*
 * Copyright (C) 2018 Apple Inc. All rights reserved.
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

#if ENABLE(WEB_AUTHN)

#include "WebAuthenticationFlags.h"
#include <WebCore/AuthenticatorTransport.h>
#include <wtf/AbstractRefCountedAndCanMakeWeakPtr.h>
#include <wtf/TZoneMalloc.h>
#include <wtf/UniqueRef.h>
#include <wtf/WeakPtr.h>

namespace WebCore {
struct MockWebAuthenticationConfiguration;
}

namespace WebKit {

class Authenticator;

class AuthenticatorTransportServiceObserver : public AbstractRefCountedAndCanMakeWeakPtr<AuthenticatorTransportServiceObserver> {
public:
    virtual ~AuthenticatorTransportServiceObserver() = default;

    virtual void authenticatorAdded(Ref<Authenticator>&&) = 0;
    virtual void serviceStatusUpdated(WebAuthenticationStatus) = 0;

    virtual bool isAuthenticatorManager() const { return false; }

protected:
    AuthenticatorTransportServiceObserver() = default;
};

class AuthenticatorTransportService : public AbstractRefCountedAndCanMakeWeakPtr<AuthenticatorTransportService> {
    WTF_MAKE_TZONE_ALLOCATED(AuthenticatorTransportService);
    WTF_MAKE_NONCOPYABLE(AuthenticatorTransportService);
public:
    static Ref<AuthenticatorTransportService> create(WebCore::AuthenticatorTransport, AuthenticatorTransportServiceObserver&);
    static Ref<AuthenticatorTransportService> createMock(WebCore::AuthenticatorTransport, AuthenticatorTransportServiceObserver&, const WebCore::MockWebAuthenticationConfiguration&);

    virtual ~AuthenticatorTransportService() = default;

    // These operations are guaranteed to execute asynchronously.
    void startDiscovery();
    void restartDiscovery();

protected:
    explicit AuthenticatorTransportService(AuthenticatorTransportServiceObserver&);

    AuthenticatorTransportServiceObserver* observer() const { return m_observer.get(); }

private:
    virtual void startDiscoveryInternal() = 0;
    // NFC service's polling is one shot. It halts after the first tags are detected.
    // Therefore, a restart process is needed to resume polling after exceptions.
    virtual void restartDiscoveryInternal() { };

    WeakPtr<AuthenticatorTransportServiceObserver> m_observer;
};

} // namespace WebKit

#endif // ENABLE(WEB_AUTHN)
