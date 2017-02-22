#import "AppDelegate.h"

#import "MapDocument.h"

NSString * const MGLMapboxAccessTokenDefaultsKey = @"MGLMapboxAccessToken";
NSString * const MGLLastMapCameraDefaultsKey = @"MGLLastMapCamera";
NSString * const MGLLastMapStyleURLDefaultsKey = @"MGLLastMapStyleURL";
NSString * const MGLLastMapDebugMaskDefaultsKey = @"MGLLastMapDebugMask";

/**
 Some convenience methods to make offline pack properties easier to bind to.
 */
@implementation MGLOfflinePack (Additions)

+ (NSSet *)keyPathsForValuesAffectingStateImage {
    return [NSSet setWithObjects:@"state", nil];
}

- (NSImage *)stateImage {
    switch (self.state) {
        case MGLOfflinePackStateComplete:
            return [NSImage imageNamed:@"NSMenuOnStateTemplate"];

        case MGLOfflinePackStateActive:
            return [NSImage imageNamed:@"NSFollowLinkFreestandingTemplate"];

        default:
            return nil;
    }
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCountOfResourcesCompleted {
    return [NSSet setWithObjects:@"progress", nil];
}

- (uint64_t)countOfResourcesCompleted {
    return self.progress.countOfResourcesCompleted;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCountOfResourcesExpected {
    return [NSSet setWithObjects:@"progress", nil];
}

- (uint64_t)countOfResourcesExpected {
    return self.progress.countOfResourcesExpected;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCountOfBytesCompleted {
    return [NSSet setWithObjects:@"progress", nil];
}

- (uint64_t)countOfBytesCompleted {
    return self.progress.countOfBytesCompleted;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCountOfTilesCompleted {
    return [NSSet setWithObjects:@"progress", nil];
}

- (uint64_t)countOfTilesCompleted {
    return self.progress.countOfTilesCompleted;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCountOfTileBytesCompleted {
    return [NSSet setWithObjects:@"progress", nil];
}

- (uint64_t)countOfTileBytesCompleted {
    return self.progress.countOfTileBytesCompleted;
}

@end

@interface AppDelegate () <NSWindowDelegate>

@property (weak) IBOutlet NSArrayController *offlinePacksArrayController;
@property (weak) IBOutlet NSPanel *offlinePacksPanel;

@end

@implementation AppDelegate

#pragma mark Lifecycle

+ (void)load {
    // Set access token, unless MGLAccountManager already read it in from Info.plist.
    if (![MGLAccountManager accessToken]) {
        NSString *accessToken = [NSProcessInfo processInfo].environment[@"MAPBOX_ACCESS_TOKEN"];
        if (accessToken) {
            // Store to preferences so that we can launch the app later on without having to specify
            // token.
            [[NSUserDefaults standardUserDefaults] setObject:accessToken forKey:MGLMapboxAccessTokenDefaultsKey];
        } else {
            // Try to retrieve from preferences, maybe we've stored them there previously and can reuse
            // the token.
            accessToken = [[NSUserDefaults standardUserDefaults] stringForKey:MGLMapboxAccessTokenDefaultsKey];
        }
        [MGLAccountManager setAccessToken:accessToken];
    }
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification {
    [[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
                                                       andSelector:@selector(handleGetURLEvent:withReplyEvent:)
                                                     forEventClass:kInternetEventClass
                                                        andEventID:kAEGetURL];

    if (![[NSUserDefaults standardUserDefaults] boolForKey:@"NSQuitAlwaysKeepsWindows"]) {
        NSData *cameraData = [[NSUserDefaults standardUserDefaults] objectForKey:MGLLastMapCameraDefaultsKey];
        if (cameraData) {
            NSKeyedUnarchiver *coder = [[NSKeyedUnarchiver alloc] initForReadingWithData:cameraData];
            self.pendingZoomLevel = -1;
            self.pendingCamera = [[MGLMapCamera alloc] initWithCoder:coder];
        }
        NSString *styleURLString = [[NSUserDefaults standardUserDefaults] objectForKey:MGLLastMapStyleURLDefaultsKey];
        if (styleURLString) {
            self.pendingStyleURL = [NSURL URLWithString:styleURLString];
        }
        self.pendingDebugMask = [[NSUserDefaults standardUserDefaults] integerForKey:MGLLastMapDebugMaskDefaultsKey];
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Set access token, unless MGLAccountManager already read it in from Info.plist.
    if (![MGLAccountManager accessToken]) {
        NSAlert *alert = [[NSAlert alloc] init];
        alert.messageText = @"Access token required";
        alert.informativeText = @"To load Mapbox-hosted tiles and styles, enter your Mapbox access token in Preferences.";
        [alert addButtonWithTitle:@"Open Preferences"];
        [alert runModal];
        [self showPreferences:nil];
    }

    [self.offlinePacksArrayController bind:@"content" toObject:[MGLOfflineStorage sharedOfflineStorage] withKeyPath:@"packs" options:nil];
}

- (void)applicationWillTerminate:(NSNotification *)notification {
    if (![[NSUserDefaults standardUserDefaults] boolForKey:@"NSQuitAlwaysKeepsWindows"]) {
        NSDocument *currentDocument = [NSDocumentController sharedDocumentController].currentDocument;
        if ([currentDocument isKindOfClass:[MapDocument class]]) {
            MGLMapView *mapView = [(MapDocument *)currentDocument mapView];
            NSMutableData *cameraData = [NSMutableData data];
            NSKeyedArchiver *coder = [[NSKeyedArchiver alloc] initForWritingWithMutableData:cameraData];
            [mapView.camera encodeWithCoder:coder];
            [coder finishEncoding];
            [[NSUserDefaults standardUserDefaults] setObject:cameraData forKey:MGLLastMapCameraDefaultsKey];
            [[NSUserDefaults standardUserDefaults] setObject:mapView.styleURL.absoluteString forKey:MGLLastMapStyleURLDefaultsKey];
            [[NSUserDefaults standardUserDefaults] setInteger:mapView.debugMask forKey:MGLLastMapDebugMaskDefaultsKey];
        }
    }

    [self.offlinePacksArrayController unbind:@"content"];
}

#pragma mark Services

- (void)handleGetURLEvent:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent {
    // mapboxgl://?center=29.95,-90.066667&zoom=14&bearing=45&pitch=30
    NSURL *url = [NSURL URLWithString:[event paramDescriptorForKeyword:keyDirectObject].stringValue];
    NS_MUTABLE_DICTIONARY_OF(NSString *, NSString *) *params = [[NSMutableDictionary alloc] init];
    for (NSString *param in [url.query componentsSeparatedByString:@"&"]) {
        NSArray *parts = [param componentsSeparatedByString:@"="];
        if (parts.count >= 2) {
            params[parts[0]] = [parts[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        }
    }

    MGLMapCamera *camera = [MGLMapCamera camera];
    NSString *zoomLevelString = params[@"zoom"];
    self.pendingZoomLevel = zoomLevelString.length ? zoomLevelString.doubleValue : -1;

    NSString *directionString = params[@"bearing"];
    if (directionString.length) {
        camera.heading = directionString.doubleValue;
    }

    NSString *centerString = params[@"center"];
    if (centerString) {
        NSArray *coordinateValues = [centerString componentsSeparatedByString:@","];
        if (coordinateValues.count == 2) {
            camera.centerCoordinate = CLLocationCoordinate2DMake([coordinateValues[0] doubleValue],
                                                                 [coordinateValues[1] doubleValue]);
        }
    }

    NSString *pitchString = params[@"pitch"];
    if (pitchString.length) {
        camera.pitch = pitchString.doubleValue;
    }

    self.pendingCamera = camera;
    [[NSDocumentController sharedDocumentController] openUntitledDocumentAndDisplay:YES error:NULL];
}

#pragma mark Offline pack management

- (IBAction)showOfflinePacksPanel:(id)sender {
    [self.offlinePacksPanel makeKeyAndOrderFront:sender];

    for (MGLOfflinePack *pack in self.offlinePacksArrayController.arrangedObjects) {
        [pack requestProgress];
    }
}

- (IBAction)delete:(id)sender {
    for (MGLOfflinePack *pack in self.offlinePacksArrayController.selectedObjects) {
        [[MGLOfflineStorage sharedOfflineStorage] removePack:pack withCompletionHandler:^(NSError * _Nullable error) {
            if (error) {
                [[NSAlert alertWithError:error] runModal];
            }
        }];
    }
}

- (IBAction)chooseOfflinePack:(id)sender {
    for (MGLOfflinePack *pack in self.offlinePacksArrayController.selectedObjects) {
        switch (pack.state) {
            case MGLOfflinePackStateComplete:
            {
                if ([pack.region isKindOfClass:[MGLTilePyramidOfflineRegion class]]) {
                    MGLTilePyramidOfflineRegion *region = (MGLTilePyramidOfflineRegion *)pack.region;
                    self.pendingVisibleCoordinateBounds = region.bounds;
                    self.pendingMinimumZoomLevel = region.minimumZoomLevel;
                    self.pendingMaximumZoomLevel = region.maximumZoomLevel;
                    [[NSDocumentController sharedDocumentController] openUntitledDocumentAndDisplay:YES error:NULL];
                }
                break;
            }

            case MGLOfflinePackStateInactive:
                [pack resume];
                break;

            case MGLOfflinePackStateActive:
                [pack suspend];
                break;

            default:
                break;
        }
    }
}

#pragma mark Help methods

- (IBAction)showShortcuts:(id)sender {
    NSAlert *alert = [[NSAlert alloc] init];
    alert.messageText = @"Mapbox GL Help";
    alert.informativeText = @"\
• To scroll, swipe with two fingers on a trackpad, or drag the cursor, or press the arrow keys.\n\
• To zoom in, pinch two fingers apart on a trackpad, or double-click, or hold down Shift while dragging the cursor down, or hold down Option while pressing the up key.\n\
• To zoom out, pinch two fingers together on a trackpad, or double-tap with two fingers on a trackpad, or double-tap on a mouse, or hold down Shift while dragging the cursor up, or hold down Option while pressing the down key.\n\
• To rotate, move two fingers opposite each other in a circle on a trackpad, or hold down Option while dragging the cursor left and right, or hold down Option while pressing the left and right arrow keys.\n\
• To tilt, hold down Option while dragging the cursor up and down.\n\
• To drop a pin, click and hold.\
";
    [alert runModal];
}

- (IBAction)showPreferences:(id)sender {
    [self.preferencesWindow makeKeyAndOrderFront:sender];
}

- (IBAction)print:(id)sender {
    NSDocument *currentDocument = [NSDocumentController sharedDocumentController].currentDocument;
    if ([currentDocument isKindOfClass:[MapDocument class]]) {
        MGLMapView *mapView = [(MapDocument *)currentDocument mapView];
        [mapView print:sender];
    }
}

- (IBAction)openAccessTokenManager:(id)sender {
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://www.mapbox.com/studio/account/tokens/"]];
}

#pragma mark User interface validation

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem {
    if (menuItem.action == @selector(showShortcuts:)) {
        return YES;
    }
    if (menuItem.action == @selector(showPreferences:)) {
        return YES;
    }
    if (menuItem.action == @selector(showOfflinePacksPanel:)) {
        return YES;
    }
    if (menuItem.action == @selector(print:)) {
        return YES;
    }
    if (menuItem.action == @selector(delete:)) {
        return self.offlinePacksArrayController.selectedObjects.count;
    }
    return NO;
}

#pragma mark NSWindowDelegate methods

- (void)windowWillClose:(NSNotification *)notification {
    NSWindow *window = notification.object;
    if (window == self.preferencesWindow) {
        [window makeFirstResponder:nil];
    }
}

@end
