/*
 * Copyright (C) 2025 Apple Inc. All rights reserved.
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

#import "config.h"
#import "_WKCaptionStyleMenuController.h"

#if PLATFORM(IOS_FAMILY)

#import <UIKit/UIKit.h>
#import <WebCore/CaptionUserPreferencesMediaAF.h>
#import <WebCore/LocalizedStrings.h>
#import <wtf/BlockPtr.h>
#import <wtf/Vector.h>
#import <wtf/WeakObjCPtr.h>
#import <wtf/text/WTFString.h>

using namespace WebCore;
using namespace WTF;

static const UIMenuIdentifier WKCaptionStyleMenuIdentifier = @"WKCaptionStyleMenuIdentifier";
static const UIMenuIdentifier WKCaptionStyleMenuProfileGroupIdentifier = @"WKCaptionStyleMenuProfileGroupIdentifier";
static const UIMenuIdentifier WKCaptionStyleMenuProfileIdentifier = @"WKCaptionStyleMenuProfileIdentifier";
static const UIMenuIdentifier WKCaptionStyleMenuSystemSettingsIdentifier = @"WKCaptionStyleMenuSystemSettingsIdentifier";

@interface WKCaptionStyleMenuController () <UIContextMenuInteractionDelegate> {
    Vector<String> _profileIDs;
    String _savedActiveProfileID;
    RetainPtr<UIMenu> _menu;
    RetainPtr<UIContextMenuInteraction> _interaction;
}
@end

@implementation WKCaptionStyleMenuController

- (instancetype)init
{
    if (!(self = [super init]))
        return nil;

    _savedActiveProfileID = CaptionUserPreferencesMediaAF::platformActiveProfileID();
    [self rebuildMenu];

    return self;
}

- (void)rebuildMenu
{
    RetainPtr stylesMenuTitle = WEB_UI_NSSTRING_KEY(@"Styles", @"Styles (Media Controls Menu)", @"Subtitles media controls menu title");

    auto profileIDs = CaptionUserPreferencesMediaAF::platformProfileIDs();
    if (profileIDs.isEmpty()) {
        _menu = [UIMenu menuWithTitle:stylesMenuTitle.get() children:@[]];
        return;
    }

    NSMutableArray<UIMenuElement *> *profileElements = [NSMutableArray array];
    for (const auto& profileID : profileIDs) {
        auto title = CaptionUserPreferencesMediaAF::nameForProfileID(profileID);

        auto profileSelectedHandler = makeBlockPtr([weakSelf = WeakObjCPtr<WKCaptionStyleMenuController>(self), profileID = profileID] (UIAction *) {
            if (auto strongSelf = weakSelf.get())
                [strongSelf profileActionSelected:profileID];
        });
        UIAction *profileAction = [UIAction actionWithTitle:title.createNSString().get() image:nil identifier:profileID.createNSString().get() handler:profileSelectedHandler.get()];

        if (profileID == _savedActiveProfileID)
            profileAction.state = UIMenuElementStateOn;

        [profileElements addObject:profileAction];
    }
    auto *profileGroup = [UIMenu menuWithTitle:@"" image:nil identifier:WKCaptionStyleMenuProfileGroupIdentifier options:UIMenuOptionsDisplayInline children:profileElements];

    RetainPtr systemCaptionSettingsTitle = WEB_UI_NSSTRING_KEY(@"Manage Styles", @"Manage Styles (Caption Style Menu Title)", @"Subtitle Styles Submenu Link to System Preferences");

    auto systemSettingsSelectedHandler = makeBlockPtr([weakSelf = WeakObjCPtr<WKCaptionStyleMenuController>(self)] (UIAction *action) {
        if (auto strongSelf = weakSelf.get())
            [strongSelf systemCaptionStyleSettingsActionSelected:action];
    });

    UIImage *gearImage = [UIImage systemImageNamed:@"gear"];
    UIAction *systemSettingsAction = [UIAction actionWithTitle:systemCaptionSettingsTitle.get() image:gearImage identifier:WKCaptionStyleMenuSystemSettingsIdentifier handler:systemSettingsSelectedHandler.get()];

    _menu = [UIMenu menuWithTitle:stylesMenuTitle.get() children:@[profileGroup, systemSettingsAction]];
}

- (BOOL)isAncestorOf:(PlatformMenu*)menu
{
    return [menu.identifier isEqualToString:WKCaptionStyleMenuIdentifier]
        || [menu.identifier isEqualToString:WKCaptionStyleMenuProfileGroupIdentifier]
        || [menu.identifier isEqualToString:WKCaptionStyleMenuProfileIdentifier]
        || [menu.identifier isEqualToString:WKCaptionStyleMenuSystemSettingsIdentifier];
}

#pragma mark - Properties

- (UIMenu *)captionStyleMenu
{
    return _menu.get();
}

- (UIContextMenuInteraction *)contextMenuInteraction
{
#if !PLATFORM(WATCHOS)
    if (!_interaction)
        _interaction = adoptNS([[UIContextMenuInteraction alloc] initWithDelegate:self]);
    return _interaction.get();
#else
    return nil;
#endif
}

#pragma mark - Actions

- (void)profileActionSelected:(const WTF::String&)profileID
{
    _savedActiveProfileID = profileID;
    CaptionUserPreferencesMediaAF::setActiveProfileID(_savedActiveProfileID);
}

- (void)systemCaptionStyleSettingsActionSelected:(UIAction *)action
{
    NSURL *settingsURL = [NSURL URLWithString:@"App-prefs:ACCESSIBILITY"];
    if ([[UIApplication sharedApplication] canOpenURL:settingsURL])
        [[UIApplication sharedApplication] openURL:settingsURL options:@{ } completionHandler:nil];
}

- (void)notifyMenuWillOpen
{
    _savedActiveProfileID = CaptionUserPreferencesMediaAF::platformActiveProfileID();

    if (auto delegate = self.delegate)
        [delegate captionStyleMenuWillOpen:_menu.get()];
}

- (void)notifyMenuDidClose
{
    if (!_savedActiveProfileID.isEmpty())
        CaptionUserPreferencesMediaAF::setActiveProfileID(_savedActiveProfileID);
    _savedActiveProfileID = nullString();

    if (auto delegate = self.delegate)
        [delegate captionStyleMenuDidClose:_menu.get()];
}

- (nullable UIContextMenuConfiguration *)contextMenuInteraction:(nonnull UIContextMenuInteraction *)interaction configurationForMenuAtLocation:(CGPoint)location {
#if USE(UICONTEXTMENU)
    return [UIContextMenuConfiguration configurationWithIdentifier:nil previewProvider:nil actionProvider:makeBlockPtr([weakSelf = WeakObjCPtr<WKCaptionStyleMenuController>(self)] (NSArray<UIMenuElement *> *) -> UIMenu * {
        if (auto strongSelf = weakSelf.get())
            return strongSelf->_menu.get();
        return nil;
    }).get()];
#else
    return nil;
#endif
}

- (void)contextMenuInteraction:(UIContextMenuInteraction *)interaction willDisplayMenuForConfiguration:(UIContextMenuConfiguration *)configuration animator:(nullable id<UIContextMenuInteractionAnimating>)animator
{
    [self notifyMenuWillOpen];
}

- (void)contextMenuInteraction:(UIContextMenuInteraction *)interaction willEndForConfiguration:(UIContextMenuConfiguration *)configuration animator:(nullable id<UIContextMenuInteractionAnimating>)animator
{
    [self notifyMenuDidClose];
}
@end

#endif
