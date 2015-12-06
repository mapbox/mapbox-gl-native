#import "MGLMapView_Private.h"
#import "MGLAccountManager_Private.h"
#import "MGLAttributionButton.h"
#import "MGLCompassCell.h"
#import "MGLOpenGLLayer.h"
#import "MGLStyle.h"

#import "../../darwin/MGLGeometry_Private.h"
#import "../../darwin/MGLMultiPoint_Private.h"

#import <mbgl/darwin/MGLMapCamera.h>
#import <mbgl/darwin/MGLPolygon.h>
#import <mbgl/darwin/MGLPolyline.h>
#import <mbgl/osx/MGLAnnotationImage.h>
#import <mbgl/osx/MGLMapViewDelegate.h>

#import <mbgl/mbgl.hpp>
#import <mbgl/annotation/point_annotation.hpp>
#import <mbgl/map/camera.hpp>
#import <mbgl/platform/darwin/reachability.h>
#import <mbgl/platform/gl.hpp>
#import <mbgl/sprite/sprite_image.hpp>
#import <mbgl/storage/default_file_source.hpp>
#import <mbgl/storage/network_status.hpp>
#import <mbgl/storage/sqlite_cache.hpp>
#import <mbgl/util/constants.hpp>
#import <mbgl/util/math.hpp>
#import <mbgl/util/std.hpp>

#import <map>
#import <unordered_set>

#import "NSBundle+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "../../darwin/NSException+MGLAdditions.h"
#import "../../darwin/NSString+MGLAdditions.h"

#import <QuartzCore/QuartzCore.h>

class MGLMapViewImpl;
class MGLAnnotationContext;

/// Distance from the edge of the view to ornament views (logo, attribution, etc.).
const CGFloat MGLOrnamentPadding = 12;

/// Alpha value of the ornament views (logo, attribution, etc.).
const CGFloat MGLOrnamentOpacity = 0.9;

/// Default duration for programmatic animations.
const NSTimeInterval MGLAnimationDuration = 0.3;

/// Angular field of view for determining altitude of viewpoint.
const CLLocationDegrees MGLAngularFieldOfView = M_PI / 6.;

/// Minimum allowed pitch in degrees.
const CGFloat MGLMinimumPitch = 0;

/// Maximum allowed pitch in degrees.
const CGFloat MGLMaximumPitch = 60;

/// Distance in points that a single press of the panning keyboard shortcut pans the map by.
const CGFloat MGLKeyPanningIncrement = 150;

/// Degrees that a single press of the rotation keyboard shortcut rotates the map by.
const CLLocationDegrees MGLKeyRotationIncrement = 25;

/// Reuse identifier and file name of the default point annotation image.
static NSString * const MGLDefaultStyleMarkerSymbolName = @"default_marker";

/// Prefix that denotes a sprite installed by MGLMapView, to avoid collisions
/// with style-defined sprites.
static NSString * const MGLAnnotationSpritePrefix = @"com.mapbox.sprites.";

/// Slop area around the hit testing point, allowing for imprecise annotation selection.
const CGFloat MGLAnnotationImagePaddingForHitTest = 4;

/// Distance from the callout’s anchor point to the annotation it points to.
const CGFloat MGLAnnotationImagePaddingForCallout = 4;

/// Copyright notices displayed in the attribution view.
struct MGLAttribution {
    /// Attribution button label text. A copyright symbol is prepended to this string.
    NSString *title;
    /// URL to open when the attribution button is clicked.
    NSString *urlString;
} MGLAttributions[] = {
    { @"Mapbox", @"https://www.mapbox.com/about/maps/" },
    { @"OpenStreetMap", @"http://www.openstreetmap.org/about/" },
};

/// Unique identifier representing a single annotation in mbgl.
typedef uint32_t MGLAnnotationTag;

/// An indication that the requested annotation was not found or is nonexistent.
enum { MGLAnnotationTagNotFound = UINT32_MAX };

/// Mapping from an annotation tag to metadata about that annotation, including
/// the annotation itself.
typedef std::map<MGLAnnotationTag, MGLAnnotationContext> MGLAnnotationContextMap;

/// Returns an NSImage for the default marker image.
NSImage *MGLDefaultMarkerImage() {
    NSString *path = [[NSBundle mgl_frameworkBundle] pathForResource:MGLDefaultStyleMarkerSymbolName
                                                             ofType:@"pdf"];
    return [[NSImage alloc] initWithContentsOfFile:path];
}

/// Converts from a duration in seconds to a duration object usable in mbgl.
std::chrono::steady_clock::duration MGLDurationInSeconds(NSTimeInterval duration) {
    return std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<NSTimeInterval, std::chrono::seconds::period>(duration));
}

/// Converts a media timing function into a unit bezier object usable in mbgl.
mbgl::util::UnitBezier MGLUnitBezierForMediaTimingFunction(CAMediaTimingFunction *function) {
    if (!function) {
        function = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionDefault];
    }
    float p1[2], p2[2];
    [function getControlPointAtIndex:0 values:p1];
    [function getControlPointAtIndex:1 values:p2];
    return { p1[0], p1[1], p2[0], p2[1] };
}

/// Converts the given color into an mbgl::Color in calibrated RGB space.
mbgl::Color MGLColorObjectFromNSColor(NSColor *color) {
    if (!color) {
        return {{ 0, 0, 0, 0 }};
    }
    CGFloat r, g, b, a;
    [[color colorUsingColorSpaceName:NSCalibratedRGBColorSpace] getRed:&r green:&g blue:&b alpha:&a];
    return {{ (float)r, (float)g, (float)b, (float)a }};
}

/// Lightweight container for metadata about an annotation, including the annotation itself.
class MGLAnnotationContext {
public:
    id <MGLAnnotation> annotation;
    /// mbgl-given identifier for the annotation image used by this annotation.
    /// Based on the annotation image’s reusable identifier.
    NSString *symbolIdentifier;
};

@interface MGLMapView () <NSPopoverDelegate, MGLMultiPointDelegate>

@property (nonatomic, readwrite) NSSegmentedControl *zoomControls;
@property (nonatomic, readwrite) NSSlider *compass;
@property (nonatomic, readwrite) NSImageView *logoView;
@property (nonatomic, readwrite) NSView *attributionView;

/// Mapping from reusable identifiers to annotation images.
@property (nonatomic) NS_MUTABLE_DICTIONARY_OF(NSString *, MGLAnnotationImage *) *annotationImagesByIdentifier;
/// Currently shown popover representing the selected annotation.
@property (nonatomic) NSPopover *calloutForSelectedAnnotation;

@property (nonatomic, readwrite, getter=isDormant) BOOL dormant;

@end

@implementation MGLMapView {
    /// Cross-platform map view controller.
    mbgl::Map *_mbglMap;
    MGLMapViewImpl *_mbglView;
    std::shared_ptr<mbgl::SQLiteCache> _mbglFileCache;
    mbgl::DefaultFileSource *_mbglFileSource;
    
    NSPanGestureRecognizer *_panGestureRecognizer;
    NSMagnificationGestureRecognizer *_magnificationGestureRecognizer;
    NSRotationGestureRecognizer *_rotationGestureRecognizer;
    double _scaleAtBeginningOfGesture;
    CLLocationDirection _directionAtBeginningOfGesture;
    CGFloat _pitchAtBeginningOfGesture;
    
    MGLAnnotationContextMap _annotationContextsByAnnotationTag;
    MGLAnnotationTag _selectedAnnotationTag;
    MGLAnnotationTag _lastSelectedAnnotationTag;
    /// Size of the rectangle formed by unioning the maximum slop area around every annotation image.
    NSSize _unionedAnnotationImageSize;
    std::vector<MGLAnnotationTag> _annotationsNearbyLastClick;
    /// True if any annotations that have tooltips have been installed.
    BOOL _wantsToolTipRects;
    /// True if any annotation images that have custom cursors have been installed.
    BOOL _wantsCursorRects;
    
    // Cached checks for delegate method implementations that may be called from
    // MGLMultiPointDelegate methods.
    
    BOOL _delegateHasAlphasForShapeAnnotations;
    BOOL _delegateHasStrokeColorsForShapeAnnotations;
    BOOL _delegateHasFillColorsForShapeAnnotations;
    BOOL _delegateHasLineWidthsForShapeAnnotations;
    
    /// True if the current process is the Interface Builder designable
    /// renderer. When drawing the designable, the map is paused, so any call to
    /// it may hang the process.
    BOOL _isTargetingInterfaceBuilder;
    CLLocationDegrees _pendingLatitude;
    CLLocationDegrees _pendingLongitude;
}

#pragma mark Lifecycle

- (instancetype)initWithFrame:(NSRect)frameRect {
    if (self = [super initWithFrame:frameRect]) {
        [self commonInit];
        self.styleURL = nil;
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame styleURL:(nullable NSURL *)styleURL {
    if (self = [super initWithFrame:frame]) {
        [self commonInit];
        self.styleURL = styleURL;
    }
    return self;
}

- (instancetype)initWithCoder:(nonnull NSCoder *)decoder {
    if (self = [super initWithCoder:decoder]) {
        [self commonInit];
    }
    return self;
}

- (void)awakeFromNib {
    [super awakeFromNib];
    
    self.styleURL = nil;
}

+ (NSArray *)restorableStateKeyPaths {
    return @[@"camera"];
}

- (void)commonInit {
    _isTargetingInterfaceBuilder = NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent;
    
    // Set up cross-platform controllers and resources.
    _mbglView = new MGLMapViewImpl(self, [NSScreen mainScreen].backingScaleFactor);
    
    // Place the cache in a location that can be shared among all the
    // applications that embed the Mapbox OS X SDK.
    NSURL *cacheDirectoryURL = [[NSFileManager defaultManager] URLForDirectory:NSCachesDirectory
                                                                      inDomain:NSUserDomainMask
                                                             appropriateForURL:nil
                                                                        create:YES
                                                                         error:nil];
    cacheDirectoryURL = [cacheDirectoryURL URLByAppendingPathComponent:
                         [[NSBundle mgl_frameworkBundle] bundleIdentifier]];
    [[NSFileManager defaultManager] createDirectoryAtURL:cacheDirectoryURL
                             withIntermediateDirectories:YES
                                              attributes:nil
                                                   error:nil];
    NSURL *cacheURL = [cacheDirectoryURL URLByAppendingPathComponent:@"cache.db"];
    NSString *cachePath = cacheURL ? cacheURL.path : @"";
    _mbglFileCache = mbgl::SharedSQLiteCache::get(cachePath.UTF8String);
    _mbglFileSource = new mbgl::DefaultFileSource(_mbglFileCache.get());
    
    _mbglMap = new mbgl::Map(*_mbglView, *_mbglFileSource, mbgl::MapMode::Continuous);
    
    // Install the OpenGL layer. Interface Builder’s synchronous drawing means
    // we can’t display a map, so don’t even bother to have a map layer.
    self.layer = _isTargetingInterfaceBuilder ? [CALayer layer] : [MGLOpenGLLayer layer];
    
    // Observe for changes to the global access token (and find out the current one).
    [[MGLAccountManager sharedManager] addObserver:self
                                        forKeyPath:@"accessToken"
                                           options:(NSKeyValueObservingOptionInitial |
                                                    NSKeyValueObservingOptionNew)
                                           context:NULL];
    
    // Notify map object when network reachability status changes.
    MGLReachability *reachability = [MGLReachability reachabilityForInternetConnection];
    reachability.reachableBlock = ^(MGLReachability *) {
        mbgl::NetworkStatus::Reachable();
    };
    [reachability startNotifier];
    
    // Install ornaments and gesture recognizers.
    [self installZoomControls];
    [self installCompass];
    [self installLogoView];
    [self installAttributionView];
    [self installGestureRecognizers];
    
    // Set up annotation management and selection state.
    _annotationImagesByIdentifier = [NSMutableDictionary dictionary];
    _annotationContextsByAnnotationTag = {};
    _selectedAnnotationTag = MGLAnnotationTagNotFound;
    _lastSelectedAnnotationTag = MGLAnnotationTagNotFound;
    _annotationsNearbyLastClick = {};
    
    // Jump to Null Island initially.
    mbgl::CameraOptions options;
    options.center = mbgl::LatLng(0, 0);
    options.zoom = _mbglMap->getMinZoom();
    _mbglMap->jumpTo(options);
    _pendingLatitude = NAN;
    _pendingLongitude = NAN;
}

/// Adds zoom controls to the lower-right corner.
- (void)installZoomControls {
    _zoomControls = [[NSSegmentedControl alloc] initWithFrame:NSZeroRect];
    _zoomControls.wantsLayer = YES;
    _zoomControls.layer.opacity = MGLOrnamentOpacity;
    [(NSSegmentedCell *)_zoomControls.cell setTrackingMode:NSSegmentSwitchTrackingMomentary];
    _zoomControls.continuous = YES;
    _zoomControls.segmentCount = 2;
    [_zoomControls setLabel:@"−" forSegment:0]; // U+2212 MINUS SIGN
    [(NSSegmentedCell *)_zoomControls.cell setTag:0 forSegment:0];
    [(NSSegmentedCell *)_zoomControls.cell setToolTip:@"Zoom Out" forSegment:0];
    [_zoomControls setLabel:@"+" forSegment:1];
    [(NSSegmentedCell *)_zoomControls.cell setTag:1 forSegment:1];
    [(NSSegmentedCell *)_zoomControls.cell setToolTip:@"Zoom In" forSegment:1];
    _zoomControls.target = self;
    _zoomControls.action = @selector(zoomInOrOut:);
    _zoomControls.controlSize = NSRegularControlSize;
    [_zoomControls sizeToFit];
    _zoomControls.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_zoomControls];
}

/// Adds a rudimentary compass control to the lower-right corner.
- (void)installCompass {
    _compass = [[NSSlider alloc] initWithFrame:NSZeroRect];
    _compass.wantsLayer = YES;
    _compass.layer.opacity = MGLOrnamentOpacity;
    _compass.cell = [[MGLCompassCell alloc] init];
    _compass.continuous = YES;
    _compass.target = self;
    _compass.action = @selector(rotate:);
    [_compass sizeToFit];
    _compass.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_compass];
}

/// Adds a Mapbox logo to the lower-left corner.
- (void)installLogoView {
    _logoView = [[NSImageView alloc] initWithFrame:NSZeroRect];
    _logoView.wantsLayer = YES;
    NSImage *logoImage = [[NSImage alloc] initWithContentsOfFile:
                          [[NSBundle mgl_frameworkBundle] pathForResource:@"mapbox" ofType:@"pdf"]];
    // Account for the image’s built-in padding when aligning other controls to the logo.
    logoImage.alignmentRect = NSInsetRect(logoImage.alignmentRect, 3, 3);
    _logoView.image = logoImage;
    _logoView.translatesAutoresizingMaskIntoConstraints = NO;
    _logoView.accessibilityTitle = @"Mapbox";
    [self addSubview:_logoView];
}

/// Adds legally required map attribution to the lower-left corner.
- (void)installAttributionView {
    _attributionView = [[NSView alloc] initWithFrame:NSZeroRect];
    _attributionView.wantsLayer = YES;
    
    // Make the background and foreground translucent to be unobtrusive.
    _attributionView.layer.opacity = 0.6;
    
    // Blur the background to prevent text underneath the view from running into
    // the text in the view, rendering it illegible.
    CIFilter *attributionBlurFilter = [CIFilter filterWithName:@"CIGaussianBlur"];
    [attributionBlurFilter setDefaults];
    
    // Brighten the background. This is similar to applying a translucent white
    // background on the view, but the effect is a bit more subtle and works
    // well with the blur above.
    CIFilter *attributionColorFilter = [CIFilter filterWithName:@"CIColorControls"];
    [attributionColorFilter setDefaults];
    [attributionColorFilter setValue:@(0.1) forKey:kCIInputBrightnessKey];
    
    // Apply the background effects and a standard button corner radius.
    _attributionView.backgroundFilters = @[attributionColorFilter, attributionBlurFilter];
    _attributionView.layer.cornerRadius = 4;
    
    _attributionView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_attributionView];
    [self updateAttributionView];
}

/// Adds gesture recognizers for manipulating the viewport and selecting annotations.
- (void)installGestureRecognizers {
    self.acceptsTouchEvents = YES;
    _scrollEnabled = YES;
    _zoomEnabled = YES;
    _rotateEnabled = YES;
    _pitchEnabled = YES;
    
    _panGestureRecognizer = [[NSPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
    _panGestureRecognizer.delaysKeyEvents = YES;
    [self addGestureRecognizer:_panGestureRecognizer];
    
    NSClickGestureRecognizer *clickGestureRecognizer = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(handleClickGesture:)];
    clickGestureRecognizer.delaysPrimaryMouseButtonEvents = NO;
    [self addGestureRecognizer:clickGestureRecognizer];
    
    NSClickGestureRecognizer *secondaryClickGestureRecognizer = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(handleSecondaryClickGesture:)];
    secondaryClickGestureRecognizer.buttonMask = 0x2;
    [self addGestureRecognizer:secondaryClickGestureRecognizer];
    
    NSClickGestureRecognizer *doubleClickGestureRecognizer = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleClickGesture:)];
    doubleClickGestureRecognizer.numberOfClicksRequired = 2;
    doubleClickGestureRecognizer.delaysPrimaryMouseButtonEvents = NO;
    [self addGestureRecognizer:doubleClickGestureRecognizer];
    
    _magnificationGestureRecognizer = [[NSMagnificationGestureRecognizer alloc] initWithTarget:self action:@selector(handleMagnificationGesture:)];
    [self addGestureRecognizer:_magnificationGestureRecognizer];
    
    _rotationGestureRecognizer = [[NSRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotationGesture:)];
    [self addGestureRecognizer:_rotationGestureRecognizer];
}

/// Updates the attribution view to reflect the sources used. For now, this is
/// hard-coded to the standard Mapbox and OpenStreetMap attribution.
- (void)updateAttributionView {
    self.attributionView.subviews = @[];
    
    for (NSUInteger i = 0; i < sizeof(MGLAttributions) / sizeof(MGLAttributions[0]); i++) {
        // For each attribution, add a borderless button that responds to clicks
        // and feels like a hyperlink.
        NSURL *url = [NSURL URLWithString:MGLAttributions[i].urlString];
        NSButton *button = [[MGLAttributionButton alloc] initWithTitle:MGLAttributions[i].title URL:url];
        button.controlSize = NSMiniControlSize;
        button.translatesAutoresizingMaskIntoConstraints = NO;
        
        // Set the new button flush with the buttom of the container and to the
        // right of the previous button, with standard spacing. If there is no
        // previous button, align to the container instead.
        NSView *previousView = self.attributionView.subviews.lastObject;
        [self.attributionView addSubview:button];
        [_attributionView addConstraint:
         [NSLayoutConstraint constraintWithItem:button
                                      attribute:NSLayoutAttributeBottom
                                      relatedBy:NSLayoutRelationEqual
                                         toItem:_attributionView
                                      attribute:NSLayoutAttributeBottom
                                     multiplier:1
                                       constant:0]];
        [_attributionView addConstraint:
         [NSLayoutConstraint constraintWithItem:button
                                      attribute:NSLayoutAttributeLeading
                                      relatedBy:NSLayoutRelationEqual
                                         toItem:previousView ? previousView : _attributionView
                                      attribute:previousView ? NSLayoutAttributeTrailing : NSLayoutAttributeLeading
                                     multiplier:1
                                       constant:8]];
    }
}

- (void)dealloc {
    [[MGLAccountManager sharedManager] removeObserver:self forKeyPath:@"accessToken"];
    
    // Close any annotation callout immediately.
    [self.calloutForSelectedAnnotation close];
    self.calloutForSelectedAnnotation = nil;
    
    if (_mbglMap) {
        delete _mbglMap;
        _mbglMap = nullptr;
    }
    if (_mbglFileSource) {
        delete _mbglFileSource;
        _mbglFileSource = nullptr;
    }
    if (_mbglView) {
        delete _mbglView;
        _mbglView = nullptr;
    }
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(__unused void *)context {
    // Synchronize mbgl::Map’s access token with the global one in MGLAccountManager.
    if ([keyPath isEqualToString:@"accessToken"] && object == [MGLAccountManager sharedManager]) {
        NSString *accessToken = change[NSKeyValueChangeNewKey];
        if (![accessToken isKindOfClass:[NSNull class]]) {
            _mbglFileSource->setAccessToken((std::string)accessToken.UTF8String);
        }
    }
}

+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)key {
    return [key isEqualToString:@"annotations"] ? YES : [super automaticallyNotifiesObserversForKey:key];
}

- (void)setDelegate:(id<MGLMapViewDelegate>)delegate {
    _delegate = delegate;
    
    // Cache checks for delegate method implementations that may be called in a
    // hot loop, namely the annotation style methods.
    _delegateHasAlphasForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:alphaForShapeAnnotation:)];
    _delegateHasStrokeColorsForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:strokeColorForShapeAnnotation:)];
    _delegateHasFillColorsForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:fillColorForPolygonAnnotation:)];
    _delegateHasLineWidthsForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:lineWidthForPolylineAnnotation:)];
    
    if ([self.delegate respondsToSelector:@selector(mapView:regionWillChangeAnimated:)]) {
        NSLog(@"-mapView:regionWillChangeAnimated: is not supported by the OS X SDK, but %@ implements it anyways. "
              @"Please implement -[%@ mapView:cameraWillChangeAnimated:] instead.",
              NSStringFromClass([delegate class]), NSStringFromClass([delegate class]));
    }
    if ([self.delegate respondsToSelector:@selector(mapViewRegionIsChanging:)]) {
        NSLog(@"-mapViewRegionIsChanging: is not supported by the OS X SDK, but %@ implements it anyways. "
              @"Please implement -[%@ mapViewCameraIsChanging:] instead.",
              NSStringFromClass([delegate class]), NSStringFromClass([delegate class]));
    }
    if ([self.delegate respondsToSelector:@selector(mapView:regionDidChangeAnimated:)]) {
        NSLog(@"-mapView:regionDidChangeAnimated: is not supported by the OS X SDK, but %@ implements it anyways. "
              @"Please implement -[%@ mapView:cameraDidChangeAnimated:] instead.",
              NSStringFromClass([delegate class]), NSStringFromClass([delegate class]));
    }
}

#pragma mark Style

- (nonnull NSURL *)styleURL {
    NSString *styleURLString = @(_mbglMap->getStyleURL().c_str()).mgl_stringOrNilIfEmpty;
    return styleURLString ? [NSURL URLWithString:styleURLString] : [MGLStyle streetsStyleURL];
}

- (void)setStyleURL:(nullable NSURL *)styleURL {
    if (_isTargetingInterfaceBuilder) {
        return;
    }
    
    // Default to Streets.
    if (!styleURL) {
        // An access token is required to load any default style, including
        // Streets.
        if (![MGLAccountManager accessToken]) {
            return;
        }
        styleURL = [MGLStyle streetsStyleURL];
    }
    
    if (![styleURL scheme]) {
        // Assume a relative path into the application’s resource folder,
        // defined in mbgl::platform::assetRoot().
        styleURL = [NSURL URLWithString:[@"asset://" stringByAppendingString:styleURL.absoluteString]];
    }
    
    _mbglMap->setStyleURL(styleURL.absoluteString.UTF8String);
}

- (IBAction)reloadStyle:(__unused id)sender {
    NSURL *styleURL = self.styleURL;
    _mbglMap->setStyleURL("");
    self.styleURL = styleURL;
}

#pragma mark View hierarchy and drawing

- (void)viewWillMoveToWindow:(NSWindow *)newWindow {
    [self deselectAnnotation:self.selectedAnnotation];
    if (!self.dormant && !newWindow) {
        self.dormant = YES;
        _mbglMap->pause();
    }
}

- (void)viewDidMoveToWindow {
    if (self.dormant && self.window) {
        _mbglMap->resume();
        self.dormant = NO;
    }
}

- (BOOL)wantsLayer {
    return YES;
}

- (BOOL)wantsBestResolutionOpenGLSurface {
    // Use an OpenGL layer, except when drawing the designable, which is just
    // ordinary Cocoa.
    return !_isTargetingInterfaceBuilder;
}

- (void)setFrame:(NSRect)frame {
    super.frame = frame;
    if (!_isTargetingInterfaceBuilder) {
        _mbglMap->update(mbgl::Update::Dimensions);
    }
}

- (void)updateConstraints {
    // Place the zoom controls at the lower-right corner of the view.
    [self addConstraint:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeBottom
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:_zoomControls
                                  attribute:NSLayoutAttributeBottom
                                 multiplier:1
                                   constant:MGLOrnamentPadding]];
    [self addConstraint:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeTrailing
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:_zoomControls
                                  attribute:NSLayoutAttributeTrailing
                                 multiplier:1
                                   constant:MGLOrnamentPadding]];
    
    // Center the compass above the zoom controls, assuming that the compass is
    // narrower than the zoom controls.
    [self addConstraint:
     [NSLayoutConstraint constraintWithItem:_compass
                                  attribute:NSLayoutAttributeCenterX
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:_zoomControls
                                  attribute:NSLayoutAttributeCenterX
                                 multiplier:1
                                   constant:0]];
    [self addConstraint:
     [NSLayoutConstraint constraintWithItem:_zoomControls
                                  attribute:NSLayoutAttributeTop
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:_compass
                                  attribute:NSLayoutAttributeBottom
                                 multiplier:1
                                   constant:8]];
    
    // Place the logo view in the lower-left corner of the view, accounting for
    // the logo’s alignment rect.
    [self addConstraint:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeBottom
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:_logoView
                                  attribute:NSLayoutAttributeBottom
                                 multiplier:1
                                   constant:MGLOrnamentPadding - _logoView.image.alignmentRect.origin.y]];
    [self addConstraint:
     [NSLayoutConstraint constraintWithItem:_logoView
                                  attribute:NSLayoutAttributeLeading
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeLeading
                                 multiplier:1
                                   constant:MGLOrnamentPadding - _logoView.image.alignmentRect.origin.x]];
    
    // Place the attribution view to the right of the logo view and size it to
    // fit the buttons inside.
    [self addConstraint:[NSLayoutConstraint constraintWithItem:_logoView
                                                     attribute:NSLayoutAttributeBaseline
                                                     relatedBy:NSLayoutRelationEqual
                                                        toItem:_attributionView
                                                     attribute:NSLayoutAttributeBaseline
                                                    multiplier:1
                                                      constant:_logoView.image.alignmentRect.origin.y]];
    [self addConstraint:[NSLayoutConstraint constraintWithItem:_attributionView
                                                     attribute:NSLayoutAttributeLeading
                                                     relatedBy:NSLayoutRelationEqual
                                                        toItem:_logoView
                                                     attribute:NSLayoutAttributeTrailing
                                                    multiplier:1
                                                      constant:8]];
    [self addConstraint:[NSLayoutConstraint constraintWithItem:_attributionView.subviews.firstObject
                                                     attribute:NSLayoutAttributeTop
                                                     relatedBy:NSLayoutRelationEqual
                                                        toItem:_attributionView
                                                     attribute:NSLayoutAttributeTop
                                                    multiplier:1
                                                      constant:0]];
    [self addConstraint:[NSLayoutConstraint constraintWithItem:_attributionView
                                                     attribute:NSLayoutAttributeTrailing
                                                     relatedBy:NSLayoutRelationEqual
                                                        toItem:_attributionView.subviews.lastObject
                                                     attribute:NSLayoutAttributeTrailing
                                                    multiplier:1
                                                      constant:8]];
    
    [super updateConstraints];
}

- (void)renderSync {
    if (!self.dormant) {
        CGFloat zoomFactor   = _mbglMap->getMaxZoom() - _mbglMap->getMinZoom() + 1;
        CGFloat cpuFactor    = (CGFloat)[NSProcessInfo processInfo].processorCount;
        CGFloat memoryFactor = (CGFloat)[NSProcessInfo processInfo].physicalMemory / 1000 / 1000 / 1000;
        CGFloat sizeFactor   = ((CGFloat)_mbglMap->getWidth() / mbgl::util::tileSize) * ((CGFloat)_mbglMap->getHeight() / mbgl::util::tileSize);
        
        NSUInteger cacheSize = zoomFactor * cpuFactor * memoryFactor * sizeFactor * 0.5;
        
        _mbglMap->setSourceTileCacheSize(cacheSize);
        _mbglMap->renderSync();
        
//        [self updateUserLocationAnnotationView];
    }
}

- (void)invalidate {
    MGLAssertIsMainThread();
    
    [self.layer setNeedsDisplay];
}

- (void)notifyMapChange:(mbgl::MapChange)change {
    // Ignore map updates when the Map object isn't set.
    if (!_mbglMap) {
        return;
    }
    
    switch (change) {
        case mbgl::MapChangeRegionWillChange:
        case mbgl::MapChangeRegionWillChangeAnimated:
        {
            if ([self.delegate respondsToSelector:@selector(mapView:cameraWillChangeAnimated:)]) {
                BOOL animated = change == mbgl::MapChangeRegionWillChangeAnimated;
                [self.delegate mapView:self cameraWillChangeAnimated:animated];
            }
            break;
        }
        case mbgl::MapChangeRegionIsChanging:
        {
            // Update a minimum of UI that needs to stay attached to the map
            // while animating.
            [self updateCompass];
            [self updateAnnotationCallouts];
            
            if ([self.delegate respondsToSelector:@selector(mapViewCameraIsChanging:)]) {
                [self.delegate mapViewCameraIsChanging:self];
            }
            break;
        }
        case mbgl::MapChangeRegionDidChange:
        case mbgl::MapChangeRegionDidChangeAnimated:
        {
            // Update all UI at the end of an animation or atomic change to the
            // viewport. More expensive updates can happen here, but care should
            // still be taken to minimize the work done here because scroll
            // gesture recognition and momentum scrolling is performed as a
            // series of atomic changes, not an animation.
            [self updateZoomControls];
            [self updateCompass];
            [self updateAnnotationCallouts];
            [self updateAnnotationTrackingAreas];
            
            if ([self.delegate respondsToSelector:@selector(mapView:cameraDidChangeAnimated:)]) {
                BOOL animated = change == mbgl::MapChangeRegionDidChangeAnimated;
                [self.delegate mapView:self cameraDidChangeAnimated:animated];
            }
            break;
        }
        case mbgl::MapChangeWillStartLoadingMap:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewWillStartLoadingMap:)]) {
                [self.delegate mapViewWillStartLoadingMap:self];
            }
            break;
        }
        case mbgl::MapChangeDidFinishLoadingMap:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewDidFinishLoadingMap:)]) {
                [self.delegate mapViewDidFinishLoadingMap:self];
            }
            break;
        }
        case mbgl::MapChangeDidFailLoadingMap:
        {
            // Not yet implemented.
            break;
        }
        case mbgl::MapChangeWillStartRenderingMap:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewWillStartRenderingMap:)]) {
                [self.delegate mapViewWillStartRenderingMap:self];
            }
            break;
        }
        case mbgl::MapChangeDidFinishRenderingMap:
        case mbgl::MapChangeDidFinishRenderingMapFullyRendered:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewDidFinishRenderingMap:fullyRendered:)]) {
                BOOL fullyRendered = change == mbgl::MapChangeDidFinishRenderingMapFullyRendered;
                [self.delegate mapViewDidFinishRenderingMap:self fullyRendered:fullyRendered];
            }
            break;
        }
        case mbgl::MapChangeWillStartRenderingFrame:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewWillStartRenderingFrame:)]) {
                [self.delegate mapViewWillStartRenderingFrame:self];
            }
            break;
        }
        case mbgl::MapChangeDidFinishRenderingFrame:
        case mbgl::MapChangeDidFinishRenderingFrameFullyRendered:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewDidFinishRenderingFrame:fullyRendered:)]) {
                BOOL fullyRendered = change == mbgl::MapChangeDidFinishRenderingFrameFullyRendered;
                [self.delegate mapViewDidFinishRenderingFrame:self fullyRendered:fullyRendered];
            }
            break;
        }
    }
}

#pragma mark Viewport

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCenterCoordinate {
    return [NSSet setWithObjects:@"latitude", @"longitude", @"camera", nil];
}

- (CLLocationCoordinate2D)centerCoordinate {
    return MGLLocationCoordinate2DFromLatLng(_mbglMap->getLatLng());
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate {
    [self setCenterCoordinate:centerCoordinate animated:NO];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate animated:(BOOL)animated {
    [self willChangeValueForKey:@"centerCoordinate"];
    _mbglMap->setLatLng(MGLLatLngFromLocationCoordinate2D(centerCoordinate),
                        MGLDurationInSeconds(animated ? MGLAnimationDuration : 0));
    [self didChangeValueForKey:@"centerCoordinate"];
}

- (void)offsetCenterCoordinateBy:(NSPoint)delta animated:(BOOL)animated {
    [self willChangeValueForKey:@"centerCoordinate"];
    _mbglMap->cancelTransitions();
    _mbglMap->moveBy({ delta.x, delta.y },
                     MGLDurationInSeconds(animated ? MGLAnimationDuration : 0));
    [self didChangeValueForKey:@"centerCoordinate"];
}

- (CLLocationDegrees)pendingLatitude {
    return _pendingLatitude;
}

- (void)setPendingLatitude:(CLLocationDegrees)pendingLatitude {
    _pendingLatitude = pendingLatitude;
}

- (CLLocationDegrees)pendingLongitude {
    return _pendingLongitude;
}

- (void)setPendingLongitude:(CLLocationDegrees)pendingLongitude {
    _pendingLongitude = pendingLongitude;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingZoomLevel {
    return [NSSet setWithObject:@"camera"];
}

- (double)zoomLevel {
    return _mbglMap->getZoom();
}

- (void)setZoomLevel:(double)zoomLevel {
    [self setZoomLevel:zoomLevel animated:NO];
}

- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated {
    _mbglMap->setZoom(zoomLevel, MGLDurationInSeconds(animated ? MGLAnimationDuration : 0));
}

- (void)scaleBy:(double)scaleFactor atPoint:(NSPoint)point animated:(BOOL)animated {
    [self willChangeValueForKey:@"zoomLevel"];
    mbgl::PrecisionPoint center(point.x, point.y);
    _mbglMap->scaleBy(scaleFactor, center, MGLDurationInSeconds(animated ? MGLAnimationDuration : 0));
    [self didChangeValueForKey:@"zoomLevel"];
}

- (double)maximumZoomLevel {
    return _mbglMap->getMaxZoom();
}

- (double)minimumZoomLevel {
    return _mbglMap->getMinZoom();
}

/// Respond to a click on the zoom control.
- (IBAction)zoomInOrOut:(NSSegmentedControl *)sender {
    switch (sender.selectedSegment) {
        case 0:
            // Zoom out.
            [self moveToEndOfParagraph:sender];
            break;
        case 1:
            // Zoom in.
            [self moveToBeginningOfParagraph:sender];
            break;
        default:
            break;
    }
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingDirection {
    return [NSSet setWithObject:@"camera"];
}

- (CLLocationDirection)direction {
    return mbgl::util::wrap(_mbglMap->getBearing(), 0., 360.);
}

- (void)setDirection:(CLLocationDirection)direction {
    [self setDirection:direction animated:NO];
}

- (void)setDirection:(CLLocationDirection)direction animated:(BOOL)animated {
    [self willChangeValueForKey:@"direction"];
    _mbglMap->setBearing(direction, MGLDurationInSeconds(animated ? MGLAnimationDuration : 0));
    [self didChangeValueForKey:@"direction"];
}

- (void)offsetDirectionBy:(CLLocationDegrees)delta animated:(BOOL)animated {
    [self willChangeValueForKey:@"direction"];
    _mbglMap->cancelTransitions();
    _mbglMap->setBearing(_mbglMap->getBearing() + delta, MGLDurationInSeconds(animated ? MGLAnimationDuration : 0));
    [self didChangeValueForKey:@"direction"];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCamera {
    return [NSSet setWithObjects:@"latitude", @"longitude", @"centerCoordinate", @"zoomLevel", @"direction", nil];
}

- (MGLMapCamera *)camera {
    CGRect frame = self.frame;
    CGPoint edgePoint;
    // Constrain by the shorter of the two axes.
    if (frame.size.width > frame.size.height) { // landscape
        edgePoint = CGPointMake(0, frame.size.height / 2.);
    } else { // portrait
        edgePoint = CGPointMake(frame.size.width / 2., 0);
    }
    mbgl::LatLng edgeLatLng = [self convertPoint:edgePoint toLatLngFromView:self];
    mbgl::ProjectedMeters edgeMeters = _mbglMap->projectedMetersForLatLng(edgeLatLng);

    // Because we constrain the zoom level vertically in portrait orientation,
    // the visible medial span is affected by pitch: the distance from the
    // center point to the near edge is less than than distance from the center
    // point to the far edge. Average the two distances.
    mbgl::ProjectedMeters nearEdgeMeters;
    if (frame.size.width > frame.size.height) {
        nearEdgeMeters = edgeMeters;
    } else {
        CGPoint nearEdgePoint = CGPointMake(frame.size.width / 2., frame.size.height);
        mbgl::LatLng nearEdgeLatLng = [self convertPoint:nearEdgePoint toLatLngFromView:self];
        nearEdgeMeters = _mbglMap->projectedMetersForLatLng(nearEdgeLatLng);
    }

    // The opposite side is the distance between the center and one edge.
    mbgl::LatLng centerLatLng = MGLLatLngFromLocationCoordinate2D(self.centerCoordinate);
    mbgl::ProjectedMeters centerMeters = _mbglMap->projectedMetersForLatLng(centerLatLng);
    CLLocationDistance centerToEdge = std::hypot(centerMeters.easting - edgeMeters.easting,
                                                 centerMeters.northing - edgeMeters.northing);
    CLLocationDistance centerToNearEdge = std::hypot(centerMeters.easting - nearEdgeMeters.easting,
                                                     centerMeters.northing - nearEdgeMeters.northing);
    CLLocationDistance altitude = (centerToEdge + centerToNearEdge) / 2 / std::tan(MGLAngularFieldOfView / 2.);

    CGFloat pitch = _mbglMap->getPitch();

    return [MGLMapCamera cameraLookingAtCenterCoordinate:self.centerCoordinate
                                            fromDistance:altitude
                                                   pitch:pitch
                                                 heading:self.direction];
}

- (void)setCamera:(MGLMapCamera *)camera {
    [self setCamera:camera animated:NO];
}

- (void)setCamera:(MGLMapCamera *)camera animated:(BOOL)animated {
    [self setCamera:camera withDuration:animated ? MGLAnimationDuration : 0 animationTimingFunction:nil completionHandler:NULL];
}

- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion {
    _mbglMap->cancelTransitions();
    
    // The opposite side is the distance between the center and one edge.
    mbgl::LatLng centerLatLng = MGLLatLngFromLocationCoordinate2D(camera.centerCoordinate);
    mbgl::ProjectedMeters centerMeters = _mbglMap->projectedMetersForLatLng(centerLatLng);
    CLLocationDistance centerToEdge = camera.altitude * std::tan(MGLAngularFieldOfView / 2.);
    
    double angle = -1;
    if (camera.heading >= 0) {
        angle = MGLRadiansFromDegrees(-camera.heading);
    }
    double pitch = -1;
    if (camera.pitch >= 0) {
        pitch = MGLRadiansFromDegrees(mbgl::util::clamp(camera.pitch, MGLMinimumPitch, MGLMaximumPitch));
    }
    
    // Make a visible bounds that extends in the constrained direction (the
    // shorter of the two axes).
    CGRect frame = self.frame;
    mbgl::LatLng sw, ne;
    if (frame.size.width > frame.size.height) { // landscape
        sw = _mbglMap->latLngForProjectedMeters({
            centerMeters.northing - centerToEdge * std::sin(angle),
            centerMeters.easting - centerToEdge * std::cos(angle),
        });
        ne = _mbglMap->latLngForProjectedMeters({
            centerMeters.northing + centerToEdge * std::sin(angle),
            centerMeters.easting + centerToEdge * std::cos(angle),
        });
    }
    else { // portrait
        sw = _mbglMap->latLngForProjectedMeters({
            centerMeters.northing - centerToEdge * std::cos(-angle) + centerToEdge * std::cos(-angle) * std::sin(pitch) / 2,
            centerMeters.easting - centerToEdge * std::sin(-angle) + centerToEdge * std::sin(-angle) * std::sin(pitch) / 2,
        });
        ne = _mbglMap->latLngForProjectedMeters({
            centerMeters.northing + centerToEdge * std::cos(-angle) - centerToEdge * std::cos(-angle) * std::sin(pitch) / 2,
            centerMeters.easting + centerToEdge * std::sin(-angle) - centerToEdge * std::sin(-angle) * std::sin(pitch) / 2,
        });
    }
    
    // Fit the viewport to the bounds. Correct the center in case pitch should
    // cause the visual center to lie above the screen center.
    mbgl::CameraOptions options = _mbglMap->cameraForLatLngs({ sw, ne }, {});
    options.center = centerLatLng;
    
    if (camera.heading >= 0) {
        options.angle = angle;
    }
    if (pitch >= 0) {
        options.pitch = pitch;
    }
    if (duration > 0) {
        options.duration = MGLDurationInSeconds(duration);
        options.easing = MGLUnitBezierForMediaTimingFunction(function);
    }
    if (completion) {
        options.transitionFinishFn = [completion]() {
            // Must run asynchronously after the transition is completely over.
            // Otherwise, a call to -setCamera: within the completion handler
            // would reenter the completion handler’s caller.
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    
    [self willChangeValueForKey:@"camera"];
    _mbglMap->easeTo(options);
    [self didChangeValueForKey:@"camera"];
}

+ (NSSet *)keyPathsForValuesAffectingVisibleCoordinateBounds {
    return [NSSet setWithObjects:@"centerCoordinate", @"zoomLevel", @"direction", @"bounds", nil];
}

- (MGLCoordinateBounds)visibleCoordinateBounds {
    return [self convertRect:self.bounds toCoordinateBoundsFromView:self];
}

- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds {
    [self setVisibleCoordinateBounds:bounds animated:NO];
}

- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds animated:(BOOL)animated {
    [self setVisibleCoordinateBounds:bounds edgePadding:NSEdgeInsetsZero animated:animated];
}

- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(NSEdgeInsets)insets animated:(BOOL)animated {
    _mbglMap->cancelTransitions();
    
    mbgl::EdgeInsets mbglInsets = MGLEdgeInsetsFromNSEdgeInsets(insets);
    mbgl::CameraOptions options = _mbglMap->cameraForLatLngBounds(MGLLatLngBoundsFromCoordinateBounds(bounds), mbglInsets);
    if (animated) {
        options.duration = MGLDurationInSeconds(MGLAnimationDuration);
    }
    
    [self willChangeValueForKey:@"visibleCoordinateBounds"];
    options.transitionFinishFn = ^() {
        [self didChangeValueForKey:@"visibleCoordinateBounds"];
    };
    _mbglMap->easeTo(options);
}

#pragma mark Mouse events and gestures

- (BOOL)acceptsFirstResponder {
    return YES;
}

/// Drag to pan, plus drag to zoom, rotate, and tilt when a modifier key is held
/// down.
- (void)handlePanGesture:(NSPanGestureRecognizer *)gestureRecognizer {
    NSPoint delta = [gestureRecognizer translationInView:self];
    NSPoint endPoint = [gestureRecognizer locationInView:self];
    NSPoint startPoint = NSMakePoint(endPoint.x - delta.x, self.bounds.size.height - (endPoint.y - delta.y));
    
    NSEventModifierFlags flags = [NSApp currentEvent].modifierFlags;
    if (flags & NSShiftKeyMask) {
        // Shift-drag to zoom.
        if (!self.zoomEnabled) {
            return;
        }
        
        _mbglMap->cancelTransitions();
        
        if (gestureRecognizer.state == NSGestureRecognizerStateBegan) {
            _mbglMap->setGestureInProgress(true);
            _scaleAtBeginningOfGesture = _mbglMap->getScale();
        } else if (gestureRecognizer.state == NSGestureRecognizerStateChanged) {
            CGFloat newZoomLevel = log2f(_scaleAtBeginningOfGesture) - delta.y / 75;
            [self scaleBy:powf(2, newZoomLevel) / _mbglMap->getScale() atPoint:startPoint animated:NO];
        } else if (gestureRecognizer.state == NSGestureRecognizerStateEnded
                   || gestureRecognizer.state == NSGestureRecognizerStateCancelled) {
            _mbglMap->setGestureInProgress(false);
            // Maps.app locks the cursor to the start point, but that would
            // interfere with the pan gesture recognizer. Just move the cursor
            // back at the end of the gesture.
            CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, startPoint);
        }
    } else if (flags & NSAlternateKeyMask) {
        // Option-drag to rotate and/or tilt.
        _mbglMap->cancelTransitions();
        
        if (gestureRecognizer.state == NSGestureRecognizerStateBegan) {
            _mbglMap->setGestureInProgress(true);
            _directionAtBeginningOfGesture = self.direction;
            _pitchAtBeginningOfGesture = _mbglMap->getPitch();
        } else if (gestureRecognizer.state == NSGestureRecognizerStateChanged) {
            mbgl::PrecisionPoint center(startPoint.x, startPoint.y);
            if (self.rotateEnabled) {
                CLLocationDirection newDirection = _directionAtBeginningOfGesture - delta.x / 10;
                [self willChangeValueForKey:@"direction"];
                _mbglMap->setBearing(newDirection, center);
                [self didChangeValueForKey:@"direction"];
            }
            if (self.pitchEnabled) {
                _mbglMap->setPitch(_pitchAtBeginningOfGesture + delta.y / 5);
            }
        } else if (gestureRecognizer.state == NSGestureRecognizerStateEnded
                   || gestureRecognizer.state == NSGestureRecognizerStateCancelled) {
            _mbglMap->setGestureInProgress(false);
        }
    } else if (self.scrollEnabled) {
        // Otherwise, drag to pan.
        _mbglMap->cancelTransitions();
        
        if (gestureRecognizer.state == NSGestureRecognizerStateBegan) {
            [self.window invalidateCursorRectsForView:self];
            _mbglMap->setGestureInProgress(true);
        } else if (gestureRecognizer.state == NSGestureRecognizerStateChanged) {
            delta.y *= -1;
            [self offsetCenterCoordinateBy:delta animated:NO];
            [gestureRecognizer setTranslation:NSZeroPoint inView:self];
        } else if (gestureRecognizer.state == NSGestureRecognizerStateEnded
                   || gestureRecognizer.state == NSGestureRecognizerStateCancelled) {
            _mbglMap->setGestureInProgress(false);
            [self.window invalidateCursorRectsForView:self];
        }
    }
}

/// Pinch to zoom.
- (void)handleMagnificationGesture:(NSMagnificationGestureRecognizer *)gestureRecognizer {
    if (!self.zoomEnabled) {
        return;
    }
    
    _mbglMap->cancelTransitions();
    
    if (gestureRecognizer.state == NSGestureRecognizerStateBegan) {
        _mbglMap->setGestureInProgress(true);
        _scaleAtBeginningOfGesture = _mbglMap->getScale();
    } else if (gestureRecognizer.state == NSGestureRecognizerStateChanged) {
        NSPoint zoomInPoint = [gestureRecognizer locationInView:self];
        mbgl::PrecisionPoint center(zoomInPoint.x, self.bounds.size.height - zoomInPoint.y);
        if (gestureRecognizer.magnification > -1) {
            [self willChangeValueForKey:@"zoomLevel"];
            [self willChangeValueForKey:@"centerCoordinate"];
            _mbglMap->setScale(_scaleAtBeginningOfGesture * (1 + gestureRecognizer.magnification), center);
            [self didChangeValueForKey:@"centerCoordinate"];
            [self didChangeValueForKey:@"zoomLevel"];
        }
    } else if (gestureRecognizer.state == NSGestureRecognizerStateEnded
               || gestureRecognizer.state == NSGestureRecognizerStateCancelled) {
        _mbglMap->setGestureInProgress(false);
    }
}

/// Click or tap to select an annotation.
- (void)handleClickGesture:(NSClickGestureRecognizer *)gestureRecognizer {
    if (gestureRecognizer.state != NSGestureRecognizerStateEnded) {
        return;
    }
    
    NSPoint gesturePoint = [gestureRecognizer locationInView:self];
    MGLAnnotationTag hitAnnotationTag = [self annotationTagAtPoint:gesturePoint persistingResults:YES];
    if (hitAnnotationTag != MGLAnnotationTagNotFound) {
        if (hitAnnotationTag != _selectedAnnotationTag) {
            id <MGLAnnotation> annotation = [self annotationWithTag:hitAnnotationTag];
            NSAssert(annotation, @"Cannot select nonexistent annotation with ID %i", hitAnnotationTag);
            [self selectAnnotation:annotation];
        }
    } else {
        [self deselectAnnotation:self.selectedAnnotation];
    }
}

/// Tap with two fingers (“right-click”) to zoom out.
- (void)handleSecondaryClickGesture:(NSClickGestureRecognizer *)gestureRecognizer {
    if (!self.zoomEnabled) {
        return;
    }
    
    _mbglMap->cancelTransitions();
    
    NSPoint gesturePoint = [gestureRecognizer locationInView:self];
    [self scaleBy:0.5 atPoint:NSMakePoint(gesturePoint.x, self.bounds.size.height - gesturePoint.y) animated:YES];
}

/// Double-click or double-tap to zoom in.
- (void)handleDoubleClickGesture:(NSClickGestureRecognizer *)gestureRecognizer {
    if (!self.zoomEnabled) {
        return;
    }
    
    _mbglMap->cancelTransitions();
    
    NSPoint gesturePoint = [gestureRecognizer locationInView:self];
    [self scaleBy:2 atPoint:NSMakePoint(gesturePoint.x, self.bounds.size.height - gesturePoint.y) animated:YES];
}

/// Rotate fingers to rotate.
- (void)handleRotationGesture:(NSRotationGestureRecognizer *)gestureRecognizer {
    if (!self.rotateEnabled) {
        return;
    }
    
    _mbglMap->cancelTransitions();
    
    if (gestureRecognizer.state == NSGestureRecognizerStateBegan) {
        _mbglMap->setGestureInProgress(true);
        _directionAtBeginningOfGesture = self.direction;
    } else if (gestureRecognizer.state == NSGestureRecognizerStateChanged) {
        NSPoint rotationPoint = [gestureRecognizer locationInView:self];
        mbgl::PrecisionPoint center(rotationPoint.x, rotationPoint.y);
        _mbglMap->setBearing(_directionAtBeginningOfGesture + gestureRecognizer.rotationInDegrees, center);
    } else if (gestureRecognizer.state == NSGestureRecognizerStateEnded
               || gestureRecognizer.state == NSGestureRecognizerStateCancelled) {
        _mbglMap->setGestureInProgress(false);
    }
}

- (BOOL)wantsScrollEventsForSwipeTrackingOnAxis:(__unused NSEventGestureAxis)axis {
    // Track both horizontal and vertical swipes in -scrollWheel:.
    return YES;
}

- (void)scrollWheel:(NSEvent *)event {
    // https://developer.apple.com/library/mac/releasenotes/AppKit/RN-AppKitOlderNotes/#10_7Dragging
    if (event.phase == NSEventPhaseNone && event.momentumPhase == NSEventPhaseNone) {
        // A traditional, vertical scroll wheel zooms instead of panning.
        if (self.zoomEnabled && std::abs(event.scrollingDeltaX) < std::abs(event.scrollingDeltaY)) {
            _mbglMap->cancelTransitions();
            
            [self willChangeValueForKey:@"zoomLevel"];
            [self willChangeValueForKey:@"centerCoordinate"];
            NSPoint gesturePoint = [self convertPoint:event.locationInWindow fromView:nil];
            mbgl::PrecisionPoint center(gesturePoint.x, self.bounds.size.height - gesturePoint.y);
            _mbglMap->scaleBy(exp2(event.scrollingDeltaY / 20), center);
            [self didChangeValueForKey:@"centerCoordinate"];
            [self didChangeValueForKey:@"zoomLevel"];
        }
    } else if (self.scrollEnabled
               && _magnificationGestureRecognizer.state == NSGestureRecognizerStatePossible
               && _rotationGestureRecognizer.state == NSGestureRecognizerStatePossible) {
        // Scroll to pan.
        _mbglMap->cancelTransitions();
        
        CGFloat x = event.scrollingDeltaX;
        CGFloat y = event.scrollingDeltaY;
        if (x || y) {
            [self offsetCenterCoordinateBy:NSMakePoint(x, y) animated:NO];
        }
        
        // Drift pan.
        if (event.momentumPhase != NSEventPhaseNone) {
            [self offsetCenterCoordinateBy:NSMakePoint(x, y) animated:NO];
        }
    }
}

#pragma mark Keyboard events

- (void)keyDown:(NSEvent *)event {
    if (event.modifierFlags & NSNumericPadKeyMask) {
        // This is the recommended way to handle arrow key presses, causing
        // methods like -moveUp: and -moveToBeginningOfParagraph: to be called
        // for various standard keybindings.
        [self interpretKeyEvents:@[event]];
    } else {
        [super keyDown:event];
    }
}

- (IBAction)moveUp:(__unused id)sender {
    [self offsetCenterCoordinateBy:NSMakePoint(0, MGLKeyPanningIncrement) animated:YES];
}

- (IBAction)moveDown:(__unused id)sender {
    [self offsetCenterCoordinateBy:NSMakePoint(0, -MGLKeyPanningIncrement) animated:YES];
}

- (IBAction)moveLeft:(__unused id)sender {
    [self offsetCenterCoordinateBy:NSMakePoint(MGLKeyPanningIncrement, 0) animated:YES];
}

- (IBAction)moveRight:(__unused id)sender {
    [self offsetCenterCoordinateBy:NSMakePoint(-MGLKeyPanningIncrement, 0) animated:YES];
}

- (IBAction)moveToBeginningOfParagraph:(__unused id)sender {
    if (self.zoomEnabled) {
        [self scaleBy:2 atPoint:NSZeroPoint animated:YES];
    }
}

- (IBAction)moveToEndOfParagraph:(__unused id)sender {
    if (self.zoomEnabled) {
        [self scaleBy:0.5 atPoint:NSZeroPoint animated:YES];
    }
}

- (IBAction)moveWordLeft:(__unused id)sender {
    if (self.rotateEnabled) {
        [self offsetDirectionBy:MGLKeyRotationIncrement animated:YES];
    }
}

- (IBAction)moveWordRight:(__unused id)sender {
    if (self.rotateEnabled) {
        [self offsetDirectionBy:-MGLKeyRotationIncrement animated:YES];
    }
}

- (void)setZoomEnabled:(BOOL)zoomEnabled {
    _zoomEnabled = zoomEnabled;
    _zoomControls.enabled = zoomEnabled;
    _zoomControls.hidden = !zoomEnabled;
}

- (void)setRotateEnabled:(BOOL)rotateEnabled {
    _rotateEnabled = rotateEnabled;
    _compass.enabled = rotateEnabled;
    _compass.hidden = !rotateEnabled;
}

#pragma mark Ornaments

/// Updates the zoom controls’ enabled state based on the current zoom level.
- (void)updateZoomControls {
    [_zoomControls setEnabled:self.zoomLevel > self.minimumZoomLevel forSegment:0];
    [_zoomControls setEnabled:self.zoomLevel < self.maximumZoomLevel forSegment:1];
}

/// Updates the compass to point in the same direction as the map.
- (void)updateCompass {
    // The circular slider control goes counterclockwise, whereas our map
    // measures its direction clockwise.
    _compass.doubleValue = -self.direction;
}

- (IBAction)rotate:(NSSlider *)sender {
    [self setDirection:-sender.doubleValue animated:YES];
}

#pragma mark Annotations

- (nullable NS_ARRAY_OF(id <MGLAnnotation>) *)annotations {
    if (_annotationContextsByAnnotationTag.empty()) {
        return nil;
    }
    
    // Map all the annotation tags to the annotations themselves.
    std::vector<id <MGLAnnotation>> annotations;
    std::transform(_annotationContextsByAnnotationTag.begin(),
                   _annotationContextsByAnnotationTag.end(),
                   std::back_inserter(annotations),
                   ^ id <MGLAnnotation> (const std::pair<MGLAnnotationTag, MGLAnnotationContext> &pair) {
                       return pair.second.annotation;
                   });
    return [NSArray arrayWithObjects:&annotations[0] count:annotations.size()];
}

/// Returns the annotation assigned the given tag. Cheap.
- (id <MGLAnnotation>)annotationWithTag:(MGLAnnotationTag)tag {
    if (!_annotationContextsByAnnotationTag.count(tag)) {
        return nil;
    }
    
    MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag[tag];
    return annotationContext.annotation;
}

/// Returns the annotation tag assigned to the given annotation. Relatively expensive.
- (MGLAnnotationTag)annotationTagForAnnotation:(id <MGLAnnotation>)annotation {
    if (!annotation) {
        return MGLAnnotationTagNotFound;
    }
    
    for (auto &pair : _annotationContextsByAnnotationTag) {
        if (pair.second.annotation == annotation) {
            return pair.first;
        }
    }
    return MGLAnnotationTagNotFound;
}

- (void)addAnnotation:(id <MGLAnnotation>)annotation {
    if (annotation) {
        [self addAnnotations:@[annotation]];
    }
}

- (void)addAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations {
    if (!annotations) {
        return;
    }
    
    [self willChangeValueForKey:@"annotations"];
    
    BOOL delegateHasImagesForAnnotations = [self.delegate respondsToSelector:@selector(mapView:imageForAnnotation:)];
    
    std::vector<mbgl::PointAnnotation> points;
    std::vector<mbgl::ShapeAnnotation> shapes;
    
    for (id <MGLAnnotation> annotation in annotations) {
        NSAssert([annotation conformsToProtocol:@protocol(MGLAnnotation)], @"Annotation does not conform to MGLAnnotation");
        
        if ([annotation isKindOfClass:[MGLMultiPoint class]]) {
            // The multipoint knows how to style itself (with the map view’s help).
            [(MGLMultiPoint *)annotation addShapeAnnotationObjectToCollection:shapes withDelegate:self];
        } else {
            MGLAnnotationImage *annotationImage = nil;
            if (delegateHasImagesForAnnotations) {
                annotationImage = [self.delegate mapView:self imageForAnnotation:annotation];
            }
            if (!annotationImage) {
                annotationImage = [self dequeueReusableAnnotationImageWithIdentifier:MGLDefaultStyleMarkerSymbolName];
            }
            if (!annotationImage) {
                // Create a default annotation image that depicts a round pin
                // rising from the center, with a shadow slightly below center.
                // The alignment rect therefore excludes the bottom half.
                NSImage *image = MGLDefaultMarkerImage();
                NSRect alignmentRect = image.alignmentRect;
                alignmentRect.origin.y = NSMidY(alignmentRect);
                alignmentRect.size.height /= 2;
                image.alignmentRect = alignmentRect;
                annotationImage = [MGLAnnotationImage annotationImageWithImage:image
                                                               reuseIdentifier:MGLDefaultStyleMarkerSymbolName];
            }
            
            if (!self.annotationImagesByIdentifier[annotationImage.reuseIdentifier]) {
                self.annotationImagesByIdentifier[annotationImage.reuseIdentifier] = annotationImage;
                [self installAnnotationImage:annotationImage];
            }
            
            NSString *symbolName = [MGLAnnotationSpritePrefix stringByAppendingString:annotationImage.reuseIdentifier];
            points.emplace_back(MGLLatLngFromLocationCoordinate2D(annotation.coordinate), symbolName ? [symbolName UTF8String] : "");
            
            // Opt into potentially expensive tooltip tracking areas.
            if (annotation.toolTip.length) {
                _wantsToolTipRects = YES;
            }
        }
    }
    
    // Add any point annotations to mbgl and our own index.
    if (points.size()) {
        std::vector<MGLAnnotationTag> pointAnnotationTags = _mbglMap->addPointAnnotations(points);
        
        for (size_t i = 0; i < pointAnnotationTags.size(); ++i) {
            MGLAnnotationContext context;
            context.annotation = annotations[i];
            context.symbolIdentifier = @(points[i].icon.c_str());
            _annotationContextsByAnnotationTag[pointAnnotationTags[i]] = context;
        }
    }
    
    // Add any shape annotations to mbgl and our own index.
    if (shapes.size()) {
        std::vector<MGLAnnotationTag> shapeAnnotationTags = _mbglMap->addShapeAnnotations(shapes);
        
        for (size_t i = 0; i < shapeAnnotationTags.size(); ++i) {
            MGLAnnotationContext context;
            context.annotation = annotations[i];
            _annotationContextsByAnnotationTag[shapeAnnotationTags[i]] = context;
        }
    }
    
    [self didChangeValueForKey:@"annotations"];
    
    [self updateAnnotationTrackingAreas];
}

/// Sends the raw pixel data of the annotation image’s image to mbgl and
/// calculates state needed for hit testing later.
- (void)installAnnotationImage:(MGLAnnotationImage *)annotationImage {
    NSImage *image = annotationImage.image;
    NSSize size = image.size;
    if (size.width == 0 || size.height == 0 || !image.valid) {
        // Can’t create an empty sprite. An image that hasn’t loaded is also useless.
        return;
    }
    
    // Create a bitmap image representation from the image, respecting backing
    // scale factor and any resizing done on the image at runtime.
    // http://www.cocoabuilder.com/archive/cocoa/82430-nsimage-getting-raw-bitmap-data.html#82431
    [image lockFocus];
    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithFocusedViewRect:{ NSZeroPoint, size }];
    [image unlockFocus];
    
    // Get the image’s raw pixel data as an RGBA buffer.
    std::string pixelString((const char *)rep.bitmapData, rep.pixelsWide * rep.pixelsHigh * 4 /* RGBA */);
    auto cSpriteImage = std::make_shared<mbgl::SpriteImage>((uint16_t)rep.size.width,
                                                            (uint16_t)rep.size.height,
                                                            (float)(rep.pixelsWide / size.width),
                                                            std::move(pixelString));
    NSString *symbolName = [MGLAnnotationSpritePrefix stringByAppendingString:annotationImage.reuseIdentifier];
    _mbglMap->addAnnotationIcon(symbolName.UTF8String, cSpriteImage);
    
    // Create a slop area with a “radius” equal to the annotation image’s entire
    // size, allowing the eventual click to be on any point within this image.
    // Union this slop area with any existing slop areas.
    _unionedAnnotationImageSize = NSMakeSize(MAX(_unionedAnnotationImageSize.width, size.width),
                                             MAX(_unionedAnnotationImageSize.height, size.height));
    
    // Opt into potentially expensive cursor tracking areas.
    if (annotationImage.cursor) {
        _wantsCursorRects = YES;
    }
}

- (void)removeAnnotation:(id <MGLAnnotation>)annotation {
    if (annotation) {
        [self removeAnnotations:@[annotation]];
    }
}

- (void)removeAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations {
    if (!annotations) {
        return;
    }
    
    std::vector<MGLAnnotationTag> annotationTagsToRemove;
    annotationTagsToRemove.reserve(annotations.count);
    
    for (id <MGLAnnotation> annotation in annotations) {
        NSAssert([annotation conformsToProtocol:@protocol(MGLAnnotation)], @"Annotation does not conform to MGLAnnotation");
        
        MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
        NSAssert(annotationTag != MGLAnnotationTagNotFound, @"No ID for annotation %@", annotation);
        annotationTagsToRemove.push_back(annotationTag);
        
        if (annotationTag == _selectedAnnotationTag) {
            [self deselectAnnotation:annotation];
        }
        if (annotationTag == _lastSelectedAnnotationTag) {
            _lastSelectedAnnotationTag = MGLAnnotationTagNotFound;
        }
        
        _annotationContextsByAnnotationTag.erase(annotationTag);
    }
    
    [self willChangeValueForKey:@"annotations"];
    _mbglMap->removeAnnotations(annotationTagsToRemove);
    [self didChangeValueForKey:@"annotations"];
    
    [self updateAnnotationTrackingAreas];
}

- (id <MGLAnnotation>)selectedAnnotation {
    if (!_annotationContextsByAnnotationTag.count(_selectedAnnotationTag)) {
        return nil;
    }
    MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(_selectedAnnotationTag);
    return annotationContext.annotation;
}

- (nullable MGLAnnotationImage *)dequeueReusableAnnotationImageWithIdentifier:(NSString *)identifier {
    // This prefix is used to avoid collisions with style-defined sprites in
    // mbgl, but reusable identifiers are never prefixed.
    if ([identifier hasPrefix:MGLAnnotationSpritePrefix]) {
        identifier = [identifier substringFromIndex:MGLAnnotationSpritePrefix.length];
    }
    return self.annotationImagesByIdentifier[identifier];
}

- (id <MGLAnnotation>)annotationAtPoint:(NSPoint)point {
    return [self annotationWithTag:[self annotationTagAtPoint:point persistingResults:NO]];
}

/**
    Returns the tag of the annotation at the given point in the view.
    
    This is more involved than it sounds: if multiple point annotations overlap
    near the point, this method cycles through them so that each of them is
    accessible to the user at some point.
    
    @param persist True to remember the cycleable set of annotations, so that a
        different annotation is returned the next time this method is called
        with the same point. Setting this parameter to false is useful for
        asking “what if?”
 */
- (MGLAnnotationTag)annotationTagAtPoint:(NSPoint)point persistingResults:(BOOL)persist {
    // Look for any annotation near the click. An annotation is “near” if the
    // distance between its center and the click is less than the maximum height
    // or width of an installed annotation image.
    NSRect queryRect = NSInsetRect({ point, NSZeroSize },
                                   -_unionedAnnotationImageSize.width / 2,
                                   -_unionedAnnotationImageSize.height / 2);
    queryRect = NSInsetRect(queryRect, -MGLAnnotationImagePaddingForHitTest,
                            -MGLAnnotationImagePaddingForHitTest);
    std::vector<MGLAnnotationTag> nearbyAnnotations = [self annotationTagsInRect:queryRect];
    
    if (nearbyAnnotations.size()) {
        // Assume that the user is fat-fingering an annotation.
        NSRect hitRect = NSInsetRect({ point, NSZeroSize },
                                     -MGLAnnotationImagePaddingForHitTest,
                                     -MGLAnnotationImagePaddingForHitTest);
        
        // Filter out any annotation whose image is unselectable or for which
        // hit testing fails.
        mbgl::util::erase_if(nearbyAnnotations, [&](const MGLAnnotationTag annotationTag) {
            NSAssert(_annotationContextsByAnnotationTag.count(annotationTag) != 0, @"Unknown annotation found nearby click");
            id <MGLAnnotation> annotation = [self annotationWithTag:annotationTag];
            if (!annotation) {
                return true;
            }
            
            MGLAnnotationImage *annotationImage = [self imageOfAnnotationWithTag:annotationTag];
            if (!annotationImage.selectable) {
                return true;
            }
            
            // Filter out the annotation if the fattened finger didn’t land on a
            // translucent or opaque pixel in the image.
            NSRect annotationRect = [self frameOfImage:annotationImage.image
                                  centeredAtCoordinate:annotation.coordinate];
            return !!![annotationImage.image hitTestRect:hitRect withImageDestinationRect:annotationRect
                                                 context:nil hints:nil flipped:NO];
        });
    }
    
    MGLAnnotationTag hitAnnotationTag = MGLAnnotationTagNotFound;
    if (nearbyAnnotations.size()) {
        // The annotation tags need to be stable in order to compare them with
        // the remembered tags.
        std::sort(nearbyAnnotations.begin(), nearbyAnnotations.end());
        
        if (nearbyAnnotations == _annotationsNearbyLastClick) {
            // The last time we persisted a set of annotations, we had the same
            // set of annotations as we do now. Cycle through them.
            if (_lastSelectedAnnotationTag == MGLAnnotationTagNotFound
                || _lastSelectedAnnotationTag == _annotationsNearbyLastClick.back()) {
                // Either an annotation from this set hasn’t been selected
                // before or the last annotation in the set was selected. Wrap
                // around to the first annotation in the set.
                hitAnnotationTag = _annotationsNearbyLastClick.front();
            } else {
                // Step to the next annotation in the set.
                auto result = std::find(_annotationsNearbyLastClick.begin(),
                                        _annotationsNearbyLastClick.end(),
                                        _lastSelectedAnnotationTag);
                auto distance = std::distance(_annotationsNearbyLastClick.begin(), result);
                hitAnnotationTag = _annotationsNearbyLastClick[distance + 1];
            }
        } else {
            // Remember the nearby annotations for the next time this method is
            // called.
            if (persist) {
                _annotationsNearbyLastClick = nearbyAnnotations;
            }
            
            // Choose the first nearby annotation.
            if (_annotationsNearbyLastClick.size()) {
                hitAnnotationTag = _annotationsNearbyLastClick.front();
            }
        }
    }
    
    return hitAnnotationTag;
}

/// Returns the tags of the annotations coincident with the given rectangle.
- (std::vector<MGLAnnotationTag>)annotationTagsInRect:(NSRect)rect {
    mbgl::LatLngBounds queryBounds = [self convertRect:rect toLatLngBoundsFromView:self];
    return _mbglMap->getPointAnnotationsInBounds(queryBounds);
}

- (NS_ARRAY_OF(id <MGLAnnotation>) *)selectedAnnotations {
    id <MGLAnnotation> selectedAnnotation = self.selectedAnnotation;
    return selectedAnnotation ? @[selectedAnnotation] : @[];
}

- (void)setSelectedAnnotation:(id <MGLAnnotation>)selectedAnnotation {
    _selectedAnnotationTag = [self annotationTagForAnnotation:selectedAnnotation];
}

- (void)setSelectedAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)selectedAnnotations {
    if (!selectedAnnotations.count) {
        return;
    }
    
    id <MGLAnnotation> firstAnnotation = selectedAnnotations[0];
    NSAssert([firstAnnotation conformsToProtocol:@protocol(MGLAnnotation)], @"Annotation does not conform to MGLAnnotation");
    if ([firstAnnotation isKindOfClass:[MGLMultiPoint class]]) {
        return;
    }
    
    // Select the annotation if it’s visible.
    if (MGLCoordinateInCoordinateBounds(firstAnnotation.coordinate, self.visibleCoordinateBounds)) {
        [self selectAnnotation:firstAnnotation];
    }
}

- (void)selectAnnotation:(id <MGLAnnotation>)annotation
{
    // Only point annotations can be selected.
    if (!annotation || [annotation isKindOfClass:[MGLMultiPoint class]]) {
        return;
    }
    
    id <MGLAnnotation> selectedAnnotation = self.selectedAnnotation;
    if (annotation == selectedAnnotation) {
        return;
    }
    
    // Deselect the annotation before reselecting it.
    [self deselectAnnotation:selectedAnnotation];
    
    // Add the annotation to the map if it hasn’t been added yet.
    MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
    if (annotationTag == MGLAnnotationTagNotFound) {
        [self addAnnotation:annotation];
    }
    
    // The annotation can’t be selected if no part of it is hittable.
    NSRect positioningRect = [self positioningRectForCalloutForAnnotationWithTag:annotationTag];
    if (NSIsEmptyRect(NSIntersectionRect(positioningRect, self.bounds))) {
        return;
    }
    
    [self willChangeValueForKey:@"selectedAnnotation"];
    _selectedAnnotationTag = annotationTag;
    _lastSelectedAnnotationTag = _selectedAnnotationTag;
    [self didChangeValueForKey:@"selectedAnnotation"];
    
    // For the callout to be shown, the annotation must have a title, its
    // callout must not already be shown, and the annotation must be able to
    // show a callout according to the delegate.
    if ([annotation respondsToSelector:@selector(title)]
        && annotation.title
        && !self.calloutForSelectedAnnotation.shown
        && [self.delegate respondsToSelector:@selector(mapView:annotationCanShowCallout:)]
        && [self.delegate mapView:self annotationCanShowCallout:annotation]) {
        NSPopover *callout = [self calloutForAnnotation:annotation];
        
        // Hang the callout off the right edge of the annotation image’s
        // alignment rect, or off the left edge in a right-to-left UI.
        callout.delegate = self;
        self.calloutForSelectedAnnotation = callout;
        NSRectEdge edge = (self.userInterfaceLayoutDirection == NSUserInterfaceLayoutDirectionRightToLeft
                           ? NSMinXEdge
                           : NSMaxXEdge);
        [callout showRelativeToRect:positioningRect ofView:self preferredEdge:edge];
    }
}

/// Returns a popover detailing the annotation.
- (NSPopover *)calloutForAnnotation:(id <MGLAnnotation>)annotation {
    NSPopover *callout = [[NSPopover alloc] init];
    callout.behavior = NSPopoverBehaviorTransient;
    
    NSViewController *viewController;
    if ([self.delegate respondsToSelector:@selector(mapView:calloutViewControllerForAnnotation:)]) {
        NSViewController *viewControllerFromDelegate = [self.delegate mapView:self
                                           calloutViewControllerForAnnotation:annotation];
        if (viewControllerFromDelegate) {
            viewController = viewControllerFromDelegate;
        }
    }
    if (!viewController) {
        viewController = self.calloutViewController;
    }
    NSAssert(viewController, @"Unable to load MGLAnnotationCallout view controller");
    // The popover’s view controller can bind to KVO-compliant key paths of the
    // annotation.
    viewController.representedObject = annotation;
    callout.contentViewController = viewController;
    
    return callout;
}

- (NSViewController *)calloutViewController {
    // Lazily load a default view controller.
    if (!_calloutViewController) {
        _calloutViewController = [[NSViewController alloc] initWithNibName:@"MGLAnnotationCallout"
                                                                    bundle:[NSBundle mgl_frameworkBundle]];
    }
    return _calloutViewController;
}

/// Returns the rectangle that represents the annotation image of the annotation
/// with the given tag. This rectangle is fitted to the image’s alignment rect
/// and is appropriate for positioning a popover.
- (NSRect)positioningRectForCalloutForAnnotationWithTag:(MGLAnnotationTag)annotationTag {
    id <MGLAnnotation> annotation = [self annotationWithTag:annotationTag];
    if (!annotation) {
        return NSZeroRect;
    }
    NSImage *image = [self imageOfAnnotationWithTag:annotationTag].image;
    if (!image) {
        return NSZeroRect;
    }
    
    NSRect positioningRect = [self frameOfImage:image centeredAtCoordinate:annotation.coordinate];
    positioningRect = NSOffsetRect(image.alignmentRect, positioningRect.origin.x, positioningRect.origin.y);
    return NSInsetRect(positioningRect, -MGLAnnotationImagePaddingForCallout,
                       -MGLAnnotationImagePaddingForCallout);
}

/// Returns the rectangle relative to the viewport that represents the given
/// image centered at the given coordinate.
- (NSRect)frameOfImage:(NSImage *)image centeredAtCoordinate:(CLLocationCoordinate2D)coordinate {
    NSPoint calloutAnchorPoint = [self convertCoordinate:coordinate toPointToView:self];
    return NSInsetRect({ calloutAnchorPoint, NSZeroSize }, -image.size.width / 2, -image.size.height / 2);
}

/// Returns the annotation image assigned to the annotation with the given tag.
- (MGLAnnotationImage *)imageOfAnnotationWithTag:(MGLAnnotationTag)annotationTag {
    if (annotationTag == MGLAnnotationTagNotFound
        || _annotationContextsByAnnotationTag.count(annotationTag) == 0) {
        return nil;
    }
    
    NSString *customSymbol = _annotationContextsByAnnotationTag.at(annotationTag).symbolIdentifier;
    NSString *symbolName = customSymbol.length ? customSymbol : MGLDefaultStyleMarkerSymbolName;
    
    return [self dequeueReusableAnnotationImageWithIdentifier:symbolName];
}

- (void)deselectAnnotation:(id <MGLAnnotation>)annotation {
    if (!annotation || self.selectedAnnotation != annotation) {
        return;
    }
    
    // Close the callout popover gracefully.
    NSPopover *callout = self.calloutForSelectedAnnotation;
    [callout performClose:self];
    
    self.selectedAnnotation = nil;
}

/// Move the annotation callout to point to the selected annotation at its
/// current position.
- (void)updateAnnotationCallouts {
    NSPopover *callout = self.calloutForSelectedAnnotation;
    if (callout) {
        callout.positioningRect = [self positioningRectForCalloutForAnnotationWithTag:_selectedAnnotationTag];
    }
}

#pragma mark MGLMultiPointDelegate methods

- (double)alphaForShapeAnnotation:(MGLShape *)annotation {
    if (_delegateHasAlphasForShapeAnnotations) {
        return [self.delegate mapView:self alphaForShapeAnnotation:annotation];
    }
    return 1.0;
}

- (mbgl::Color)strokeColorForShapeAnnotation:(MGLShape *)annotation {
    NSColor *color = (_delegateHasStrokeColorsForShapeAnnotations
                      ? [self.delegate mapView:self strokeColorForShapeAnnotation:annotation]
                      : [NSColor blackColor]);
    return MGLColorObjectFromNSColor(color);
}

- (mbgl::Color)fillColorForPolygonAnnotation:(MGLPolygon *)annotation {
    NSColor *color = (_delegateHasFillColorsForShapeAnnotations
                      ? [self.delegate mapView:self fillColorForPolygonAnnotation:annotation]
                      : [NSColor blueColor]);
    return MGLColorObjectFromNSColor(color);
}

- (CGFloat)lineWidthForPolylineAnnotation:(MGLPolyline *)annotation {
    if (_delegateHasLineWidthsForShapeAnnotations) {
        return [self.delegate mapView:self lineWidthForPolylineAnnotation:(MGLPolyline *)annotation];
    }
    return 3.0;
}

#pragma mark MGLPopoverDelegate methods

- (void)popoverDidShow:(__unused NSNotification *)notification {
    id <MGLAnnotation> annotation = self.selectedAnnotation;
    if ([self.delegate respondsToSelector:@selector(mapView:didSelectAnnotation:)]) {
        [self.delegate mapView:self didSelectAnnotation:annotation];
    }
}

- (void)popoverDidClose:(__unused NSNotification *)notification {
    // Deselect the closed popover, in case the popover was closed due to user
    // action.
    id <MGLAnnotation> annotation = self.calloutForSelectedAnnotation.contentViewController.representedObject;
    self.calloutForSelectedAnnotation = nil;
    self.selectedAnnotation = nil;
    
    if ([self.delegate respondsToSelector:@selector(mapView:didDeselectAnnotation:)]) {
        [self.delegate mapView:self didDeselectAnnotation:annotation];
    }
}

#pragma mark Overlays

- (void)addOverlay:(id <MGLOverlay>)overlay {
    [self addOverlays:@[overlay]];
}

- (void)addOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays
{
    for (id <MGLOverlay> overlay in overlays) {
        NSAssert([overlay conformsToProtocol:@protocol(MGLOverlay)], @"Overlay does not conform to MGLOverlay");
    }
    [self addAnnotations:overlays];
}

- (void)removeOverlay:(id <MGLOverlay>)overlay {
    [self removeOverlays:@[overlay]];
}

- (void)removeOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays {
    for (id <MGLOverlay> overlay in overlays) {
        NSAssert([overlay conformsToProtocol:@protocol(MGLOverlay)], @"Overlay does not conform to MGLOverlay");
    }
    [self removeAnnotations:overlays];
}

#pragma mark Tooltips and cursors

- (void)updateAnnotationTrackingAreas {
    if (_wantsToolTipRects) {
        [self removeAllToolTips];
        std::vector<MGLAnnotationTag> annotationTags = [self annotationTagsInRect:self.bounds];
        for (MGLAnnotationTag annotationTag : annotationTags) {
            MGLAnnotationImage *annotationImage = [self imageOfAnnotationWithTag:annotationTag];
            id <MGLAnnotation> annotation = [self annotationWithTag:annotationTag];
            if (annotation.toolTip.length) {
                // Add a tooltip tracking area over the annotation image’s
                // frame, accounting for the image’s alignment rect.
                NSImage *image = annotationImage.image;
                NSRect annotationRect = [self frameOfImage:image
                                      centeredAtCoordinate:annotation.coordinate];
                annotationRect = NSOffsetRect(image.alignmentRect, annotationRect.origin.x, annotationRect.origin.y);
                if (!NSIsEmptyRect(annotationRect)) {
                    [self addToolTipRect:annotationRect owner:self userData:(void *)(NSUInteger)annotationTag];
                }
            }
            // Opt into potentially expensive cursor tracking areas.
            if (annotationImage.cursor) {
                _wantsCursorRects = YES;
            }
        }
    }
    
    // Blow away any cursor tracking areas and rebuild them. That’s the
    // potentially expensive part.
    if (_wantsCursorRects) {
        [self.window invalidateCursorRectsForView:self];
    }
}

- (NSString *)view:(__unused NSView *)view stringForToolTip:(__unused NSToolTipTag)tag point:(__unused NSPoint)point userData:(void *)data {
    if ((NSUInteger)data >= MGLAnnotationTagNotFound) {
        return nil;
    }
    MGLAnnotationTag annotationTag = (NSUInteger)data;
    id <MGLAnnotation> annotation = [self annotationWithTag:annotationTag];
    return annotation.toolTip;
}

- (void)resetCursorRects {
    // Drag to pan has a grabbing hand cursor.
    if (_panGestureRecognizer.state == NSGestureRecognizerStateBegan
        || _panGestureRecognizer.state == NSGestureRecognizerStateChanged) {
        [self addCursorRect:self.bounds cursor:[NSCursor closedHandCursor]];
        return;
    }
    
    // The rest of this method can be expensive, so bail if no annotations have
    // ever had custom cursors.
    if (!_wantsCursorRects) {
        return;
    }
    
    std::vector<MGLAnnotationTag> annotationTags = [self annotationTagsInRect:self.bounds];
    for (MGLAnnotationTag annotationTag : annotationTags) {
        id <MGLAnnotation> annotation = [self annotationWithTag:annotationTag];
        MGLAnnotationImage *annotationImage = [self imageOfAnnotationWithTag:annotationTag];
        if (annotationImage.cursor) {
            // Add a cursor tracking area over the annotation image, respecting
            // the image’s alignment rect.
            NSImage *image = annotationImage.image;
            NSRect annotationRect = [self frameOfImage:image
                                  centeredAtCoordinate:annotation.coordinate];
            annotationRect = NSOffsetRect(image.alignmentRect, annotationRect.origin.x, annotationRect.origin.y);
            [self addCursorRect:annotationRect cursor:annotationImage.cursor];
        }
    }
}

#pragma mark Interface Builder methods

- (void)prepareForInterfaceBuilder {
    [super prepareForInterfaceBuilder];
    
    // Color the background a glorious Mapbox teal.
    self.layer.borderColor = [NSColor colorWithRed:59/255.
                                             green:178/255.
                                              blue:208/255.
                                             alpha:0.8].CGColor;
    self.layer.borderWidth = 2;
    self.layer.backgroundColor = [NSColor colorWithRed:59/255.
                                                 green:178/255.
                                                  blue:208/255.
                                                 alpha:0.6].CGColor;
    
    // Place a playful marker right smack dab in the middle.
    self.layer.contents = MGLDefaultMarkerImage();
    self.layer.contentsGravity = kCAGravityCenter;
    self.layer.contentsScale = [NSScreen mainScreen].backingScaleFactor;
}

#pragma mark Geometric methods

- (NSPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(nullable NSView *)view {
    return [self convertLatLng:MGLLatLngFromLocationCoordinate2D(coordinate) toPointToView:view];
}

/// Converts a geographic coordinate to a point in the view’s coordinate system.
- (NSPoint)convertLatLng:(mbgl::LatLng)latLng toPointToView:(nullable NSView *)view {
    mbgl::vec2<double> pixel = _mbglMap->pixelForLatLng(latLng);
    return [self convertPoint:NSMakePoint(pixel.x, pixel.y) toView:view];
}

- (CLLocationCoordinate2D)convertPoint:(NSPoint)point toCoordinateFromView:(nullable NSView *)view {
    return MGLLocationCoordinate2DFromLatLng([self convertPoint:point toLatLngFromView:view]);
}

/// Converts a point in the view’s coordinate system to a geographic coordinate.
- (mbgl::LatLng)convertPoint:(NSPoint)point toLatLngFromView:(nullable NSView *)view {
    NSPoint convertedPoint = [self convertPoint:point fromView:view];
    return _mbglMap->latLngForPixel(mbgl::PrecisionPoint(convertedPoint.x, convertedPoint.y));
}

- (NSRect)convertCoordinateBounds:(MGLCoordinateBounds)bounds toRectToView:(nullable NSView *)view {
    return [self convertLatLngBounds:MGLLatLngBoundsFromCoordinateBounds(bounds) toRectToView:view];
}

/// Converts a geographic bounding box to a rectangle in the view’s coordinate
/// system.
- (NSRect)convertLatLngBounds:(mbgl::LatLngBounds)bounds toRectToView:(nullable NSView *)view {
    NSRect rect = { [self convertLatLng:bounds.sw toPointToView:view], NSZeroSize };
    rect = MGLExtendRect(rect, [self convertLatLng:bounds.ne toPointToView:view]);
    return rect;
}

- (MGLCoordinateBounds)convertRect:(NSRect)rect toCoordinateBoundsFromView:(nullable NSView *)view {
    return MGLCoordinateBoundsFromLatLngBounds([self convertRect:rect toLatLngBoundsFromView:view]);
}

/// Converts a rectangle in the given view’s coordinate system to a geographic
/// bounding box.
- (mbgl::LatLngBounds)convertRect:(NSRect)rect toLatLngBoundsFromView:(nullable NSView *)view {
    mbgl::LatLngBounds bounds = mbgl::LatLngBounds::getExtendable();
    bounds.extend([self convertPoint:rect.origin toLatLngFromView:view]);
    bounds.extend([self convertPoint:{ NSMaxX(rect), NSMinY(rect) } toLatLngFromView:view]);
    bounds.extend([self convertPoint:{ NSMaxX(rect), NSMaxY(rect) } toLatLngFromView:view]);
    bounds.extend([self convertPoint:{ NSMinX(rect), NSMaxY(rect) } toLatLngFromView:view]);
    
    // The world is wrapping if a point just outside the bounds is also within
    // the rect.
    mbgl::LatLng outsideLatLng;
    if (bounds.sw.longitude > -180) {
        outsideLatLng = {
            (bounds.sw.latitude + bounds.ne.latitude) / 2,
            bounds.sw.longitude - 1,
        };
    } else if (bounds.ne.longitude < 180) {
        outsideLatLng = {
            (bounds.sw.latitude + bounds.ne.latitude) / 2,
            bounds.ne.longitude + 1,
        };
    }
    
    // If the world is wrapping, extend the bounds to cover all longitudes.
    if (NSPointInRect([self convertLatLng:outsideLatLng toPointToView:view], rect)) {
        bounds.sw.longitude = -180;
        bounds.ne.longitude = 180;
    }
    
    return bounds;
}

- (CLLocationDistance)metersPerPointAtLatitude:(CLLocationDegrees)latitude {
    return _mbglMap->getMetersPerPixelAtLatitude(latitude, self.zoomLevel);
}

#pragma mark Debugging

- (MGLMapDebugMaskOptions)debugMask {
    mbgl::MapDebugOptions options = _mbglMap->getDebug();
    MGLMapDebugMaskOptions mask = 0;
    if (options & mbgl::MapDebugOptions::TileBorders) {
        mask |= MGLMapDebugTileBoundariesMask;
    }
    if (options & mbgl::MapDebugOptions::ParseStatus) {
        mask |= MGLMapDebugTileInfoMask;
    }
    if (options & mbgl::MapDebugOptions::Timestamps) {
        mask |= MGLMapDebugTimestampsMask;
    }
    if (options & mbgl::MapDebugOptions::Collision) {
        mask |= MGLMapDebugCollisionBoxesMask;
    }
    return mask;
}

- (void)setDebugMask:(MGLMapDebugMaskOptions)debugMask {
    mbgl::MapDebugOptions options = mbgl::MapDebugOptions::NoDebug;
    if (debugMask & MGLMapDebugTileBoundariesMask) {
        options |= mbgl::MapDebugOptions::TileBorders;
    }
    if (debugMask & MGLMapDebugTileInfoMask) {
        options |= mbgl::MapDebugOptions::ParseStatus;
    }
    if (debugMask & MGLMapDebugTimestampsMask) {
        options |= mbgl::MapDebugOptions::Timestamps;
    }
    if (debugMask & MGLMapDebugCollisionBoxesMask) {
        options |= mbgl::MapDebugOptions::Collision;
    }
    _mbglMap->setDebug(options);
}

/// Adapter responsible for bridging calls from mbgl to MGLMapView and Cocoa.
class MGLMapViewImpl : public mbgl::View {
public:
    MGLMapViewImpl(MGLMapView *nativeView_, const float scaleFactor_)
        : nativeView(nativeView_), scaleFactor(scaleFactor_) {}
    virtual ~MGLMapViewImpl() {}
    
    
    float getPixelRatio() const override {
        return scaleFactor;
    }
    
    std::array<uint16_t, 2> getSize() const override {
        return {{ static_cast<uint16_t>(nativeView.bounds.size.width),
            static_cast<uint16_t>(nativeView.bounds.size.height) }};
    }
    
    std::array<uint16_t, 2> getFramebufferSize() const override {
        NSRect bounds = [nativeView convertRectToBacking:nativeView.bounds];
        return {{ static_cast<uint16_t>(bounds.size.width),
            static_cast<uint16_t>(bounds.size.height) }};
    }
    
    void notify() override {}
    
    void notifyMapChange(mbgl::MapChange change) override {
        assert([[NSThread currentThread] isMainThread]);
        [nativeView notifyMapChange:change];
    }
    
    void activate() override {
        MGLOpenGLLayer *layer = (MGLOpenGLLayer *)nativeView.layer;
        if ([NSOpenGLContext currentContext] != layer.openGLContext) {
            // Enable our OpenGL context on the Map thread.
            [layer.openGLContext makeCurrentContext];
            
            // Enable vertex buffer objects.
            mbgl::gl::InitializeExtensions([](const char *name) {
                static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
                if (!framework) {
                    throw std::runtime_error("Failed to load OpenGL framework.");
                }
                
                CFStringRef str = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
                void *symbol = CFBundleGetFunctionPointerForName(framework, str);
                CFRelease(str);
                
                return reinterpret_cast<mbgl::gl::glProc>(symbol);
            });
        }
    }
    
    void deactivate() override {
        [NSOpenGLContext clearCurrentContext];
    }
    
    void invalidate() override {
        [nativeView performSelectorOnMainThread:@selector(invalidate)
                                     withObject:nil
                                  waitUntilDone:NO];
    }
    
    void beforeRender() override {
        // This normally gets called right away by mbgl::Map, but only on the
        // main thread. OpenGL contexts and extensions are thread-local, so this
        // has to happen on the Map thread too.
        activate();
    }
    
    void afterRender() override {}
    
private:
    /// Cocoa map view that this adapter bridges to.
    __weak MGLMapView *nativeView = nullptr;
    
    /// Backing scale factor of the view.
    const float scaleFactor;
};

@end
