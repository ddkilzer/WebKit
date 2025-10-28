/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#pragma once

#include <wtf/CheckedRef.h>
#include <wtf/Noncopyable.h>
#include <wtf/TZoneMalloc.h>
#include <wtf/UniqueRef.h>
#include <wtf/WeakHashSet.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class Page;
#if ENABLE(VIDEO)
class CaptionUserPreferences;
#endif

class PageGroup final : public CanMakeWeakPtr<PageGroup>, public CanMakeCheckedPtr<PageGroup> {
    WTF_MAKE_TZONE_ALLOCATED(PageGroup);
    WTF_MAKE_NONCOPYABLE(PageGroup);
    WTF_OVERRIDE_DELETE_FOR_CHECKED_PTR(PageGroup);
public:
    WEBCORE_EXPORT static UniqueRef<PageGroup> create(const String&);
    WEBCORE_EXPORT static UniqueRef<PageGroup> create(Page&);
    WEBCORE_EXPORT ~PageGroup();

    WEBCORE_EXPORT static PageGroup* pageGroup(const String& groupName);

    const WeakHashSet<Page>& pages() const { return m_pages; }

    void addPage(Page&);
    void removePage(Page&);

    const String& name() { return m_name; }
    unsigned identifier() { return m_identifier; }

#if ENABLE(VIDEO)
    WEBCORE_EXPORT void captionPreferencesChanged();
    WEBCORE_EXPORT CaptionUserPreferences& ensureCaptionPreferences();
    Ref<CaptionUserPreferences> ensureProtectedCaptionPreferences();
    CaptionUserPreferences* captionPreferences() const { return m_captionPreferences.get(); }
#endif

private:
    WEBCORE_EXPORT explicit PageGroup(const String&);
    WEBCORE_EXPORT explicit PageGroup(Page&);

    String m_name;
    WeakHashSet<Page> m_pages;

    unsigned m_identifier;

#if ENABLE(VIDEO)
    RefPtr<CaptionUserPreferences> m_captionPreferences;
#endif
};

} // namespace WebCore
