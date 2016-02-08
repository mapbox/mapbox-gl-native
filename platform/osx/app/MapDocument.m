#import "MapDocument.h"

#import "AppDelegate.h"
#import "DroppedPinAnnotation.h"
#import "NSValue+Additions.h"

#import <Mapbox/Mapbox.h>

static NSString * const MGLDroppedPinAnnotationImageIdentifier = @"dropped";

static const CLLocationCoordinate2D WorldTourDestinations[] = {
    { .latitude = 38.9131982, .longitude = -77.0325453144239 },
    { .latitude = 37.7757368, .longitude = -122.4135302 },
    { .latitude = 12.9810816, .longitude = 77.6368034 },
    { .latitude = -13.15589555, .longitude = -74.2178961777998 },
};

@interface MapDocument () <NSWindowDelegate, NSSharingServicePickerDelegate, NSMenuDelegate, MGLMapViewDelegate>

@property (weak) IBOutlet NSMenu *mapViewContextMenu;

@end

@implementation MapDocument {
    /// Style URL inherited from an existing document at the time this document
    /// was created.
    NSURL *_inheritedStyleURL;
    
    NSPoint _mouseLocationForMapViewContextMenu;
    NSUInteger _droppedPinCounter;
    NSNumberFormatter *_spellOutNumberFormatter;
    
    BOOL _showsToolTipsOnDroppedPins;
    BOOL _randomizesCursorsOnDroppedPins;
    BOOL _isTouringWorld;
}

#pragma mark Lifecycle

- (NSString *)windowNibName {
    return @"MapDocument";
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)windowControllerWillLoadNib:(NSWindowController *)windowController {
    NSDocument *currentDocument = [NSDocumentController sharedDocumentController].currentDocument;
    if ([currentDocument isKindOfClass:[MapDocument class]]) {
        _inheritedStyleURL = [(MapDocument *)currentDocument mapView].styleURL;
    }
}

- (void)windowControllerDidLoadNib:(NSWindowController *)controller {
    [super windowControllerDidLoadNib:controller];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(userDefaultsDidChange:)
                                                 name:NSUserDefaultsDidChangeNotification
                                               object:nil];
    
    _spellOutNumberFormatter = [[NSNumberFormatter alloc] init];
    
    NSPressGestureRecognizer *pressGestureRecognizer = [[NSPressGestureRecognizer alloc] initWithTarget:self action:@selector(handlePressGesture:)];
    [self.mapView addGestureRecognizer:pressGestureRecognizer];
    
    [self applyPendingState];
}

- (NSWindow *)window {
    return self.windowControllers.firstObject.window;
}

- (void)userDefaultsDidChange:(NSNotification *)notification {
    NSUserDefaults *userDefaults = notification.object;
    NSString *accessToken = [userDefaults stringForKey:MGLMapboxAccessTokenDefaultsKey];
    if (![accessToken isEqualToString:[MGLAccountManager accessToken]]) {
        [MGLAccountManager setAccessToken:accessToken];
        [self reload:self];
    }
}

#pragma mark NSWindowDelegate methods

- (void)window:(NSWindow *)window willEncodeRestorableState:(NSCoder *)state {
    [state encodeObject:self.mapView.styleURL forKey:@"MBXMapViewStyleURL"];
}

- (void)window:(NSWindow *)window didDecodeRestorableState:(NSCoder *)state {
    self.mapView.styleURL = [state decodeObjectForKey:@"MBXMapViewStyleURL"];
}

#pragma mark Services

- (IBAction)showShareMenu:(id)sender {
    NSSharingServicePicker *picker = [[NSSharingServicePicker alloc] initWithItems:@[self.shareURL]];
    picker.delegate = self;
    [picker showRelativeToRect:[sender bounds] ofView:sender preferredEdge:NSMinYEdge];
}

- (NSURL *)shareURL {
    NSArray *components = self.mapView.styleURL.pathComponents;
    CLLocationCoordinate2D centerCoordinate = self.mapView.centerCoordinate;
    return [NSURL URLWithString:
            [NSString stringWithFormat:@"https://api.mapbox.com/styles/v1/%@/%@.html?access_token=%@#%.2f/%.5f/%.5f/%.f",
             components[1], components[2], [MGLAccountManager accessToken],
             self.mapView.zoomLevel, centerCoordinate.latitude, centerCoordinate.longitude, self.mapView.direction]];
}

#pragma mark View methods

- (IBAction)setStyle:(id)sender {
    NSInteger tag;
    if ([sender isKindOfClass:[NSMenuItem class]]) {
        tag = [sender tag];
    } else if ([sender isKindOfClass:[NSPopUpButton class]]) {
        tag = [sender selectedTag];
    }
    NSURL *styleURL;
    switch (tag) {
        case 1:
            styleURL = [MGLStyle streetsStyleURL];
            break;
        case 2:
            styleURL = [MGLStyle emeraldStyleURL];
            break;
        case 3:
            styleURL = [MGLStyle lightStyleURL];
            break;
        case 4:
            styleURL = [MGLStyle darkStyleURL];
            break;
        case 5:
            styleURL = [MGLStyle satelliteStyleURL];
            break;
        case 6:
            styleURL = [MGLStyle hybridStyleURL];
            break;
        default:
            NSAssert(NO, @"Cannot set style from control with tag %li", (long)tag);
            break;
    }
    self.mapView.styleURL = styleURL;
    [self.window.toolbar validateVisibleItems];
}

- (IBAction)chooseCustomStyle:(id)sender {
    NSAlert *alert = [[NSAlert alloc] init];
    alert.messageText = @"Apply custom style";
    alert.informativeText = @"Enter the URL to a JSON file that conforms to the Mapbox GL style specification, such as a style designed in Mapbox Studio:";
    NSTextField *textField = [[NSTextField alloc] initWithFrame:NSZeroRect];
    [textField sizeToFit];
    NSRect textFieldFrame = textField.frame;
    textFieldFrame.size.width = 300;
    textField.frame = textFieldFrame;
    NSURL *savedURL = [[NSUserDefaults standardUserDefaults] URLForKey:@"MBXCustomStyleURL"];
    if (savedURL) {
        textField.stringValue = savedURL.absoluteString;
    }
    alert.accessoryView = textField;
    [alert addButtonWithTitle:@"Apply"];
    [alert addButtonWithTitle:@"Cancel"];
    if ([alert runModal] == NSAlertFirstButtonReturn) {
        self.mapView.styleURL = [NSURL URLWithString:textField.stringValue];
        [[NSUserDefaults standardUserDefaults] setURL:self.mapView.styleURL forKey:@"MBXCustomStyleURL"];
        [self.window.toolbar validateVisibleItems];
    }
}

- (IBAction)zoomIn:(id)sender {
    [self.mapView setZoomLevel:self.mapView.zoomLevel + 1 animated:YES];
}

- (IBAction)zoomOut:(id)sender {
    [self.mapView setZoomLevel:self.mapView.zoomLevel - 1 animated:YES];
}

- (IBAction)snapToNorth:(id)sender {
    [self.mapView setDirection:0 animated:YES];
}

- (IBAction)reload:(id)sender {
    [self.mapView reloadStyle:sender];
}

- (void)applyPendingState {
    if (_inheritedStyleURL) {
        self.mapView.styleURL = _inheritedStyleURL;
        _inheritedStyleURL = nil;
    }
    
    AppDelegate *appDelegate = (AppDelegate *)NSApp.delegate;
    if (appDelegate.pendingStyleURL) {
        self.mapView.styleURL = appDelegate.pendingStyleURL;
    }
    if (appDelegate.pendingCamera) {
        if (appDelegate.pendingZoomLevel >= 0) {
            self.mapView.zoomLevel = appDelegate.pendingZoomLevel;
            appDelegate.pendingCamera.altitude = self.mapView.camera.altitude;
        }
        self.mapView.camera = appDelegate.pendingCamera;
        appDelegate.pendingZoomLevel = -1;
        appDelegate.pendingCamera = nil;
    }
    if (appDelegate.pendingDebugMask) {
        self.mapView.debugMask = appDelegate.pendingDebugMask;
    }
    
    // Temporarily set the display name to the default center coordinate instead
    // of “Untitled” until the binding kicks in.
    NSValue *coordinateValue = [NSValue valueWithCLLocationCoordinate2D:self.mapView.centerCoordinate];
    self.displayName = [[NSValueTransformer valueTransformerForName:@"LocationCoordinate2DTransformer"]
                        transformedValue:coordinateValue];
}

#pragma mark Debug methods

- (IBAction)toggleTileBoundaries:(id)sender {
    self.mapView.debugMask ^= MGLMapDebugTileBoundariesMask;
}

- (IBAction)toggleTileInfo:(id)sender {
    self.mapView.debugMask ^= MGLMapDebugTileInfoMask;
}

- (IBAction)toggleTileTimestamps:(id)sender {
    self.mapView.debugMask ^= MGLMapDebugTimestampsMask;
}

- (IBAction)toggleCollisionBoxes:(id)sender {
    self.mapView.debugMask ^= MGLMapDebugCollisionBoxesMask;
}

- (IBAction)toggleShowsToolTipsOnDroppedPins:(id)sender {
    _showsToolTipsOnDroppedPins = !_showsToolTipsOnDroppedPins;
}

- (IBAction)toggleRandomizesCursorsOnDroppedPins:(id)sender {
    _randomizesCursorsOnDroppedPins = !_randomizesCursorsOnDroppedPins;
}

- (IBAction)dropManyPins:(id)sender {
    [self.mapView removeAnnotations:self.mapView.annotations];
    
    NSRect bounds = self.mapView.bounds;
    NSMutableArray *annotations = [NSMutableArray array];
    for (CGFloat x = NSMinX(bounds); x < NSMaxX(bounds); x += arc4random_uniform(50)) {
        for (CGFloat y = NSMaxY(bounds); y >= NSMinY(bounds); y -= arc4random_uniform(100)) {
            [annotations addObject:[self pinAtPoint:NSMakePoint(x, y)]];
        }
    }
    
    [NSTimer scheduledTimerWithTimeInterval:1/60
                                     target:self
                                   selector:@selector(dropOneOfManyPins:)
                                   userInfo:annotations
                                    repeats:YES];
}

- (void)dropOneOfManyPins:(NSTimer *)timer {
    NSMutableArray *annotations = timer.userInfo;
    NSUInteger numberOfAnnotationsToAdd = 50;
    if (annotations.count < numberOfAnnotationsToAdd) {
        numberOfAnnotationsToAdd = annotations.count;
    }
    NSArray *annotationsToAdd = [annotations subarrayWithRange:
                                 NSMakeRange(0, numberOfAnnotationsToAdd)];
    [self.mapView addAnnotations:annotationsToAdd];
    [annotations removeObjectsInRange:NSMakeRange(0, numberOfAnnotationsToAdd)];
    if (!annotations.count) {
        [timer invalidate];
    }
}

- (IBAction)removeAllPins:(id)sender {
    [self.mapView removeAnnotations:self.mapView.annotations];
}

- (IBAction)startWorldTour:(id)sender {
    _isTouringWorld = YES;
    
    [self removeAllPins:sender];
    NSUInteger numberOfAnnotations = sizeof(WorldTourDestinations) / sizeof(WorldTourDestinations[0]);
    NSMutableArray *annotations = [NSMutableArray arrayWithCapacity:numberOfAnnotations];
    for (NSUInteger i = 0; i < numberOfAnnotations; i++) {
        MGLPointAnnotation *annotation = [[MGLPointAnnotation alloc] init];
        annotation.coordinate = WorldTourDestinations[i];
        [annotations addObject:annotation];
    }
    [self.mapView addAnnotations:annotations];
    [self continueWorldTourWithRemainingAnnotations:annotations];
}

- (void)continueWorldTourWithRemainingAnnotations:(NS_MUTABLE_ARRAY_OF(MGLPointAnnotation *) *)annotations {
    MGLPointAnnotation *nextAnnotation = annotations.firstObject;
    if (!nextAnnotation || !_isTouringWorld) {
        _isTouringWorld = NO;
        return;
    }
    
    [annotations removeObjectAtIndex:0];
    MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:nextAnnotation.coordinate
                                                            fromDistance:0
                                                                   pitch:arc4random_uniform(60)
                                                                 heading:arc4random_uniform(360)];
    __weak MapDocument *weakSelf = self;
    [self.mapView flyToCamera:camera completionHandler:^{
        MapDocument *strongSelf = weakSelf;
        [strongSelf performSelector:@selector(continueWorldTourWithRemainingAnnotations:)
                         withObject:annotations
                         afterDelay:2];
    }];
}

- (IBAction)stopWorldTour:(id)sender {
    _isTouringWorld = NO;
    // Any programmatic viewpoint change cancels outstanding animations.
    self.mapView.camera = self.mapView.camera;
}

#pragma mark Help methods

- (IBAction)giveFeedback:(id)sender {
    CLLocationCoordinate2D centerCoordinate = self.mapView.centerCoordinate;
    NSURL *feedbackURL = [NSURL URLWithString:[NSString stringWithFormat:@"https://www.mapbox.com/map-feedback/#/%.5f/%.5f/%.0f",
                                               centerCoordinate.longitude, centerCoordinate.latitude, round(self.mapView.zoomLevel + 1)]];
    [[NSWorkspace sharedWorkspace] openURL:feedbackURL];
}

#pragma mark Mouse events

- (void)handlePressGesture:(NSPressGestureRecognizer *)gestureRecognizer {
    if (gestureRecognizer.state == NSGestureRecognizerStateBegan) {
        NSPoint location = [gestureRecognizer locationInView:self.mapView];
        [self dropPinAtPoint:location];
    }
}

- (IBAction)dropPin:(NSMenuItem *)sender {
    [self dropPinAtPoint:_mouseLocationForMapViewContextMenu];
}

- (void)dropPinAtPoint:(NSPoint)point {
    DroppedPinAnnotation *annotation = [self pinAtPoint:point];
    [self.mapView addAnnotation:annotation];
    [self.mapView selectAnnotation:annotation];
}

- (DroppedPinAnnotation *)pinAtPoint:(NSPoint)point {
    DroppedPinAnnotation *annotation = [[DroppedPinAnnotation alloc] init];
    annotation.coordinate = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
    annotation.title = @"Dropped Pin";
    _spellOutNumberFormatter.numberStyle = NSNumberFormatterSpellOutStyle;
    if (_showsToolTipsOnDroppedPins) {
        NSString *formattedNumber = [_spellOutNumberFormatter stringFromNumber:@(++_droppedPinCounter)];
        annotation.toolTip = formattedNumber;
    }
    return annotation;
}

- (IBAction)removePin:(NSMenuItem *)sender {
    [self removePinAtPoint:_mouseLocationForMapViewContextMenu];
}

- (void)removePinAtPoint:(NSPoint)point {
    [self.mapView removeAnnotation:[self.mapView annotationAtPoint:point]];
}

#pragma mark User interface validation

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem {
    if (menuItem.action == @selector(setStyle:)) {
        NSURL *styleURL = self.mapView.styleURL;
        NSCellStateValue state;
        switch (menuItem.tag) {
            case 1:
                state = [styleURL isEqual:[MGLStyle streetsStyleURL]];
                break;
            case 2:
                state = [styleURL isEqual:[MGLStyle emeraldStyleURL]];
                break;
            case 3:
                state = [styleURL isEqual:[MGLStyle lightStyleURL]];
                break;
            case 4:
                state = [styleURL isEqual:[MGLStyle darkStyleURL]];
                break;
            case 5:
                state = [styleURL isEqual:[MGLStyle satelliteStyleURL]];
                break;
            case 6:
                state = [styleURL isEqual:[MGLStyle hybridStyleURL]];
                break;
            default:
                return NO;
        }
        menuItem.state = state;
        return YES;
    }
    if (menuItem.action == @selector(chooseCustomStyle:)) {
        menuItem.state = self.indexOfStyleInToolbarItem == NSNotFound;
        return YES;
    }
    if (menuItem.action == @selector(zoomIn:)) {
        return self.mapView.zoomLevel < self.mapView.maximumZoomLevel;
    }
    if (menuItem.action == @selector(zoomOut:)) {
        return self.mapView.zoomLevel > self.mapView.minimumZoomLevel;
    }
    if (menuItem.action == @selector(snapToNorth:)) {
        return self.mapView.direction != 0;
    }
    if (menuItem.action == @selector(reload:)) {
        return YES;
    }
    if (menuItem.action == @selector(dropPin:)) {
        BOOL isOverAnnotation = [self.mapView annotationAtPoint:_mouseLocationForMapViewContextMenu];
        menuItem.hidden = isOverAnnotation;
        return YES;
    }
    if (menuItem.action == @selector(removePin:)) {
        BOOL isOverAnnotation = [self.mapView annotationAtPoint:_mouseLocationForMapViewContextMenu];
        menuItem.hidden = !isOverAnnotation;
        return YES;
    }
    if (menuItem.action == @selector(toggleTileBoundaries:)) {
        BOOL isShown = self.mapView.debugMask & MGLMapDebugTileBoundariesMask;
        menuItem.title = isShown ? @"Hide Tile Boundaries" : @"Show Tile Boundaries";
        return YES;
    }
    if (menuItem.action == @selector(toggleTileInfo:)) {
        BOOL isShown = self.mapView.debugMask & MGLMapDebugTileInfoMask;
        menuItem.title = isShown ? @"Hide Tile Info" : @"Show Tile Info";
        return YES;
    }
    if (menuItem.action == @selector(toggleTileTimestamps:)) {
        BOOL isShown = self.mapView.debugMask & MGLMapDebugTimestampsMask;
        menuItem.title = isShown ? @"Hide Tile Timestamps" : @"Show Tile Timestamps";
        return YES;
    }
    if (menuItem.action == @selector(toggleCollisionBoxes:)) {
        BOOL isShown = self.mapView.debugMask & MGLMapDebugCollisionBoxesMask;
        menuItem.title = isShown ? @"Hide Collision Boxes" : @"Show Collision Boxes";
        return YES;
    }
    if (menuItem.action == @selector(toggleShowsToolTipsOnDroppedPins:)) {
        BOOL isShown = _showsToolTipsOnDroppedPins;
        menuItem.title = isShown ? @"Hide Tooltips on Dropped Pins" : @"Show Tooltips on Dropped Pins";
        return YES;
    }
    if (menuItem.action == @selector(toggleRandomizesCursorsOnDroppedPins:)) {
        BOOL isRandom = _randomizesCursorsOnDroppedPins;
        menuItem.title = isRandom ? @"Use Default Cursor for Dropped Pins" : @"Use Random Cursors for Dropped Pins";
        return _showsToolTipsOnDroppedPins;
    }
    if (menuItem.action == @selector(dropManyPins:)) {
        return YES;
    }
    if (menuItem.action == @selector(removeAllPins:)) {
        return self.mapView.annotations.count;
    }
    if (menuItem.action == @selector(startWorldTour:)) {
        return !_isTouringWorld;
    }
    if (menuItem.action == @selector(stopWorldTour:)) {
        return _isTouringWorld;
    }
    if (menuItem.action == @selector(giveFeedback:)) {
        return YES;
    }
    return NO;
}

- (NSUInteger)indexOfStyleInToolbarItem {
    if (![MGLAccountManager accessToken]) {
        return NSNotFound;
    }
    
    NSArray *styleURLs = @[
        [MGLStyle streetsStyleURL],
        [MGLStyle emeraldStyleURL],
        [MGLStyle lightStyleURL],
        [MGLStyle darkStyleURL],
        [MGLStyle satelliteStyleURL],
        [MGLStyle hybridStyleURL],
    ];
    return [styleURLs indexOfObject:self.mapView.styleURL];
}

- (BOOL)validateToolbarItem:(NSToolbarItem *)toolbarItem {
    if (!self.mapView) {
        return NO;
    }
    
    if (toolbarItem.action == @selector(showShareMenu:)) {
        [(NSButton *)toolbarItem.view sendActionOn:NSLeftMouseDownMask];
        if (![MGLAccountManager accessToken]) {
            return NO;
        }
        NSURL *styleURL = self.mapView.styleURL;
        return ([styleURL.scheme isEqualToString:@"mapbox"]
                && [styleURL.pathComponents.firstObject isEqualToString:@"styles"]);
    }
    if (toolbarItem.action == @selector(setStyle:)) {
        NSPopUpButton *popUpButton = (NSPopUpButton *)toolbarItem.view;
        NSUInteger index = self.indexOfStyleInToolbarItem;
        if (index == NSNotFound) {
            [popUpButton addItemWithTitle:@"Custom"];
            index = [popUpButton numberOfItems] - 1;
        }
        [popUpButton selectItemAtIndex:index];
    }
    return NO;
}

#pragma mark NSSharingServicePickerDelegate methods

- (NS_ARRAY_OF(NSSharingService *) *)sharingServicePicker:(NSSharingServicePicker *)sharingServicePicker sharingServicesForItems:(NSArray *)items proposedSharingServices:(NS_ARRAY_OF(NSSharingService *) *)proposedServices {
    NSURL *shareURL = self.shareURL;
    NSURL *browserURL = [[NSWorkspace sharedWorkspace] URLForApplicationToOpenURL:shareURL];
    NSImage *browserIcon = [[NSWorkspace sharedWorkspace] iconForFile:browserURL.path];
    NSString *browserName = [[NSFileManager defaultManager] displayNameAtPath:browserURL.path];
    NSString *browserServiceName = [NSString stringWithFormat:@"Open in %@", browserName];
    
    NSSharingService *browserService = [[NSSharingService alloc] initWithTitle:browserServiceName
                                                                         image:browserIcon
                                                                alternateImage:nil
                                                                       handler:^{
        [[NSWorkspace sharedWorkspace] openURL:self.shareURL];
    }];
    
    NSMutableArray *sharingServices = [proposedServices mutableCopy];
    [sharingServices insertObject:browserService atIndex:0];
    return sharingServices;
}

#pragma mark NSMenuDelegate methods

- (void)menuWillOpen:(NSMenu *)menu {
    if (menu == self.mapViewContextMenu) {
        _mouseLocationForMapViewContextMenu = [self.window.contentView convertPoint:self.window.mouseLocationOutsideOfEventStream
                                                                             toView:self.mapView];
    }
}

#pragma mark MGLMapViewDelegate methods

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id <MGLAnnotation>)annotation {
    return YES;
}

- (MGLAnnotationImage *)mapView:(MGLMapView *)mapView imageForAnnotation:(id <MGLAnnotation>)annotation {
    MGLAnnotationImage *annotationImage = [self.mapView dequeueReusableAnnotationImageWithIdentifier:MGLDroppedPinAnnotationImageIdentifier];
    if (!annotationImage) {
        NSString *imagePath = [[NSBundle bundleForClass:[MGLMapView class]]
                               pathForResource:@"default_marker" ofType:@"pdf"];
        NSImage *image = [[NSImage alloc] initWithContentsOfFile:imagePath];
        NSRect alignmentRect = image.alignmentRect;
        alignmentRect.origin.y = NSMidY(alignmentRect);
        alignmentRect.size.height /= 2;
        image.alignmentRect = alignmentRect;
        annotationImage = [MGLAnnotationImage annotationImageWithImage:image
                                                       reuseIdentifier:MGLDroppedPinAnnotationImageIdentifier];
    }
    if (_randomizesCursorsOnDroppedPins) {
        NSArray *cursors = @[
            [NSCursor IBeamCursor],
            [NSCursor crosshairCursor],
            [NSCursor pointingHandCursor],
            [NSCursor disappearingItemCursor],
            [NSCursor IBeamCursorForVerticalLayout],
            [NSCursor operationNotAllowedCursor],
            [NSCursor dragLinkCursor],
            [NSCursor dragCopyCursor],
            [NSCursor contextualMenuCursor],
        ];
        annotationImage.cursor = cursors[arc4random_uniform(cursors.count)];
    } else {
        annotationImage.cursor = nil;
    }
    return annotationImage;
}

- (void)mapView:(MGLMapView *)mapView didSelectAnnotation:(id <MGLAnnotation>)annotation {
    if ([annotation isKindOfClass:[DroppedPinAnnotation class]]) {
        DroppedPinAnnotation *droppedPin = annotation;
        [droppedPin resume];
    }
}

- (void)mapView:(MGLMapView *)mapView didDeselectAnnotation:(id <MGLAnnotation>)annotation {
    if ([annotation isKindOfClass:[DroppedPinAnnotation class]]) {
        DroppedPinAnnotation *droppedPin = annotation;
        [droppedPin pause];
    }
}

@end

@interface ValidatedToolbarItem : NSToolbarItem

@end

@implementation ValidatedToolbarItem

- (void)validate {
    [(MapDocument *)self.toolbar.delegate validateToolbarItem:self];
}

@end
