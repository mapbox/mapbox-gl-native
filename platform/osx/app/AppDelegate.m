#import "AppDelegate.h"

#import "LocationCoordinate2DTransformer.h"
#import "MapDocument.h"
#import "NSValue+Additions.h"

#import <Mapbox/Mapbox.h>

static NSString * const MGLMapboxAccessTokenDefaultsKey = @"MGLMapboxAccessToken";

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
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(userDefaultsDidChange:)
                                                 name:NSUserDefaultsDidChangeNotification
                                               object:nil];
    
    [[NSAppleEventManager sharedAppleEventManager] setEventHandler:self
                                                       andSelector:@selector(handleGetURLEvent:withReplyEvent:)
                                                     forEventClass:kInternetEventClass
                                                        andEventID:kAEGetURL];
    
    // Normally, an application should respect the “Close windows when quitting
    // an application” setting in the General pane of System Preferences. But
    // the map would only be restored to its last opened location if the user
    // quits the application using Quit and Keep Windows. An application that
    // displays only a map should restore the last viewed map, like Maps.app
    // does.
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSQuitAlwaysKeepsWindows"];
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
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)userDefaultsDidChange:(NSNotification *)notification {
    NSUserDefaults *userDefaults = notification.object;
    NSString *accessToken = [userDefaults stringForKey:MGLMapboxAccessTokenDefaultsKey];
    if (![accessToken isEqualToString:[MGLAccountManager accessToken]]) {
        [MGLAccountManager setAccessToken:accessToken];
        [self.mainDocument reload:self];
    }
}

- (MapDocument *)mainDocument {
    NSDocument *mainDocument = [NSApp mainWindow].windowController.document;
    return [mainDocument isKindOfClass:[MapDocument class]] ? (MapDocument *)mainDocument : nil;
}

#pragma mark Services

- (void)handleGetURLEvent:(NSAppleEventDescriptor *)event withReplyEvent:(NSAppleEventDescriptor *)replyEvent {
    [[NSDocumentController sharedDocumentController] newDocument:self];
    
    NSURL *url = [NSURL URLWithString:[event paramDescriptorForKeyword:keyDirectObject].stringValue];
    NS_MUTABLE_DICTIONARY_OF(NSString *, NSString *) *params = [[NSMutableDictionary alloc] init];
    for (NSString *param in [url.query componentsSeparatedByString:@"&"]) {
        NSArray *parts = [param componentsSeparatedByString:@"="];
        if (parts.count >= 2) {
            params[parts[0]] = [parts[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        }
    }
    
    NSString *centerString = params[@"center"];
    if (centerString) {
        NSArray *coordinateValues = [centerString componentsSeparatedByString:@","];
        if (coordinateValues.count == 2) {
            self.mainDocument.mapView.centerCoordinate = CLLocationCoordinate2DMake([coordinateValues[0] doubleValue],
                                                                                    [coordinateValues[1] doubleValue]);
        }
    }
    
    NSString *zoomLevelString = params[@"zoom"];
    if (zoomLevelString.length) {
        self.mainDocument.mapView.zoomLevel = zoomLevelString.doubleValue;
    }
    
    NSString *directionString = params[@"bearing"];
    if (directionString.length) {
        self.mainDocument.mapView.direction = directionString.doubleValue;
    }
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
