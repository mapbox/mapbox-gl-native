#import "MGLMapView_Private.hpp"
#import "MGLAnnotationImage_Private.h"
#import "MGLAttributionButton.h"
#import "MGLCompassCell.h"
#import "MGLOpenGLLayer.h"
#import "MGLStyle.h"

#import "MGLFeature_Private.h"
#import "MGLGeometry_Private.h"
#import "MGLMultiPoint_Private.h"
#import "MGLOfflineStorage_Private.h"
#import "MGLStyle_Private.hpp"

#import "MGLAccountManager.h"
#import "MGLMapCamera.h"
#import "MGLPolygon.h"
#import "MGLPolyline.h"
#import "MGLAnnotationImage.h"
#import "MGLMapViewDelegate.h"

#import <mbgl/mbgl.hpp>
#import <mbgl/annotation/annotation.hpp>
#import <mbgl/map/camera.hpp>
#import <mbgl/platform/darwin/reachability.h>
#import <mbgl/gl/gl.hpp>
#import <mbgl/sprite/sprite_image.hpp>
#import <mbgl/storage/default_file_source.hpp>
#import <mbgl/storage/network_status.hpp>
#import <mbgl/math/wrap.hpp>
#import <mbgl/util/constants.hpp>
#import <mbgl/util/chrono.hpp>

#import <map>
#import <unordered_set>

#import "NSBundle+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSException+MGLAdditions.h"
#import "NSString+MGLAdditions.h"

#import <QuartzCore/QuartzCore.h>

class MGLMapViewImpl;
class MGLAnnotationContext;

/// Distance from the edge of the view to ornament views (logo, attribution, etc.).
const CGFloat MGLOrnamentPadding = 12;

/// Alpha value of the ornament views (logo, attribution, etc.).
const CGFloat MGLOrnamentOpacity = 0.9;

/// Default duration for programmatic animations.
const NSTimeInterval MGLAnimationDuration = 0.3;

/// Distance in points that a single press of the panning keyboard shortcut pans the map by.
const CGFloat MGLKeyPanningIncrement = 150;

/// Degrees that a single press of the rotation keyboard shortcut rotates the map by.
const CLLocationDegrees MGLKeyRotationIncrement = 25;

/// Key for the user default that, when true, causes the map view to zoom in and out on scroll wheel events.
NSString * const MGLScrollWheelZoomsMapViewDefaultKey = @"MGLScrollWheelZoomsMapView";

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
    {
        .title = NSLocalizedStringWithDefaultValue(@"COPYRIGHT_MAPBOX", nil, nil, @"Mapbox", @"Linked part of copyright notice"),
        .urlString = NSLocalizedStringWithDefaultValue(@"COPYRIGHT_MAPBOX_LINK", nil, nil, @"https://www.mapbox.com/about/maps/", @"Copyright notice link"),
    },
    {
        .title = NSLocalizedStringWithDefaultValue(@"COPYRIGHT_OSM", nil, nil, @"OpenStreetMap", @"Linked part of copyright notice"),
        .urlString = NSLocalizedStringWithDefaultValue(@"COPYRIGHT_OSM_LINK", nil, nil, @"http://www.openstreetmap.org/about/", @"Copyright notice link"),
    },
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
mbgl::Duration MGLDurationInSeconds(NSTimeInterval duration) {
    return std::chrono::duration_cast<mbgl::Duration>(std::chrono::duration<NSTimeInterval>(duration));
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
        return { 0, 0, 0, 0 };
    }
    CGFloat r, g, b, a;
    [[color colorUsingColorSpaceName:NSCalibratedRGBColorSpace] getRed:&r green:&g blue:&b alpha:&a];
    return { (float)r, (float)g, (float)b, (float)a };
}

/// Lightweight container for metadata about an annotation, including the annotation itself.
class MGLAnnotationContext {
public:
    id <MGLAnnotation> annotation;
    /// The annotation’s image’s reuse identifier.
    NSString *imageReuseIdentifier;
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
    
    NSPanGestureRecognizer *_panGestureRecognizer;
    NSMagnificationGestureRecognizer *_magnificationGestureRecognizer;
    NSRotationGestureRecognizer *_rotationGestureRecognizer;
    double _scaleAtBeginningOfGesture;
    CLLocationDirection _directionAtBeginningOfGesture;
    CGFloat _pitchAtBeginningOfGesture;
    BOOL _didHideCursorDuringGesture;
    
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
    
    /// True if the view is currently printing itself.
    BOOL _isPrinting;
}

#pragma mark Lifecycle

+ (void)initialize {
    if (self == [MGLMapView class]) {
        [[NSUserDefaults standardUserDefaults] registerDefaults:@{
            MGLScrollWheelZoomsMapViewDefaultKey: @NO,
        }];
    }
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    if (self = [super initWithFrame:frameRect]) {
        [self commonInit];
        self.styleURL = nil;
    }
    return self;
}

- (instancetype)initWithFrame:(NSRect)frame styleURL:(nullable NSURL *)styleURL {
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
    return @[@"camera", @"debugMask"];
}

- (void)commonInit {
    _isTargetingInterfaceBuilder = NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent;
    
    // Set up cross-platform controllers and resources.
    _mbglView = new MGLMapViewImpl(self, [NSScreen mainScreen].backingScaleFactor);
    
    // Delete the pre-offline ambient cache at
    // ~/Library/Caches/com.mapbox.sdk.ios/cache.db.
    NSURL *cachesDirectoryURL = [[NSFileManager defaultManager] URLForDirectory:NSCachesDirectory
                                                                       inDomain:NSUserDomainMask
                                                              appropriateForURL:nil
                                                                         create:NO
                                                                          error:nil];
    cachesDirectoryURL = [cachesDirectoryURL URLByAppendingPathComponent:
                          [NSBundle mgl_frameworkBundle].bundleIdentifier];
    NSURL *legacyCacheURL = [cachesDirectoryURL URLByAppendingPathComponent:@"cache.db"];
    [[NSFileManager defaultManager] removeItemAtURL:legacyCacheURL error:NULL];
    
    mbgl::DefaultFileSource *mbglFileSource = [MGLOfflineStorage sharedOfflineStorage].mbglFileSource;
    _mbglMap = new mbgl::Map(*_mbglView, *mbglFileSource, mbgl::MapMode::Continuous, mbgl::GLContextMode::Unique, mbgl::ConstrainMode::None, mbgl::ViewportMode::Default);
    [self validateTileCacheSize];
    
    // Install the OpenGL layer. Interface Builder’s synchronous drawing means
    // we can’t display a map, so don’t even bother to have a map layer.
    self.layer = _isTargetingInterfaceBuilder ? [CALayer layer] : [MGLOpenGLLayer layer];
    
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
    self.automaticallyAdjustsContentInsets = YES;
    mbgl::CameraOptions options;
    options.center = mbgl::LatLng(0, 0);
    options.padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInsets);
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
    [_zoomControls setLabel:NSLocalizedStringWithDefaultValue(@"ZOOM_OUT_LABEL", nil, nil, @"−", @"Label of Zoom Out button; U+2212 MINUS SIGN") forSegment:0];
    [(NSSegmentedCell *)_zoomControls.cell setTag:0 forSegment:0];
    [(NSSegmentedCell *)_zoomControls.cell setToolTip:NSLocalizedStringWithDefaultValue(@"ZOOM_OUT_TOOLTIP", nil, nil, @"Zoom Out", @"Tooltip of Zoom Out button") forSegment:0];
    [_zoomControls setLabel:NSLocalizedStringWithDefaultValue(@"ZOOM_IN_LABEL", nil, nil, @"+", @"Label of Zoom In button") forSegment:1];
    [(NSSegmentedCell *)_zoomControls.cell setTag:1 forSegment:1];
    [(NSSegmentedCell *)_zoomControls.cell setToolTip:NSLocalizedStringWithDefaultValue(@"ZOOM_IN_TOOLTIP", nil, nil, @"Zoom In", @"Tooltip of Zoom In button") forSegment:1];
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
    _logoView.accessibilityTitle = NSLocalizedStringWithDefaultValue(@"MAP_A11Y_TITLE", nil, nil, @"Mapbox", @"Accessibility title");
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
    
    NSClickGestureRecognizer *rightClickGestureRecognizer = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(handleRightClickGesture:)];
    rightClickGestureRecognizer.buttonMask = 0x2;
    [self addGestureRecognizer:rightClickGestureRecognizer];
    
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
    [self.window removeObserver:self forKeyPath:@"contentLayoutRect"];
    [self.window removeObserver:self forKeyPath:@"titlebarAppearsTransparent"];
    
    // Close any annotation callout immediately.
    [self.calloutForSelectedAnnotation close];
    self.calloutForSelectedAnnotation = nil;
    
    // Removing the annotations unregisters any outstanding KVO observers.
    [self removeAnnotations:self.annotations];
    
    if (_mbglMap) {
        delete _mbglMap;
        _mbglMap = nullptr;
    }
    if (_mbglView) {
        delete _mbglView;
        _mbglView = nullptr;
    }
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(__unused NSDictionary *)change context:(void *)context {
    if ([keyPath isEqualToString:@"contentLayoutRect"] ||
        [keyPath isEqualToString:@"titlebarAppearsTransparent"]) {
        [self adjustContentInsets];
    } else if ([keyPath isEqualToString:@"coordinate"] &&
               [object conformsToProtocol:@protocol(MGLAnnotation)] &&
               ![object isKindOfClass:[MGLMultiPoint class]]) {
        id <MGLAnnotation> annotation = object;
        MGLAnnotationTag annotationTag = (MGLAnnotationTag)(NSUInteger)context;
        // We can get here because a subclass registered itself as an observer
        // of the coordinate key path of a non-multipoint annotation but failed
        // to handle the change. This check deters us from treating the
        // subclass’s context as an annotation tag. If the context happens to
        // match a valid annotation tag, the annotation will be unnecessarily
        // but safely updated.
        if (annotation == [self annotationWithTag:annotationTag]) {
            const mbgl::Point<double> point = MGLPointFromLocationCoordinate2D(annotation.coordinate);
            MGLAnnotationImage *annotationImage = [self imageOfAnnotationWithTag:annotationTag];
            _mbglMap->updateAnnotation(annotationTag, mbgl::SymbolAnnotation { point, annotationImage.styleIconIdentifier.UTF8String ?: "" });
            if (annotationTag == _selectedAnnotationTag) {
                [self deselectAnnotation:annotation];
            }
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
    
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundeclared-selector"
    if ([self.delegate respondsToSelector:@selector(mapView:regionWillChangeAnimated:)]) {
        NSLog(@"-mapView:regionWillChangeAnimated: is not supported by the macOS SDK, but %@ implements it anyways. "
              @"Please implement -[%@ mapView:cameraWillChangeAnimated:] instead.",
              NSStringFromClass([delegate class]), NSStringFromClass([delegate class]));
    }
    if ([self.delegate respondsToSelector:@selector(mapViewRegionIsChanging:)]) {
        NSLog(@"-mapViewRegionIsChanging: is not supported by the macOS SDK, but %@ implements it anyways. "
              @"Please implement -[%@ mapViewCameraIsChanging:] instead.",
              NSStringFromClass([delegate class]), NSStringFromClass([delegate class]));
    }
    if ([self.delegate respondsToSelector:@selector(mapView:regionDidChangeAnimated:)]) {
        NSLog(@"-mapView:regionDidChangeAnimated: is not supported by the macOS SDK, but %@ implements it anyways. "
              @"Please implement -[%@ mapView:cameraDidChangeAnimated:] instead.",
              NSStringFromClass([delegate class]), NSStringFromClass([delegate class]));
    }
#pragma clang diagnostic pop
}

#pragma mark Style

- (nonnull NSURL *)styleURL {
    NSString *styleURLString = @(_mbglMap->getStyleURL().c_str()).mgl_stringOrNilIfEmpty;
    return styleURLString ? [NSURL URLWithString:styleURLString] : [MGLStyle streetsStyleURLWithVersion:MGLStyleDefaultVersion];
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
        styleURL = [MGLStyle streetsStyleURLWithVersion:MGLStyleDefaultVersion];
    }
    
    if (![styleURL scheme]) {
        // Assume a relative path into the application’s resource folder.
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
    }
    
    [self.window removeObserver:self forKeyPath:@"contentLayoutRect"];
    [self.window removeObserver:self forKeyPath:@"titlebarAppearsTransparent"];
}

- (void)viewDidMoveToWindow {
    NSWindow *window = self.window;
    if (self.dormant && window) {
        self.dormant = NO;
    }
    
    if (window && _mbglMap->getConstrainMode() == mbgl::ConstrainMode::None) {
        _mbglMap->setConstrainMode(mbgl::ConstrainMode::HeightOnly);
    }
    
    [window addObserver:self
             forKeyPath:@"contentLayoutRect"
                options:NSKeyValueObservingOptionInitial
                context:NULL];
    [window addObserver:self
             forKeyPath:@"titlebarAppearsTransparent"
                options:NSKeyValueObservingOptionInitial
                context:NULL];
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
    if (!NSEqualRects(frame, self.frame)) {
        [self validateTileCacheSize];
    }
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

        _mbglMap->render();

        if (_isPrinting) {
            _isPrinting = NO;
            std::string png = encodePNG(_mbglView->readStillImage());
            NSData *data = [[NSData alloc] initWithBytes:png.data() length:png.size()];
            NSImage *image = [[NSImage alloc] initWithData:data];
            [self performSelector:@selector(printWithImage:) withObject:image afterDelay:0];
        }

//        [self updateUserLocationAnnotationView];
    }
}

- (void)validateTileCacheSize {
    if (!_mbglMap) {
        return;
    }
    
    CGFloat zoomFactor   = self.maximumZoomLevel - self.minimumZoomLevel + 1;
    CGFloat cpuFactor    = [NSProcessInfo processInfo].processorCount;
    CGFloat memoryFactor = (CGFloat)[NSProcessInfo processInfo].physicalMemory / 1000 / 1000 / 1000;
    CGFloat sizeFactor   = (NSWidth(self.bounds) / mbgl::util::tileSize) * (NSHeight(self.bounds) / mbgl::util::tileSize);
    
    NSUInteger cacheSize = zoomFactor * cpuFactor * memoryFactor * sizeFactor * 0.5;
    
    _mbglMap->setSourceTileCacheSize(cacheSize);
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

#pragma mark Printing

- (void)print:(__unused id)sender {
    _isPrinting = YES;
    [self invalidate];
}

- (void)printWithImage:(NSImage *)image {
    NSImageView *imageView = [[NSImageView alloc] initWithFrame:self.bounds];
    imageView.image = image;
    
    NSPrintOperation *op = [NSPrintOperation printOperationWithView:imageView];
    [op runOperation];
}

#pragma mark Viewport

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCenterCoordinate {
    return [NSSet setWithObjects:@"latitude", @"longitude", @"camera", nil];
}

- (CLLocationCoordinate2D)centerCoordinate {
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInsets);
    return MGLLocationCoordinate2DFromLatLng(_mbglMap->getLatLng(padding));
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate {
    [self setCenterCoordinate:centerCoordinate animated:NO];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate animated:(BOOL)animated {
    [self willChangeValueForKey:@"centerCoordinate"];
    _mbglMap->setLatLng(MGLLatLngFromLocationCoordinate2D(centerCoordinate),
                        MGLEdgeInsetsFromNSEdgeInsets(self.contentInsets),
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
    [self willChangeValueForKey:@"zoomLevel"];
    _mbglMap->setZoom(zoomLevel,
                      MGLEdgeInsetsFromNSEdgeInsets(self.contentInsets),
                      MGLDurationInSeconds(animated ? MGLAnimationDuration : 0));
    [self didChangeValueForKey:@"zoomLevel"];
}

- (void)zoomBy:(double)zoomDelta animated:(BOOL)animated {
    [self setZoomLevel:self.zoomLevel + zoomDelta animated:animated];
}

- (void)scaleBy:(double)scaleFactor atPoint:(NSPoint)point animated:(BOOL)animated {
    [self willChangeValueForKey:@"centerCoordinate"];
    [self willChangeValueForKey:@"zoomLevel"];
    mbgl::ScreenCoordinate center(point.x, self.bounds.size.height - point.y);
    _mbglMap->scaleBy(scaleFactor, center, MGLDurationInSeconds(animated ? MGLAnimationDuration : 0));
    [self didChangeValueForKey:@"zoomLevel"];
    [self didChangeValueForKey:@"centerCoordinate"];
}

- (void)setMinimumZoomLevel:(double)minimumZoomLevel
{
    _mbglMap->setMinZoom(minimumZoomLevel);
    [self validateTileCacheSize];
}

- (void)setMaximumZoomLevel:(double)maximumZoomLevel
{
    _mbglMap->setMaxZoom(maximumZoomLevel);
    [self validateTileCacheSize];
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
    _mbglMap->setBearing(direction,
                         MGLEdgeInsetsFromNSEdgeInsets(self.contentInsets),
                         MGLDurationInSeconds(animated ? MGLAnimationDuration : 0));
    [self didChangeValueForKey:@"direction"];
}

- (void)offsetDirectionBy:(CLLocationDegrees)delta animated:(BOOL)animated {
    [self setDirection:_mbglMap->getBearing() + delta animated:animated];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCamera {
    return [NSSet setWithObjects:@"latitude", @"longitude", @"centerCoordinate", @"zoomLevel", @"direction", nil];
}

- (MGLMapCamera *)camera {
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInsets);
    return [self cameraForCameraOptions:_mbglMap->getCameraOptions(padding)];
}

- (void)setCamera:(MGLMapCamera *)camera {
    [self setCamera:camera animated:NO];
}

- (void)setCamera:(MGLMapCamera *)camera animated:(BOOL)animated {
    [self setCamera:camera withDuration:animated ? MGLAnimationDuration : 0 animationTimingFunction:nil completionHandler:NULL];
}

- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion {
    _mbglMap->cancelTransitions();
    if ([self.camera isEqual:camera]) {
        return;
    }
    
    mbgl::CameraOptions cameraOptions = [self cameraOptionsObjectForAnimatingToCamera:camera];
    mbgl::AnimationOptions animationOptions;
    if (duration > 0) {
        animationOptions.duration.emplace(MGLDurationInSeconds(duration));
        animationOptions.easing.emplace(MGLUnitBezierForMediaTimingFunction(function));
    }
    if (completion) {
        animationOptions.transitionFinishFn = [completion]() {
            // Must run asynchronously after the transition is completely over.
            // Otherwise, a call to -setCamera: within the completion handler
            // would reenter the completion handler’s caller.
            dispatch_async(dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    
    [self willChangeValueForKey:@"camera"];
    _mbglMap->easeTo(cameraOptions, animationOptions);
    [self didChangeValueForKey:@"camera"];
}

- (void)flyToCamera:(MGLMapCamera *)camera completionHandler:(nullable void (^)(void))completion {
    [self flyToCamera:camera withDuration:-1 completionHandler:completion];
}

- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration completionHandler:(nullable void (^)(void))completion {
    [self flyToCamera:camera withDuration:duration peakAltitude:-1 completionHandler:completion];
}

- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration peakAltitude:(CLLocationDistance)peakAltitude completionHandler:(nullable void (^)(void))completion {
    _mbglMap->cancelTransitions();
    if ([self.camera isEqual:camera]) {
        return;
    }
    
    mbgl::CameraOptions cameraOptions = [self cameraOptionsObjectForAnimatingToCamera:camera];
    mbgl::AnimationOptions animationOptions;
    if (duration >= 0) {
        animationOptions.duration = MGLDurationInSeconds(duration);
    }
    if (peakAltitude >= 0) {
        CLLocationDegrees peakLatitude = (self.centerCoordinate.latitude + camera.centerCoordinate.latitude) / 2;
        CLLocationDegrees peakPitch = (self.camera.pitch + camera.pitch) / 2;
        animationOptions.minZoom = MGLZoomLevelForAltitude(peakAltitude, peakPitch,
                                                           peakLatitude, self.frame.size);
    }
    if (completion) {
        animationOptions.transitionFinishFn = [completion]() {
            // Must run asynchronously after the transition is completely over.
            // Otherwise, a call to -setCamera: within the completion handler
            // would reenter the completion handler’s caller.
            dispatch_async(dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    
    [self willChangeValueForKey:@"camera"];
    _mbglMap->flyTo(cameraOptions, animationOptions);
    [self didChangeValueForKey:@"camera"];
}

/// Returns a CameraOptions object that specifies parameters for animating to
/// the given camera.
- (mbgl::CameraOptions)cameraOptionsObjectForAnimatingToCamera:(MGLMapCamera *)camera {
    mbgl::CameraOptions options;
    options.center = MGLLatLngFromLocationCoordinate2D(camera.centerCoordinate);
    options.padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInsets);
    options.zoom = MGLZoomLevelForAltitude(camera.altitude, camera.pitch,
                                           camera.centerCoordinate.latitude,
                                           self.frame.size);
    if (camera.heading >= 0) {
        options.angle = MGLRadiansFromDegrees(-camera.heading);
    }
    if (camera.pitch >= 0) {
        options.pitch = MGLRadiansFromDegrees(camera.pitch);
    }
    return options;
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
    
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    padding += MGLEdgeInsetsFromNSEdgeInsets(self.contentInsets);
    mbgl::CameraOptions cameraOptions = _mbglMap->cameraForLatLngBounds(MGLLatLngBoundsFromCoordinateBounds(bounds), padding);
    mbgl::AnimationOptions animationOptions;
    if (animated) {
        animationOptions.duration = MGLDurationInSeconds(MGLAnimationDuration);
    }
    
    [self willChangeValueForKey:@"visibleCoordinateBounds"];
    animationOptions.transitionFinishFn = ^() {
        [self didChangeValueForKey:@"visibleCoordinateBounds"];
    };
    _mbglMap->easeTo(cameraOptions, animationOptions);
}

- (MGLMapCamera *)cameraThatFitsCoordinateBounds:(MGLCoordinateBounds)bounds {
    return [self cameraThatFitsCoordinateBounds:bounds edgePadding:NSEdgeInsetsZero];
}

- (MGLMapCamera *)cameraThatFitsCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(NSEdgeInsets)insets {
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    padding += MGLEdgeInsetsFromNSEdgeInsets(self.contentInsets);
    mbgl::CameraOptions cameraOptions = _mbglMap->cameraForLatLngBounds(MGLLatLngBoundsFromCoordinateBounds(bounds), padding);
    return [self cameraForCameraOptions:cameraOptions];
}

- (MGLMapCamera *)cameraForCameraOptions:(const mbgl::CameraOptions &)cameraOptions {
    CLLocationCoordinate2D centerCoordinate = MGLLocationCoordinate2DFromLatLng(cameraOptions.center ? *cameraOptions.center : _mbglMap->getLatLng());
    double zoomLevel = cameraOptions.zoom ? *cameraOptions.zoom : self.zoomLevel;
    CLLocationDirection direction = cameraOptions.angle ? -MGLDegreesFromRadians(*cameraOptions.angle) : self.direction;
    CGFloat pitch = cameraOptions.pitch ? MGLDegreesFromRadians(*cameraOptions.pitch) : _mbglMap->getPitch();
    CLLocationDistance altitude = MGLAltitudeForZoomLevel(zoomLevel, pitch,
                                                          centerCoordinate.latitude,
                                                          self.frame.size);
    return [MGLMapCamera cameraLookingAtCenterCoordinate:centerCoordinate
                                            fromDistance:altitude
                                                   pitch:pitch
                                                 heading:direction];
}

- (void)setAutomaticallyAdjustsContentInsets:(BOOL)automaticallyAdjustsContentInsets {
    _automaticallyAdjustsContentInsets = automaticallyAdjustsContentInsets;
    [self adjustContentInsets];
}

/// Updates `contentInsets` to reflect the current window geometry.
- (void)adjustContentInsets {
    if (!_automaticallyAdjustsContentInsets) {
        return;
    }
    
    NSEdgeInsets contentInsets = self.contentInsets;
    if ((self.window.styleMask & NSFullSizeContentViewWindowMask)
        && !self.window.titlebarAppearsTransparent) {
        NSRect contentLayoutRect = [self convertRect:self.window.contentLayoutRect fromView:nil];
        if (NSMaxX(contentLayoutRect) > 0 && NSMaxY(contentLayoutRect) > 0) {
            contentInsets = NSEdgeInsetsMake(NSHeight(self.bounds) - NSMaxY(contentLayoutRect),
                                             NSMinX(contentLayoutRect),
                                             NSMinY(contentLayoutRect),
                                             NSWidth(self.bounds) - NSMaxX(contentLayoutRect));
        }
    } else {
        contentInsets = NSEdgeInsetsZero;
    }
    
    self.contentInsets = contentInsets;
}

- (void)setContentInsets:(NSEdgeInsets)contentInsets {
    [self setContentInsets:contentInsets animated:NO];
}

- (void)setContentInsets:(NSEdgeInsets)contentInsets animated:(BOOL)animated {
    if (NSEdgeInsetsEqual(contentInsets, self.contentInsets)) {
        return;
    }
    
    // After adjusting the content insets, move the center coordinate from the
    // old frame of reference to the new one represented by the newly set
    // content insets.
    CLLocationCoordinate2D oldCenter = self.centerCoordinate;
    _contentInsets = contentInsets;
    [self setCenterCoordinate:oldCenter animated:animated];
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
    NSPoint startPoint = NSMakePoint(endPoint.x - delta.x, endPoint.y - delta.y);
    
    NSEventModifierFlags flags = [NSApp currentEvent].modifierFlags;
    if (gestureRecognizer.state == NSGestureRecognizerStateBegan) {
        [self.window invalidateCursorRectsForView:self];
        _mbglMap->setGestureInProgress(true);
        
        if (![self isPanningWithGesture]) {
            // Hide the cursor except when panning.
            CGDisplayHideCursor(kCGDirectMainDisplay);
            _didHideCursorDuringGesture = YES;
        }
    } else if (gestureRecognizer.state == NSGestureRecognizerStateEnded
               || gestureRecognizer.state == NSGestureRecognizerStateCancelled) {
        _mbglMap->setGestureInProgress(false);
        [self.window invalidateCursorRectsForView:self];
        
        if (_didHideCursorDuringGesture) {
            _didHideCursorDuringGesture = NO;
            // Move the cursor back to the start point and show it again, creating
            // the illusion that it has stayed in place during the entire gesture.
            CGPoint cursorPoint = [self convertPoint:startPoint toView:nil];
            cursorPoint = [self.window convertRectToScreen:{ startPoint, NSZeroSize }].origin;
            cursorPoint.y = [NSScreen mainScreen].frame.size.height - cursorPoint.y;
            CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, cursorPoint);
            CGDisplayShowCursor(kCGDirectMainDisplay);
        }
    }
    
    if (flags & NSShiftKeyMask) {
        // Shift-drag to zoom.
        if (!self.zoomEnabled) {
            return;
        }
        
        _mbglMap->cancelTransitions();
        
        if (gestureRecognizer.state == NSGestureRecognizerStateBegan) {
            _scaleAtBeginningOfGesture = _mbglMap->getScale();
        } else if (gestureRecognizer.state == NSGestureRecognizerStateChanged) {
            CGFloat newZoomLevel = log2f(_scaleAtBeginningOfGesture) - delta.y / 75;
            [self scaleBy:powf(2, newZoomLevel) / _mbglMap->getScale() atPoint:startPoint animated:NO];
        }
    } else if (flags & NSAlternateKeyMask) {
        // Option-drag to rotate and/or tilt.
        _mbglMap->cancelTransitions();
        
        if (gestureRecognizer.state == NSGestureRecognizerStateBegan) {
            _directionAtBeginningOfGesture = self.direction;
            _pitchAtBeginningOfGesture = _mbglMap->getPitch();
        } else if (gestureRecognizer.state == NSGestureRecognizerStateChanged) {
            mbgl::ScreenCoordinate center(startPoint.x, self.bounds.size.height - startPoint.y);
            if (self.rotateEnabled) {
                CLLocationDirection newDirection = _directionAtBeginningOfGesture - delta.x / 10;
                [self willChangeValueForKey:@"direction"];
                _mbglMap->setBearing(newDirection, center);
                [self didChangeValueForKey:@"direction"];
            }
            if (self.pitchEnabled) {
                _mbglMap->setPitch(_pitchAtBeginningOfGesture + delta.y / 5, center);
            }
        }
    } else if (self.scrollEnabled) {
        // Otherwise, drag to pan.
        _mbglMap->cancelTransitions();
        
        if (gestureRecognizer.state == NSGestureRecognizerStateChanged) {
            delta.y *= -1;
            [self offsetCenterCoordinateBy:delta animated:NO];
            [gestureRecognizer setTranslation:NSZeroPoint inView:nil];
        }
    }
}

/// Returns whether the user is panning using a gesture.
- (BOOL)isPanningWithGesture {
    NSGestureRecognizerState state = _panGestureRecognizer.state;
    NSEventModifierFlags flags = [NSApp currentEvent].modifierFlags;
    return ((state == NSGestureRecognizerStateBegan || state == NSGestureRecognizerStateChanged)
            && !(flags & NSShiftKeyMask || flags & NSAlternateKeyMask));
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
        mbgl::ScreenCoordinate center(zoomInPoint.x, self.bounds.size.height - zoomInPoint.y);
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
    if (gestureRecognizer.state != NSGestureRecognizerStateEnded
        || [self subviewContainingGesture:gestureRecognizer]) {
        return;
    }
    
    NSPoint gesturePoint = [gestureRecognizer locationInView:self];
    MGLAnnotationTag hitAnnotationTag = [self annotationTagAtPoint:gesturePoint persistingResults:YES];
    if (hitAnnotationTag != MGLAnnotationTagNotFound) {
        if (hitAnnotationTag != _selectedAnnotationTag) {
            id <MGLAnnotation> annotation = [self annotationWithTag:hitAnnotationTag];
            NSAssert(annotation, @"Cannot select nonexistent annotation with tag %u", hitAnnotationTag);
            [self selectAnnotation:annotation];
        }
    } else {
        [self deselectAnnotation:self.selectedAnnotation];
    }
}

/// Right-click to show the context menu.
- (void)handleRightClickGesture:(NSClickGestureRecognizer *)gestureRecognizer {
    NSMenu *menu = [self menuForEvent:[NSApp currentEvent]];
    if (menu) {
        [NSMenu popUpContextMenu:menu withEvent:[NSApp currentEvent] forView:self];
    }
}

/// Double-click or double-tap to zoom in.
- (void)handleDoubleClickGesture:(NSClickGestureRecognizer *)gestureRecognizer {
    if (!self.zoomEnabled || gestureRecognizer.state != NSGestureRecognizerStateEnded
        || [self subviewContainingGesture:gestureRecognizer]) {
        return;
    }
    
    _mbglMap->cancelTransitions();
    
    NSPoint gesturePoint = [gestureRecognizer locationInView:self];
    [self scaleBy:2 atPoint:gesturePoint animated:YES];
}

- (void)smartMagnifyWithEvent:(NSEvent *)event {
    if (!self.zoomEnabled) {
        return;
    }
    
    _mbglMap->cancelTransitions();
    
    // Tap with two fingers (“right-click”) to zoom out on mice but not trackpads.
    NSPoint gesturePoint = [self convertPoint:event.locationInWindow fromView:nil];
    [self scaleBy:0.5 atPoint:gesturePoint animated:YES];
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
        mbgl::ScreenCoordinate center(rotationPoint.x, self.bounds.size.height - rotationPoint.y);
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
    BOOL isScrollWheel = event.phase == NSEventPhaseNone && event.momentumPhase == NSEventPhaseNone && !event.hasPreciseScrollingDeltas;
    if (isScrollWheel || [[NSUserDefaults standardUserDefaults] boolForKey:MGLScrollWheelZoomsMapViewDefaultKey]) {
        // A traditional, vertical scroll wheel zooms instead of panning.
        if (self.zoomEnabled) {
            const double delta =
                event.scrollingDeltaY / ([event hasPreciseScrollingDeltas] ? 100 : 10);
            if (delta != 0) {
                double scale = 2.0 / (1.0 + std::exp(-std::abs(delta)));

                // Zooming out.
                if (delta < 0) {
                    scale = 1.0 / scale;
                }

                NSPoint gesturePoint = [self convertPoint:event.locationInWindow fromView:nil];
                [self scaleBy:scale atPoint:gesturePoint animated:NO];
            }
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

/// Returns the subview that the gesture is located in.
- (NSView *)subviewContainingGesture:(NSGestureRecognizer *)gestureRecognizer {
    if (NSPointInRect([gestureRecognizer locationInView:self.compass], self.compass.bounds)) {
        return self.compass;
    }
    if (NSPointInRect([gestureRecognizer locationInView:self.zoomControls], self.zoomControls.bounds)) {
        return self.zoomControls;
    }
    if (NSPointInRect([gestureRecognizer locationInView:self.attributionView], self.attributionView.bounds)) {
        return self.attributionView;
    }
    return nil;
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
        [self zoomBy:1 animated:YES];
    }
}

- (IBAction)moveToEndOfParagraph:(__unused id)sender {
    if (self.zoomEnabled) {
        [self zoomBy:-1 animated:YES];
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
    
    for (id <MGLAnnotation> annotation in annotations) {
        NSAssert([annotation conformsToProtocol:@protocol(MGLAnnotation)], @"Annotation does not conform to MGLAnnotation");
        
        if ([annotation isKindOfClass:[MGLMultiPoint class]]) {
            // Actual multipoints aren’t supported as annotations.
            if ([annotation isMemberOfClass:[MGLMultiPoint class]]
                || [annotation isMemberOfClass:[MGLMultiPointFeature class]]) {
                continue;
            }
            
            // The multipoint knows how to style itself (with the map view’s help).
            MGLMultiPoint *multiPoint = (MGLMultiPoint *)annotation;
            if (!multiPoint.pointCount) {
                continue;
            }

            MGLAnnotationTag annotationTag = _mbglMap->addAnnotation([multiPoint annotationObjectWithDelegate:self]);
            MGLAnnotationContext context;
            context.annotation = annotation;
            _annotationContextsByAnnotationTag[annotationTag] = context;
        } else if (![annotation isKindOfClass:[MGLMultiPolyline class]]
                   || ![annotation isKindOfClass:[MGLMultiPolygon class]]
                   || ![annotation isKindOfClass:[MGLShapeCollection class]]) {
            MGLAnnotationImage *annotationImage = nil;
            if (delegateHasImagesForAnnotations) {
                annotationImage = [self.delegate mapView:self imageForAnnotation:annotation];
            }
            if (!annotationImage) {
                annotationImage = [self dequeueReusableAnnotationImageWithIdentifier:MGLDefaultStyleMarkerSymbolName];
            }
            if (!annotationImage) {
                annotationImage = self.defaultAnnotationImage;
            }
            
            NSString *symbolName = annotationImage.styleIconIdentifier;
            if (!symbolName) {
                symbolName = [MGLAnnotationSpritePrefix stringByAppendingString:annotationImage.reuseIdentifier];
                annotationImage.styleIconIdentifier = symbolName;
            }
            
            if (!self.annotationImagesByIdentifier[annotationImage.reuseIdentifier]) {
                self.annotationImagesByIdentifier[annotationImage.reuseIdentifier] = annotationImage;
                [self installAnnotationImage:annotationImage];
            }

            MGLAnnotationTag annotationTag = _mbglMap->addAnnotation(mbgl::SymbolAnnotation {
                MGLPointFromLocationCoordinate2D(annotation.coordinate),
                symbolName.UTF8String ?: ""
            });

            MGLAnnotationContext context;
            context.annotation = annotation;
            context.imageReuseIdentifier = annotationImage.reuseIdentifier;
            _annotationContextsByAnnotationTag[annotationTag] = context;
            
            if ([annotation isKindOfClass:[NSObject class]]) {
                NSAssert(![annotation isKindOfClass:[MGLMultiPoint class]], @"Point annotation should not be MGLMultiPoint.");
                [(NSObject *)annotation addObserver:self forKeyPath:@"coordinate" options:0 context:(void *)(NSUInteger)annotationTag];
            }

            // Opt into potentially expensive tooltip tracking areas.
            if (annotation.toolTip.length) {
                _wantsToolTipRects = YES;
            }
        }
    }

    [self didChangeValueForKey:@"annotations"];
    
    [self updateAnnotationTrackingAreas];
}

/// Initializes and returns a default annotation image that depicts a round pin
/// rising from the center, with a shadow slightly below center. The alignment
/// rect therefore excludes the bottom half.
- (MGLAnnotationImage *)defaultAnnotationImage {
    NSImage *image = MGLDefaultMarkerImage();
    NSRect alignmentRect = image.alignmentRect;
    alignmentRect.origin.y = NSMidY(alignmentRect);
    alignmentRect.size.height /= 2;
    image.alignmentRect = alignmentRect;
    return [MGLAnnotationImage annotationImageWithImage:image
                                        reuseIdentifier:MGLDefaultStyleMarkerSymbolName];
}

/// Sends the raw pixel data of the annotation image’s image to mbgl and
/// calculates state needed for hit testing later.
- (void)installAnnotationImage:(MGLAnnotationImage *)annotationImage {
    NSString *iconIdentifier = annotationImage.styleIconIdentifier;
    self.annotationImagesByIdentifier[annotationImage.reuseIdentifier] = annotationImage;
    
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

    mbgl::PremultipliedImage cPremultipliedImage(rep.pixelsWide, rep.pixelsHigh);
    std::copy(rep.bitmapData, rep.bitmapData + cPremultipliedImage.size(), cPremultipliedImage.data.get());
    auto cSpriteImage = std::make_shared<mbgl::SpriteImage>(std::move(cPremultipliedImage),
                                                            (float)(rep.pixelsWide / size.width));
    _mbglMap->addAnnotationIcon(iconIdentifier.UTF8String, cSpriteImage);
    
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
    
    [self willChangeValueForKey:@"annotations"];

    for (id <MGLAnnotation> annotation in annotations) {
        NSAssert([annotation conformsToProtocol:@protocol(MGLAnnotation)], @"Annotation does not conform to MGLAnnotation");
        
        MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
        NSAssert(annotationTag != MGLAnnotationTagNotFound, @"No ID for annotation %@", annotation);

        if (annotationTag == _selectedAnnotationTag) {
            [self deselectAnnotation:annotation];
        }
        if (annotationTag == _lastSelectedAnnotationTag) {
            _lastSelectedAnnotationTag = MGLAnnotationTagNotFound;
        }
        
        _annotationContextsByAnnotationTag.erase(annotationTag);
        
        if ([annotation isKindOfClass:[NSObject class]] &&
            ![annotation isKindOfClass:[MGLMultiPoint class]]) {
            [(NSObject *)annotation removeObserver:self forKeyPath:@"coordinate" context:(void *)(NSUInteger)annotationTag];
        }

        _mbglMap->removeAnnotation(annotationTag);
    }
    
    [self didChangeValueForKey:@"annotations"];
    
    [self updateAnnotationTrackingAreas];
}

- (nullable MGLAnnotationImage *)dequeueReusableAnnotationImageWithIdentifier:(NSString *)identifier {
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
        auto end = std::remove_if(nearbyAnnotations.begin(), nearbyAnnotations.end(), [&](const MGLAnnotationTag annotationTag) {
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
        nearbyAnnotations.resize(std::distance(nearbyAnnotations.begin(), end));
    }
    
    MGLAnnotationTag hitAnnotationTag = MGLAnnotationTagNotFound;
    if (nearbyAnnotations.size()) {
        // The annotation tags need to be stable in order to compare them with
        // the remembered tags.
        std::sort(nearbyAnnotations.begin(), nearbyAnnotations.end());
        
        if (nearbyAnnotations == _annotationsNearbyLastClick) {
            // The first selection in the cycle should be the one nearest to the
            // click.
            CLLocationCoordinate2D currentCoordinate = [self convertPoint:point toCoordinateFromView:self];
            std::sort(nearbyAnnotations.begin(), nearbyAnnotations.end(), [&](const MGLAnnotationTag tagA, const MGLAnnotationTag tagB) {
                CLLocationCoordinate2D coordinateA = [[self annotationWithTag:tagA] coordinate];
                CLLocationCoordinate2D coordinateB = [[self annotationWithTag:tagB] coordinate];
                CLLocationDegrees distanceA = hypot(coordinateA.latitude - currentCoordinate.latitude,
                                                    coordinateA.longitude - currentCoordinate.longitude);
                CLLocationDegrees distanceB = hypot(coordinateB.latitude - currentCoordinate.latitude,
                                                    coordinateB.longitude - currentCoordinate.longitude);
                return distanceA < distanceB;
            });
            
            // The last time we persisted a set of annotations, we had the same
            // set of annotations as we do now. Cycle through them.
            if (_lastSelectedAnnotationTag == MGLAnnotationTagNotFound
                || _lastSelectedAnnotationTag == _annotationsNearbyLastClick.back()) {
                // Either no annotation is selected or the last annotation in
                // the set was selected. Wrap around to the first annotation in
                // the set.
                hitAnnotationTag = _annotationsNearbyLastClick.front();
            } else {
                auto result = std::find(_annotationsNearbyLastClick.begin(),
                                        _annotationsNearbyLastClick.end(),
                                        _lastSelectedAnnotationTag);
                if (result == _annotationsNearbyLastClick.end()) {
                    // An annotation from this set hasn’t been selected before.
                    // Select the first (nearest) one.
                    hitAnnotationTag = _annotationsNearbyLastClick.front();
                } else {
                    // Step to the next annotation in the set.
                    auto distance = std::distance(_annotationsNearbyLastClick.begin(), result);
                    hitAnnotationTag = _annotationsNearbyLastClick[distance + 1];
                }
            }
        } else {
            // Remember the nearby annotations for the next time this method is
            // called.
            if (persist) {
                _annotationsNearbyLastClick = nearbyAnnotations;
            }
            
            // Choose the first nearby annotation.
            if (nearbyAnnotations.size()) {
                hitAnnotationTag = nearbyAnnotations.front();
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

- (id <MGLAnnotation>)selectedAnnotation {
    if (!_annotationContextsByAnnotationTag.count(_selectedAnnotationTag)) {
        return nil;
    }
    MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(_selectedAnnotationTag);
    return annotationContext.annotation;
}

- (void)setSelectedAnnotation:(id <MGLAnnotation>)annotation {
    [self willChangeValueForKey:@"selectedAnnotations"];
    _selectedAnnotationTag = [self annotationTagForAnnotation:annotation];
    if (_selectedAnnotationTag != MGLAnnotationTagNotFound) {
        _lastSelectedAnnotationTag = _selectedAnnotationTag;
    }
    [self didChangeValueForKey:@"selectedAnnotations"];
}

- (NS_ARRAY_OF(id <MGLAnnotation>) *)selectedAnnotations {
    id <MGLAnnotation> selectedAnnotation = self.selectedAnnotation;
    return selectedAnnotation ? @[selectedAnnotation] : @[];
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
    
    self.selectedAnnotation = annotation;
    
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

- (void)showAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations animated:(BOOL)animated {
    CGFloat maximumPadding = 100;
    CGFloat yPadding = (NSHeight(self.bounds) / 5 <= maximumPadding) ? (NSHeight(self.bounds) / 5) : maximumPadding;
    CGFloat xPadding = (NSWidth(self.bounds) / 5 <= maximumPadding) ? (NSWidth(self.bounds) / 5) : maximumPadding;

    NSEdgeInsets edgeInsets = NSEdgeInsetsMake(yPadding, xPadding, yPadding, xPadding);

    [self showAnnotations:annotations edgePadding:edgeInsets animated:animated];
}

- (void)showAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations edgePadding:(NSEdgeInsets)insets animated:(BOOL)animated {
    if ( ! annotations || ! annotations.count) return;

    mbgl::LatLngBounds bounds = mbgl::LatLngBounds::empty();

    for (id <MGLAnnotation> annotation in annotations)
    {
        if ([annotation conformsToProtocol:@protocol(MGLOverlay)])
        {
            bounds.extend(MGLLatLngBoundsFromCoordinateBounds(((id <MGLOverlay>)annotation).overlayBounds));
        }
        else
        {
            bounds.extend(MGLLatLngFromLocationCoordinate2D(annotation.coordinate));
        }
    }

    [self setVisibleCoordinateBounds:MGLCoordinateBoundsFromLatLngBounds(bounds)
                         edgePadding:insets
                            animated:animated];
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
        image = [self dequeueReusableAnnotationImageWithIdentifier:MGLDefaultStyleMarkerSymbolName].image;
    }
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
    
    NSString *customSymbol = _annotationContextsByAnnotationTag.at(annotationTag).imageReuseIdentifier;
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

#pragma mark - Runtime styling

- (MGLStyle *)style
{
    MGLStyle *style = [[MGLStyle alloc] init];
    style.mapView = self;
    return style;
}

- (mbgl::Map *)mbglMap
{
    return _mbglMap;
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
                      : [NSColor selectedMenuItemColor]);
    return MGLColorObjectFromNSColor(color);
}

- (mbgl::Color)fillColorForPolygonAnnotation:(MGLPolygon *)annotation {
    NSColor *color = (_delegateHasFillColorsForShapeAnnotations
                      ? [self.delegate mapView:self fillColorForPolygonAnnotation:annotation]
                      : [NSColor selectedMenuItemColor]);
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
    if (annotation && [self.delegate respondsToSelector:@selector(mapView:didSelectAnnotation:)]) {
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
    NSAssert((NSUInteger)data < MGLAnnotationTagNotFound, @"Invalid annotation tag in tooltip rect user data.");
    MGLAnnotationTag annotationTag = (MGLAnnotationTag)MIN((NSUInteger)data, MGLAnnotationTagNotFound);
    id <MGLAnnotation> annotation = [self annotationWithTag:annotationTag];
    return annotation.toolTip;
}

- (void)resetCursorRects {
    // Drag to pan has a grabbing hand cursor.
    if ([self isPanningWithGesture]) {
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

#pragma mark Data

- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesAtPoint:(NSPoint)point {
    return [self visibleFeaturesAtPoint:point inStyleLayersWithIdentifiers:nil];
}

- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesAtPoint:(NSPoint)point inStyleLayersWithIdentifiers:(NS_SET_OF(NSString *) *)styleLayerIdentifiers {
    // Cocoa origin is at the lower-left corner.
    mbgl::ScreenCoordinate screenCoordinate = { point.x, NSHeight(self.bounds) - point.y };
    
    mbgl::optional<std::vector<std::string>> optionalLayerIDs;
    if (styleLayerIdentifiers) {
        __block std::vector<std::string> layerIDs;
        layerIDs.reserve(styleLayerIdentifiers.count);
        [styleLayerIdentifiers enumerateObjectsUsingBlock:^(NSString * _Nonnull identifier, BOOL * _Nonnull stop) {
            layerIDs.push_back(identifier.UTF8String);
        }];
        optionalLayerIDs = layerIDs;
    }
    
    std::vector<mbgl::Feature> features = _mbglMap->queryRenderedFeatures(screenCoordinate, optionalLayerIDs);
    return MGLFeaturesFromMBGLFeatures(features);
}

- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesInRect:(NSRect)rect {
    return [self visibleFeaturesInRect:rect inStyleLayersWithIdentifiers:nil];
}

- (NS_ARRAY_OF(id <MGLFeature>) *)visibleFeaturesInRect:(NSRect)rect inStyleLayersWithIdentifiers:(NS_SET_OF(NSString *) *)styleLayerIdentifiers {
    // Cocoa origin is at the lower-left corner.
    mbgl::ScreenBox screenBox = {
        { NSMinX(rect), NSHeight(self.bounds) - NSMaxY(rect) },
        { NSMaxX(rect), NSHeight(self.bounds) - NSMinY(rect) },
    };
    
    mbgl::optional<std::vector<std::string>> optionalLayerIDs;
    if (styleLayerIdentifiers) {
        __block std::vector<std::string> layerIDs;
        layerIDs.reserve(styleLayerIdentifiers.count);
        [styleLayerIdentifiers enumerateObjectsUsingBlock:^(NSString * _Nonnull identifier, BOOL * _Nonnull stop) {
            layerIDs.push_back(identifier.UTF8String);
        }];
        optionalLayerIDs = layerIDs;
    }
    
    std::vector<mbgl::Feature> features = _mbglMap->queryRenderedFeatures(screenBox, optionalLayerIDs);
    return MGLFeaturesFromMBGLFeatures(features);
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
    mbgl::ScreenCoordinate pixel = _mbglMap->pixelForLatLng(latLng);
    // Cocoa origin is at the lower-left corner.
    pixel.y = NSHeight(self.bounds) - pixel.y;
    return [self convertPoint:NSMakePoint(pixel.x, pixel.y) toView:view];
}

- (CLLocationCoordinate2D)convertPoint:(NSPoint)point toCoordinateFromView:(nullable NSView *)view {
    return MGLLocationCoordinate2DFromLatLng([self convertPoint:point toLatLngFromView:view]);
}

/// Converts a point in the view’s coordinate system to a geographic coordinate.
- (mbgl::LatLng)convertPoint:(NSPoint)point toLatLngFromView:(nullable NSView *)view {
    NSPoint convertedPoint = [self convertPoint:point fromView:view];
    return _mbglMap->latLngForPixel({
        convertedPoint.x,
        // mbgl origin is at the top-left corner.
        NSHeight(self.bounds) - convertedPoint.y,
    }).wrapped();
}

- (NSRect)convertCoordinateBounds:(MGLCoordinateBounds)bounds toRectToView:(nullable NSView *)view {
    return [self convertLatLngBounds:MGLLatLngBoundsFromCoordinateBounds(bounds) toRectToView:view];
}

/// Converts a geographic bounding box to a rectangle in the view’s coordinate
/// system.
- (NSRect)convertLatLngBounds:(mbgl::LatLngBounds)bounds toRectToView:(nullable NSView *)view {
    NSRect rect = { [self convertLatLng:bounds.southwest() toPointToView:view], NSZeroSize };
    rect = MGLExtendRect(rect, [self convertLatLng:bounds.northeast() toPointToView:view]);
    return rect;
}

- (MGLCoordinateBounds)convertRect:(NSRect)rect toCoordinateBoundsFromView:(nullable NSView *)view {
    return MGLCoordinateBoundsFromLatLngBounds([self convertRect:rect toLatLngBoundsFromView:view]);
}

/// Converts a rectangle in the given view’s coordinate system to a geographic
/// bounding box.
- (mbgl::LatLngBounds)convertRect:(NSRect)rect toLatLngBoundsFromView:(nullable NSView *)view {
    mbgl::LatLngBounds bounds = mbgl::LatLngBounds::empty();
    bounds.extend([self convertPoint:rect.origin toLatLngFromView:view]);
    bounds.extend([self convertPoint:{ NSMaxX(rect), NSMinY(rect) } toLatLngFromView:view]);
    bounds.extend([self convertPoint:{ NSMaxX(rect), NSMaxY(rect) } toLatLngFromView:view]);
    bounds.extend([self convertPoint:{ NSMinX(rect), NSMaxY(rect) } toLatLngFromView:view]);
    
    // The world is wrapping if a point just outside the bounds is also within
    // the rect.
    mbgl::LatLng outsideLatLng;
    if (bounds.west() > -180) {
        outsideLatLng = {
            (bounds.south() + bounds.north()) / 2,
            bounds.west() - 1,
        };
    } else if (bounds.northeast().longitude < 180) {
        outsideLatLng = {
            (bounds.south() + bounds.north()) / 2,
            bounds.east() + 1,
        };
    }
    
    // If the world is wrapping, extend the bounds to cover all longitudes.
    if (NSPointInRect([self convertLatLng:outsideLatLng toPointToView:view], rect)) {
        bounds.extend(mbgl::LatLng(bounds.south(), -180));
        bounds.extend(mbgl::LatLng(bounds.south(),  180));
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
    if (options & mbgl::MapDebugOptions::Overdraw) {
        mask |= MGLMapDebugOverdrawVisualizationMask;
    }
    if (options & mbgl::MapDebugOptions::StencilClip) {
        mask |= MGLMapDebugStencilBufferMask;
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
    if (debugMask & MGLMapDebugOverdrawVisualizationMask) {
        options |= mbgl::MapDebugOptions::Overdraw;
    }
    if (debugMask & MGLMapDebugStencilBufferMask) {
        options |= mbgl::MapDebugOptions::StencilClip;
    }
    _mbglMap->setDebug(options);
}

/// Adapter responsible for bridging calls from mbgl to MGLMapView and Cocoa.
class MGLMapViewImpl : public mbgl::View {
public:
    MGLMapViewImpl(MGLMapView *nativeView_, const float scaleFactor_)
        : nativeView(nativeView_), scaleFactor(scaleFactor_) {}

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

    void notifyMapChange(mbgl::MapChange change) override {
        [nativeView notifyMapChange:change];
    }

    void invalidate() override {
        [nativeView invalidate];
    }

    void activate() override {
        MGLOpenGLLayer *layer = (MGLOpenGLLayer *)nativeView.layer;
        [layer.openGLContext makeCurrentContext];
    }

    void deactivate() override {
        [NSOpenGLContext clearCurrentContext];
    }

    mbgl::PremultipliedImage readStillImage() override {
        auto size = getFramebufferSize();
        const unsigned int w = size[0];
        const unsigned int h = size[1];
        
        mbgl::PremultipliedImage image { w, h };
        MBGL_CHECK_ERROR(glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, image.data.get()));
        
        const size_t stride = image.stride();
        auto tmp = std::make_unique<uint8_t[]>(stride);
        uint8_t *rgba = image.data.get();
        for (int i = 0, j = h - 1; i < j; i++, j--) {
            std::memcpy(tmp.get(), rgba + i * stride, stride);
            std::memcpy(rgba + i * stride, rgba + j * stride, stride);
            std::memcpy(rgba + j * stride, tmp.get(), stride);
        }
        
        return image;
    }
    
private:
    /// Cocoa map view that this adapter bridges to.
    __weak MGLMapView *nativeView = nullptr;
    
    /// Backing scale factor of the view.
    const float scaleFactor;
};

@end
