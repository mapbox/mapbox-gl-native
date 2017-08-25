#import "MapDocument.h"

#import "AppDelegate.h"
#import "LimeGreenStyleLayer.h"
#import "DroppedPinAnnotation.h"

#import "MGLStyle+MBXAdditions.h"
#import "MGLVectorSource+MGLAdditions.h"

#import <Mapbox/Mapbox.h>

static NSString * const MGLDroppedPinAnnotationImageIdentifier = @"dropped";

static const CLLocationCoordinate2D WorldTourDestinations[] = {
    { .latitude = 38.9131982, .longitude = -77.0325453144239 },
    { .latitude = 37.7757368, .longitude = -122.4135302 },
    { .latitude = 12.9810816, .longitude = 77.6368034 },
    { .latitude = -13.15589555, .longitude = -74.2178961777998 },
};

NS_ARRAY_OF(id <MGLAnnotation>) *MBXFlattenedShapes(NS_ARRAY_OF(id <MGLAnnotation>) *shapes) {
    NSMutableArray *flattenedShapes = [NSMutableArray arrayWithCapacity:shapes.count];
    for (id <MGLAnnotation> shape in shapes) {
        NSArray *subshapes;
        if ([shape isKindOfClass:[MGLMultiPolyline class]]) {
            subshapes = [(MGLMultiPolyline *)shape polylines];
        } else if ([shape isKindOfClass:[MGLMultiPolygon class]]) {
            subshapes = [(MGLMultiPolygon *)shape polygons];
        } else if ([shape isKindOfClass:[MGLPointCollection class]]) {
            NSUInteger pointCount = [(MGLPointCollection *)shape pointCount];
            CLLocationCoordinate2D *coordinates = [(MGLPointCollection *)shape coordinates];
            NSMutableArray *pointAnnotations = [NSMutableArray arrayWithCapacity:pointCount];
            for (NSUInteger i = 0; i < pointCount; i++) {
                MGLPointAnnotation *pointAnnotation = [[MGLPointAnnotation alloc] init];
                pointAnnotation.coordinate = coordinates[i];
                [pointAnnotations addObject:pointAnnotation];
            }
            subshapes = pointAnnotations;
        } else if ([shape isKindOfClass:[MGLShapeCollection class]]) {
            subshapes = MBXFlattenedShapes([(MGLShapeCollection *)shape shapes]);
        }

        if (subshapes) {
            [flattenedShapes addObjectsFromArray:subshapes];
        } else {
            [flattenedShapes addObject:shape];
        }
    }
    return flattenedShapes;
}

@interface MapDocument () <NSWindowDelegate, NSSharingServicePickerDelegate, NSMenuDelegate, NSSplitViewDelegate, MGLMapViewDelegate>

@property (weak) IBOutlet NSArrayController *styleLayersArrayController;
@property (weak) IBOutlet NSTableView *styleLayersTableView;
@property (weak) IBOutlet NSMenu *mapViewContextMenu;
@property (weak) IBOutlet NSSplitView *splitView;

@end

@implementation MapDocument {
    /// Style URL inherited from an existing document at the time this document
    /// was created.
    NSURL *_inheritedStyleURL;

    NSPoint _mouseLocationForMapViewContextMenu;
    NSUInteger _droppedPinCounter;
    NSNumberFormatter *_spellOutNumberFormatter;

    BOOL _isLocalizingLabels;
    BOOL _showsToolTipsOnDroppedPins;
    BOOL _randomizesCursorsOnDroppedPins;
    BOOL _isTouringWorld;
    BOOL _isShowingPolygonAndPolylineAnnotations;
    BOOL _isShowingAnimatedAnnotation;
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

    [self.splitView setPosition:0 ofDividerAtIndex:0];

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
    [state encodeBool:_isLocalizingLabels forKey:@"MBXLocalizeLabels"];
}

- (void)window:(NSWindow *)window didDecodeRestorableState:(NSCoder *)state {
    self.mapView.styleURL = [state decodeObjectForKey:@"MBXMapViewStyleURL"];
    _isLocalizingLabels = [state decodeBoolForKey:@"MBXLocalizeLabels"];
}

#pragma mark Services

- (IBAction)showShareMenu:(id)sender {
    NSSharingServicePicker *picker = [[NSSharingServicePicker alloc] initWithItems:@[self.shareURL]];
    picker.delegate = self;
    [picker showRelativeToRect:[sender bounds] ofView:sender preferredEdge:NSMinYEdge];
}

- (NSURL *)shareURL {
    NSArray *components = self.mapView.styleURL.pathComponents;
    MGLMapCamera *camera = self.mapView.camera;
    return [NSURL URLWithString:
            [NSString stringWithFormat:@"https://api.mapbox.com/styles/v1/%@/%@.html?access_token=%@#%.2f/%.5f/%.5f/%.f/%.f",
             components[1], components[2], [MGLAccountManager accessToken],
             self.mapView.zoomLevel, camera.centerCoordinate.latitude, camera.centerCoordinate.longitude,
             camera.heading, camera.pitch]];
}

#pragma mark View methods

- (IBAction)showStyle:(id)sender {
    NSInteger tag = -1;
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
            styleURL = [MGLStyle outdoorsStyleURL];
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
            styleURL = [MGLStyle satelliteStreetsStyleURL];
            break;
        case 7:
            styleURL = [MGLStyle trafficDayStyleURL];
            break;
        case 8:
            styleURL = [MGLStyle trafficNightStyleURL];
            break;
        default:
            NSAssert(NO, @"Cannot set style from control with tag %li", (long)tag);
            break;
    }
    [self.undoManager removeAllActionsWithTarget:self];
    self.mapView.styleURL = styleURL;
    [self.window.toolbar validateVisibleItems];
}

- (IBAction)chooseCustomStyle:(id)sender {
    NSAlert *alert = [[NSAlert alloc] init];
    alert.messageText = @"Apply custom style";
    alert.informativeText = @"Enter the URL to a JSON file that conforms to the Mapbox Style Specification, such as a style designed in Mapbox Studio:";
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
        [self.undoManager removeAllActionsWithTarget:self];
        self.mapView.styleURL = [NSURL URLWithString:textField.stringValue];
        [[NSUserDefaults standardUserDefaults] setURL:self.mapView.styleURL forKey:@"MBXCustomStyleURL"];
        [self.window.toolbar validateVisibleItems];
    }
}

- (IBAction)zoomIn:(id)sender {
    [self.mapView setZoomLevel:round(self.mapView.zoomLevel) + 1 animated:YES];
}

- (IBAction)zoomOut:(id)sender {
    [self.mapView setZoomLevel:round(self.mapView.zoomLevel) - 1 animated:YES];
}

- (IBAction)snapToNorth:(id)sender {
    [self.mapView setDirection:0 animated:YES];
}

- (IBAction)reload:(id)sender {
    [self.undoManager removeAllActionsWithTarget:self];
    [self.mapView reloadStyle:sender];
}

/**
 Show or hide the Layers sidebar.
 */
- (IBAction)toggleLayers:(id)sender {
    BOOL isShown = ![self.splitView isSubviewCollapsed:self.splitView.arrangedSubviews.firstObject];
    [NSAnimationContext runAnimationGroup:^(NSAnimationContext * _Nonnull context) {
        context.allowsImplicitAnimation = YES;
        [self.splitView setPosition:isShown ? 0 : 100 ofDividerAtIndex:0];
        [self.window.toolbar validateVisibleItems];
    } completionHandler:nil];
}

/**
 Show or hide the selected layers.
 */
- (IBAction)toggleStyleLayers:(id)sender {
    NSInteger clickedRow = self.styleLayersTableView.clickedRow;
    NSIndexSet *indices = self.styleLayersTableView.selectedRowIndexes;
    if (clickedRow >= 0 && ![indices containsIndex:clickedRow]) {
        indices = [NSIndexSet indexSetWithIndex:clickedRow];
    }
    [self toggleStyleLayersAtArrangedObjectIndexes:indices];
}

- (void)toggleStyleLayersAtArrangedObjectIndexes:(NSIndexSet *)indices {
    NS_ARRAY_OF(MGLStyleLayer *) *layers = [self.mapView.style.reversedLayers objectsAtIndexes:indices];
    BOOL isVisible = layers.firstObject.visible;
    [self.undoManager registerUndoWithTarget:self handler:^(MapDocument * _Nonnull target) {
        [target toggleStyleLayersAtArrangedObjectIndexes:indices];
    }];

    if (!self.undoManager.undoing) {
        NSString *actionName;
        if (indices.count == 1) {
            actionName = [NSString stringWithFormat:@"%@ Layer “%@”", isVisible ? @"Hide" : @"Show", layers.firstObject.identifier];
        } else {
            actionName = [NSString stringWithFormat:@"%@ %@ Layers", isVisible ? @"Hide" : @"Show",
                          [NSNumberFormatter localizedStringFromNumber:@(indices.count)
                                                           numberStyle:NSNumberFormatterDecimalStyle]];
        }
        [self.undoManager setActionIsDiscardable:YES];
        [self.undoManager setActionName:actionName];
    }

    for (MGLStyleLayer *layer in layers) {
        layer.visible = !isVisible;
    }

    NSIndexSet *columnIndices = [NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, 2)];
    [self.styleLayersTableView reloadDataForRowIndexes:indices columnIndexes:columnIndices];
}

- (IBAction)deleteStyleLayers:(id)sender {
    NSInteger clickedRow = self.styleLayersTableView.clickedRow;
    NSIndexSet *indices = self.styleLayersTableView.selectedRowIndexes;
    if (clickedRow >= 0 && ![indices containsIndex:clickedRow]) {
        indices = [NSIndexSet indexSetWithIndex:clickedRow];
    }
    [self deleteStyleLayersAtArrangedObjectIndexes:indices];
}

- (void)insertStyleLayers:(NS_ARRAY_OF(MGLStyleLayer *) *)layers atArrangedObjectIndexes:(NSIndexSet *)indices {
    [self.undoManager registerUndoWithTarget:self handler:^(id  _Nonnull target) {
        [self deleteStyleLayersAtArrangedObjectIndexes:indices];
    }];

    if (!self.undoManager.undoing) {
        NSString *actionName;
        if (indices.count == 1) {
            actionName = [NSString stringWithFormat:@"Add Layer “%@”", layers.firstObject.identifier];
        } else {
            actionName = [NSString stringWithFormat:@"Add %@ Layers",
                          [NSNumberFormatter localizedStringFromNumber:@(indices.count) numberStyle:NSNumberFormatterDecimalStyle]];
        }
        [self.undoManager setActionName:actionName];
    }

    [self.styleLayersArrayController insertObjects:layers atArrangedObjectIndexes:indices];
}

- (void)deleteStyleLayersAtArrangedObjectIndexes:(NSIndexSet *)indices {
    NS_ARRAY_OF(MGLStyleLayer *) *layers = [self.mapView.style.reversedLayers objectsAtIndexes:indices];
    [self.undoManager registerUndoWithTarget:self handler:^(id  _Nonnull target) {
        [self insertStyleLayers:layers atArrangedObjectIndexes:indices];
    }];

    if (!self.undoManager.undoing) {
        NSString *actionName;
        if (indices.count == 1) {
            actionName = [NSString stringWithFormat:@"Delete Layer “%@”", layers.firstObject.identifier];
        } else {
            actionName = [NSString stringWithFormat:@"Delete %@ Layers",
                          [NSNumberFormatter localizedStringFromNumber:@(indices.count) numberStyle:NSNumberFormatterDecimalStyle]];
        }
        [self.undoManager setActionName:actionName];
    }

    [self.styleLayersArrayController removeObjectsAtArrangedObjectIndexes:indices];
}

- (IBAction)setLabelLanguage:(NSMenuItem *)sender {
    _isLocalizingLabels = sender.tag;
    [self reload:sender];
}

- (void)updateLabels {
    self.mapView.style.localizesLabels = _isLocalizingLabels;
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
    if (!MGLCoordinateBoundsIsEmpty(appDelegate.pendingVisibleCoordinateBounds)) {
        self.mapView.visibleCoordinateBounds = appDelegate.pendingVisibleCoordinateBounds;
        appDelegate.pendingVisibleCoordinateBounds = (MGLCoordinateBounds){ { 0, 0 }, { 0, 0 } };
    }
    if (appDelegate.pendingDebugMask) {
        self.mapView.debugMask = appDelegate.pendingDebugMask;
    }
    if (appDelegate.pendingMinimumZoomLevel >= 0) {
        self.mapView.zoomLevel = MAX(appDelegate.pendingMinimumZoomLevel, self.mapView.zoomLevel);
        appDelegate.pendingMaximumZoomLevel = -1;
    }
    if (appDelegate.pendingMaximumZoomLevel >= 0) {
        self.mapView.zoomLevel = MIN(appDelegate.pendingMaximumZoomLevel, self.mapView.zoomLevel);
        appDelegate.pendingMaximumZoomLevel = -1;
    }

    // Temporarily set the display name to the default center coordinate instead
    // of “Untitled” until the binding kicks in.
    NSValue *coordinateValue = [NSValue valueWithMGLCoordinate:self.mapView.centerCoordinate];
    NSString *coordinateString = [[NSValueTransformer valueTransformerForName:@"LocationCoordinate2DTransformer"]
                        transformedValue:coordinateValue];


    self.displayName = [NSString stringWithFormat:@"%@ @ %f", coordinateString, _mapView.zoomLevel];
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

- (IBAction)toggleOverdrawVisualization:(id)sender {
    self.mapView.debugMask ^= MGLMapDebugOverdrawVisualizationMask;
}

- (IBAction)showColorBuffer:(id)sender {
    self.mapView.debugMask &= ~MGLMapDebugStencilBufferMask;
    self.mapView.debugMask &= ~MGLMapDebugDepthBufferMask;
}

- (IBAction)showStencilBuffer:(id)sender {
    self.mapView.debugMask &= ~MGLMapDebugDepthBufferMask;
    self.mapView.debugMask |= MGLMapDebugStencilBufferMask;
}

- (IBAction)showDepthBuffer:(id)sender {
    self.mapView.debugMask &= ~MGLMapDebugStencilBufferMask;
    self.mapView.debugMask |= MGLMapDebugDepthBufferMask;
}

- (IBAction)toggleShowsToolTipsOnDroppedPins:(id)sender {
    _showsToolTipsOnDroppedPins = !_showsToolTipsOnDroppedPins;
}

- (IBAction)toggleRandomizesCursorsOnDroppedPins:(id)sender {
    _randomizesCursorsOnDroppedPins = !_randomizesCursorsOnDroppedPins;
}

- (IBAction)dropManyPins:(id)sender {
    [self removeAllAnnotations:sender];

    NSRect bounds = self.mapView.bounds;
    NSMutableArray *annotations = [NSMutableArray array];
    for (CGFloat x = NSMinX(bounds); x < NSMaxX(bounds); x += arc4random_uniform(50)) {
        for (CGFloat y = NSMaxY(bounds); y >= NSMinY(bounds); y -= arc4random_uniform(100)) {
            [annotations addObject:[self pinAtPoint:NSMakePoint(x, y)]];
        }
    }

    [NSTimer scheduledTimerWithTimeInterval:1.0/60.0
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

- (IBAction)showAllAnnotations:(id)sender {
    [self.mapView showAnnotations:self.mapView.annotations animated:YES];
}

- (IBAction)removeAllAnnotations:(id)sender {
    [self.mapView removeAnnotations:self.mapView.annotations];
    _isShowingPolygonAndPolylineAnnotations = NO;
    _isShowingAnimatedAnnotation = NO;
}

- (IBAction)startWorldTour:(id)sender {
    _isTouringWorld = YES;

    [self removeAllAnnotations:sender];
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

- (IBAction)drawPolygonAndPolyLineAnnotations:(id)sender {

    if (_isShowingPolygonAndPolylineAnnotations) {
        [self removeAllAnnotations:sender];
        return;
    }

    _isShowingPolygonAndPolylineAnnotations = YES;

    // Pacific Northwest triangle
    CLLocationCoordinate2D triangleCoordinates[3] = {
        CLLocationCoordinate2DMake(44, -122),
        CLLocationCoordinate2DMake(46, -122),
        CLLocationCoordinate2DMake(46, -121)
    };
    MGLPolygon *triangle = [MGLPolygon polygonWithCoordinates:triangleCoordinates count:3];
    [self.mapView addAnnotation:triangle];

    // West coast line
    CLLocationCoordinate2D lineCoordinates[4] = {
        CLLocationCoordinate2DMake(47.6025, -122.3327),
        CLLocationCoordinate2DMake(45.5189, -122.6726),
        CLLocationCoordinate2DMake(37.7790, -122.4177),
        CLLocationCoordinate2DMake(34.0532, -118.2349)
    };
    MGLPolyline *line = [MGLPolyline polylineWithCoordinates:lineCoordinates count:4];
    [self.mapView addAnnotation:line];
}

- (IBAction)drawAnimatedAnnotation:(id)sender {
    DroppedPinAnnotation *annotation = [[DroppedPinAnnotation alloc] init];
    [self.mapView addAnnotation:annotation];

    _isShowingAnimatedAnnotation = YES;

    [NSTimer scheduledTimerWithTimeInterval:1.0/60.0
                                     target:self
                                   selector:@selector(updateAnimatedAnnotation:)
                                   userInfo:annotation
                                    repeats:YES];
}

- (void)updateAnimatedAnnotation:(NSTimer *)timer {
    DroppedPinAnnotation *annotation = timer.userInfo;
    double angle = timer.fireDate.timeIntervalSinceReferenceDate;
    annotation.coordinate = CLLocationCoordinate2DMake(
        sin(angle) * 20,
        cos(angle) * 20);
}

- (IBAction) addAnimatedImageSource:(id)sender {

    MGLImage *image = [[NSBundle bundleForClass:[self class]] imageForResource:@"southeast_0"];

    MGLCoordinateBounds bounds = { {22.551103322318994, -90.24006072802854}, {36.928147474567794, -75.1441643681673} };
    MGLImageSource *imageSource = [[MGLImageSource alloc] initWithIdentifier:@"animated-radar-source" coordinateQuad:MGLCoordinateQuadFromCoordinateBounds(bounds) image:image];
    [self.mapView.style addSource:imageSource];

    MGLRasterStyleLayer * imageLayer = [[MGLRasterStyleLayer alloc] initWithIdentifier:@"animated-radar-layer" source:imageSource];
    [self.mapView.style addLayer:imageLayer];
    
    [NSTimer scheduledTimerWithTimeInterval:1.0
                                     target:self
                                   selector:@selector(updateAnimatedImageSource:)
                                   userInfo:imageSource
                                    repeats:YES];
}


- (void)updateAnimatedImageSource:(NSTimer *)timer {
    static int radarSuffix = 0;
    MGLImageSource *imageSource = (MGLImageSource *)timer.userInfo;
    
    MGLImage *image = [[NSBundle bundleForClass:[self class]] imageForResource:[NSString stringWithFormat:@"southeast_%d", radarSuffix++]];
    [imageSource setValue:image forKey:@"image"];

    if(radarSuffix > 3) {
        radarSuffix = 0 ;
    }
}

- (IBAction)insertCustomStyleLayer:(id)sender {
    [self.undoManager registerUndoWithTarget:self handler:^(id  _Nonnull target) {
        [self removeCustomStyleLayer:sender];
    }];

    if (!self.undoManager.isUndoing) {
        [self.undoManager setActionName:@"Add Lime Green Layer"];
    }

    LimeGreenStyleLayer *layer = [[LimeGreenStyleLayer alloc] initWithIdentifier:@"mbx-custom"];
    MGLStyleLayer *houseNumberLayer = [self.mapView.style layerWithIdentifier:@"housenum-label"];
    if (houseNumberLayer) {
        [self.mapView.style insertLayer:layer belowLayer:houseNumberLayer];
    } else {
        [self.mapView.style addLayer:layer];
    }
}

- (IBAction)removeCustomStyleLayer:(id)sender {
    [self.undoManager registerUndoWithTarget:self handler:^(id  _Nonnull target) {
        [self insertCustomStyleLayer:sender];
    }];

    if (!self.undoManager.isUndoing) {
        [self.undoManager setActionName:@"Delete Lime Green Layer"];
    }

    MGLStyleLayer *layer = [self.mapView.style layerWithIdentifier:@"mbx-custom"];
    [self.mapView.style removeLayer:layer];
}

#pragma mark Offline packs

- (IBAction)addOfflinePack:(id)sender {
    NSAlert *namePrompt = [[NSAlert alloc] init];
    namePrompt.messageText = @"Add offline pack";
    namePrompt.informativeText = @"Choose a name for the pack:";
    NSTextField *nameTextField = [[NSTextField alloc] initWithFrame:NSZeroRect];
    nameTextField.placeholderString = MGLStringFromCoordinateBounds(self.mapView.visibleCoordinateBounds);
    [nameTextField sizeToFit];
    NSRect textFieldFrame = nameTextField.frame;
    textFieldFrame.size.width = 300;
    nameTextField.frame = textFieldFrame;
    namePrompt.accessoryView = nameTextField;
    [namePrompt addButtonWithTitle:@"Add"];
    [namePrompt addButtonWithTitle:@"Cancel"];
    if ([namePrompt runModal] != NSAlertFirstButtonReturn) {
        return;
    }

    id <MGLOfflineRegion> region = [[MGLTilePyramidOfflineRegion alloc] initWithStyleURL:self.mapView.styleURL bounds:self.mapView.visibleCoordinateBounds fromZoomLevel:self.mapView.zoomLevel toZoomLevel:self.mapView.maximumZoomLevel];
    NSData *context = [[NSValueTransformer valueTransformerForName:@"OfflinePackNameValueTransformer"] reverseTransformedValue:nameTextField.stringValue];
    [[MGLOfflineStorage sharedOfflineStorage] addPackForRegion:region withContext:context completionHandler:^(MGLOfflinePack * _Nullable pack, NSError * _Nullable error) {
        if (error) {
            [[NSAlert alertWithError:error] runModal];
        } else {
            [pack resume];
        }
    }];
}

#pragma mark Mouse events

- (void)handlePressGesture:(NSPressGestureRecognizer *)gestureRecognizer {
    if (gestureRecognizer.state == NSGestureRecognizerStateBegan) {
        NSPoint location = [gestureRecognizer locationInView:self.mapView];
        if (!NSPointInRect([gestureRecognizer locationInView:self.mapView.compass], self.mapView.compass.bounds)
            && !NSPointInRect([gestureRecognizer locationInView:self.mapView.zoomControls], self.mapView.zoomControls.bounds)
            && !NSPointInRect([gestureRecognizer locationInView:self.mapView.attributionView], self.mapView.attributionView.bounds)) {
            [self dropPinAtPoint:location];
        }
    }
}

- (IBAction)manipulateStyle:(id)sender {
    MGLTransition transition = { .duration = 5, .delay = 1 };
    self.mapView.style.transition = transition;

    MGLStyleLayer *waterLayer = [self.mapView.style layerWithIdentifier:@"water"];
    MGLStyleValue *colorFunction = [MGLStyleValue<NSColor *> valueWithInterpolationMode:MGLInterpolationModeExponential cameraStops:@{
        @0.0: [MGLStyleValue<NSColor *> valueWithRawValue:[NSColor redColor]],
        @10.0: [MGLStyleValue<NSColor *> valueWithRawValue:[NSColor yellowColor]],
        @20.0: [MGLStyleValue<NSColor *> valueWithRawValue:[NSColor blackColor]],
    } options:nil];
    
    if ([waterLayer respondsToSelector:@selector(fillColor)]) {
        [waterLayer setValue:colorFunction forKey:@"fillColor"];
    } else if ([waterLayer respondsToSelector:@selector(lineColor)]) {
        [waterLayer setValue:colorFunction forKey:@"lineColor"];
    }

    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *geoJSONURL = [NSURL fileURLWithPath:filePath];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"ams" URL:geoJSONURL options:nil];
    [self.mapView.style addSource:source];

    MGLCircleStyleLayer *circleLayer = [[MGLCircleStyleLayer alloc] initWithIdentifier:@"test" source:source];
    circleLayer.circleColor = [MGLStyleValue<NSColor *> valueWithRawValue:[NSColor greenColor]];
    circleLayer.circleRadius = [MGLStyleValue<NSNumber *> valueWithRawValue:[NSNumber numberWithInteger:40]];
//    fillLayer.predicate = [NSPredicate predicateWithFormat:@"%K == %@", @"type", @"park"];
    [self.mapView.style addLayer:circleLayer];

    MGLSource *streetsSource = [self.mapView.style sourceWithIdentifier:@"composite"];
    if (streetsSource) {
    NSImage *image = [NSImage imageNamed:NSImageNameIChatTheaterTemplate];
    [self.mapView.style setImage:image forName:NSImageNameIChatTheaterTemplate];

        MGLSymbolStyleLayer *theaterLayer = [[MGLSymbolStyleLayer alloc] initWithIdentifier:@"theaters" source:streetsSource];
        theaterLayer.sourceLayerIdentifier = @"poi_label";
        theaterLayer.predicate = [NSPredicate predicateWithFormat:@"maki == 'theatre'"];
        theaterLayer.iconImageName = [MGLStyleValue valueWithRawValue:NSImageNameIChatTheaterTemplate];
        theaterLayer.iconScale = [MGLStyleValue valueWithRawValue:@2];
        theaterLayer.iconColor = [MGLStyleValue valueWithInterpolationMode:MGLInterpolationModeExponential cameraStops:@{
            @16.0: [MGLStyleValue valueWithRawValue:[NSColor redColor]],
            @18.0: [MGLStyleValue valueWithRawValue:[NSColor yellowColor]],
            @20.0: [MGLStyleValue valueWithRawValue:[NSColor blackColor]],
        } options:nil];
        [self.mapView.style addLayer:theaterLayer];
    }

    NSURL *imageURL = [NSURL URLWithString:@"https://www.mapbox.com/mapbox-gl-js/assets/radar.gif"];
    MGLCoordinateQuad quad = { {46.437, -80.425},
      {37.936, -80.425},
      {37.936, -71.516},
      {46.437, -71.516} };
    MGLImageSource *imageSource = [[MGLImageSource alloc] initWithIdentifier:@"radar-source" coordinateQuad:quad URL:imageURL];
    [self.mapView.style addSource:imageSource];

    MGLRasterStyleLayer * imageLayer = [[MGLRasterStyleLayer alloc] initWithIdentifier:@"radar-layer" source:imageSource];
    [self.mapView.style addLayer:imageLayer];
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
    NSArray *features = [self.mapView visibleFeaturesAtPoint:point];
    NSString *title;
    for (id <MGLFeature> feature in features) {
        if (!title) {
            title = [feature attributeForKey:@"name_en"] ?: [feature attributeForKey:@"name"];
        }
    }

    DroppedPinAnnotation *annotation = [[DroppedPinAnnotation alloc] init];
    annotation.coordinate = [self.mapView convertPoint:point toCoordinateFromView:self.mapView];
    annotation.title = title ?: @"Dropped Pin";
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

- (IBAction)selectFeatures:(id)sender {
    [self selectFeaturesAtPoint:_mouseLocationForMapViewContextMenu];
}

- (void)selectFeaturesAtPoint:(NSPoint)point {
    NSArray *features = [self.mapView visibleFeaturesAtPoint:point];
    NSArray *flattenedFeatures = MBXFlattenedShapes(features);
    [self.mapView addAnnotations:flattenedFeatures];
}

#pragma mark User interface validation

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem {
    if (menuItem.action == @selector(showStyle:)) {
        NSURL *styleURL = self.mapView.styleURL;
        NSCellStateValue state;
        switch (menuItem.tag) {
            case 1:
                state = [styleURL isEqual:[MGLStyle streetsStyleURL]];
                break;
            case 2:
                state = [styleURL isEqual:[MGLStyle outdoorsStyleURL]];
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
                state = [styleURL isEqual:[MGLStyle satelliteStreetsStyleURL]];
                break;
            case 7:
                state = [styleURL isEqual:[MGLStyle trafficDayStyleURL]];
                break;
            case 8:
                state = [styleURL isEqual:[MGLStyle trafficNightStyleURL]];
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
    if (menuItem.action == @selector(toggleLayers:)) {
        BOOL isShown = ![self.splitView isSubviewCollapsed:self.splitView.arrangedSubviews.firstObject];
        menuItem.title = isShown ? @"Hide Layers" : @"Show Layers";
        return YES;
    }
    if (menuItem.action == @selector(toggleStyleLayers:)) {
        NSInteger row = self.styleLayersTableView.clickedRow;
        if (row == -1) {
            row = self.styleLayersTableView.selectedRow;
        }
        if (row == -1) {
            menuItem.title = @"Show";
        } else {
            BOOL isVisible = self.mapView.style.reversedLayers[row].visible;
            menuItem.title = isVisible ? @"Hide" : @"Show";
        }
        return row != -1;
    }
    if (menuItem.action == @selector(deleteStyleLayers:)) {
        return self.styleLayersTableView.clickedRow >= 0 || self.styleLayersTableView.selectedRow >= 0;
    }
    if (menuItem.action == @selector(setLabelLanguage:)) {
        menuItem.state = menuItem.tag == _isLocalizingLabels ? NSOnState: NSOffState;
        if (menuItem.tag) {
            NSLocale *locale = [NSLocale localeWithLocaleIdentifier:[NSBundle mainBundle].developmentLocalization];
            NSString *preferredLanguage = [MGLVectorSource preferredMapboxStreetsLanguage];
            menuItem.title = [locale displayNameForKey:NSLocaleIdentifier value:preferredLanguage];
        }
        return YES;
    }
    if (menuItem.action == @selector(manipulateStyle:)) {
        return YES;
    }
    if (menuItem.action == @selector(dropPin:)) {
        id <MGLAnnotation> annotationUnderCursor = [self.mapView annotationAtPoint:_mouseLocationForMapViewContextMenu];
        menuItem.hidden = annotationUnderCursor != nil;
        return YES;
    }
    if (menuItem.action == @selector(removePin:)) {
        id <MGLAnnotation> annotationUnderCursor = [self.mapView annotationAtPoint:_mouseLocationForMapViewContextMenu];
        menuItem.hidden = annotationUnderCursor == nil;
        return YES;
    }
    if (menuItem.action == @selector(selectFeatures:)) {
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
    if (menuItem.action == @selector(toggleOverdrawVisualization:)) {
        BOOL isShown = self.mapView.debugMask & MGLMapDebugOverdrawVisualizationMask;
        menuItem.title = isShown ? @"Hide Overdraw Visualization" : @"Show Overdraw Visualization";
        return YES;
    }
    if (menuItem.action == @selector(showColorBuffer:)) {
        BOOL enabled = self.mapView.debugMask & (MGLMapDebugStencilBufferMask | MGLMapDebugDepthBufferMask);
        menuItem.state = enabled ? NSOffState : NSOnState;
        return YES;
    }
    if (menuItem.action == @selector(showStencilBuffer:)) {
        BOOL enabled = self.mapView.debugMask & MGLMapDebugStencilBufferMask;
        menuItem.state = enabled ? NSOnState : NSOffState;
        return YES;
    }
    if (menuItem.action == @selector(showDepthBuffer:)) {
        BOOL enabled = self.mapView.debugMask & MGLMapDebugDepthBufferMask;
        menuItem.state = enabled ? NSOnState : NSOffState;
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
    if (menuItem.action == @selector(drawPolygonAndPolyLineAnnotations:)) {
        return !_isShowingPolygonAndPolylineAnnotations;
    }
    if (menuItem.action == @selector(drawAnimatedAnnotation:)) {
        return !_isShowingAnimatedAnnotation;
    }
    if (menuItem.action == @selector(addAnimatedImageSource:)) {
        return YES;
    }
    if (menuItem.action == @selector(insertCustomStyleLayer:)) {
        return ![self.mapView.style layerWithIdentifier:@"mbx-custom"];
    }
    if (menuItem.action == @selector(showAllAnnotations:) || menuItem.action == @selector(removeAllAnnotations:)) {
        return self.mapView.annotations.count > 0;
    }
    if (menuItem.action == @selector(startWorldTour:)) {
        return !_isTouringWorld;
    }
    if (menuItem.action == @selector(stopWorldTour:)) {
        return _isTouringWorld;
    }
    if (menuItem.action == @selector(addOfflinePack:)) {
        NSURL *styleURL = self.mapView.styleURL;
        return !styleURL.isFileURL;
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
        [MGLStyle outdoorsStyleURL],
        [MGLStyle lightStyleURL],
        [MGLStyle darkStyleURL],
        [MGLStyle satelliteStyleURL],
        [MGLStyle satelliteStreetsStyleURL],
        [MGLStyle trafficDayStyleURL],
        [MGLStyle trafficNightStyleURL],
    ];
    return [styleURLs indexOfObject:self.mapView.styleURL];
}

- (BOOL)validateToolbarItem:(NSToolbarItem *)toolbarItem {
    if (!self.mapView) {
        return NO;
    }

    SEL action = toolbarItem.action;
    if (action == @selector(showShareMenu:)) {
        [(NSButton *)toolbarItem.view sendActionOn:NSLeftMouseDownMask];
        if (![MGLAccountManager accessToken]) {
            return NO;
        }
        NSURL *styleURL = self.mapView.styleURL;
        return ([styleURL.scheme isEqualToString:@"mapbox"]
                && [styleURL.pathComponents.firstObject isEqualToString:@"styles"]);
    }
    if (action == @selector(showStyle:)) {
        NSPopUpButton *popUpButton = (NSPopUpButton *)toolbarItem.view;
        NSUInteger index = self.indexOfStyleInToolbarItem;
        if (index == NSNotFound) {
            index = -1;
        }
        [popUpButton selectItemAtIndex:index];
        if (index == -1) {
            NSString *name = self.mapView.style.name;
            popUpButton.title = name ?: @"Custom";
        }
    }
    if (action == @selector(toggleLayers:)) {
        BOOL isShown = ![self.splitView isSubviewCollapsed:self.splitView.arrangedSubviews.firstObject];
        [(NSButton *)toolbarItem.view setState:isShown ? NSOnState : NSOffState];
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

#pragma mark NSSplitViewDelegate methods

- (BOOL)splitView:(NSSplitView *)splitView canCollapseSubview:(NSView *)subview {
    return subview != self.mapView;
}

- (BOOL)splitView:(NSSplitView *)splitView shouldCollapseSubview:(NSView *)subview forDoubleClickOnDividerAtIndex:(NSInteger)dividerIndex {
    return YES;
}

#pragma mark MGLMapViewDelegate methods

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    [self updateLabels];
}

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
        annotationImage.cursor = cursors[arc4random_uniform((uint32_t)cursors.count) % cursors.count];
    } else {
        annotationImage.cursor = nil;
    }
    return annotationImage;
}

- (void)mapView:(MGLMapView *)mapView didSelectAnnotation:(id <MGLAnnotation>)annotation {
    if ([annotation isKindOfClass:[DroppedPinAnnotation class]]) {
        DroppedPinAnnotation *droppedPin = (DroppedPinAnnotation *)annotation;
        [droppedPin resume];
    }
}

- (void)mapView:(MGLMapView *)mapView didDeselectAnnotation:(id <MGLAnnotation>)annotation {
    if ([annotation isKindOfClass:[DroppedPinAnnotation class]]) {
        DroppedPinAnnotation *droppedPin = (DroppedPinAnnotation *)annotation;
        [droppedPin pause];
    }
}

- (CGFloat)mapView:(MGLMapView *)mapView alphaForShapeAnnotation:(MGLShape *)annotation {
    return 0.8;
}

@end

@interface ValidatedToolbarItem : NSToolbarItem

@end

@implementation ValidatedToolbarItem

- (void)validate {
    [(MapDocument *)self.toolbar.delegate validateToolbarItem:self];
}

@end
