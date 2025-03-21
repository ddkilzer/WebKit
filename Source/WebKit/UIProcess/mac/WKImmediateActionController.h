/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
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

#if PLATFORM(MAC)

#import "WKImmediateActionTypes.h"
#import "WebHitTestResultData.h"
#import <pal/spi/mac/NSImmediateActionGestureRecognizerSPI.h>
#import <wtf/CheckedPtr.h>
#import <wtf/NakedPtr.h>
#import <wtf/RetainPtr.h>

namespace WebKit {
class WebPageProxy;
class WebViewImpl;

enum class ImmediateActionState {
    None = 0,
    Pending,
    TimedOut,
    Ready
};
}

#if HAVE(SECURE_ACTION_CONTEXT)
@class DDSecureActionContext;
#else
@class DDActionContext;
#endif
@class QLPreviewMenuItem;

@interface WKImmediateActionController : NSObject <NSImmediateActionGestureRecognizerDelegate> {
@private
    WeakPtr<WebKit::WebPageProxy> _page;
    NSView *_view;
    WeakPtr<WebKit::WebViewImpl> _viewImpl;

    WebKit::ImmediateActionState _state;
    WebKit::WebHitTestResultData _hitTestResultData;
    BOOL _contentPreventsDefault;
    RefPtr<API::Object> _userData;
    uint32_t _type;
    RetainPtr<NSImmediateActionGestureRecognizer> _immediateActionRecognizer;

    BOOL _hasActivatedActionContext;
#if HAVE(SECURE_ACTION_CONTEXT)
    RetainPtr<DDSecureActionContext> _currentActionContext;
#else
    RetainPtr<DDActionContext> _currentActionContext;
#endif
    RetainPtr<QLPreviewMenuItem> _currentQLPreviewMenuItem;

    BOOL _hasActiveImmediateAction;
}

- (instancetype)initWithPage:(std::reference_wrapper<WebKit::WebPageProxy>)page view:(NSView *)view viewImpl:(std::reference_wrapper<WebKit::WebViewImpl>)viewImpl recognizer:(NSImmediateActionGestureRecognizer *)immediateActionRecognizer;
- (void)willDestroyView:(NSView *)view;
- (void)didPerformImmediateActionHitTest:(const WebKit::WebHitTestResultData&)hitTestResult contentPreventsDefault:(BOOL)contentPreventsDefault userData:(API::Object*)userData;
- (void)dismissContentRelativeChildWindows;
- (BOOL)hasActiveImmediateAction;

@end

#endif // PLATFORM(MAC)
