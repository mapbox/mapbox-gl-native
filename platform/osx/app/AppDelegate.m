#import "AppDelegate.h"

#import "MapDocument.h"

NSString * const MGLMapboxAccessTokenDefaultsKey = @"MGLMapboxAccessToken";
NSString * const MGLLastMapCameraDefaultsKey = @"MGLLastMapCamera";
NSString * const MGLLastMapStyleURLDefaultsKey = @"MGLLastMapStyleURL";
NSString * const MGLLastMapDebugMaskDefaultsKey = @"MGLLastMapDebugMask";

@interface AppDelegate ()

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

#pragma mark Help methods

- (IBAction)showShortcuts:(id)sender {
    NSAlert *alert = [[NSAlert alloc] init];
    alert.messageText = @"Mapbox GL Help";
    alert.informativeText = @"\
• To scroll, swipe with two fingers, drag the cursor, or press the arrow keys.\n\
• To zoom, pinch with two fingers, or hold down Shift while dragging the cursor up and down, or hold down Option while pressing the up and down arrow keys.\n\
• To rotate, move two fingers opposite each other in a circle, or hold down Option while dragging the cursor left and right, or hold down Option while pressing the left and right arrow keys.\n\
• To tilt, hold down Option while dragging the cursor up and down.\n\
• To drop a pin, click and hold.\
";
    [alert runModal];
}

- (IBAction)showPreferences:(id)sender {
    [self.preferencesWindow makeKeyAndOrderFront:sender];
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
    return NO;
}

@end
