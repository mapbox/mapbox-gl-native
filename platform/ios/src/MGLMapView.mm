#import "MGLMapView_Private.h"

#import <GLKit/GLKit.h>
#import <OpenGLES/EAGL.h>

#include <mbgl/map/map.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/storage/reachability.h>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/renderer/mode.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/math/wrap.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/shared_thread_pool.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/projection.hpp>

#import "Mapbox.h"
#import "MGLShape_Private.h"
#import "MGLFeature_Private.h"
#import "MGLGeometry_Private.h"
#import "MGLMultiPoint_Private.h"
#import "MGLOfflineStorage_Private.h"
#import "MGLVectorTileSource_Private.h"
#import "MGLFoundation_Private.h"
#import "MGLRendererFrontend.h"
#import "MGLRendererConfiguration.h"

#import "NSBundle+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "NSException+MGLAdditions.h"
#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSString+MGLAdditions.h"
#import "NSURL+MGLAdditions.h"
#import "UIDevice+MGLAdditions.h"
#import "UIImage+MGLAdditions.h"
#import "UIViewController+MGLAdditions.h"

#import "MGLFaux3DUserLocationAnnotationView.h"
#import "MGLUserLocationAnnotationView.h"
#import "MGLUserLocationAnnotationView_Private.h"
#import "MGLUserLocation_Private.h"
#import "MGLAnnotationImage_Private.h"
#import "MGLAnnotationView_Private.h"
#import "MGLScaleBar.h"
#import "MGLStyle_Private.h"
#import "MGLStyleLayer_Private.h"
#import "MGLMapboxEvents.h"
#import "MMEConstants.h"
#import "MGLSDKUpdateChecker.h"
#import "MGLCompactCalloutView.h"
#import "MGLAnnotationContainerView.h"
#import "MGLAnnotationContainerView_Private.h"
#import "MGLAttributionInfo_Private.h"
#import "MGLMapAccessibilityElement.h"
#import "MGLLocationManager_Private.h"
#import "MGLLoggingConfiguration_Private.h"

#include <algorithm>
#include <cstdlib>
#include <map>
#include <unordered_set>

class MBGLView;
class MGLAnnotationContext;

const MGLMapViewDecelerationRate MGLMapViewDecelerationRateNormal = UIScrollViewDecelerationRateNormal;
const MGLMapViewDecelerationRate MGLMapViewDecelerationRateFast = UIScrollViewDecelerationRateFast;
const MGLMapViewDecelerationRate MGLMapViewDecelerationRateImmediate = 0.0;

const MGLMapViewPreferredFramesPerSecond MGLMapViewPreferredFramesPerSecondDefault = -1;
const MGLMapViewPreferredFramesPerSecond MGLMapViewPreferredFramesPerSecondLowPower = 30;
const MGLMapViewPreferredFramesPerSecond MGLMapViewPreferredFramesPerSecondMaximum = 0;

const MGLExceptionName MGLMissingLocationServicesUsageDescriptionException = @"MGLMissingLocationServicesUsageDescriptionException";
const MGLExceptionName MGLUserLocationAnnotationTypeException = @"MGLUserLocationAnnotationTypeException";
const MGLExceptionName MGLResourceNotFoundException = @"MGLResourceNotFoundException";
const MGLExceptionName MGLUnderlyingMapUnavailableException = @"MGLUnderlyingMapUnavailableException";

/// Indicates the manner in which the map view is tracking the user location.
typedef NS_ENUM(NSUInteger, MGLUserTrackingState) {
    /// The map view is not yet tracking the user location.
    MGLUserTrackingStatePossible = 0,
    /// The map view has begun to move to the first reported user location.
    MGLUserTrackingStateBegan,
    /// The map view begins a significant transition.
    MGLUserTrackingStateBeginSignificantTransition,
    /// The map view has finished moving to the first reported user location.
    MGLUserTrackingStateChanged,
};

const NSTimeInterval MGLAnimationDuration = 0.3;

/// Duration of an animation due to a user location update, typically chosen to
/// match a typical interval between user location updates.
const NSTimeInterval MGLUserLocationAnimationDuration = 1.0;

/// Distance between the map view’s edge and that of the user location
/// annotation view.
const UIEdgeInsets MGLUserLocationAnnotationViewInset = UIEdgeInsetsMake(50, 0, 50, 0);

const CGSize MGLAnnotationUpdateViewportOutset = {150, 150};
const CGFloat MGLMinimumZoom = 3;

/// Minimum initial zoom level when entering user tracking mode.
const double MGLMinimumZoomLevelForUserTracking = 10.5;

/// Initial zoom level when entering user tracking mode from a low zoom level.
const double MGLDefaultZoomLevelForUserTracking = 14.0;

/// Tolerance for snapping to true north, measured in degrees in either direction.
const CLLocationDirection MGLToleranceForSnappingToNorth = 7;

/// Distance threshold to stop the camera while animating.
const CLLocationDistance MGLDistanceThresholdForCameraPause = 500;

/// Reuse identifier and file name of the default point annotation image.
static NSString * const MGLDefaultStyleMarkerSymbolName = @"default_marker";

/// Reuse identifier and file name of the invisible point annotation image used
/// by annotations that are visually backed by MGLAnnotationView objects
static NSString * const MGLInvisibleStyleMarkerSymbolName = @"invisible_marker";

/// Prefix that denotes a sprite installed by MGLMapView, to avoid collisions
/// with style-defined sprites.
NSString * const MGLAnnotationSpritePrefix = @"com.mapbox.sprites.";

/// Slop area around the hit testing point, allowing for imprecise annotation selection.
const CGFloat MGLAnnotationImagePaddingForHitTest = 5;

/// Distance from the callout’s anchor point to the annotation it points to.
const CGFloat MGLAnnotationImagePaddingForCallout = 1;

const CGSize MGLAnnotationAccessibilityElementMinimumSize = CGSizeMake(10, 10);

/// An indication that the requested annotation was not found or is nonexistent.
enum { MGLAnnotationTagNotFound = UINT32_MAX };

/// Mapping from an annotation tag to metadata about that annotation, including
/// the annotation itself.
typedef std::unordered_map<MGLAnnotationTag, MGLAnnotationContext> MGLAnnotationTagContextMap;

/// Mapping from an annotation object to an annotation tag.
typedef std::map<id<MGLAnnotation>, MGLAnnotationTag> MGLAnnotationObjectTagMap;

mbgl::util::UnitBezier MGLUnitBezierForMediaTimingFunction(CAMediaTimingFunction *function)
{
    if ( ! function)
    {
        function = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionDefault];
    }
    float p1[2], p2[2];
    [function getControlPointAtIndex:0 values:p1];
    [function getControlPointAtIndex:1 values:p2];
    return { p1[0], p1[1], p2[0], p2[1] };
}

/// Lightweight container for metadata about an annotation, including the annotation itself.
class MGLAnnotationContext {
public:
    id <MGLAnnotation> annotation;
    /// The annotation’s image’s reuse identifier.
    NSString *imageReuseIdentifier;
    MGLAnnotationAccessibilityElement *accessibilityElement;
    MGLAnnotationView *annotationView;
    NSString *viewReuseIdentifier;
};

#pragma mark - Private -

@interface MGLMapView () <UIGestureRecognizerDelegate,
                          GLKViewDelegate,
                          MGLLocationManagerDelegate,
                          MGLSMCalloutViewDelegate,
                          MGLCalloutViewDelegate,
                          MGLMultiPointDelegate,
                          MGLAnnotationImageDelegate>

@property (nonatomic) EAGLContext *context;
@property (nonatomic) GLKView *glView;
@property (nonatomic) UIImageView *glSnapshotView;

@property (nonatomic) NSMutableArray<NSLayoutConstraint *> *scaleBarConstraints;
@property (nonatomic, readwrite) MGLScaleBar *scaleBar;
@property (nonatomic, readwrite) UIImageView *compassView;
@property (nonatomic) NSMutableArray<NSLayoutConstraint *> *compassViewConstraints;
@property (nonatomic, readwrite) UIImageView *logoView;
@property (nonatomic) NSMutableArray<NSLayoutConstraint *> *logoViewConstraints;
@property (nonatomic, readwrite) UIButton *attributionButton;
@property (nonatomic) NSMutableArray<NSLayoutConstraint *> *attributionButtonConstraints;

@property (nonatomic, readwrite) MGLStyle *style;

@property (nonatomic) UITapGestureRecognizer *singleTapGestureRecognizer;
@property (nonatomic) UITapGestureRecognizer *doubleTap;
@property (nonatomic) UITapGestureRecognizer *twoFingerTap;
@property (nonatomic) UIPanGestureRecognizer *pan;
@property (nonatomic) UIPinchGestureRecognizer *pinch;
@property (nonatomic) UIRotationGestureRecognizer *rotate;
@property (nonatomic) UILongPressGestureRecognizer *quickZoom;
@property (nonatomic) UIPanGestureRecognizer *twoFingerDrag;

@property (nonatomic) UIInterfaceOrientation currentOrientation;
@property (nonatomic) UIInterfaceOrientationMask applicationSupportedInterfaceOrientations;

@property (nonatomic) MGLCameraChangeReason cameraChangeReasonBitmask;

/// Mapping from reusable identifiers to annotation images.
@property (nonatomic) NSMutableDictionary<NSString *, MGLAnnotationImage *> *annotationImagesByIdentifier;

/// Currently shown popover representing the selected annotation.
@property (nonatomic) UIView<MGLCalloutView> *calloutViewForSelectedAnnotation;

/// Anchor coordinate from which to present callout views (for example, for shapes this
/// could be the touch point rather than its centroid)
@property (nonatomic) CLLocationCoordinate2D anchorCoordinateForSelectedAnnotation;

@property (nonatomic) MGLUserLocationAnnotationView *userLocationAnnotationView;

/// Indicates how thoroughly the map view is tracking the user location.
@property (nonatomic) MGLUserTrackingState userTrackingState;
@property (nonatomic) CGFloat scale;
@property (nonatomic) CGFloat angle;
@property (nonatomic) CGFloat quickZoomStart;
@property (nonatomic, getter=isDormant) BOOL dormant;
@property (nonatomic, readonly, getter=isRotationAllowed) BOOL rotationAllowed;
@property (nonatomic) BOOL shouldTriggerHapticFeedbackForCompass;
@property (nonatomic) MGLMapViewProxyAccessibilityElement *mapViewProxyAccessibilityElement;
@property (nonatomic) MGLAnnotationContainerView *annotationContainerView;
@property (nonatomic) MGLUserLocation *userLocation;
@property (nonatomic) NSMutableDictionary<NSString *, NSMutableArray<MGLAnnotationView *> *> *annotationViewReuseQueueByIdentifier;

/// Experimental rendering performance measurement.
@property (nonatomic) BOOL experimental_enableFrameRateMeasurement;
@property (nonatomic) CGFloat averageFrameRate;
@property (nonatomic) CFTimeInterval frameTime;
@property (nonatomic) CFTimeInterval averageFrameTime;

/// Residual properties (saved on app termination)
@property (nonatomic) BOOL terminated;
@property (nonatomic, copy) MGLMapCamera *residualCamera;
@property (nonatomic) MGLMapDebugMaskOptions residualDebugMask;
@property (nonatomic, copy) NSURL *residualStyleURL;

- (mbgl::Map &)mbglMap;

@end

@implementation MGLMapView
{
    mbgl::Map *_mbglMap;
    MBGLView *_mbglView;
    std::unique_ptr<MGLRenderFrontend> _rendererFrontend;
    
    std::shared_ptr<mbgl::ThreadPool> _mbglThreadPool;

    BOOL _opaque;

    MGLAnnotationTagContextMap _annotationContextsByAnnotationTag;
    MGLAnnotationObjectTagMap _annotationTagsByAnnotation;

    /// Tag of the selected annotation. If the user location annotation is selected, this ivar is set to `MGLAnnotationTagNotFound`.
    MGLAnnotationTag _selectedAnnotationTag;

    BOOL _userLocationAnnotationIsSelected;
    /// Size of the rectangle formed by unioning the maximum slop area around every annotation image and annotation image view.
    CGSize _unionedAnnotationRepresentationSize;
    CGSize _largestAnnotationViewSize;
    std::vector<MGLAnnotationTag> _annotationsNearbyLastTap;
    CGPoint _initialImplicitCalloutViewOffset;
    NSDate *_userLocationAnimationCompletionDate;
    /// True if a willChange notification has been issued for shape annotation layers and a didChange notification is pending.
    BOOL _isChangingAnnotationLayers;
    BOOL _isWaitingForRedundantReachableNotification;
    BOOL _isTargetingInterfaceBuilder;

    CLLocationDegrees _pendingLatitude;
    CLLocationDegrees _pendingLongitude;

    CADisplayLink *_displayLink;
    BOOL _needsDisplayRefresh;

    NSInteger _changeDelimiterSuppressionDepth;

    /// Center coordinate of the pinch gesture on the previous iteration of the gesture.
    CLLocationCoordinate2D _previousPinchCenterCoordinate;
    NSUInteger _previousPinchNumberOfTouches;
    
    CLLocationDistance _distanceFromOldUserLocation;

    BOOL _delegateHasAlphasForShapeAnnotations;
    BOOL _delegateHasStrokeColorsForShapeAnnotations;
    BOOL _delegateHasFillColorsForShapeAnnotations;
    BOOL _delegateHasLineWidthsForShapeAnnotations;

    MGLCompassDirectionFormatter *_accessibilityCompassFormatter;
    NSArray<id <MGLFeature>> *_visiblePlaceFeatures;
    NSArray<id <MGLFeature>> *_visibleRoadFeatures;
    NSMutableSet<MGLFeatureAccessibilityElement *> *_featureAccessibilityElements;
    BOOL _accessibilityValueAnnouncementIsPending;

    MGLReachability *_reachability;

    /// Experimental rendering performance measurement.
    CFTimeInterval _frameCounterStartTime;
    NSInteger _frameCount;
    CFTimeInterval _frameDurations;
}

#pragma mark - Setup & Teardown -

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        MGLLogInfo(@"Starting %@ initialization.", NSStringFromClass([self class]));
        MGLLogDebug(@"Initializing frame: %@", NSStringFromCGRect(frame));
        [self commonInit];
        self.styleURL = nil;
        MGLLogInfo(@"Finalizing %@ initialization.", NSStringFromClass([self class]));
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame styleURL:(nullable NSURL *)styleURL
{
    if (self = [super initWithFrame:frame])
    {
        MGLLogInfo(@"Starting %@ initialization.", NSStringFromClass([self class]));
        MGLLogDebug(@"Initializing frame: %@ styleURL: %@", NSStringFromCGRect(frame), styleURL);
        [self commonInit];
        self.styleURL = styleURL;
        MGLLogInfo(@"Finalizing %@ initialization.", NSStringFromClass([self class]));
    }
    return self;
}

- (instancetype)initWithCoder:(nonnull NSCoder *)decoder
{
    if (self = [super initWithCoder:decoder])
    {
        MGLLogInfo(@"Starting %@ initialization.", NSStringFromClass([self class]));
        [self commonInit];
        self.styleURL = nil;
        MGLLogInfo(@"Finalizing %@ initialization.", NSStringFromClass([self class]));
    }
    return self;
}

+ (void)initialize
{
    if (self == [MGLMapView class])
    {
        [MGLSDKUpdateChecker checkForUpdates];
    }
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingStyle
{
    return [NSSet setWithObject:@"styleURL"];
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingStyleURL
{
    return [NSSet setWithObjects:@"styleURL__", nil];
}

- (nonnull NSURL *)styleURL
{
    if (!_mbglMap)
    {
        NSAssert(self.terminated, @"_mbglMap should only be unavailable during app termination");
        return self.residualStyleURL;
    }

    NSString *styleURLString = @(self.mbglMap.getStyle().getURL().c_str()).mgl_stringOrNilIfEmpty;
    MGLAssert(styleURLString || _isTargetingInterfaceBuilder, @"Invalid style URL string %@", styleURLString);
    return styleURLString ? [NSURL URLWithString:styleURLString] : nil;
}

- (void)setStyleURL:(nullable NSURL *)styleURL
{
    if (_isTargetingInterfaceBuilder) return;

    if ( ! styleURL)
    {
        styleURL = [MGLStyle streetsStyleURLWithVersion:MGLStyleDefaultVersion];
    }
    MGLLogDebug(@"Setting styleURL: %@", styleURL);
    styleURL = styleURL.mgl_URLByStandardizingScheme;
    self.style = nil;
    self.mbglMap.getStyle().loadURL([[styleURL absoluteString] UTF8String]);
}

- (IBAction)reloadStyle:(__unused id)sender {
    MGLLogInfo(@"Reloading style.");
    NSURL *styleURL = self.styleURL;
    self.mbglMap.getStyle().loadURL("");
    self.styleURL = styleURL;
}

- (mbgl::Map &)mbglMap
{
    if (!_mbglMap)
    {
        [NSException raise:MGLUnderlyingMapUnavailableException
                    format:@"The underlying map is not available - this happens during app termination"];
    }
    return *_mbglMap;
}

- (mbgl::Renderer *)renderer
{
    return _rendererFrontend->getRenderer();
}

- (void)commonInit
{
    _isTargetingInterfaceBuilder = NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent;
    _opaque = NO;

    BOOL background = [UIApplication sharedApplication].applicationState == UIApplicationStateBackground;
    if (!background)
    {
        [self createGLView];
    }

    // setup accessibility
    //
//    self.isAccessibilityElement = YES;
    self.accessibilityLabel = NSLocalizedStringWithDefaultValue(@"MAP_A11Y_LABEL", nil, nil, @"Map", @"Accessibility label");
    self.accessibilityTraits = UIAccessibilityTraitAllowsDirectInteraction | UIAccessibilityTraitAdjustable;
    _accessibilityCompassFormatter = [[MGLCompassDirectionFormatter alloc] init];
    _accessibilityCompassFormatter.unitStyle = NSFormattingUnitStyleLong;
    self.backgroundColor = [UIColor clearColor];
    self.clipsToBounds = YES;
    if (@available(iOS 11.0, *)) { self.accessibilityIgnoresInvertColors = YES; }

    self.preferredFramesPerSecond = MGLMapViewPreferredFramesPerSecondDefault;

    // setup mbgl view
    _mbglView = new MBGLView(self);

    // Delete the pre-offline ambient cache at ~/Library/Caches/cache.db.
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *fileCachePath = [paths.firstObject stringByAppendingPathComponent:@"cache.db"];
    [[NSFileManager defaultManager] removeItemAtPath:fileCachePath error:NULL];

    // setup mbgl map
    MGLRendererConfiguration *config = [MGLRendererConfiguration currentConfiguration];
    _mbglThreadPool = mbgl::sharedThreadPool();

    auto renderer = std::make_unique<mbgl::Renderer>(*_mbglView, config.scaleFactor, *config.fileSource, *_mbglThreadPool, config.contextMode, config.cacheDir, config.localFontFamilyName);
    BOOL enableCrossSourceCollisions = !config.perSourceCollisions;
    _rendererFrontend = std::make_unique<MGLRenderFrontend>(std::move(renderer), self, *_mbglView);
    
    NSAssert(!_mbglMap, @"_mbglMap should be NULL");
    _mbglMap = new mbgl::Map(*_rendererFrontend, *_mbglView, self.size, config.scaleFactor, *[config fileSource], *_mbglThreadPool, mbgl::MapMode::Continuous, mbgl::ConstrainMode::None, mbgl::ViewportMode::Default, enableCrossSourceCollisions);

    // start paused if in IB
    if (_isTargetingInterfaceBuilder || background) {
        self.dormant = YES;
    }

    // Notify map object when network reachability status changes.
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(reachabilityChanged:)
                                                 name:kMGLReachabilityChangedNotification
                                               object:nil];

    _reachability = [MGLReachability reachabilityForInternetConnection];
    if ([_reachability isReachable])
    {
        _isWaitingForRedundantReachableNotification = YES;
    }
    [_reachability startNotifier];
    
    // setup default location manager
    self.locationManager = nil;

    // Set up annotation management and selection state.
    _annotationImagesByIdentifier = [NSMutableDictionary dictionary];
    _annotationContextsByAnnotationTag = {};
    _annotationTagsByAnnotation = {};
    _annotationViewReuseQueueByIdentifier = [NSMutableDictionary dictionary];
    _selectedAnnotationTag = MGLAnnotationTagNotFound;
    _annotationsNearbyLastTap = {};

    // setup logo
    //
    UIImage *logo = [MGLMapView resourceImageNamed:@"mapbox"];
    _logoView = [[UIImageView alloc] initWithImage:logo];
    _logoView.accessibilityTraits = UIAccessibilityTraitStaticText;
    _logoView.accessibilityLabel = NSLocalizedStringWithDefaultValue(@"LOGO_A11Y_LABEL", nil, nil, @"Mapbox", @"Accessibility label");
    _logoView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_logoView];
    _logoViewConstraints = [NSMutableArray array];

    // setup attribution
    //
    _attributionButton = [UIButton buttonWithType:UIButtonTypeInfoLight];
    _attributionButton.accessibilityLabel = NSLocalizedStringWithDefaultValue(@"INFO_A11Y_LABEL", nil, nil, @"About this map", @"Accessibility label");
    _attributionButton.accessibilityHint = NSLocalizedStringWithDefaultValue(@"INFO_A11Y_HINT", nil, nil, @"Shows credits, a feedback form, and more", @"Accessibility hint");
    [_attributionButton addTarget:self action:@selector(showAttribution:) forControlEvents:UIControlEventTouchUpInside];
    _attributionButton.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_attributionButton];
    _attributionButtonConstraints = [NSMutableArray array];
    [_attributionButton addObserver:self forKeyPath:@"hidden" options:NSKeyValueObservingOptionNew context:NULL];

    UILongPressGestureRecognizer *attributionLongPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(showAttribution:)];
    [_attributionButton addGestureRecognizer:attributionLongPress];

    // setup compass
    //
    _compassView = [[UIImageView alloc] initWithImage:self.compassImage];
    _compassView.alpha = 0;
    _compassView.userInteractionEnabled = YES;
    [_compassView addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleCompassTapGesture:)]];
    _compassView.accessibilityTraits = UIAccessibilityTraitButton;
    _compassView.accessibilityLabel = NSLocalizedStringWithDefaultValue(@"COMPASS_A11Y_LABEL", nil, nil, @"Compass", @"Accessibility label");
    _compassView.accessibilityHint = NSLocalizedStringWithDefaultValue(@"COMPASS_A11Y_HINT", nil, nil, @"Rotates the map to face due north", @"Accessibility hint");
    _compassView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_compassView];
    _compassViewConstraints = [NSMutableArray array];
    
    // setup scale control
    //
    _scaleBar = [[MGLScaleBar alloc] init];
    _scaleBar.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_scaleBar];
    _scaleBarConstraints = [NSMutableArray array];
    
    // setup interaction
    //
    _pan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
    _pan.delegate = self;
    _pan.maximumNumberOfTouches = 1;
    [self addGestureRecognizer:_pan];
    _scrollEnabled = YES;

    _pinch = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)];
    _pinch.delegate = self;
    [self addGestureRecognizer:_pinch];
    _zoomEnabled = YES;

    _rotate = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotateGesture:)];
    _rotate.delegate = self;
    [self addGestureRecognizer:_rotate];
    _rotateEnabled = YES;

    _doubleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleTapGesture:)];
    _doubleTap.numberOfTapsRequired = 2;
    [self addGestureRecognizer:_doubleTap];

    _twoFingerDrag = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleTwoFingerDragGesture:)];
    _twoFingerDrag.minimumNumberOfTouches = 2;
    _twoFingerDrag.maximumNumberOfTouches = 2;
    _twoFingerDrag.delegate = self;
    [_twoFingerDrag requireGestureRecognizerToFail:_pan];
    [self addGestureRecognizer:_twoFingerDrag];
    _pitchEnabled = YES;

    _twoFingerTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTwoFingerTapGesture:)];
    _twoFingerTap.numberOfTouchesRequired = 2;
    [_twoFingerTap requireGestureRecognizerToFail:_pinch];
    [_twoFingerTap requireGestureRecognizerToFail:_rotate];
    [_twoFingerTap requireGestureRecognizerToFail:_twoFingerDrag];
    [self addGestureRecognizer:_twoFingerTap];

    _hapticFeedbackEnabled = YES;

    _decelerationRate = MGLMapViewDecelerationRateNormal;

    _quickZoom = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleQuickZoomGesture:)];
    _quickZoom.numberOfTapsRequired = 1;
    _quickZoom.minimumPressDuration = 0;
    [_quickZoom requireGestureRecognizerToFail:_doubleTap];
    [self addGestureRecognizer:_quickZoom];

    _singleTapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTapGesture:)];
    [_singleTapGestureRecognizer requireGestureRecognizerToFail:_doubleTap];
    _singleTapGestureRecognizer.delegate = self;
    [_singleTapGestureRecognizer requireGestureRecognizerToFail:_quickZoom];
    [self addGestureRecognizer:_singleTapGestureRecognizer];

    // observe app activity
    //
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willTerminate) name:UIApplicationWillTerminateNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sleepGL:) name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(wakeGL:) name:UIApplicationWillEnterForegroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(wakeGL:) name:UIApplicationDidBecomeActiveNotification object:nil];
    // As of 3.7.5, we intentionally do not listen for `UIApplicationWillResignActiveNotification` or call `sleepGL:` in response to it, as doing
    // so causes a loop when asking for location permission. See: https://github.com/mapbox/mapbox-gl-native/issues/11225

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveMemoryWarning) name:UIApplicationDidReceiveMemoryWarningNotification object:nil];

    // Device orientation management
    self.currentOrientation = UIInterfaceOrientationUnknown;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(deviceOrientationDidChange:) name:UIDeviceOrientationDidChangeNotification object:nil];
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];

    // set initial position
    //
    mbgl::CameraOptions options;
    options.center = mbgl::LatLng(0, 0);
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    options.padding = padding;
    options.zoom = 0;

    _cameraChangeReasonBitmask = MGLCameraChangeReasonNone;

    _mbglMap->jumpTo(options);
    _pendingLatitude = NAN;
    _pendingLongitude = NAN;
    _targetCoordinate = kCLLocationCoordinate2DInvalid;

    if ([UIApplication sharedApplication].applicationState != UIApplicationStateBackground) {
        [MGLMapboxEvents pushTurnstileEvent];
        [MGLMapboxEvents pushEvent:MMEEventTypeMapLoad withAttributes:@{}];
    }

}

- (mbgl::Size)size
{
    // check for minimum texture size supported by OpenGL ES 2.0
    //
    CGSize size = CGSizeMake(MAX(self.bounds.size.width, 64), MAX(self.bounds.size.height, 64));
    return { static_cast<uint32_t>(size.width),
             static_cast<uint32_t>(size.height) };
}

- (mbgl::Size)framebufferSize
{
    return { static_cast<uint32_t>(self.glView.drawableWidth),
             static_cast<uint32_t>(self.glView.drawableHeight) };
}

- (void)createGLView
{
    if (_context) return;

    // create context
    //
    _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    MGLAssert(_context, @"Failed to create OpenGL ES context.");

    // create GL view
    //
    _glView = [[GLKView alloc] initWithFrame:self.bounds context:_context];
    _glView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    _glView.enableSetNeedsDisplay = NO;
    _glView.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    _glView.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    _glView.contentScaleFactor = [UIScreen instancesRespondToSelector:@selector(nativeScale)] ? [[UIScreen mainScreen] nativeScale] : [[UIScreen mainScreen] scale];
    _glView.layer.opaque = _opaque;
    _glView.delegate = self;

    CAEAGLLayer *eaglLayer = MGL_OBJC_DYNAMIC_CAST(_glView.layer, CAEAGLLayer);
    eaglLayer.presentsWithTransaction = YES;
    
    [_glView bindDrawable];
    [self insertSubview:_glView atIndex:0];
    _glView.contentMode = UIViewContentModeCenter;
}

- (UIImage *)compassImage
{
    UIImage *scaleImage = [MGLMapView resourceImageNamed:@"Compass"];
    UIGraphicsBeginImageContextWithOptions(scaleImage.size, NO, [UIScreen mainScreen].scale);
    [scaleImage drawInRect:{ CGPointZero, scaleImage.size }];

    NSAttributedString *north = [[NSAttributedString alloc] initWithString:NSLocalizedStringWithDefaultValue(@"COMPASS_NORTH", nil, nil, @"N", @"Compass abbreviation for north") attributes:@{
        NSFontAttributeName: [UIFont systemFontOfSize:11 weight:UIFontWeightUltraLight],
        NSForegroundColorAttributeName: [UIColor whiteColor],
    }];
    CGRect stringRect = CGRectMake((scaleImage.size.width - north.size.width) / 2,
                                   scaleImage.size.height * 0.435,
                                   north.size.width, north.size.height);
    [north drawInRect:stringRect];

    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

- (void)reachabilityChanged:(NSNotification *)notification
{
    MGLAssertIsMainThread();

    MGLReachability *reachability = [notification object];
    if ( ! _isWaitingForRedundantReachableNotification && [reachability isReachable])
    {
        mbgl::NetworkStatus::Reachable();
    }
    _isWaitingForRedundantReachableNotification = NO;
}


- (void)destroyCoreObjects {
    // Record the current state. Currently only saving a limited set of properties.
    self.terminated = YES;
    self.residualCamera = self.camera;
    self.residualDebugMask = self.debugMask;
    self.residualStyleURL = self.styleURL;
    
    // Tear down C++ objects, insuring worker threads correctly terminate.
    // Because of how _mbglMap is constructed, we need to destroy it first.
    delete _mbglMap;
    _mbglMap = nullptr;

    delete _mbglView;
    _mbglView = nullptr;

    _rendererFrontend.reset();
    _mbglThreadPool.reset();
}

- (void)dealloc
{
    MGLLogInfo(@"Deallocating MGLMapView.");
    [_reachability stopNotifier];

    [[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [_attributionButton removeObserver:self forKeyPath:@"hidden"];

    // Removing the annotations unregisters any outstanding KVO observers.
    NSArray *annotations = self.annotations;
    if (annotations)
    {
        [self removeAnnotations:annotations];
    }

    [self validateDisplayLink];

    [self destroyCoreObjects];

    if ([[EAGLContext currentContext] isEqual:_context])
    {
        [EAGLContext setCurrentContext:nil];
    }
    
    [self.compassViewConstraints removeAllObjects];
    self.compassViewConstraints = nil;
    
    [self.scaleBarConstraints removeAllObjects];
    self.scaleBarConstraints = nil;
    
    [self.logoViewConstraints removeAllObjects];
    self.logoViewConstraints = nil;
    
    [self.attributionButtonConstraints removeAllObjects];
    self.attributionButtonConstraints = nil;
    
    [_locationManager stopUpdatingLocation];
    [_locationManager stopUpdatingHeading];
    _locationManager.delegate = nil;
}

- (void)setDelegate:(nullable id<MGLMapViewDelegate>)delegate
{
    MGLLogDebug(@"Setting delegate: %@", delegate);
    if (_delegate == delegate) return;

    _delegate = delegate;

    _delegateHasAlphasForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:alphaForShapeAnnotation:)];
    _delegateHasStrokeColorsForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:strokeColorForShapeAnnotation:)];
    _delegateHasFillColorsForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:fillColorForPolygonAnnotation:)];
    _delegateHasLineWidthsForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:lineWidthForPolylineAnnotation:)];
}

- (void)didReceiveMemoryWarning
{
    MGLAssertIsMainThread();

    _rendererFrontend->reduceMemoryUse();
}

#pragma mark - Layout -

+ (BOOL)requiresConstraintBasedLayout
{
    return YES;
}

- (UIViewController *)viewControllerForLayoutGuides
{
    // Per -[UIResponder nextResponder] documentation, a UIView’s next responder
    // is its managing UIViewController if applicable, or otherwise its
    // superview. UIWindow’s next responder is UIApplication, which has no next
    // responder.
    UIResponder *laterResponder = self;
    while ([laterResponder isKindOfClass:[UIView class]])
    {
        laterResponder = laterResponder.nextResponder;
    }
    if ([laterResponder isKindOfClass:[UIViewController class]])
    {
        return (UIViewController *)laterResponder;
    }
    return nil;
}

- (void)updateConstraintsPreiOS11 {
    // If we have a view controller reference and its automaticallyAdjustsScrollViewInsets
    // is set to YES, use its view as the parent for constraints. -[MGLMapView adjustContentInset]
    // already take top and bottom layout guides into account. If we don't have a reference, apply
    // constraints against ourself to maintain placement of the subviews.
    //
    UIViewController *viewController = self.viewControllerForLayoutGuides;
    BOOL useLayoutGuides = viewController.view && viewController.automaticallyAdjustsScrollViewInsets;
    UIView *containerView = useLayoutGuides ? viewController.view : self;
    
    // compass view
    //
    [containerView removeConstraints:self.compassViewConstraints];
    [self.compassViewConstraints removeAllObjects];
    
    if (useLayoutGuides) {
        [self.compassViewConstraints addObject:
         [NSLayoutConstraint constraintWithItem:self.compassView
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationGreaterThanOrEqual
                                         toItem:viewController.topLayoutGuide
                                      attribute:NSLayoutAttributeBottom
                                     multiplier:1.0
                                       constant:8.0]];
    }
    [self.compassViewConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self.compassView
                                  attribute:NSLayoutAttributeTop
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeTop
                                 multiplier:1.0
                                   constant:8.0 + self.contentInset.top]];
    
    [self.compassViewConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeTrailing
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:self.compassView
                                  attribute:NSLayoutAttributeTrailing
                                 multiplier:1.0
                                   constant:8.0 + self.contentInset.right]];
    
    [containerView addConstraints:self.compassViewConstraints];
    
    // scale bar view
    //
    [containerView removeConstraints:self.scaleBarConstraints];
    [self.scaleBarConstraints removeAllObjects];
    
    if (useLayoutGuides) {
        [self.scaleBarConstraints addObject:
         [NSLayoutConstraint constraintWithItem:self.scaleBar
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationGreaterThanOrEqual
                                         toItem:viewController.topLayoutGuide
                                      attribute:NSLayoutAttributeBottom
                                     multiplier:1.0
                                       constant:8.0]];
    }
    [self.scaleBarConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self.scaleBar
                                  attribute:NSLayoutAttributeTop
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeTop
                                 multiplier:1.0
                                   constant:8.0 + self.contentInset.top]];
    [self.scaleBarConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self.scaleBar
                                  attribute:NSLayoutAttributeLeft
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeLeft
                                 multiplier:1.0
                                   constant:8.0 + self.contentInset.left]];
    
    [containerView addConstraints:self.scaleBarConstraints];
    
    // logo view
    //
    [containerView removeConstraints:self.logoViewConstraints];
    [self.logoViewConstraints removeAllObjects];
    
    if (useLayoutGuides) {
        [self.logoViewConstraints addObject:
         [NSLayoutConstraint constraintWithItem:viewController.bottomLayoutGuide
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationGreaterThanOrEqual
                                         toItem:self.logoView
                                      attribute:NSLayoutAttributeBaseline
                                     multiplier:1.0
                                       constant:8.0 + self.contentInset.bottom]];
    }
    [self.logoViewConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeBottom
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self.logoView
                                  attribute:NSLayoutAttributeBaseline
                                 multiplier:1
                                   constant:8.0 + self.contentInset.bottom]];
    [self.logoViewConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self.logoView
                                  attribute:NSLayoutAttributeLeading
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeLeading
                                 multiplier:1.0
                                   constant:8.0 + self.contentInset.left]];
    [containerView addConstraints:self.logoViewConstraints];
    
    // attribution button
    //
    [containerView removeConstraints:self.attributionButtonConstraints];
    [self.attributionButtonConstraints removeAllObjects];
    
    if (useLayoutGuides) {
        [self.attributionButtonConstraints addObject:
         [NSLayoutConstraint constraintWithItem:viewController.bottomLayoutGuide
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationGreaterThanOrEqual
                                         toItem:self.attributionButton
                                      attribute:NSLayoutAttributeBaseline
                                     multiplier:1
                                       constant:8.0 + self.contentInset.bottom]];
    }
    [self.attributionButtonConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeBottom
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self.attributionButton
                                  attribute:NSLayoutAttributeBaseline
                                 multiplier:1
                                   constant:8.0 + self.contentInset.bottom]];
    
    [self.attributionButtonConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeTrailing
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:self.attributionButton
                                  attribute:NSLayoutAttributeTrailing
                                 multiplier:1
                                   constant:8.0 + self.contentInset.right]];
    [containerView addConstraints:self.attributionButtonConstraints];
}

- (void)updateConstraints
{
    // If safeAreaLayoutGuide API exists
    if (@available(iOS 11.0, *)) {
        UILayoutGuide *safeAreaLayoutGuide = self.safeAreaLayoutGuide;

        // compass view
        [self removeConstraints:self.compassViewConstraints];
        [self.compassViewConstraints removeAllObjects];
        [self.compassViewConstraints addObject:[self constraintForYAxisAnchor:self.compassView.topAnchor belowAnchor:safeAreaLayoutGuide.topAnchor]];
        [self.compassViewConstraints addObject:[safeAreaLayoutGuide.rightAnchor constraintEqualToAnchor:self.compassView.rightAnchor
                                                                                          constant:8.0 + self.contentInset.right]];
        [self addConstraints:self.compassViewConstraints];
        
        // scale bar view
        [self removeConstraints:self.scaleBarConstraints];
        [self.scaleBarConstraints removeAllObjects];
        [self.scaleBarConstraints addObject:[self constraintForYAxisAnchor:self.scaleBar.topAnchor belowAnchor:safeAreaLayoutGuide.topAnchor]];
        [self.scaleBarConstraints addObject:[self.scaleBar.leftAnchor constraintEqualToAnchor:safeAreaLayoutGuide.leftAnchor
                                                                                     constant:8.0 + self.contentInset.left]];
        [self addConstraints:self.scaleBarConstraints];
        
        // logo view
        [self removeConstraints:self.logoViewConstraints];
        [self.logoViewConstraints removeAllObjects];
        [self.logoViewConstraints addObject:[self constraintForYAxisAnchor:safeAreaLayoutGuide.bottomAnchor belowAnchor:self.logoView.bottomAnchor]];
        [self.logoViewConstraints addObject:[self.logoView.leftAnchor constraintEqualToAnchor:safeAreaLayoutGuide.leftAnchor
                                                                                     constant:8.0 + self.contentInset.left]];
        [self addConstraints:self.logoViewConstraints];
        
        // attribution button
        [self removeConstraints:self.attributionButtonConstraints];
        [self.attributionButtonConstraints removeAllObjects];
        [self.attributionButtonConstraints addObject:[self constraintForYAxisAnchor:safeAreaLayoutGuide.bottomAnchor belowAnchor:self.attributionButton.bottomAnchor]];
        [self.attributionButtonConstraints addObject:[safeAreaLayoutGuide.rightAnchor constraintEqualToAnchor:self.attributionButton.rightAnchor
                                                                                               constant:8.0 + self.contentInset.right]];
        [self addConstraints:self.attributionButtonConstraints];
    } else {
        [self updateConstraintsPreiOS11];
    }
    
    [super updateConstraints];
}

- (NSLayoutConstraint *)constraintForYAxisAnchor:(NSLayoutYAxisAnchor *)yAxisAnchor belowAnchor:(NSLayoutYAxisAnchor *)anchor
{
    if (@available(iOS 11.0, *)) {
        return [yAxisAnchor constraintEqualToSystemSpacingBelowAnchor:anchor multiplier:1];
    } else {
        return nil;
    }
}

- (BOOL)isOpaque
{
    return _opaque;
}

- (void)setOpaque:(BOOL)opaque
{
    _glView.layer.opaque = _opaque = opaque;
}

// This is the delegate of the GLKView object's display call.
- (void)glkView:(__unused GLKView *)view drawInRect:(__unused CGRect)rect
{
    if ( ! self.dormant || ! _rendererFrontend)
    {
        _rendererFrontend->render();
    }
}

// This gets called when the view dimension changes, e.g. because the device is being rotated.
- (void)layoutSubviews
{
    // Calling this here instead of in the scale bar itself because if this is done in the
    // scale bar instance, it triggers a call to this `layoutSubviews` method that calls
    // `_mbglMap->setSize()` just below that triggers rendering update which triggers
    // another scale bar update which causes a rendering update loop and a major performace
    // degradation. The only time the scale bar's intrinsic content size _must_ invalidated
    // is here as a reaction to this object's view dimension changes.
    [self.scaleBar invalidateIntrinsicContentSize];
    
    [super layoutSubviews];

    [self adjustContentInset];

    if (!_isTargetingInterfaceBuilder && _mbglMap) {
        self.mbglMap.setSize([self size]);
    }

    if (self.compassView.alpha)
    {
        [self updateCompass];
    }

    if (self.compassView.alpha || self.showsUserHeadingIndicator)
    {
        [self updateHeadingForDeviceOrientation];
    }

    [self updateUserLocationAnnotationView];
}

/// Updates `contentInset` to reflect the current window geometry.
- (void)adjustContentInset
{
    // We could crawl all the way up the responder chain using
    // -viewControllerForLayoutGuides, but an intervening view means that any
    // manual contentInset should not be overridden; something other than the
    // top and bottom bars may be influencing the manual inset.
    UIViewController *viewController;
    if ([self.nextResponder isKindOfClass:[UIViewController class]])
    {
        // This map view is the content view of a view controller.
        viewController = (UIViewController *)self.nextResponder;
    }
    else if ([self.superview.nextResponder isKindOfClass:[UIViewController class]])
    {
        // This map view is an immediate child of a view controller’s content view.
        viewController = (UIViewController *)self.superview.nextResponder;
    }

    if ( ! viewController.automaticallyAdjustsScrollViewInsets)
    {
        return;
    }

    UIEdgeInsets contentInset = UIEdgeInsetsZero;
    CGPoint topPoint = CGPointMake(0, viewController.topLayoutGuide.length);
    contentInset.top = [self convertPoint:topPoint fromView:viewController.view].y;
    CGPoint bottomPoint = CGPointMake(0, CGRectGetMaxY(viewController.view.bounds)
                                      - viewController.bottomLayoutGuide.length);
    contentInset.bottom = (CGRectGetMaxY(self.bounds)
                           - [self convertPoint:bottomPoint fromView:viewController.view].y);

    // Negative insets are invalid, replace with 0.
    contentInset.top = fmaxf(contentInset.top, 0);
    contentInset.bottom = fmaxf(contentInset.bottom, 0);

    self.contentInset = contentInset;
}

- (void)setContentInset:(UIEdgeInsets)contentInset
{
    MGLLogDebug(@"Setting contentInset: %@", NSStringFromUIEdgeInsets(contentInset));
    [self setContentInset:contentInset animated:NO];
}

- (void)setContentInset:(UIEdgeInsets)contentInset animated:(BOOL)animated
{
    MGLLogDebug(@"Setting contentInset: %@ animated:", NSStringFromUIEdgeInsets(contentInset), MGLStringFromBOOL(animated));
    if (UIEdgeInsetsEqualToEdgeInsets(contentInset, self.contentInset))
    {
        return;
    }

    // After adjusting the content inset, move the center coordinate from the
    // old frame of reference to the new one represented by the newly set
    // content inset.
    CLLocationCoordinate2D oldCenter = self.centerCoordinate;

    _contentInset = contentInset;

    if (self.userTrackingMode == MGLUserTrackingModeNone)
    {
        // Don’t call -setCenterCoordinate:, which resets the user tracking mode.
        [self _setCenterCoordinate:oldCenter animated:animated];
    }
    else
    {
        [self didUpdateLocationWithUserTrackingAnimated:animated];
    }

    // Compass, logo and attribution button constraints needs to be updated.
    [self setNeedsUpdateConstraints];
}

/// Returns the frame of inset content within the map view.
- (CGRect)contentFrame
{
    return UIEdgeInsetsInsetRect(self.bounds, self.contentInset);
}

/// Returns the center point of the inset content within the map view.
- (CGPoint)contentCenter
{
    CGRect contentFrame = self.contentFrame;
    return CGPointMake(CGRectGetMidX(contentFrame), CGRectGetMidY(contentFrame));
}

#pragma mark - Life Cycle -

- (void)updateFromDisplayLink
{
    MGLAssertIsMainThread();

    if (_needsDisplayRefresh)
    {
        _needsDisplayRefresh = NO;

        // Update UIKit elements, prior to rendering
        [self updateUserLocationAnnotationView];
        [self updateAnnotationViews];
        [self updateCalloutView];
        
        [self.glView display];
    }

    if (self.experimental_enableFrameRateMeasurement)
    {
        CFTimeInterval now = CACurrentMediaTime();

        self.frameTime = now - _displayLink.timestamp;
        _frameDurations += self.frameTime;

        _frameCount++;

        CFTimeInterval elapsed = now - _frameCounterStartTime;

        if (elapsed >= 1.0) {
            self.averageFrameRate = _frameCount / elapsed;
            self.averageFrameTime = (_frameDurations / _frameCount) * 1000;

            _frameCount = 0;
            _frameDurations = 0;
            _frameCounterStartTime = now;
        }
    }
}

- (void)setNeedsGLDisplay
{
    MGLAssertIsMainThread();

    _needsDisplayRefresh = YES;
}

- (void)willTerminate
{
    MGLAssertIsMainThread();

    if ( ! self.dormant)
    {
        [self validateDisplayLink];
        self.dormant = YES;
        [self.glView deleteDrawable];
    }

    [self destroyCoreObjects];
}

- (void)validateDisplayLink
{
    BOOL isVisible = self.superview && self.window;
    if (isVisible && ! _displayLink)
    {
        if (_mbglMap && self.mbglMap.getConstrainMode() == mbgl::ConstrainMode::None)
        {
            self.mbglMap.setConstrainMode(mbgl::ConstrainMode::HeightOnly);
        }

        _displayLink = [self.window.screen displayLinkWithTarget:self selector:@selector(updateFromDisplayLink)];
        [self updateDisplayLinkPreferredFramesPerSecond];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        _needsDisplayRefresh = YES;
        [self updateFromDisplayLink];
    }
    else if ( ! isVisible && _displayLink)
    {
        [_displayLink invalidate];
        _displayLink = nil;
    }
}

- (void)updateDisplayLinkPreferredFramesPerSecond
{
    if (!_displayLink)
    {
        return;
    }

    NSInteger newFrameRate;
    if (_preferredFramesPerSecond == MGLMapViewPreferredFramesPerSecondDefault)
    {
        // On legacy devices that cannot maintain a reasonable frame rate, set
        // a lower limit to avoid jank.
        newFrameRate = UIDevice.currentDevice.mgl_isLegacyDevice ? MGLMapViewPreferredFramesPerSecondLowPower : MGLMapViewPreferredFramesPerSecondMaximum;
    }
    else
    {
        newFrameRate = _preferredFramesPerSecond;
    }

    if (@available(iOS 10.0, *))
    {
        _displayLink.preferredFramesPerSecond = newFrameRate;
    }
    else
    {
        // CADisplayLink.frameInterval does not support more than 60 FPS (and
        // no device that supports >60 FPS ever supported iOS 9).
        NSInteger maximumFrameRate = 60;

        // `0` is an alias for maximum frame rate.
        newFrameRate = newFrameRate ?: maximumFrameRate;

        _displayLink.frameInterval = maximumFrameRate / MIN(newFrameRate, maximumFrameRate);
    }
}

- (void)setPreferredFramesPerSecond:(MGLMapViewPreferredFramesPerSecond)preferredFramesPerSecond
{
    MGLLogDebug(@"Setting preferredFramesPerSecond: %ld", preferredFramesPerSecond);
    if (_preferredFramesPerSecond == preferredFramesPerSecond)
    {
        return;
    }

    _preferredFramesPerSecond = preferredFramesPerSecond;
    [self updateDisplayLinkPreferredFramesPerSecond];
}

- (void)willMoveToWindow:(UIWindow *)newWindow {
    [super willMoveToWindow:newWindow];
    [self refreshSupportedInterfaceOrientationsWithWindow:newWindow];
}

- (void)didMoveToWindow
{
    [self validateDisplayLink];
    [super didMoveToWindow];
}

- (void)didMoveToSuperview
{
    [self validateDisplayLink];
    [super didMoveToSuperview];
}

- (void)refreshSupportedInterfaceOrientationsWithWindow:(UIWindow *)window {
    
    // "The system intersects the view controller's supported orientations with
    // the app's supported orientations (as determined by the Info.plist file or
    // the app delegate's application:supportedInterfaceOrientationsForWindow:
    // method) and the device's supported orientations to determine whether to rotate.
    
    UIApplication *application = [UIApplication sharedApplication];
    
    if (window && [application.delegate respondsToSelector:@selector(application:supportedInterfaceOrientationsForWindow:)]) {
        self.applicationSupportedInterfaceOrientations = [application.delegate application:application supportedInterfaceOrientationsForWindow:window];
        return;
    }
    
    // If no delegate method, check the application's plist.
    static UIInterfaceOrientationMask orientationMask = UIInterfaceOrientationMaskAll;

    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        // No application delegate
        NSArray *orientations = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"UISupportedInterfaceOrientations"];
        
        // Application's info plist provided supported orientations.
        if (orientations.count > 0) {
            orientationMask = 0;
            
            NSDictionary *lookup =
            @{
              @"UIInterfaceOrientationPortrait" : @(UIInterfaceOrientationMaskPortrait),
              @"UIInterfaceOrientationPortraitUpsideDown" : @(UIInterfaceOrientationMaskPortraitUpsideDown),
              @"UIInterfaceOrientationLandscapeLeft" : @(UIInterfaceOrientationMaskLandscapeLeft),
              @"UIInterfaceOrientationLandscapeRight" : @(UIInterfaceOrientationMaskLandscapeRight)
              };
            
            for (NSString *orientation in orientations) {
                UIInterfaceOrientationMask mask = ((NSNumber*)lookup[orientation]).unsignedIntegerValue;
                orientationMask |= mask;
            }
        }
    });

    self.applicationSupportedInterfaceOrientations = orientationMask;
}

- (void)deviceOrientationDidChange:(__unused NSNotification *)notification
{
    UIDeviceOrientation deviceOrientation = [[UIDevice currentDevice] orientation];
    
    // The docs for `UIViewController.supportedInterfaceOrientations` states:
    //
    //  When the user changes the device orientation, the system calls this method
    //  on the root view controller or the topmost presented view controller that
    //  fills the window. If the view controller supports the new orientation, the
    //  window and view controller are rotated to the new orientation. This method
    //  is only called if the view controller's shouldAutorotate method returns YES.
    //
    // We want to match similar behaviour. However, it may be preferable to look
    // at the owning view controller (in cases where the map view may be covered
    // by another view.
    
    UIViewController *viewController = [self.window.rootViewController mgl_topMostViewController];
    
    if (![viewController shouldAutorotate]) {
        return;
    }
    
    if ((self.currentOrientation == (UIInterfaceOrientation)deviceOrientation) &&
        (self.currentOrientation != UIInterfaceOrientationUnknown)) {
        return;
    }
    
    // "The system intersects the view controller's supported orientations with
    // the app's supported orientations (as determined by the Info.plist file or
    // the app delegate's application:supportedInterfaceOrientationsForWindow:
    // method) and the device's supported orientations to determine whether to rotate.
    
    UIInterfaceOrientationMask supportedOrientations = viewController.supportedInterfaceOrientations;
    supportedOrientations &= self.applicationSupportedInterfaceOrientations;
    
    // Interface orientations are defined by device orientations
    UIInterfaceOrientationMask interfaceOrientation = 1 << deviceOrientation;
    UIInterfaceOrientationMask validOrientation = interfaceOrientation & UIInterfaceOrientationMaskAll;
    
    if (!(validOrientation & supportedOrientations)) {
        return;
    }
    
    self.currentOrientation = (UIInterfaceOrientation)deviceOrientation;

    // Q. Do we need to re-layout if we're just going from Portrait -> Portrait
    // Upside Down (or from Left to Right)?
    [self setNeedsLayout];
}

- (void)sleepGL:(__unused NSNotification *)notification
{
    // If this view targets an external display, such as AirPlay or CarPlay, we
    // can safely continue to render OpenGL content without tripping
    // gpus_ReturnNotPermittedKillClient in libGPUSupportMercury, because the
    // external connection keeps the application from truly receding to the
    // background.
    if (self.window.screen != [UIScreen mainScreen])
    {
        return;
    }
    
    MGLLogInfo(@"Entering background.");
    MGLAssertIsMainThread();
    
    // Ideally we would wait until we actually received a memory warning but the bulk of the memory
    // we have to release is tied up in GL buffers that we can't touch once we're in the background.
    // Compromise position: release everything but currently rendering tiles
    // A possible improvement would be to store a copy of the GL buffers that we could use to rapidly
    // restart, but that we could also discard in response to a memory warning.
    _rendererFrontend->reduceMemoryUse();

    if ( ! self.dormant)
    {
        self.dormant = YES;

        [self validateLocationServices];

        [MGLMapboxEvents flush];

        _displayLink.paused = YES;

        if ( ! self.glSnapshotView)
        {
            self.glSnapshotView = [[UIImageView alloc] initWithFrame:self.glView.frame];
            self.glSnapshotView.autoresizingMask = self.glView.autoresizingMask;
            self.glSnapshotView.contentMode = UIViewContentModeCenter;
            [self insertSubview:self.glSnapshotView aboveSubview:self.glView];
        }

        self.glSnapshotView.image = self.glView.snapshot;
        self.glSnapshotView.hidden = NO;

        if (self.debugMask && [self.glSnapshotView.subviews count] == 0)
        {
            UIView *snapshotTint = [[UIView alloc] initWithFrame:self.glSnapshotView.bounds];
            snapshotTint.autoresizingMask = self.glSnapshotView.autoresizingMask;
            snapshotTint.backgroundColor = [[UIColor redColor] colorWithAlphaComponent:0.25];
            [self.glSnapshotView addSubview:snapshotTint];
        }

        [self.glView deleteDrawable];
    }
}

- (void)wakeGL:(__unused NSNotification *)notification
{
    MGLLogInfo(@"Entering foreground.");
    MGLAssertIsMainThread();

    if (self.dormant && [UIApplication sharedApplication].applicationState != UIApplicationStateBackground)
    {
        self.dormant = NO;

        [self createGLView];

        self.glSnapshotView.hidden = YES;

        [self.glSnapshotView.subviews makeObjectsPerformSelector:@selector(removeFromSuperview)];

        [self.glView bindDrawable];

        _displayLink.paused = NO;

        [self validateLocationServices];

        [MGLMapboxEvents pushTurnstileEvent];
        [MGLMapboxEvents pushEvent:MMEEventTypeMapLoad withAttributes:@{}];
    }
}

- (void)setHidden:(BOOL)hidden
{
    super.hidden = hidden;
    _displayLink.paused = hidden;
}

- (void)tintColorDidChange
{
    for (UIView *subview in self.subviews) [self updateTintColorForView:subview];
}

- (void)updateTintColorForView:(UIView *)view
{
    // Don't update:
    //   - annotation views
    //   - attribution button (handled automatically)
    if ([view isEqual:self.annotationContainerView] || [view isEqual:self.attributionButton]) return;

    if ([view respondsToSelector:@selector(setTintColor:)]) view.tintColor = self.tintColor;

    for (UIView *subview in view.subviews) [self updateTintColorForView:subview];
}

- (BOOL)canBecomeFirstResponder {
    return YES;
}

#pragma mark - Gestures -

- (void)handleCompassTapGesture:(__unused id)sender
{
    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonResetNorth;

    [self resetNorthAnimated:YES];

    if (self.userTrackingMode == MGLUserTrackingModeFollowWithHeading ||
        self.userTrackingMode == MGLUserTrackingModeFollowWithCourse)
    {
        self.userTrackingMode = MGLUserTrackingModeFollow;
    }
}

- (void)touchesBegan:(__unused NSSet<UITouch *> *)touches withEvent:(__unused UIEvent *)event
{
    if (!self.zoomEnabled && !self.pitchEnabled && !self.rotateEnabled && !self.scrollEnabled)
    {
        return;
    };
    
    self.mbglMap.setGestureInProgress(false);
    if (self.userTrackingState == MGLUserTrackingStateBegan)
    {
        [self setUserTrackingMode:MGLUserTrackingModeNone animated:NO];
    }
    
    [self cancelTransitions];
}

- (void)notifyGestureDidBegin {
    BOOL animated = NO;

    [self cameraWillChangeAnimated:animated];
    self.mbglMap.setGestureInProgress(true);
    _changeDelimiterSuppressionDepth++;
}

- (void)notifyGestureDidEndWithDrift:(BOOL)drift {
    _changeDelimiterSuppressionDepth--;
    MGLAssert(_changeDelimiterSuppressionDepth >= 0,
             @"Unbalanced change delimiter suppression/unsuppression");
    if (_changeDelimiterSuppressionDepth == 0) {
        self.mbglMap.setGestureInProgress(false);
    }
    if ( ! drift)
    {
        BOOL animated = NO;
        [self cameraDidChangeAnimated:animated];
    }
}

- (BOOL)isSuppressingChangeDelimiters {
    return _changeDelimiterSuppressionDepth > 0;
}

- (BOOL)_shouldChangeFromCamera:(nonnull MGLMapCamera *)oldCamera toCamera:(nonnull MGLMapCamera *)newCamera
{
    // Check delegates first
    if ([self.delegate respondsToSelector:@selector(mapView:shouldChangeFromCamera:toCamera:reason:)])
    {
        return [self.delegate mapView:self shouldChangeFromCamera:oldCamera toCamera:newCamera reason:self.cameraChangeReasonBitmask];
    }
    else if ([self.delegate respondsToSelector:@selector(mapView:shouldChangeFromCamera:toCamera:)])
    {
        return [self.delegate mapView:self shouldChangeFromCamera:oldCamera toCamera:newCamera];
    }
    else
    {
        return YES;
    }
}

- (void)handlePanGesture:(UIPanGestureRecognizer *)pan
{
    if ( ! self.isScrollEnabled) return;

    [self cancelTransitions];

    MGLMapCamera *oldCamera = self.camera;

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonGesturePan;

    if (pan.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MMEEventGesturePanStart forRecognizer:pan];

        self.userTrackingMode = MGLUserTrackingModeNone;

        [self notifyGestureDidBegin];
    }
    else if (pan.state == UIGestureRecognizerStateChanged)
    {
        CGPoint delta = [pan translationInView:pan.view];

        MGLMapCamera *toCamera = [self cameraByPanningWithTranslation:delta panGesture:pan];

        if ([self _shouldChangeFromCamera:oldCamera toCamera:toCamera])
        {
            self.mbglMap.moveBy({ delta.x, delta.y });
            [pan setTranslation:CGPointZero inView:pan.view];
        }

        [self cameraIsChanging];
    }
    else if (pan.state == UIGestureRecognizerStateEnded || pan.state == UIGestureRecognizerStateCancelled)
    {
        CGPoint velocity = [pan velocityInView:pan.view];
        if (self.decelerationRate == MGLMapViewDecelerationRateImmediate || sqrtf(velocity.x * velocity.x + velocity.y * velocity.y) < 100)
        {
            // Not enough velocity to overcome friction
            velocity = CGPointZero;
        }

        BOOL drift = ! CGPointEqualToPoint(velocity, CGPointZero);
        if (drift)
        {
            CGPoint offset = CGPointMake(velocity.x * self.decelerationRate / 4, velocity.y * self.decelerationRate / 4);
            MGLMapCamera *toCamera = [self cameraByPanningWithTranslation:offset panGesture:pan];

            if ([self _shouldChangeFromCamera:oldCamera toCamera:toCamera])
            {
                self.mbglMap.moveBy({ offset.x, offset.y }, MGLDurationFromTimeInterval(self.decelerationRate));
            }
        }

        [self notifyGestureDidEndWithDrift:drift];

        // metrics: pan end
        CGPoint pointInView = CGPointMake([pan locationInView:pan.view].x, [pan locationInView:pan.view].y);
        CLLocationCoordinate2D panCoordinate = [self convertPoint:pointInView toCoordinateFromView:pan.view];
        int zoom = round([self zoomLevel]);

        [MGLMapboxEvents pushEvent:MMEEventTypeMapDragEnd withAttributes:@{
            MMEEventKeyLatitude: @(panCoordinate.latitude),
            MMEEventKeyLongitude: @(panCoordinate.longitude),
            MMEEventKeyZoomLevel: @(zoom)
        }];
    }

}

- (void)handlePinchGesture:(UIPinchGestureRecognizer *)pinch
{
    if ( ! self.isZoomEnabled) return;

    [self cancelTransitions];

    CGPoint centerPoint = [self anchorPointForGesture:pinch];
    MGLMapCamera *oldCamera = self.camera;

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonGesturePinch;

    if (pinch.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MMEEventGesturePinchStart forRecognizer:pinch];

        self.scale = powf(2, self.mbglMap.getZoom());

        [self notifyGestureDidBegin];
    }
    else if (pinch.state == UIGestureRecognizerStateChanged)
    {
        // Zoom limiting happens at the core level.
        CGFloat newScale = self.scale * pinch.scale;
        double newZoom = log2(newScale);

        // Calculates the final camera zoom, has no effect within current map camera.
        MGLMapCamera *toCamera = [self cameraByZoomingToZoomLevel:newZoom aroundAnchorPoint:centerPoint];

        if ([self _shouldChangeFromCamera:oldCamera toCamera:toCamera])
        {
            self.mbglMap.setZoom(newZoom, mbgl::ScreenCoordinate { centerPoint.x, centerPoint.y });
            // The gesture recognizer only reports the gesture’s current center
            // point, so use the previous center point to anchor the transition.
            // If the number of touches has changed, the remembered center point is
            // meaningless.
            if (self.userTrackingMode == MGLUserTrackingModeNone && pinch.numberOfTouches == _previousPinchNumberOfTouches)
            {
                CLLocationCoordinate2D centerCoordinate = _previousPinchCenterCoordinate;
                self.mbglMap.setLatLng(MGLLatLngFromLocationCoordinate2D(centerCoordinate),
                                    mbgl::EdgeInsets { centerPoint.y, centerPoint.x, self.size.height - centerPoint.y, self.size.width - centerPoint.x });
            }
        }
        [self cameraIsChanging];
    }
    else if (pinch.state == UIGestureRecognizerStateEnded || pinch.state == UIGestureRecognizerStateCancelled)
    {
        CGFloat velocity = pinch.velocity;
        if (isnan(velocity))
        {
            // UIPinchGestureRecognizer sometimes returns NaN for the velocity
            velocity = 0;
        }
        if (velocity > -0.5 && velocity < 3)
        {
            velocity = 0;
        }

        NSTimeInterval duration = (velocity > 0 ? 1 : 0.25) * self.decelerationRate;

        CGFloat scale = self.scale * pinch.scale;
        CGFloat newScale = scale;
        if (velocity >= 0)
        {
            newScale += scale * velocity * duration * 0.1;
        }
        else
        {
            newScale += scale / (velocity * duration) * 0.1;
        }

        if (newScale <= 0 || log2(newScale) < self.mbglMap.getMinZoom())
        {
            velocity = 0;
        }
        
        BOOL drift = velocity && duration;
        
        // Calculates the final camera zoom, this has no effect within current map camera.
        double zoom = log2(newScale);
        MGLMapCamera *toCamera = [self cameraByZoomingToZoomLevel:zoom aroundAnchorPoint:centerPoint];

        if ( ! [self _shouldChangeFromCamera:oldCamera toCamera:toCamera])
        {
            drift = NO;
        }
        else
        {
            if (drift)
            {
                self.mbglMap.setZoom(zoom, mbgl::ScreenCoordinate { centerPoint.x, centerPoint.y }, MGLDurationFromTimeInterval(duration));
            }
        }

        [self notifyGestureDidEndWithDrift:drift];
        [self unrotateIfNeededForGesture];
    }

    _previousPinchCenterCoordinate = [self convertPoint:centerPoint toCoordinateFromView:self];
    _previousPinchNumberOfTouches = pinch.numberOfTouches;
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    if ( ! self.isRotateEnabled) return;

    [self cancelTransitions];

    CGPoint centerPoint = [self anchorPointForGesture:rotate];
    MGLMapCamera *oldCamera = self.camera;

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonGestureRotate;

    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MMEEventGestureRotateStart forRecognizer:rotate];

        self.angle = MGLRadiansFromDegrees(self.mbglMap.getBearing()) * -1;

        if (self.userTrackingMode != MGLUserTrackingModeNone)
        {
            self.userTrackingMode = MGLUserTrackingModeFollow;
        }

        self.shouldTriggerHapticFeedbackForCompass = NO;

        [self notifyGestureDidBegin];
    }
    else if (rotate.state == UIGestureRecognizerStateChanged)
    {
        CGFloat newDegrees = MGLDegreesFromRadians(self.angle + rotate.rotation) * -1;

        // constrain to +/-30 degrees when merely rotating like Apple does
        //
        if ( ! self.isRotationAllowed && std::abs(self.pinch.scale) < 10)
        {
            newDegrees = fminf(newDegrees,  30);
            newDegrees = fmaxf(newDegrees, -30);
        }
        
        MGLMapCamera *toCamera = [self cameraByRotatingToDirection:newDegrees aroundAnchorPoint:centerPoint];

        if ([self _shouldChangeFromCamera:oldCamera toCamera:toCamera])
        {
           self.mbglMap.setBearing(newDegrees, mbgl::ScreenCoordinate { centerPoint.x, centerPoint.y });
        }

        [self cameraIsChanging];

        // Trigger a light haptic feedback event when the user rotates to due north.
        if (@available(iOS 10.0, *))
        {
            if (self.isHapticFeedbackEnabled && fabs(newDegrees) <= 1 && self.shouldTriggerHapticFeedbackForCompass)
            {
                UIImpactFeedbackGenerator *hapticFeedback = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleLight];
                [hapticFeedback impactOccurred];

                self.shouldTriggerHapticFeedbackForCompass = NO;
            }
            else if (fabs(newDegrees) > 1)
            {
                self.shouldTriggerHapticFeedbackForCompass = YES;
            }
        }
    }
    else if (rotate.state == UIGestureRecognizerStateEnded || rotate.state == UIGestureRecognizerStateCancelled)
    {
        CGFloat velocity = rotate.velocity;
        CGFloat decelerationRate = self.decelerationRate;
        if (decelerationRate != MGLMapViewDecelerationRateImmediate && fabs(velocity) > 3)
        {
            CGFloat radians = self.angle + rotate.rotation;
            CGFloat newRadians = radians + velocity * decelerationRate * 0.1;
            CGFloat newDegrees = MGLDegreesFromRadians(newRadians) * -1;

            MGLMapCamera *toCamera = [self cameraByRotatingToDirection:newDegrees aroundAnchorPoint:centerPoint];

            if ([self _shouldChangeFromCamera:oldCamera toCamera:toCamera])
            {
                self.mbglMap.setBearing(newDegrees, mbgl::ScreenCoordinate { centerPoint.x, centerPoint.y }, MGLDurationFromTimeInterval(decelerationRate));
                
                [self notifyGestureDidEndWithDrift:YES];
                
                __weak MGLMapView *weakSelf = self;
                
                [self animateWithDelay:decelerationRate animations:^
                 {
                     [weakSelf unrotateIfNeededForGesture];
                 }];
            }
        }
        else
        {
            [self notifyGestureDidEndWithDrift:NO];
            [self unrotateIfNeededForGesture];
        }
    }
}

- (void)handleSingleTapGesture:(UITapGestureRecognizer *)singleTap
{
    if (singleTap.state != UIGestureRecognizerStateRecognized) return;

    [self trackGestureEvent:MMEEventGestureSingleTap forRecognizer:singleTap];

    if (self.mapViewProxyAccessibilityElement.accessibilityElementIsFocused)
    {
        id nextElement;
        if (_userLocationAnnotationIsSelected)
        {
            nextElement = self.userLocationAnnotationView;
        }
        else
        {
            if (_selectedAnnotationTag != MGLAnnotationTagNotFound) {
                nextElement = _annotationContextsByAnnotationTag.at(_selectedAnnotationTag).accessibilityElement;
            }
        }
        [self deselectAnnotation:self.selectedAnnotation animated:YES];
        UIAccessibilityPostNotification(UIAccessibilityScreenChangedNotification, nextElement);

        return;
    }

    id<MGLAnnotation> annotation = [self annotationForGestureRecognizer:singleTap persistingResults:YES];
    if (annotation)
    {
        CGPoint calloutPoint = [singleTap locationInView:self];
        CGRect positionRect = [self positioningRectForAnnotation:annotation defaultCalloutPoint:calloutPoint];
        [self selectAnnotation:annotation moveIntoView:YES animateSelection:YES calloutPositioningRect:positionRect];
    }
    else if (self.selectedAnnotation)
    {
        [self deselectAnnotation:self.selectedAnnotation animated:YES];
    }
}

/**
 Returns the annotation that would be selected by a tap gesture recognizer.

 This is used when a gesture is recognized, and to check if the gesture should be recognized.

 @param singleTap An in progress tap gesture recognizer.
 @param persist True to remember the cycleable set of annotations. @see annotationTagAtPoint:persistingResults
 */
- (nullable id <MGLAnnotation>)annotationForGestureRecognizer:(UITapGestureRecognizer*)singleTap persistingResults:(BOOL)persist
{
    CGPoint tapPoint = [singleTap locationInView:self];

    if (self.userLocationVisible)
    {
        CGPoint tapPointForUserLocation;
        if (self.userLocationAnnotationView.hitTestLayer == self.userLocationAnnotationView.layer.presentationLayer)
        {
            tapPointForUserLocation = tapPoint;
        }
        else
        {
            // Get the tap point within the custom hit test layer.
            tapPointForUserLocation = [singleTap locationInView:self.userLocationAnnotationView];
        }

        CALayer *hitLayer = [self.userLocationAnnotationView.hitTestLayer hitTest:tapPointForUserLocation];

        if (hitLayer)
        {
            if ( ! _userLocationAnnotationIsSelected)
            {
                return self.userLocation;
            }
            return nil;
        }
    }
    
    MGLAnnotationTag hitAnnotationTag = [self annotationTagAtPoint:tapPoint persistingResults:persist];
    if (hitAnnotationTag != MGLAnnotationTagNotFound)
    {
        if (hitAnnotationTag != _selectedAnnotationTag)
        {
            id <MGLAnnotation> annotation = [self annotationWithTag:hitAnnotationTag];
            MGLAssert(annotation, @"Cannot select nonexistent annotation with tag %llu", hitAnnotationTag);
            return annotation;
        }
    }

    return nil;
}

- (void)handleDoubleTapGesture:(UITapGestureRecognizer *)doubleTap
{
    if (doubleTap.state != UIGestureRecognizerStateRecognized) return;

    if ( ! self.isZoomEnabled) return;

    [self cancelTransitions];

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonGestureZoomIn;

    MGLMapCamera *oldCamera = self.camera;

    double newZoom = round(self.zoomLevel) + 1.0;

    CGPoint gesturePoint = [self anchorPointForGesture:doubleTap];

    MGLMapCamera *toCamera = [self cameraByZoomingToZoomLevel:newZoom aroundAnchorPoint:gesturePoint];

    if ([self _shouldChangeFromCamera:oldCamera toCamera:toCamera])
    {
        [self trackGestureEvent:MMEEventGestureDoubleTap forRecognizer:doubleTap];

        mbgl::ScreenCoordinate center(gesturePoint.x, gesturePoint.y);
        self.mbglMap.setZoom(newZoom, center, MGLDurationFromTimeInterval(MGLAnimationDuration));

        __weak MGLMapView *weakSelf = self;

        [self animateWithDelay:MGLAnimationDuration animations:^
         {
             [weakSelf unrotateIfNeededForGesture];
         }];
    }
    else
    {
        [self unrotateIfNeededForGesture];
    }
}

- (void)handleTwoFingerTapGesture:(UITapGestureRecognizer *)twoFingerTap
{
    if (twoFingerTap.state != UIGestureRecognizerStateRecognized) return;

    if ( ! self.isZoomEnabled) return;

    if (self.mbglMap.getZoom() == self.mbglMap.getMinZoom()) return;

    [self cancelTransitions];

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonGestureZoomOut;

    MGLMapCamera *oldCamera = self.camera;

    double newZoom = round(self.zoomLevel) - 1.0;

    CGPoint gesturePoint = [self anchorPointForGesture:twoFingerTap];

    MGLMapCamera *toCamera = [self cameraByZoomingToZoomLevel:newZoom aroundAnchorPoint:gesturePoint];

    if ([self _shouldChangeFromCamera:oldCamera toCamera:toCamera])
    {
        [self trackGestureEvent:MMEEventGestureTwoFingerSingleTap forRecognizer:twoFingerTap];

        mbgl::ScreenCoordinate center(gesturePoint.x, gesturePoint.y);
        self.mbglMap.setZoom(newZoom, center, MGLDurationFromTimeInterval(MGLAnimationDuration));

        __weak MGLMapView *weakSelf = self;

        [self animateWithDelay:MGLAnimationDuration animations:^
         {
             [weakSelf unrotateIfNeededForGesture];
         }];
    }
}

- (void)handleQuickZoomGesture:(UILongPressGestureRecognizer *)quickZoom
{
    if ( ! self.isZoomEnabled) return;

    [self cancelTransitions];

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonGestureOneFingerZoom;

    if (quickZoom.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MMEEventGestureQuickZoom forRecognizer:quickZoom];

        self.scale = powf(2, self.mbglMap.getZoom());

        self.quickZoomStart = [quickZoom locationInView:quickZoom.view].y;

        [self notifyGestureDidBegin];
    }
    else if (quickZoom.state == UIGestureRecognizerStateChanged)
    {
        CGFloat distance = [quickZoom locationInView:quickZoom.view].y - self.quickZoomStart;

        CGFloat newZoom = MAX(log2f(self.scale) + (distance / 75), self.mbglMap.getMinZoom());

        if (self.mbglMap.getZoom() == newZoom) return;

        CGPoint centerPoint = [self anchorPointForGesture:quickZoom];
        
        MGLMapCamera *oldCamera = self.camera;
        MGLMapCamera *toCamera = [self cameraByZoomingToZoomLevel:newZoom aroundAnchorPoint:centerPoint];

        if ([self _shouldChangeFromCamera:oldCamera toCamera:toCamera])
        {
            self.mbglMap.setZoom(newZoom, mbgl::ScreenCoordinate { centerPoint.x, centerPoint.y });
        }

        [self cameraIsChanging];
    }
    else if (quickZoom.state == UIGestureRecognizerStateEnded || quickZoom.state == UIGestureRecognizerStateCancelled)
    {
        [self notifyGestureDidEndWithDrift:NO];
        [self unrotateIfNeededForGesture];
    }
}

- (void)handleTwoFingerDragGesture:(UIPanGestureRecognizer *)twoFingerDrag
{
    if ( ! self.isPitchEnabled) return;

    [self cancelTransitions];

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonGestureTilt;
    static CGFloat initialPitch;
    
    if (twoFingerDrag.state == UIGestureRecognizerStateBegan)
    {
        initialPitch = self.mbglMap.getPitch();
        [self trackGestureEvent:MMEEventGesturePitchStart forRecognizer:twoFingerDrag];
        [self notifyGestureDidBegin];
    }

    if (twoFingerDrag.state == UIGestureRecognizerStateBegan || twoFingerDrag.state == UIGestureRecognizerStateChanged)
    {
        CGFloat gestureDistance = CGPoint([twoFingerDrag translationInView:twoFingerDrag.view]).y;
        CGFloat slowdown = 2.0;

        CGFloat pitchNew = initialPitch - (gestureDistance / slowdown);

        CGPoint centerPoint = [self anchorPointForGesture:twoFingerDrag];

        MGLMapCamera *oldCamera = self.camera;
        MGLMapCamera *toCamera = [self cameraByTiltingToPitch:pitchNew];

        if ([self _shouldChangeFromCamera:oldCamera toCamera:toCamera])
        {
            self.mbglMap.setPitch(pitchNew, mbgl::ScreenCoordinate { centerPoint.x, centerPoint.y });
        }

        [self cameraIsChanging];
    }
    else if (twoFingerDrag.state == UIGestureRecognizerStateEnded || twoFingerDrag.state == UIGestureRecognizerStateCancelled)
    {
        [self notifyGestureDidEndWithDrift:NO];
        [self unrotateIfNeededForGesture];
    }

}

- (MGLMapCamera *)cameraByPanningWithTranslation:(CGPoint)endPoint panGesture:(UIPanGestureRecognizer *)pan
{
    MGLMapCamera *panCamera = [self.camera copy];
    
    CGPoint centerPoint = CGPointMake(CGRectGetMidX(self.bounds), CGRectGetMidY(self.bounds));
    CGPoint endCameraPoint = CGPointMake(centerPoint.x - endPoint.x, centerPoint.y - endPoint.y);
    CLLocationCoordinate2D panCoordinate = [self convertPoint:endCameraPoint toCoordinateFromView:pan.view];
    
    panCamera.centerCoordinate = panCoordinate;
    
    return panCamera;
}

- (MGLMapCamera *)cameraByZoomingToZoomLevel:(double)zoom aroundAnchorPoint:(CGPoint)anchorPoint
{
    mbgl::ScreenCoordinate anchor = mbgl::ScreenCoordinate { anchorPoint.x, anchorPoint.y };
    mbgl::EdgeInsets padding = mbgl::EdgeInsets(anchor.y, anchor.x, self.size.height - anchor.y, self.size.width - anchor.x);
    mbgl::CameraOptions currentCameraOptions = self.mbglMap.getCameraOptions(padding);

    currentCameraOptions.zoom = mbgl::util::clamp(zoom, self.minimumZoomLevel, self.maximumZoomLevel);
    currentCameraOptions.anchor = anchor;
    MGLCoordinateBounds bounds = MGLCoordinateBoundsFromLatLngBounds(self.mbglMap.latLngBoundsForCamera(currentCameraOptions));
    
    return [self cameraThatFitsCoordinateBounds:bounds];
}

- (MGLMapCamera *)cameraByRotatingToDirection:(CLLocationDirection)degrees aroundAnchorPoint:(CGPoint)anchorPoint
{
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    mbgl::CameraOptions currentCameraOptions = self.mbglMap.getCameraOptions(padding);
    
    MGLMapCamera *camera;
    
    mbgl::ScreenCoordinate anchor = mbgl::ScreenCoordinate { anchorPoint.x, anchorPoint.y };
    currentCameraOptions.angle = degrees;
    currentCameraOptions.anchor = anchor;
    camera = [self cameraForCameraOptions:currentCameraOptions];
    
    return camera;
}

- (MGLMapCamera *)cameraByTiltingToPitch:(CGFloat)pitch
{
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    mbgl::CameraOptions currentCameraOptions = self.mbglMap.getCameraOptions(padding);
    
    MGLMapCamera *camera;

    currentCameraOptions.pitch = pitch;
    camera = [self cameraForCameraOptions:currentCameraOptions];
    
    return camera;
}

- (CGPoint)anchorPointForGesture:(UIGestureRecognizer *)gesture {
    if (self.userTrackingMode != MGLUserTrackingModeNone)
    {
        return self.userLocationAnnotationViewCenter;
    }

    // Special case for two-finger drag and quickzoom
    if ([gesture isKindOfClass:[UIPanGestureRecognizer class]] || [gesture isKindOfClass:[UILongPressGestureRecognizer class]])
    {
        return self.contentCenter;
    }

    return [gesture locationInView:gesture.view];
}

- (void)handleCalloutAccessoryTapGesture:(UITapGestureRecognizer *)tap
{
    if ([self.delegate respondsToSelector:@selector(mapView:annotation:calloutAccessoryControlTapped:)])
    {
        MGLAssert([tap.view isKindOfClass:[UIControl class]], @"Tapped view %@ is not a UIControl", tap.view);
        id <MGLAnnotation> selectedAnnotation = self.selectedAnnotation;
        MGLAssert(selectedAnnotation, @"Selected annotation should not be nil.");
        [self.delegate mapView:self annotation:selectedAnnotation
            calloutAccessoryControlTapped:(UIControl *)tap.view];
    }
}

- (BOOL)calloutViewShouldHighlight:(__unused MGLCompactCalloutView *)calloutView
{
    return [self.delegate respondsToSelector:@selector(mapView:tapOnCalloutForAnnotation:)];
}

- (void)calloutViewClicked:(__unused MGLSMCalloutView *)calloutView
{
    if ([self.delegate respondsToSelector:@selector(mapView:tapOnCalloutForAnnotation:)])
    {
        id <MGLAnnotation> selectedAnnotation = self.selectedAnnotation;
        MGLAssert(selectedAnnotation, @"Selected annotation should not be nil.");
        [self.delegate mapView:self tapOnCalloutForAnnotation:selectedAnnotation];
    }
}

- (void)calloutViewTapped:(__unused MGLCompactCalloutView *)calloutView
{
    if ([self.delegate respondsToSelector:@selector(mapView:tapOnCalloutForAnnotation:)])
    {
        id <MGLAnnotation> selectedAnnotation = self.selectedAnnotation;
        MGLAssert(selectedAnnotation, @"Selected annotation should not be nil.");
        [self.delegate mapView:self tapOnCalloutForAnnotation:selectedAnnotation];
    }
}

- (void)calloutViewDidAppear:(UIView<MGLCalloutView> *)calloutView
{
    UIAccessibilityPostNotification(UIAccessibilityScreenChangedNotification, nil);
    UIAccessibilityPostNotification(UIAccessibilityLayoutChangedNotification, calloutView);
}

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    if ([gestureRecognizer isKindOfClass:[UIPanGestureRecognizer class]])
    {
        UIPanGestureRecognizer *panGesture = (UIPanGestureRecognizer *)gestureRecognizer;
        
        if (panGesture.minimumNumberOfTouches == 2)
        {
            CGPoint west = [panGesture locationOfTouch:0 inView:panGesture.view];
            CGPoint east = [panGesture locationOfTouch:1 inView:panGesture.view];
            
            if (west.x > east.x) {
                CGPoint swap = west;
                west = east;
                east = swap;
            }
            
            CLLocationDegrees horizontalToleranceDegrees = 60.0;
            if ([self angleBetweenPoints:west east:east] > horizontalToleranceDegrees) {
                return NO;
            }
        }
    }
    else if (gestureRecognizer == _singleTapGestureRecognizer)
    {
        // Gesture will be recognized if it could deselect an annotation
        if(!self.selectedAnnotation)
        {
            id<MGLAnnotation> annotation = [self annotationForGestureRecognizer:(UITapGestureRecognizer*)gestureRecognizer persistingResults:NO];
            if (!annotation) {
                return NO;
            }
        }
    }
    return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    NSArray *validSimultaneousGestures = @[ self.pan, self.pinch, self.rotate ];

    return ([validSimultaneousGestures containsObject:gestureRecognizer] && [validSimultaneousGestures containsObject:otherGestureRecognizer]);
}

- (CLLocationDegrees)angleBetweenPoints:(CGPoint)west east:(CGPoint)east
{
    CGFloat slope = (west.y - east.y) / (west.x - east.x);
    
    CGFloat angle = atan(fabs(slope));
    CLLocationDegrees degrees = MGLDegreesFromRadians(angle);
    
    return degrees;
}

- (void)trackGestureEvent:(NSString *)gestureID forRecognizer:(UIGestureRecognizer *)recognizer
{
    CGPoint pointInView = CGPointMake([recognizer locationInView:recognizer.view].x, [recognizer locationInView:recognizer.view].y);
    CLLocationCoordinate2D gestureCoordinate = [self convertPoint:pointInView toCoordinateFromView:recognizer.view];
    int zoom = round([self zoomLevel]);

    [MGLMapboxEvents pushEvent:MMEEventTypeMapTap withAttributes:@{
        MMEEventKeyLatitude: @(gestureCoordinate.latitude),
        MMEEventKeyLongitude: @(gestureCoordinate.longitude),
        MMEEventKeyZoomLevel: @(zoom),
        MMEEventKeyGestureID: gestureID
    }];
}

#pragma mark - Attribution -

- (void)showAttribution:(id)sender
{
    BOOL shouldShowVersion = [sender isKindOfClass:[UILongPressGestureRecognizer class]];
    if (shouldShowVersion)
    {
        UILongPressGestureRecognizer *longPress = (UILongPressGestureRecognizer *)sender;
        if (longPress.state != UIGestureRecognizerStateBegan)
        {
            return;
        }
    }

    NSString *actionSheetTitle = NSLocalizedStringWithDefaultValue(@"SDK_NAME", nil, nil, @"Mapbox Maps SDK for iOS", @"Action sheet title");
    UIAlertController *attributionController = [UIAlertController alertControllerWithTitle:actionSheetTitle
                                                                                   message:nil
                                                                            preferredStyle:UIAlertControllerStyleActionSheet];

    if (shouldShowVersion)
    {
        attributionController.title = [actionSheetTitle stringByAppendingFormat:@" %@", [NSBundle mgl_frameworkInfoDictionary][@"MGLSemanticVersionString"]];
    }

    NSArray *attributionInfos = [self.style attributionInfosWithFontSize:[UIFont buttonFontSize]
                                                               linkColor:nil];
    for (MGLAttributionInfo *info in attributionInfos)
    {
        UIAlertAction *action = [UIAlertAction actionWithTitle:[info.title.string mgl_titleCasedStringWithLocale:[NSLocale currentLocale]]
                                                         style:UIAlertActionStyleDefault
                                                       handler:^(UIAlertAction * _Nonnull actionBlock) {
            NSURL *url = info.URL;
            if (url)
            {
                if (info.feedbackLink)
                {
                    MGLMapCamera *camera = self.camera;
                    url = [info feedbackURLForStyleURL:self.styleURL
                                    atCenterCoordinate:camera.centerCoordinate
                                             zoomLevel:self.zoomLevel
                                             direction:camera.heading
                                                 pitch:camera.pitch];
                }
                [[UIApplication sharedApplication] openURL:url];
            }
        }];
        [attributionController addAction:action];
    }
    
    NSString *telemetryTitle = NSLocalizedStringWithDefaultValue(@"TELEMETRY_NAME", nil, nil, @"Mapbox Telemetry", @"Action in attribution sheet");
    UIAlertAction *telemetryAction = [UIAlertAction actionWithTitle:telemetryTitle
                                                              style:UIAlertActionStyleDefault
                                                            handler:^(UIAlertAction * _Nonnull action) {
        [self presentTelemetryAlertController];
    }];
    [attributionController addAction:telemetryAction];
    
    NSString *cancelTitle = NSLocalizedStringWithDefaultValue(@"CANCEL", nil, nil, @"Cancel", @"");
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:cancelTitle
                                                           style:UIAlertActionStyleCancel
                                                         handler:NULL];
    [attributionController addAction:cancelAction];
    
    attributionController.popoverPresentationController.sourceView = self;
    attributionController.popoverPresentationController.sourceRect = self.attributionButton.frame;
    
    UIViewController *viewController = [self.window.rootViewController mgl_topMostViewController];
    [viewController presentViewController:attributionController animated:YES completion:NULL];
}

- (void)presentTelemetryAlertController
{
    NSString *title = NSLocalizedStringWithDefaultValue(@"TELEMETRY_TITLE", nil, nil, @"Make Mapbox Maps Better", @"Telemetry prompt title");
    NSString *message;
    NSString *participateTitle;
    NSString *declineTitle;
    if ([[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsEnabled"])
    {
        message = NSLocalizedStringWithDefaultValue(@"TELEMETRY_ENABLED_MSG", nil, nil, @"You are helping to make OpenStreetMap and Mapbox maps better by contributing anonymous usage data.", @"Telemetry prompt message");
        participateTitle = NSLocalizedStringWithDefaultValue(@"TELEMETRY_ENABLED_ON", nil, nil, @"Keep Participating", @"Telemetry prompt button");
        declineTitle = NSLocalizedStringWithDefaultValue(@"TELEMETRY_ENABLED_OFF", nil, nil, @"Stop Participating", @"Telemetry prompt button");
    }
    else
    {
        message = NSLocalizedStringWithDefaultValue(@"TELEMETRY_DISABLED_MSG", nil, nil, @"You can help make OpenStreetMap and Mapbox maps better by contributing anonymous usage data.", @"Telemetry prompt message");
        participateTitle = NSLocalizedStringWithDefaultValue(@"TELEMETRY_DISABLED_ON", nil, nil, @"Participate", @"Telemetry prompt button");
        declineTitle = NSLocalizedStringWithDefaultValue(@"TELEMETRY_DISABLED_OFF", nil, nil, @"Don’t Participate", @"Telemetry prompt button");
    }
    
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:title
                                                                             message:message
                                                                      preferredStyle:UIAlertControllerStyleAlert];
    
    NSString *moreTitle = NSLocalizedStringWithDefaultValue(@"TELEMETRY_MORE", nil, nil, @"Tell Me More", @"Telemetry prompt button");
    UIAlertAction *moreAction = [UIAlertAction actionWithTitle:moreTitle
                                                         style:UIAlertActionStyleDefault
                                                       handler:^(UIAlertAction * _Nonnull action) {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://www.mapbox.com/telemetry/"]];
    }];
    [alertController addAction:moreAction];
    
    UIAlertAction *declineAction = [UIAlertAction actionWithTitle:declineTitle
                                                            style:UIAlertActionStyleDefault
                                                          handler:^(UIAlertAction * _Nonnull action) {
        [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"MGLMapboxMetricsEnabled"];
    }];
    [alertController addAction:declineAction];
    
    UIAlertAction *participateAction = [UIAlertAction actionWithTitle:participateTitle
                                                                style:UIAlertActionStyleCancel
                                                              handler:^(UIAlertAction * _Nonnull action) {
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"MGLMapboxMetricsEnabled"];
    }];
    [alertController addAction:participateAction];
    
    UIViewController *viewController = [self.window.rootViewController mgl_topMostViewController];
    [viewController presentViewController:alertController animated:YES completion:NULL];
}

#pragma mark - Properties -

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if ([keyPath isEqualToString:@"hidden"] && object == _attributionButton)
    {
        NSNumber *hiddenNumber = change[NSKeyValueChangeNewKey];
        BOOL attributionButtonWasHidden = [hiddenNumber boolValue];
        if (attributionButtonWasHidden)
        {
            [MGLMapboxEvents ensureMetricsOptoutExists];
        }
    }
    else if ([keyPath isEqualToString:@"coordinate"] && [object conformsToProtocol:@protocol(MGLAnnotation)] && ![object isKindOfClass:[MGLMultiPoint class]])
    {
        id <MGLAnnotation> annotation = object;
        MGLAnnotationTag annotationTag = (MGLAnnotationTag)(NSUInteger)context;
        // We can get here because a subclass registered itself as an observer
        // of the coordinate key path of a non-multipoint annotation but failed
        // to handle the change. This check deters us from treating the
        // subclass’s context as an annotation tag. If the context happens to
        // match a valid annotation tag, the annotation will be unnecessarily
        // but safely updated.
        if (annotation == [self annotationWithTag:annotationTag])
        {
            const mbgl::Point<double> point = MGLPointFromLocationCoordinate2D(annotation.coordinate);

            if (annotationTag != MGLAnnotationTagNotFound) {
                MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(annotationTag);
                if (annotationContext.annotationView)
                {
                    // Redundantly move the associated annotation view outside the scope of the animation-less transaction block in -updateAnnotationViews.
                    annotationContext.annotationView.center = [self convertCoordinate:annotationContext.annotation.coordinate toPointToView:self];
                }

                MGLAnnotationImage *annotationImage = [self imageOfAnnotationWithTag:annotationTag];
                NSString *symbolName = annotationImage.styleIconIdentifier;

                // Update the annotation’s backing geometry to match the annotation model object. Any associated annotation view is also moved by side effect. However, -updateAnnotationViews disables the view’s animation actions, because it can’t distinguish between moves due to the viewport changing and moves due to the annotation’s coordinate changing.
                self.mbglMap.updateAnnotation(annotationTag, mbgl::SymbolAnnotation { point, symbolName.UTF8String });
                [self updateCalloutView];
            }
        }
    }
    else if ([keyPath isEqualToString:@"coordinates"] && [object isKindOfClass:[MGLMultiPoint class]])
    {
        MGLMultiPoint *annotation = object;
        MGLAnnotationTag annotationTag = (MGLAnnotationTag)(NSUInteger)context;
        // We can get here because a subclass registered itself as an observer
        // of the coordinates key path of a multipoint annotation but failed
        // to handle the change. This check deters us from treating the
        // subclass’s context as an annotation tag. If the context happens to
        // match a valid annotation tag, the annotation will be unnecessarily
        // but safely updated.
        if (annotation == [self annotationWithTag:annotationTag])
        {
            // Update the annotation’s backing geometry to match the annotation model object.
            self.mbglMap.updateAnnotation(annotationTag, [annotation annotationObjectWithDelegate:self]);
            [self updateCalloutView];
        }
    }
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingZoomEnabled
{
    return [NSSet setWithObject:@"allowsZooming"];
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingScrollEnabled
{
    return [NSSet setWithObject:@"allowsScrolling"];
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingRotateEnabled
{
    return [NSSet setWithObject:@"allowsRotating"];
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingPitchEnabled
{
    return [NSSet setWithObject:@"allowsTilting"];
}

- (MGLMapDebugMaskOptions)debugMask
{
    if (!_mbglMap)
    {
        NSAssert(self.terminated, @"_mbglMap should only be unavailable during app termination");
        return self.residualDebugMask;
    }
    
    mbgl::MapDebugOptions options = self.mbglMap.getDebug();
    MGLMapDebugMaskOptions mask = 0;
    if (options & mbgl::MapDebugOptions::TileBorders)
    {
        mask |= MGLMapDebugTileBoundariesMask;
    }
    if (options & mbgl::MapDebugOptions::ParseStatus)
    {
        mask |= MGLMapDebugTileInfoMask;
    }
    if (options & mbgl::MapDebugOptions::Timestamps)
    {
        mask |= MGLMapDebugTimestampsMask;
    }
    if (options & mbgl::MapDebugOptions::Collision)
    {
        mask |= MGLMapDebugCollisionBoxesMask;
    }
    if (options & mbgl::MapDebugOptions::Overdraw)
    {
        mask |= MGLMapDebugOverdrawVisualizationMask;
    }
    return mask;
}

- (void)setDebugMask:(MGLMapDebugMaskOptions)debugMask
{
    if (!_mbglMap)
    {
        return;
    }

    mbgl::MapDebugOptions options = mbgl::MapDebugOptions::NoDebug;
    if (debugMask & MGLMapDebugTileBoundariesMask)
    {
        options |= mbgl::MapDebugOptions::TileBorders;
    }
    if (debugMask & MGLMapDebugTileInfoMask)
    {
        options |= mbgl::MapDebugOptions::ParseStatus;
    }
    if (debugMask & MGLMapDebugTimestampsMask)
    {
        options |= mbgl::MapDebugOptions::Timestamps;
    }
    if (debugMask & MGLMapDebugCollisionBoxesMask)
    {
        options |= mbgl::MapDebugOptions::Collision;
    }
    if (debugMask & MGLMapDebugOverdrawVisualizationMask)
    {
        options |= mbgl::MapDebugOptions::Overdraw;
    }
    self.mbglMap.setDebug(options);
}

- (void)resetNorth
{
    MGLLogInfo(@"Resetting the map rotation to a northern heading — a direction of 0 degrees.");
    [self resetNorthAnimated:YES];
}

- (void)resetNorthAnimated:(BOOL)animated
{
    [self setDirection:0 animated:animated];
}

- (void)resetPosition
{
    MGLLogInfo(@"Resetting the map to the current style’s default viewport.");
    auto camera = self.mbglMap.getStyle().getDefaultCamera();
    CGFloat pitch = *camera.pitch;
    CLLocationDirection heading = mbgl::util::wrap(*camera.angle, 0., 360.);
    CLLocationDistance altitude = MGLAltitudeForZoomLevel(*camera.zoom, pitch, 0, self.frame.size);
    self.camera = [MGLMapCamera cameraLookingAtCenterCoordinate:MGLLocationCoordinate2DFromLatLng(*camera.center)
                                                       altitude:altitude
                                                          pitch:pitch
                                                        heading:heading];
}

- (void)setZoomEnabled:(BOOL)zoomEnabled
{
    MGLLogDebug(@"Setting zoomEnabled: %@", MGLStringFromBOOL(zoomEnabled));
    _zoomEnabled = zoomEnabled;
    self.pinch.enabled = zoomEnabled;
    self.doubleTap.enabled = zoomEnabled;
    self.quickZoom.enabled = zoomEnabled;
    self.twoFingerTap.enabled = zoomEnabled;
}

- (void)setScrollEnabled:(BOOL)scrollEnabled
{
    MGLLogDebug(@"Setting scrollEnabled: %@", MGLStringFromBOOL(scrollEnabled));
    _scrollEnabled = scrollEnabled;
    self.pan.enabled = scrollEnabled;
}

- (void)setRotateEnabled:(BOOL)rotateEnabled
{
    MGLLogDebug(@"Setting rotateEnabled: %@", MGLStringFromBOOL(rotateEnabled));
    _rotateEnabled = rotateEnabled;
    self.rotate.enabled = rotateEnabled;
}

- (void)setPitchEnabled:(BOOL)pitchEnabled
{
    MGLLogDebug(@"Setting pitchEnabled: %@", MGLStringFromBOOL(pitchEnabled));
    _pitchEnabled = pitchEnabled;
    self.twoFingerDrag.enabled = pitchEnabled;
}

- (void)setShowsScale:(BOOL)showsScale
{
    MGLLogDebug(@"Setting showsScale: %@", MGLStringFromBOOL(showsScale));
    _showsScale = showsScale;
    self.scaleBar.hidden = !showsScale;

    if (showsScale)
    {
        [self updateScaleBar];
    }
}

#pragma mark - Accessibility -

- (NSString *)accessibilityValue
{
    NSMutableArray *facts = [NSMutableArray array];
    
    double zoomLevel = round(self.zoomLevel + 1);
    [facts addObject:[NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"MAP_A11Y_VALUE_ZOOM", nil, nil, @"Zoom %dx.", @"Map accessibility value; {zoom level}"), (int)zoomLevel]];
    
    NSInteger annotationCount = self.accessibilityAnnotationCount;
    if (annotationCount) {
        [facts addObject:[NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"MAP_A11Y_VALUE_ANNOTATIONS", nil, nil, @"%ld annotation(s) visible.", @"Map accessibility value; {number of visible annotations}"), (long)self.accessibilityAnnotationCount]];
    }
    
    NSArray *placeFeatures = self.visiblePlaceFeatures;
    if (placeFeatures.count) {
        NSMutableArray *placesArray = [NSMutableArray arrayWithCapacity:placeFeatures.count];
        NSMutableSet *placesSet = [NSMutableSet setWithCapacity:placeFeatures.count];
        for (id <MGLFeature> placeFeature in placeFeatures.reverseObjectEnumerator) {
            NSString *name = [placeFeature attributeForKey:@"name"];
            if (![placesSet containsObject:name]) {
                [placesArray addObject:name];
                [placesSet addObject:name];
            }
            if (placesArray.count >= 3) {
                break;
            }
        }
        NSString *placesString = [placesArray componentsJoinedByString:NSLocalizedStringWithDefaultValue(@"LIST_SEPARATOR", nil, nil, @", ", @"List separator")];
        [facts addObject:[NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"MAP_A11Y_VALUE_PLACES", nil, nil, @"Places visible: %@.", @"Map accessibility value; {list of visible places}"), placesString]];
    }
    
    NSArray *roadFeatures = self.visibleRoadFeatures;
    if (roadFeatures.count) {
        [facts addObject:[NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"MAP_A11Y_VALUE_ROADS", nil, nil, @"%ld road(s) visible.", @"Map accessibility value; {number of visible roads}"), roadFeatures.count]];
    }
    
    NSString *value = [facts componentsJoinedByString:@" "];
    return value;
}

- (NSArray<id <MGLFeature>> *)visiblePlaceFeatures
{
    if (!_visiblePlaceFeatures)
    {
        NSArray *placeStyleLayerIdentifiers = [self.style.placeStyleLayers valueForKey:@"identifier"];
        _visiblePlaceFeatures = [self visibleFeaturesInRect:self.bounds inStyleLayersWithIdentifiers:[NSSet setWithArray:placeStyleLayerIdentifiers]];
    }
    return _visiblePlaceFeatures;
}

- (NSArray<id <MGLFeature>> *)visibleRoadFeatures
{
    if (!_visibleRoadFeatures)
    {
        NSArray *roadStyleLayerIdentifiers = [self.style.roadStyleLayers valueForKey:@"identifier"];
        _visibleRoadFeatures = [self visibleFeaturesInRect:self.bounds inStyleLayersWithIdentifiers:[NSSet setWithArray:roadStyleLayerIdentifiers]];
    }
    return _visibleRoadFeatures;
}

- (CGRect)accessibilityFrame
{
    CGRect frame = [super accessibilityFrame];
    UIViewController *viewController = self.viewControllerForLayoutGuides;
    if (viewController)
    {
        CGFloat topInset = viewController.topLayoutGuide.length;
        frame.origin.y += topInset;
        frame.size.height -= topInset + viewController.bottomLayoutGuide.length;
    }
    return frame;
}

- (UIBezierPath *)accessibilityPath
{
    UIBezierPath *path = [UIBezierPath bezierPathWithRect:self.accessibilityFrame];

    // Exclude any visible annotation callout view.
    if (self.calloutViewForSelectedAnnotation)
    {
        UIBezierPath *calloutViewPath = [UIBezierPath bezierPathWithRect:self.calloutViewForSelectedAnnotation.frame];
        [path appendPath:calloutViewPath];
    }

    return path;
}

- (NSInteger)accessibilityElementCount
{
    if (self.calloutViewForSelectedAnnotation)
    {
        return 2 /* calloutViewForSelectedAnnotation, mapViewProxyAccessibilityElement */;
    }
    return !!self.userLocationAnnotationView + self.accessibilityAnnotationCount + self.visiblePlaceFeatures.count + self.visibleRoadFeatures.count + 2 /* compass, attributionButton */;
}

- (NSInteger)accessibilityAnnotationCount
{
    std::vector<MGLAnnotationTag> visibleAnnotations = [self annotationTagsInRect:self.bounds];
    return visibleAnnotations.size();
}

- (id)accessibilityElementAtIndex:(NSInteger)index
{
    if (self.calloutViewForSelectedAnnotation)
    {
        if (index == 0)
        {
            return self.calloutViewForSelectedAnnotation;
        }
        if (index == 1)
        {
            self.mapViewProxyAccessibilityElement.accessibilityFrame = self.accessibilityFrame;
            self.mapViewProxyAccessibilityElement.accessibilityPath = self.accessibilityPath;
            return self.mapViewProxyAccessibilityElement;
        }
        return nil;
    }
    
    // Compass
    NSInteger compassIndex = 0;
    if (index == compassIndex)
    {
        return self.compassView;
    }
    
    // User location annotation
    NSRange userLocationAnnotationRange = NSMakeRange(compassIndex + 1, !!self.userLocationAnnotationView);
    if (NSLocationInRange(index, userLocationAnnotationRange))
    {
        return self.userLocationAnnotationView;
    }
    
    CGPoint centerPoint = self.contentCenter;
    if (self.userTrackingMode != MGLUserTrackingModeNone)
    {
        centerPoint = self.userLocationAnnotationViewCenter;
    }
    
    // Visible annotations
    std::vector<MGLAnnotationTag> visibleAnnotations = [self annotationTagsInRect:self.bounds];
    NSRange visibleAnnotationRange = NSMakeRange(NSMaxRange(userLocationAnnotationRange), visibleAnnotations.size());
    if (NSLocationInRange(index, visibleAnnotationRange))
    {
        std::sort(visibleAnnotations.begin(), visibleAnnotations.end());
        std::sort(visibleAnnotations.begin(), visibleAnnotations.end(), [&](const MGLAnnotationTag tagA, const MGLAnnotationTag tagB) {
            CLLocationCoordinate2D coordinateA = [[self annotationWithTag:tagA] coordinate];
            CLLocationCoordinate2D coordinateB = [[self annotationWithTag:tagB] coordinate];
            CGPoint pointA = [self convertCoordinate:coordinateA toPointToView:self];
            CGPoint pointB = [self convertCoordinate:coordinateB toPointToView:self];
            CGFloat deltaA = hypot(pointA.x - centerPoint.x, pointA.y - centerPoint.y);
            CGFloat deltaB = hypot(pointB.x - centerPoint.x, pointB.y - centerPoint.y);
            return deltaA < deltaB;
        });
        
        NSUInteger annotationIndex = index - visibleAnnotationRange.location;
        MGLAnnotationTag annotationTag = visibleAnnotations[annotationIndex];
        MGLAssert(annotationTag != MGLAnnotationTagNotFound, @"Can’t get accessibility element for nonexistent or invisible annotation at index %li.", (long)index);
        return [self accessibilityElementForAnnotationWithTag:annotationTag];
    }
    
    // Visible place features
    NSArray *visiblePlaceFeatures = self.visiblePlaceFeatures;
    NSRange visiblePlaceFeatureRange = NSMakeRange(NSMaxRange(visibleAnnotationRange), visiblePlaceFeatures.count);
    if (NSLocationInRange(index, visiblePlaceFeatureRange))
    {
        visiblePlaceFeatures = [visiblePlaceFeatures sortedArrayUsingComparator:^NSComparisonResult(id <MGLFeature> _Nonnull featureA, id <MGLFeature> _Nonnull featureB) {
            CGPoint pointA = [self convertCoordinate:featureA.coordinate toPointToView:self];
            CGPoint pointB = [self convertCoordinate:featureB.coordinate toPointToView:self];
            CGFloat deltaA = hypot(pointA.x - centerPoint.x, pointA.y - centerPoint.y);
            CGFloat deltaB = hypot(pointB.x - centerPoint.x, pointB.y - centerPoint.y);
            return [@(deltaA) compare:@(deltaB)];
        }];
        
        id <MGLFeature> feature = visiblePlaceFeatures[index - visiblePlaceFeatureRange.location];
        return [self accessibilityElementForPlaceFeature:feature];
    }
    
    // Visible road features
    NSArray *visibleRoadFeatures = self.visibleRoadFeatures;
    NSRange visibleRoadFeatureRange = NSMakeRange(NSMaxRange(visiblePlaceFeatureRange), visibleRoadFeatures.count);
    if (NSLocationInRange(index, visibleRoadFeatureRange))
    {
        visibleRoadFeatures = [visibleRoadFeatures sortedArrayUsingComparator:^NSComparisonResult(id <MGLFeature> _Nonnull featureA, id <MGLFeature> _Nonnull featureB) {
            CGPoint pointA = [self convertCoordinate:featureA.coordinate toPointToView:self];
            CGPoint pointB = [self convertCoordinate:featureB.coordinate toPointToView:self];
            CGFloat deltaA = hypot(pointA.x - centerPoint.x, pointA.y - centerPoint.y);
            CGFloat deltaB = hypot(pointB.x - centerPoint.x, pointB.y - centerPoint.y);
            return [@(deltaA) compare:@(deltaB)];
        }];
        
        id <MGLFeature> feature = visibleRoadFeatures[index - visibleRoadFeatureRange.location];
        return [self accessibilityElementForRoadFeature:feature];
    }
    
    // Attribution button
    NSInteger attributionButtonIndex = NSMaxRange(visibleRoadFeatureRange);
    if (index == attributionButtonIndex)
    {
        return self.attributionButton;
    }
    
    MGLAssert(NO, @"Index %ld not in recognized accessibility element ranges. "
             @"User location annotation range: %@; visible annotation range: %@; "
             @"visible place feature range: %@; visible road feature range: %@.",
             (long)index, NSStringFromRange(userLocationAnnotationRange),
             NSStringFromRange(visibleAnnotationRange), NSStringFromRange(visiblePlaceFeatureRange),
             NSStringFromRange(visibleRoadFeatureRange));
    return nil;
}

/**
 Returns an accessibility element corresponding to a visible annotation with the given tag.
 
 @param annotationTag Tag of the annotation represented by the accessibility element to return.
 */
- (id)accessibilityElementForAnnotationWithTag:(MGLAnnotationTag)annotationTag
{
    MGLAssert(_annotationContextsByAnnotationTag.count(annotationTag), @"Missing annotation for tag %llu.", annotationTag);
    MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(annotationTag);
    id <MGLAnnotation> annotation = annotationContext.annotation;
    
    // Let the annotation view serve as its own accessibility element.
    MGLAnnotationView *annotationView = annotationContext.annotationView;
    if (annotationView && annotationView.superview)
    {
        return annotationView;
    }
    
    // Lazily create an accessibility element for the found annotation.
    if ( ! annotationContext.accessibilityElement)
    {
        annotationContext.accessibilityElement = [[MGLAnnotationAccessibilityElement alloc] initWithAccessibilityContainer:self tag:annotationTag];
    }
    
    // Update the accessibility element.
    MGLAnnotationImage *annotationImage = [self imageOfAnnotationWithTag:annotationTag];
    CGRect annotationFrame = [self frameOfImage:annotationImage.image centeredAtCoordinate:annotation.coordinate];
    CGPoint annotationFrameCenter = CGPointMake(CGRectGetMidX(annotationFrame), CGRectGetMidY(annotationFrame));
    CGRect minimumFrame = CGRectInset({ annotationFrameCenter, CGSizeZero },
                                      -MGLAnnotationAccessibilityElementMinimumSize.width / 2,
                                      -MGLAnnotationAccessibilityElementMinimumSize.height / 2);
    annotationFrame = CGRectUnion(annotationFrame, minimumFrame);
    CGRect screenRect = UIAccessibilityConvertFrameToScreenCoordinates(annotationFrame, self);
    annotationContext.accessibilityElement.accessibilityFrame = screenRect;
    
    if ([annotation respondsToSelector:@selector(title)])
    {
        annotationContext.accessibilityElement.accessibilityLabel = annotation.title;
    }
    if ([annotation respondsToSelector:@selector(subtitle)])
    {
        annotationContext.accessibilityElement.accessibilityValue = annotation.subtitle;
    }
    
    return annotationContext.accessibilityElement;
}

/**
 Returns an accessibility element corresponding to the given place feature.
 
 @param feature The place feature represented by the accessibility element.
 */
- (id)accessibilityElementForPlaceFeature:(id <MGLFeature>)feature
{
    if (!_featureAccessibilityElements)
    {
        _featureAccessibilityElements = [NSMutableSet set];
    }
    
    MGLFeatureAccessibilityElement *element = [_featureAccessibilityElements objectsPassingTest:^BOOL(MGLFeatureAccessibilityElement * _Nonnull testElement, BOOL * _Nonnull stop) {
        return testElement.feature.identifier && ![testElement.feature.identifier isEqual:@0] && [testElement.feature.identifier isEqual:feature.identifier];
    }].anyObject;
    if (!element)
    {
        element = [[MGLPlaceFeatureAccessibilityElement alloc] initWithAccessibilityContainer:self feature:feature];
    }
    CGPoint center = [self convertCoordinate:feature.coordinate toPointToView:self];
    CGRect annotationFrame = CGRectInset({center, CGSizeZero}, -MGLAnnotationAccessibilityElementMinimumSize.width / 2, -MGLAnnotationAccessibilityElementMinimumSize.width / 2);
    CGRect screenRect = UIAccessibilityConvertFrameToScreenCoordinates(annotationFrame, self);
    element.accessibilityFrame = screenRect;
    
    [_featureAccessibilityElements addObject:element];
    
    return element;
}

/**
 Returns an accessibility element corresponding to the given road feature.
 
 @param feature The road feature represented by the accessibility element.
 */
- (id)accessibilityElementForRoadFeature:(id <MGLFeature>)feature
{
    if (!_featureAccessibilityElements)
    {
        _featureAccessibilityElements = [NSMutableSet set];
    }
    
    MGLFeatureAccessibilityElement *element = [_featureAccessibilityElements objectsPassingTest:^BOOL(MGLFeatureAccessibilityElement * _Nonnull testElement, BOOL * _Nonnull stop) {
        return testElement.feature.identifier && ![testElement.feature.identifier isEqual:@0] && [testElement.feature.identifier isEqual:feature.identifier];
    }].anyObject;
    if (!element)
    {
        element = [[MGLRoadFeatureAccessibilityElement alloc] initWithAccessibilityContainer:self feature:feature];
    }
    
    UIBezierPath *path;
    if ([feature isKindOfClass:[MGLPointFeature class]])
    {
        CGPoint center = [self convertCoordinate:feature.coordinate toPointToView:self];
        CGRect annotationFrame = CGRectInset({center, CGSizeZero}, -MGLAnnotationAccessibilityElementMinimumSize.width / 2, -MGLAnnotationAccessibilityElementMinimumSize.width / 2);
        CGRect screenRect = UIAccessibilityConvertFrameToScreenCoordinates(annotationFrame, self);
        element.accessibilityFrame = screenRect;
    }
    else if ([feature isKindOfClass:[MGLPolylineFeature class]])
    {
        path = [self pathOfPolyline:(MGLPolyline *)feature];
    }
    else if ([feature isKindOfClass:[MGLMultiPolylineFeature class]])
    {
        path = [UIBezierPath bezierPath];
        for (MGLPolyline *polyline in [(MGLMultiPolylineFeature *)feature polylines])
        {
            [path appendPath:[self pathOfPolyline:polyline]];
        }
    }
    
    if (path)
    {
        CGPathRef strokedCGPath = CGPathCreateCopyByStrokingPath(path.CGPath, NULL, MGLAnnotationAccessibilityElementMinimumSize.width, kCGLineCapButt, kCGLineJoinMiter, 0);
        UIBezierPath *strokedPath = [UIBezierPath bezierPathWithCGPath:strokedCGPath];
        CGPathRelease(strokedCGPath);
        UIBezierPath *screenPath = UIAccessibilityConvertPathToScreenCoordinates(strokedPath, self);
        element.accessibilityPath = screenPath;
    }
    
    [_featureAccessibilityElements addObject:element];
    
    return element;
}

- (UIBezierPath *)pathOfPolyline:(MGLPolyline *)polyline
{
    CLLocationCoordinate2D *coordinates = polyline.coordinates;
    NSUInteger pointCount = polyline.pointCount;
    UIBezierPath *path = [UIBezierPath bezierPath];
    for (NSUInteger i = 0; i < pointCount; i++)
    {
        CGPoint point = [self convertCoordinate:coordinates[i] toPointToView:self];
        if (i)
        {
            [path addLineToPoint:point];
        }
        else
        {
            [path moveToPoint:point];
        }
    }
    return path;
}

- (NSInteger)indexOfAccessibilityElement:(id)element
{
    if (self.calloutViewForSelectedAnnotation)
    {
        return [@[self.calloutViewForSelectedAnnotation, self.mapViewProxyAccessibilityElement]
                indexOfObject:element];
    }
    
    // Compass
    NSUInteger compassIndex = 0;
    if (element == self.compassView)
    {
        return compassIndex;
    }
    
    // User location annotation
    NSRange userLocationAnnotationRange = NSMakeRange(compassIndex + 1, !!self.userLocationAnnotationView);
    if (element == self.userLocationAnnotationView)
    {
        return userLocationAnnotationRange.location;
    }
    
    CGPoint centerPoint = self.contentCenter;
    if (self.userTrackingMode != MGLUserTrackingModeNone)
    {
        centerPoint = self.userLocationAnnotationViewCenter;
    }
    
    // Visible annotations
    std::vector<MGLAnnotationTag> visibleAnnotations = [self annotationTagsInRect:self.bounds];
    NSRange visibleAnnotationRange = NSMakeRange(NSMaxRange(userLocationAnnotationRange), visibleAnnotations.size());
    MGLAnnotationTag tag = MGLAnnotationTagNotFound;
    if ([element isKindOfClass:[MGLAnnotationView class]])
    {
        id <MGLAnnotation> annotation = [(MGLAnnotationView *)element annotation];
        tag = [self annotationTagForAnnotation:annotation];
    }
    else if ([element isKindOfClass:[MGLAnnotationAccessibilityElement class]])
    {
        tag = [(MGLAnnotationAccessibilityElement *)element tag];
    }
    
    if (tag != MGLAnnotationTagNotFound)
    {
        std::sort(visibleAnnotations.begin(), visibleAnnotations.end());
        std::sort(visibleAnnotations.begin(), visibleAnnotations.end(), [&](const MGLAnnotationTag tagA, const MGLAnnotationTag tagB) {
            CLLocationCoordinate2D coordinateA = [[self annotationWithTag:tagA] coordinate];
            CLLocationCoordinate2D coordinateB = [[self annotationWithTag:tagB] coordinate];
            CGPoint pointA = [self convertCoordinate:coordinateA toPointToView:self];
            CGPoint pointB = [self convertCoordinate:coordinateB toPointToView:self];
            CGFloat deltaA = hypot(pointA.x - centerPoint.x, pointA.y - centerPoint.y);
            CGFloat deltaB = hypot(pointB.x - centerPoint.x, pointB.y - centerPoint.y);
            return deltaA < deltaB;
        });
        
        auto foundElement = std::find(visibleAnnotations.begin(), visibleAnnotations.end(), tag);
        if (foundElement == visibleAnnotations.end())
        {
            return NSNotFound;
        }
        return visibleAnnotationRange.location + std::distance(visibleAnnotations.begin(), foundElement);
    }
    
    // Visible place features
    NSArray *visiblePlaceFeatures = self.visiblePlaceFeatures;
    NSRange visiblePlaceFeatureRange = NSMakeRange(NSMaxRange(visibleAnnotationRange), visiblePlaceFeatures.count);
    if ([element isKindOfClass:[MGLPlaceFeatureAccessibilityElement class]])
    {
        visiblePlaceFeatures = [visiblePlaceFeatures sortedArrayUsingComparator:^NSComparisonResult(id <MGLFeature> _Nonnull featureA, id <MGLFeature> _Nonnull featureB) {
            CGPoint pointA = [self convertCoordinate:featureA.coordinate toPointToView:self];
            CGPoint pointB = [self convertCoordinate:featureB.coordinate toPointToView:self];
            CGFloat deltaA = hypot(pointA.x - centerPoint.x, pointA.y - centerPoint.y);
            CGFloat deltaB = hypot(pointB.x - centerPoint.x, pointB.y - centerPoint.y);
            return [@(deltaA) compare:@(deltaB)];
        }];
        
        id <MGLFeature> feature = [(MGLPlaceFeatureAccessibilityElement *)element feature];
        NSUInteger featureIndex = [visiblePlaceFeatures indexOfObject:feature];
        if (featureIndex == NSNotFound)
        {
            featureIndex = [visiblePlaceFeatures indexOfObjectPassingTest:^BOOL (id <MGLFeature> _Nonnull visibleFeature, NSUInteger idx, BOOL * _Nonnull stop) {
                return visibleFeature.identifier && ![visibleFeature.identifier isEqual:@0] && [visibleFeature.identifier isEqual:feature.identifier];
            }];
        }
        if (featureIndex == NSNotFound)
        {
            return NSNotFound;
        }
        return visiblePlaceFeatureRange.location + featureIndex;
    }
    
    // Visible road features
    NSArray *visibleRoadFeatures = self.visibleRoadFeatures;
    NSRange visibleRoadFeatureRange = NSMakeRange(NSMaxRange(visiblePlaceFeatureRange), visibleRoadFeatures.count);
    if ([element isKindOfClass:[MGLRoadFeatureAccessibilityElement class]])
    {
        visibleRoadFeatures = [visibleRoadFeatures sortedArrayUsingComparator:^NSComparisonResult(id <MGLFeature> _Nonnull featureA, id <MGLFeature> _Nonnull featureB) {
            CGPoint pointA = [self convertCoordinate:featureA.coordinate toPointToView:self];
            CGPoint pointB = [self convertCoordinate:featureB.coordinate toPointToView:self];
            CGFloat deltaA = hypot(pointA.x - centerPoint.x, pointA.y - centerPoint.y);
            CGFloat deltaB = hypot(pointB.x - centerPoint.x, pointB.y - centerPoint.y);
            return [@(deltaA) compare:@(deltaB)];
        }];
        
        id <MGLFeature> feature = [(MGLRoadFeatureAccessibilityElement *)element feature];
        NSUInteger featureIndex = [visibleRoadFeatures indexOfObject:feature];
        if (featureIndex == NSNotFound)
        {
            featureIndex = [visibleRoadFeatures indexOfObjectPassingTest:^BOOL (id <MGLFeature> _Nonnull visibleFeature, NSUInteger idx, BOOL * _Nonnull stop) {
                return visibleFeature.identifier && ![visibleFeature.identifier isEqual:@0] && [visibleFeature.identifier isEqual:feature.identifier];
            }];
        }
        if (featureIndex == NSNotFound)
        {
            return NSNotFound;
        }
        return visibleRoadFeatureRange.location + featureIndex;
    }
    
    // Attribution button
    NSUInteger attributionButtonIndex = NSMaxRange(visibleRoadFeatureRange);
    if (element == self.attributionButton)
    {
        return attributionButtonIndex;
    }
    
    return NSNotFound;
}

- (MGLMapViewProxyAccessibilityElement *)mapViewProxyAccessibilityElement
{
    if ( ! _mapViewProxyAccessibilityElement)
    {
        _mapViewProxyAccessibilityElement = [[MGLMapViewProxyAccessibilityElement alloc] initWithAccessibilityContainer:self];
    }
    return _mapViewProxyAccessibilityElement;
}

- (void)accessibilityIncrement
{
    // Swipe up to zoom out.
    [self accessibilityScaleBy:0.5];
}

- (void)accessibilityDecrement
{
    // Swipe down to zoom in.
    [self accessibilityScaleBy:2];
}

- (void)accessibilityScaleBy:(double)scaleFactor
{
    CGPoint centerPoint = self.contentCenter;
    if (self.userTrackingMode != MGLUserTrackingModeNone)
    {
        centerPoint = self.userLocationAnnotationViewCenter;
    }
    double newZoom = round(self.zoomLevel) + log2(scaleFactor);
    self.mbglMap.setZoom(newZoom, mbgl::ScreenCoordinate { centerPoint.x, centerPoint.y });
    [self unrotateIfNeededForGesture];

    _accessibilityValueAnnouncementIsPending = YES;
}

#pragma mark - Geography -

+ (NSSet<NSString *> *)keyPathsForValuesAffectingCenterCoordinate
{
    return [NSSet setWithObjects:@"latitude", @"longitude", @"camera", nil];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)coordinate animated:(BOOL)animated
{
    MGLLogDebug(@"Setting centerCoordinate: %@ animated: %@", MGLStringFromCLLocationCoordinate2D(coordinate), MGLStringFromBOOL(animated));
    [self setCenterCoordinate:coordinate zoomLevel:self.zoomLevel animated:animated];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
{
    MGLLogDebug(@"Setting centerCoordinate: %@", MGLStringFromCLLocationCoordinate2D(centerCoordinate));
    [self setCenterCoordinate:centerCoordinate animated:NO];
}

- (CLLocationCoordinate2D)centerCoordinate
{
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    return MGLLocationCoordinate2DFromLatLng(self.mbglMap.getLatLng(padding));
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel animated:(BOOL)animated
{
    MGLLogDebug(@"Setting centerCoordinate: %@ zoomLevel: %f animated: %@",
                MGLStringFromCLLocationCoordinate2D(centerCoordinate),
                zoomLevel,
                MGLStringFromBOOL(animated));
    [self setCenterCoordinate:centerCoordinate zoomLevel:zoomLevel direction:self.direction animated:animated];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction animated:(BOOL)animated
{
    MGLLogDebug(@"Setting centerCoordinate: %@ zoomLevel: %f direction: %f animated: %@",
                MGLStringFromCLLocationCoordinate2D(centerCoordinate),
                zoomLevel,
                direction,
                MGLStringFromBOOL(animated));
    [self setCenterCoordinate:centerCoordinate zoomLevel:zoomLevel direction:direction animated:animated completionHandler:NULL];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction animated:(BOOL)animated completionHandler:(nullable void (^)(void))completion
{
    MGLLogDebug(@"Setting centerCoordinate: %@ zoomLevel: %f direction: %f animated: %@ completionHandler: %@",
                MGLStringFromCLLocationCoordinate2D(centerCoordinate),
                zoomLevel,
                direction,
                MGLStringFromBOOL(animated),
                completion);
    self.userTrackingMode = MGLUserTrackingModeNone;

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonProgrammatic;

    [self _setCenterCoordinate:centerCoordinate edgePadding:self.contentInset zoomLevel:zoomLevel direction:direction duration:animated ? MGLAnimationDuration : 0 animationTimingFunction:nil completionHandler:completion];
}

- (void)_setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate animated:(BOOL)animated {
    [self _setCenterCoordinate:centerCoordinate edgePadding:self.contentInset zoomLevel:self.zoomLevel direction:self.direction duration:animated ? MGLAnimationDuration : 0 animationTimingFunction:nil completionHandler:NULL];
}

- (void)_setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate edgePadding:(UIEdgeInsets)insets zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction duration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion
{
    if (!_mbglMap)
    {
        if (completion)
        {
            completion();
        }
        return;
    }
    
    mbgl::CameraOptions cameraOptions;
    cameraOptions.center = MGLLatLngFromLocationCoordinate2D(centerCoordinate);
    cameraOptions.padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    cameraOptions.zoom = zoomLevel;
    if (direction >= 0)
    {
        cameraOptions.angle = direction;
    }

    mbgl::AnimationOptions animationOptions;
    if (duration)
    {
        animationOptions.duration.emplace(MGLDurationFromTimeInterval(duration));
        animationOptions.easing.emplace(MGLUnitBezierForMediaTimingFunction(function));
    }
    if (completion)
    {
        animationOptions.transitionFinishFn = [completion]() {
            // Must run asynchronously after the transition is completely over.
            // Otherwise, a call to -setCenterCoordinate: within the completion
            // handler would reenter the completion handler’s caller.
            dispatch_async(dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    
    MGLMapCamera *camera = [self cameraForCameraOptions:cameraOptions];
    if ([self.camera isEqualToMapCamera:camera])
    {
        if (completion)
        {
            [self animateWithDelay:duration animations:^{
                completion();
            }];
        }
        return;
    }
    
    [self cancelTransitions];

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonProgrammatic;

    self.mbglMap.easeTo(cameraOptions, animationOptions);
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingZoomLevel
{
    return [NSSet setWithObject:@"camera"];
}

- (double)zoomLevel
{
    return self.mbglMap.getZoom();
}

- (void)setZoomLevel:(double)zoomLevel
{
    MGLLogDebug(@"Setting zoomLevel: %f", zoomLevel);
    [self setZoomLevel:zoomLevel animated:NO];
}

- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated
{
    MGLLogDebug(@"Setting zoomLevel: %f animated: %@", zoomLevel, MGLStringFromBOOL(animated));
    if (zoomLevel == self.zoomLevel) return;
    [self cancelTransitions];

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonProgrammatic;

    CGFloat duration = animated ? MGLAnimationDuration : 0;

    self.mbglMap.setZoom(zoomLevel,
                         MGLEdgeInsetsFromNSEdgeInsets(self.contentInset),
                         MGLDurationFromTimeInterval(duration));
}

- (void)setMinimumZoomLevel:(double)minimumZoomLevel
{
    MGLLogDebug(@"Setting minimumZoomLevel: %f", minimumZoomLevel);
    self.mbglMap.setMinZoom(minimumZoomLevel);
}

- (double)minimumZoomLevel
{
    return self.mbglMap.getMinZoom();
}

- (void)setMaximumZoomLevel:(double)maximumZoomLevel
{
    MGLLogDebug(@"Setting maximumZoomLevel: %f", maximumZoomLevel);
    self.mbglMap.setMaxZoom(maximumZoomLevel);
}

- (double)maximumZoomLevel
{
    return self.mbglMap.getMaxZoom();
}

- (MGLCoordinateBounds)visibleCoordinateBounds
{
    return [self convertRect:self.bounds toCoordinateBoundsFromView:self];
}

- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds
{
    MGLLogDebug(@"Setting visibleCoordinateBounds: %@", MGLStringFromCoordinateBounds(bounds));
    [self setVisibleCoordinateBounds:bounds animated:NO];
}

- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds animated:(BOOL)animated
{
    MGLLogDebug(@"Setting visibleCoordinateBounds: %@ animated: %@", MGLStringFromCoordinateBounds(bounds), MGLStringFromBOOL(animated));
    [self setVisibleCoordinateBounds:bounds edgePadding:UIEdgeInsetsZero animated:animated];
}

- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(UIEdgeInsets)insets animated:(BOOL)animated
{
    MGLLogDebug(@"Setting visibleCoordinateBounds: %@ edgePadding: %@ animated: %@",
                MGLStringFromCoordinateBounds(bounds),
                NSStringFromUIEdgeInsets(insets),
                MGLStringFromBOOL(animated));
    CLLocationCoordinate2D coordinates[] = {
        {bounds.ne.latitude, bounds.sw.longitude},
        bounds.sw,
        {bounds.sw.latitude, bounds.ne.longitude},
        bounds.ne,
    };
    [self setVisibleCoordinates:coordinates
                          count:sizeof(coordinates) / sizeof(coordinates[0])
                    edgePadding:insets
                       animated:animated];
}

- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction animated:(BOOL)animated
{
    MGLLogDebug(@"Setting visibleCoordinateBounds: %@ edgePadding: %@ direction: %f animated: %@",
                MGLStringFromCoordinateBounds(bounds),
                NSStringFromUIEdgeInsets(insets),
                direction,
                MGLStringFromBOOL(animated));
    CLLocationCoordinate2D coordinates[] = {
        {bounds.ne.latitude, bounds.sw.longitude},
        bounds.sw,
        {bounds.sw.latitude, bounds.ne.longitude},
        bounds.ne,
    };
    [self setVisibleCoordinates:coordinates
                          count:sizeof(coordinates) / sizeof(coordinates[0])
                    edgePadding:insets
                      direction:direction
                       animated:animated];
}

- (void)setVisibleCoordinates:(const CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets animated:(BOOL)animated
{
    MGLLogDebug(@"Setting: %lu coordinates edgePadding: %@ animated: %@",
                count,
                NSStringFromUIEdgeInsets(insets),
                MGLStringFromBOOL(animated));
    [self setVisibleCoordinates:coordinates count:count edgePadding:insets direction:self.direction animated:animated];
}

- (void)setVisibleCoordinates:(const CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction animated:(BOOL)animated
{
    MGLLogDebug(@"Setting: %lu coordinates edgePadding: %@ direction: %f animated: %@",
                count,
                NSStringFromUIEdgeInsets(insets),
                direction,
                MGLStringFromBOOL(animated));
    [self setVisibleCoordinates:coordinates count:count edgePadding:insets direction:direction duration:animated ? MGLAnimationDuration : 0 animationTimingFunction:nil];
}

- (void)setVisibleCoordinates:(const CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction duration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function {
    MGLLogDebug(@"Setting: %lu coordinates edgePadding: %@ direction: %f duration: %f animationTimingFunction: %@",
                count,
                NSStringFromUIEdgeInsets(insets),
                direction,
                duration,
                function);
    [self setVisibleCoordinates:coordinates count:count edgePadding:insets direction:direction duration:duration animationTimingFunction:function completionHandler:NULL];
}

- (void)setVisibleCoordinates:(const CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction duration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion
{
    MGLLogDebug(@"Setting: %lu coordinates edgePadding: %@ direction: %f duration: %f animationTimingFunction: %@ completionHandler: %@", count, NSStringFromUIEdgeInsets(insets), direction, duration, function, completion);
    self.userTrackingMode = MGLUserTrackingModeNone;

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonProgrammatic;

    [self _setVisibleCoordinates:coordinates count:count edgePadding:insets direction:direction duration:duration animationTimingFunction:function completionHandler:completion];
}

- (void)_setVisibleCoordinates:(const CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction duration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion
{
    if (!_mbglMap)
    {
        if (completion)
        {
            completion();
        }
        return;
    }
    
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    padding += MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    std::vector<mbgl::LatLng> latLngs;
    latLngs.reserve(count);
    for (NSUInteger i = 0; i < count; i++)
    {
        latLngs.push_back({coordinates[i].latitude, coordinates[i].longitude});
    }
    
    CLLocationDirection cameraDirection = direction >= 0 ? direction : 0;

    mbgl::CameraOptions cameraOptions = self.mbglMap.cameraForLatLngs(latLngs, padding, cameraDirection);

    mbgl::AnimationOptions animationOptions;
    if (duration > 0)
    {
        animationOptions.duration.emplace(MGLDurationFromTimeInterval(duration));
        animationOptions.easing.emplace(MGLUnitBezierForMediaTimingFunction(function));
    }
    if (completion)
    {
        animationOptions.transitionFinishFn = [completion]() {
            dispatch_async(dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    
    MGLMapCamera *camera = [self cameraForCameraOptions:cameraOptions];
    if ([self.camera isEqualToMapCamera:camera])
    {
        if (completion)
        {
            [self animateWithDelay:duration animations:^{
                completion();
            }];
        }
        return;
    }
    
    [self willChangeValueForKey:@"visibleCoordinateBounds"];
    [self cancelTransitions];

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonProgrammatic;

    self.mbglMap.easeTo(cameraOptions, animationOptions);
    [self didChangeValueForKey:@"visibleCoordinateBounds"];
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingDirection
{
    return [NSSet setWithObject:@"camera"];
}

- (CLLocationDirection)direction
{
    return mbgl::util::wrap(self.mbglMap.getBearing(), 0., 360.);
}

- (void)setDirection:(CLLocationDirection)direction animated:(BOOL)animated
{
    MGLLogDebug(@"Setting direction: %f animated: %@", direction, MGLStringFromBOOL(animated));
    if ( ! animated && ! self.rotationAllowed) return;

    if (self.userTrackingMode == MGLUserTrackingModeFollowWithHeading)
    {
        self.userTrackingMode = MGLUserTrackingModeFollow;
    }

    [self _setDirection:direction animated:animated];
}

- (void)_setDirection:(CLLocationDirection)direction animated:(BOOL)animated
{
    if (!_mbglMap)
    {
        return;
    }
    
    if (direction == self.direction) return;
    [self cancelTransitions];

    CGFloat duration = animated ? MGLAnimationDuration : 0;

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonProgrammatic;

    if (self.userTrackingMode == MGLUserTrackingModeNone)
    {
        self.mbglMap.setBearing(direction,
                             MGLEdgeInsetsFromNSEdgeInsets(self.contentInset),
                             MGLDurationFromTimeInterval(duration));
    }
    else
    {
        CGPoint centerPoint = self.userLocationAnnotationViewCenter;
        self.mbglMap.setBearing(direction, mbgl::ScreenCoordinate { centerPoint.x, centerPoint.y },
                             MGLDurationFromTimeInterval(duration));
    }
}

- (void)setDirection:(CLLocationDirection)direction
{
    MGLLogDebug(@"Setting direction: %f", direction);
    [self setDirection:direction animated:NO];
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingPitch
{
    return [NSSet setWithObject:@"camera"];
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingCamera
{
    return [NSSet setWithObjects:@"longitude", @"latitude", @"centerCoordinate", @"zoomLevel", @"direction", nil];
}

- (MGLMapCamera *)camera
{
    if (!_mbglMap)
    {
        NSAssert(self.terminated, @"_mbglMap should only be unavailable during app termination");
        return self.residualCamera;
    }
    
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    return [self cameraForCameraOptions:self.mbglMap.getCameraOptions(padding)];
}

- (void)setCamera:(MGLMapCamera *)camera
{
    MGLLogDebug(@"Setting camera: %@", camera);
    [self setCamera:camera animated:NO];
}

- (void)setCamera:(MGLMapCamera *)camera animated:(BOOL)animated
{
    MGLLogDebug(@"Setting camera: %@ animated: %@", camera, MGLStringFromBOOL(animated));
    [self setCamera:camera withDuration:animated ? MGLAnimationDuration : 0 animationTimingFunction:nil];
}

- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function
{
    MGLLogDebug(@"Setting camera: %@ duration: %f animationTimingFunction: %@", camera, duration, function);
    [self setCamera:camera withDuration:duration animationTimingFunction:function completionHandler:NULL];
}

- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion
{
    MGLLogDebug(@"Setting camera: %@ duration: %f animationTimingFunction: %@ completionHandler: %@", camera, duration, function, completion);
    [self setCamera:camera withDuration:duration animationTimingFunction:function edgePadding:self.contentInset completionHandler:completion];
}

- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function edgePadding:(UIEdgeInsets)edgePadding completionHandler:(nullable void (^)(void))completion {
    if (!_mbglMap)
    {
        if (completion)
        {
            completion();
        }
        return;
    }

    MGLLogDebug(@"Setting camera: %@ duration: %f animationTimingFunction: %@ edgePadding: %@ completionHandler: %@", camera, duration, function, NSStringFromUIEdgeInsets(edgePadding), completion);

    mbgl::AnimationOptions animationOptions;
    if (duration > 0)
    {
        animationOptions.duration.emplace(MGLDurationFromTimeInterval(duration));
        animationOptions.easing.emplace(MGLUnitBezierForMediaTimingFunction(function));
    }
    if (completion)
    {
        animationOptions.transitionFinishFn = [completion]() {
            dispatch_async(dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    
    if ([self.camera isEqualToMapCamera:camera])
    {
        if (completion)
        {
            [self animateWithDelay:duration animations:^{
                completion();
            }];
        }
        return;
    }

    [self willChangeValueForKey:@"camera"];
    [self cancelTransitions];

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonProgrammatic;

    mbgl::CameraOptions cameraOptions = [self cameraOptionsObjectForAnimatingToCamera:camera edgePadding:edgePadding];
    self.mbglMap.easeTo(cameraOptions, animationOptions);
    [self didChangeValueForKey:@"camera"];
}

- (void)flyToCamera:(MGLMapCamera *)camera completionHandler:(nullable void (^)(void))completion
{
    MGLLogDebug(@"Setting flyToCamera: %@ completionHandler: %@", camera, completion);
    [self flyToCamera:camera withDuration:-1 completionHandler:completion];
}

- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration completionHandler:(nullable void (^)(void))completion
{
    MGLLogDebug(@"Setting flyToCamera: %@ withDuration: %f completionHandler: %@", camera, duration, completion);
    [self flyToCamera:camera withDuration:duration peakAltitude:-1 completionHandler:completion];
}

- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration peakAltitude:(CLLocationDistance)peakAltitude completionHandler:(nullable void (^)(void))completion
{
    MGLLogDebug(@"Setting flyToCamera: %@ withDuration: %f peakAltitude: %f completionHandler: %@", camera, duration, peakAltitude, completion);
    [self _flyToCamera:camera edgePadding:self.contentInset withDuration:duration peakAltitude:peakAltitude completionHandler:completion];
}

- (void)_flyToCamera:(MGLMapCamera *)camera edgePadding:(UIEdgeInsets)insets withDuration:(NSTimeInterval)duration peakAltitude:(CLLocationDistance)peakAltitude completionHandler:(nullable void (^)(void))completion
{
    if (!_mbglMap)
    {
        if (completion)
        {
            completion();
        }
        return;
    }

    mbgl::AnimationOptions animationOptions;
    if (duration >= 0)
    {
        animationOptions.duration = MGLDurationFromTimeInterval(duration);
    }
    if (peakAltitude >= 0)
    {
        CLLocationDegrees peakLatitude = (self.centerCoordinate.latitude + camera.centerCoordinate.latitude) / 2;
        CLLocationDegrees peakPitch = (self.camera.pitch + camera.pitch) / 2;
        animationOptions.minZoom = MGLZoomLevelForAltitude(peakAltitude, peakPitch,
                                                           peakLatitude, self.frame.size);
    }
    if (completion)
    {
        animationOptions.transitionFinishFn = [completion]() {
            dispatch_async(dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    
    if ([self.camera isEqualToMapCamera:camera])
    {
        if (completion)
        {
            [self animateWithDelay:duration animations:^{
                completion();
            }];
        }
        return;
    }

    [self willChangeValueForKey:@"camera"];
    [self cancelTransitions];

    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonProgrammatic;

    mbgl::CameraOptions cameraOptions = [self cameraOptionsObjectForAnimatingToCamera:camera edgePadding:insets];
    self.mbglMap.flyTo(cameraOptions, animationOptions);
    [self didChangeValueForKey:@"camera"];
}

- (void)cancelTransitions {
    if (!_mbglMap)
    {
        return;
    }
    self.cameraChangeReasonBitmask |= MGLCameraChangeReasonTransitionCancelled;
    self.mbglMap.cancelTransitions();
    self.cameraChangeReasonBitmask &= ~MGLCameraChangeReasonTransitionCancelled;
}

- (MGLMapCamera *)cameraThatFitsCoordinateBounds:(MGLCoordinateBounds)bounds
{
    return [self cameraThatFitsCoordinateBounds:bounds edgePadding:UIEdgeInsetsZero];
}

- (MGLMapCamera *)cameraThatFitsCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(UIEdgeInsets)insets
{
    if (!_mbglMap)
    {
        return nil;
    }
    
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    padding += MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    mbgl::CameraOptions cameraOptions = self.mbglMap.cameraForLatLngBounds(MGLLatLngBoundsFromCoordinateBounds(bounds), padding);
    return [self cameraForCameraOptions:cameraOptions];
}

- (MGLMapCamera *)camera:(MGLMapCamera *)camera fittingCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(UIEdgeInsets)insets
{
    if (!_mbglMap)
    {
        return nil;
    }

    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    padding += MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    
    MGLMapCamera *currentCamera = self.camera;
    CGFloat pitch = camera.pitch < 0 ? currentCamera.pitch : camera.pitch;
    CLLocationDirection direction = camera.heading < 0 ? currentCamera.heading : camera.heading;
    
    mbgl::CameraOptions cameraOptions = self.mbglMap.cameraForLatLngBounds(MGLLatLngBoundsFromCoordinateBounds(bounds), padding, direction, pitch);
    return [self cameraForCameraOptions:cameraOptions];
}

- (MGLMapCamera *)camera:(MGLMapCamera *)camera fittingShape:(MGLShape *)shape edgePadding:(UIEdgeInsets)insets {
    if (!_mbglMap)
    {
        return nil;
    }

    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    padding += MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    
    MGLMapCamera *currentCamera = self.camera;
    CGFloat pitch = camera.pitch < 0 ? currentCamera.pitch : camera.pitch;
    CLLocationDirection direction = camera.heading < 0 ? currentCamera.heading : camera.heading;
    
    mbgl::CameraOptions cameraOptions = self.mbglMap.cameraForGeometry([shape geometryObject], padding, direction, pitch);
    
    return [self cameraForCameraOptions: cameraOptions];
}

- (MGLMapCamera *)cameraThatFitsShape:(MGLShape *)shape direction:(CLLocationDirection)direction edgePadding:(UIEdgeInsets)insets {
    if (!_mbglMap)
    {
        return nil;
    }

    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    padding += MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    
    mbgl::CameraOptions cameraOptions = self.mbglMap.cameraForGeometry([shape geometryObject], padding, direction);
    
    return [self cameraForCameraOptions:cameraOptions];
}

- (MGLMapCamera *)cameraForCameraOptions:(const mbgl::CameraOptions &)cameraOptions
{
    if (!_mbglMap)
    {
        return nil;
    }

    CLLocationCoordinate2D centerCoordinate = MGLLocationCoordinate2DFromLatLng(cameraOptions.center ? *cameraOptions.center : self.mbglMap.getLatLng());
    double zoomLevel = cameraOptions.zoom ? *cameraOptions.zoom : self.zoomLevel;
    CLLocationDirection direction = cameraOptions.angle ? mbgl::util::wrap(*cameraOptions.angle, 0., 360.) : self.direction;
    CGFloat pitch = cameraOptions.pitch ? *cameraOptions.pitch : self.mbglMap.getPitch();
    CLLocationDistance altitude = MGLAltitudeForZoomLevel(zoomLevel, pitch, centerCoordinate.latitude, self.frame.size);
    return [MGLMapCamera cameraLookingAtCenterCoordinate:centerCoordinate altitude:altitude pitch:pitch heading:direction];
}

/// Returns a CameraOptions object that specifies parameters for animating to
/// the given camera.
- (mbgl::CameraOptions)cameraOptionsObjectForAnimatingToCamera:(MGLMapCamera *)camera edgePadding:(UIEdgeInsets)insets
{
    mbgl::CameraOptions options;
    if (CLLocationCoordinate2DIsValid(camera.centerCoordinate))
    {
        options.center = MGLLatLngFromLocationCoordinate2D(camera.centerCoordinate);
    }
    options.padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    options.zoom = MGLZoomLevelForAltitude(camera.altitude, camera.pitch,
                                           camera.centerCoordinate.latitude,
                                           self.frame.size);
    if (camera.heading >= 0)
    {
        options.angle = camera.heading;
    }
    if (camera.pitch >= 0)
    {
        options.pitch = camera.pitch;
    }
    return options;
}

- (CLLocationCoordinate2D)convertPoint:(CGPoint)point toCoordinateFromView:(nullable UIView *)view
{
    return MGLLocationCoordinate2DFromLatLng([self convertPoint:point toLatLngFromView:view]);
}

/// Converts a point in the view’s coordinate system to a geographic coordinate.
- (mbgl::LatLng)convertPoint:(CGPoint)point toLatLngFromView:(nullable UIView *)view
{
    CGPoint convertedPoint = [self convertPoint:point fromView:view];
    return self.mbglMap.latLngForPixel(mbgl::ScreenCoordinate(convertedPoint.x, convertedPoint.y)).wrapped();
}

- (CGPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(nullable UIView *)view
{
    if ( ! CLLocationCoordinate2DIsValid(coordinate))
    {
        return CGPointMake(NAN, NAN);
    }
    return [self convertLatLng:MGLLatLngFromLocationCoordinate2D(coordinate) toPointToView:view];
}

/// Converts a geographic coordinate to a point in the view’s coordinate system.
- (CGPoint)convertLatLng:(mbgl::LatLng)latLng toPointToView:(nullable UIView *)view
{
    mbgl::ScreenCoordinate pixel = self.mbglMap.pixelForLatLng(latLng);
    return [self convertPoint:CGPointMake(pixel.x, pixel.y) toView:view];
}

- (MGLCoordinateBounds)convertRect:(CGRect)rect toCoordinateBoundsFromView:(nullable UIView *)view
{
    return MGLCoordinateBoundsFromLatLngBounds([self convertRect:rect toLatLngBoundsFromView:view]);
}

- (CGRect)convertCoordinateBounds:(MGLCoordinateBounds)bounds toRectToView:(nullable UIView *)view
{
    return [self convertLatLngBounds:MGLLatLngBoundsFromCoordinateBounds(bounds) toRectToView:view];
}

/// Converts a geographic bounding box to a rectangle in the view’s coordinate
/// system.
- (CGRect)convertLatLngBounds:(mbgl::LatLngBounds)bounds toRectToView:(nullable UIView *)view {
    auto northwest = bounds.northwest();
    auto northeast = bounds.northeast();
    auto southwest = bounds.southwest();
    auto southeast = bounds.southeast();

    auto center = [self convertPoint:{ CGRectGetMidX(view.bounds), CGRectGetMidY(view.bounds) } toLatLngFromView:view];

    // Extend bounds to account for the antimeridian
    northwest.unwrapForShortestPath(center);
    northeast.unwrapForShortestPath(center);
    southwest.unwrapForShortestPath(center);
    southeast.unwrapForShortestPath(center);

    auto correctedLatLngBounds = mbgl::LatLngBounds::empty();
    correctedLatLngBounds.extend(northwest);
    correctedLatLngBounds.extend(northeast);
    correctedLatLngBounds.extend(southwest);
    correctedLatLngBounds.extend(southeast);
    
    CGRect rect = { [self convertLatLng:correctedLatLngBounds.southwest() toPointToView:view], CGSizeZero };
    rect = MGLExtendRect(rect, [self convertLatLng:correctedLatLngBounds.northeast() toPointToView:view]);
    return rect;
}

/// Converts a rectangle in the given view’s coordinate system to a geographic
/// bounding box.
- (mbgl::LatLngBounds)convertRect:(CGRect)rect toLatLngBoundsFromView:(nullable UIView *)view
{
    auto bounds = mbgl::LatLngBounds::empty();
    auto topLeft = [self convertPoint:{ CGRectGetMinX(rect), CGRectGetMinY(rect) } toLatLngFromView:view];
    auto topRight = [self convertPoint:{ CGRectGetMaxX(rect), CGRectGetMinY(rect) } toLatLngFromView:view];
    auto bottomRight = [self convertPoint:{ CGRectGetMaxX(rect), CGRectGetMaxY(rect) } toLatLngFromView:view];
    auto bottomLeft = [self convertPoint:{ CGRectGetMinX(rect), CGRectGetMaxY(rect) } toLatLngFromView:view];
    
    // If the bounds straddles the antimeridian, unwrap it so that one side
    // extends beyond ±180° longitude.
    auto center = [self convertPoint:{ CGRectGetMidX(rect), CGRectGetMidY(rect) } toLatLngFromView:view];
    topLeft.unwrapForShortestPath(center);
    topRight.unwrapForShortestPath(center);
    bottomRight.unwrapForShortestPath(center);
    bottomLeft.unwrapForShortestPath(center);
    
    bounds.extend(topLeft);
    bounds.extend(topRight);
    bounds.extend(bottomRight);
    bounds.extend(bottomLeft);

    return bounds;
}

- (CLLocationDistance)metersPerPointAtLatitude:(CLLocationDegrees)latitude
{
    return mbgl::Projection::getMetersPerPixelAtLatitude(latitude, self.zoomLevel);
}

#pragma mark - Camera Change Reason -

- (void)resetCameraChangeReason
{
    self.cameraChangeReasonBitmask = MGLCameraChangeReasonNone;
}

#pragma mark - Annotations -

- (nullable NSArray<id <MGLAnnotation>> *)annotations
{
    if (_annotationContextsByAnnotationTag.empty())
    {
        return nil;
    }

    // Map all the annotation tags to the annotations themselves.
    std::vector<id <MGLAnnotation>> annotations;
    std::transform(_annotationContextsByAnnotationTag.begin(),
                   _annotationContextsByAnnotationTag.end(),
                   std::back_inserter(annotations),
                   ^ id <MGLAnnotation> (const std::pair<MGLAnnotationTag, MGLAnnotationContext> &pair)
    {
        return pair.second.annotation;
    });

    annotations.erase(std::remove_if(annotations.begin(), annotations.end(),
                                     [](const id <MGLAnnotation> annotation) { return annotation == nullptr; }),
                      annotations.end());

    return [NSArray arrayWithObjects:&annotations[0] count:annotations.size()];
}

- (nullable NSArray<id <MGLAnnotation>> *)visibleAnnotations
{
    return [self visibleAnnotationsInRect:self.bounds];
}

- (nullable NSArray<id <MGLAnnotation>> *)visibleAnnotationsInRect:(CGRect)rect
{
    if (_annotationContextsByAnnotationTag.empty())
    {
        return nil;
    }

    std::vector<MGLAnnotationTag> annotationTags = [self annotationTagsInRect:rect];
    std::vector<MGLAnnotationTag> shapeAnnotationTags = [self shapeAnnotationTagsInRect:rect];
    
    if (shapeAnnotationTags.size()) {
        annotationTags.insert(annotationTags.end(), shapeAnnotationTags.begin(), shapeAnnotationTags.end());
    }
    
    if (annotationTags.size())
    {
        NSMutableArray *annotations = [NSMutableArray arrayWithCapacity:annotationTags.size()];

        for (auto const& annotationTag: annotationTags)
        {
            if (!_annotationContextsByAnnotationTag.count(annotationTag) ||
                annotationTag == MGLAnnotationTagNotFound)
            {
                continue;
            }

            MGLAnnotationContext annotationContext = _annotationContextsByAnnotationTag.at(annotationTag);
            MGLAssert(annotationContext.annotation, @"Missing annotation for tag %llu.", annotationTag);
            if (annotationContext.annotation)
            {
                [annotations addObject:annotationContext.annotation];
            }
        }

        return [annotations copy];
    }

    return nil;
}

/// Returns the annotation assigned the given tag. Cheap.
- (id <MGLAnnotation>)annotationWithTag:(MGLAnnotationTag)tag
{
    if ( ! _annotationContextsByAnnotationTag.count(tag) ||
        tag == MGLAnnotationTagNotFound) {
        return nil;
    }

    MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(tag);
    return annotationContext.annotation;
}

/// Returns the annotation tag assigned to the given annotation.
- (MGLAnnotationTag)annotationTagForAnnotation:(id <MGLAnnotation>)annotation
{
    if ( ! annotation || annotation == self.userLocation
        || _annotationTagsByAnnotation.count(annotation) == 0)
    {
        return MGLAnnotationTagNotFound;
    }

    return _annotationTagsByAnnotation.at(annotation);
}

- (void)addAnnotation:(id <MGLAnnotation>)annotation
{
    MGLLogDebug(@"Adding annotation: %@", annotation);
    if ( ! annotation) return;

    // The core bulk add API is efficient with respect to indexing and
    // screen refreshes, thus we should defer to it even for individual adds.
    //
    [self addAnnotations:@[ annotation ]];
}

- (void)addAnnotations:(NSArray<id <MGLAnnotation>> *)annotations
{
    MGLLogDebug(@"Adding: %lu annotations", annotations.count);
    if ( ! annotations) return;
    [self willChangeValueForKey:@"annotations"];

    NSMutableDictionary *annotationImagesForAnnotation = [NSMutableDictionary dictionary];
    NSMutableDictionary *annotationViewsForAnnotation = [NSMutableDictionary dictionary];

    BOOL delegateImplementsViewForAnnotation = [self.delegate respondsToSelector:@selector(mapView:viewForAnnotation:)];
    BOOL delegateImplementsImageForPoint = [self.delegate respondsToSelector:@selector(mapView:imageForAnnotation:)];

    NSMutableArray *newAnnotationViews = [[NSMutableArray alloc] initWithCapacity:annotations.count];

    for (id <MGLAnnotation> annotation in annotations)
    {
        MGLAssert([annotation conformsToProtocol:@protocol(MGLAnnotation)], @"annotation should conform to MGLAnnotation");

        // adding the same annotation object twice is a no-op
        if (_annotationTagsByAnnotation.count(annotation) != 0)
        {
            continue;
        }

        if ([annotation isKindOfClass:[MGLMultiPoint class]])
        {
            // The polyline or polygon knows how to style itself (with the map view’s help).
            MGLMultiPoint *multiPoint = (MGLMultiPoint *)annotation;
            if (!multiPoint.pointCount) {
                continue;
            }

            _isChangingAnnotationLayers = YES;
            MGLAnnotationTag annotationTag = self.mbglMap.addAnnotation([multiPoint annotationObjectWithDelegate:self]);
            MGLAnnotationContext context;
            context.annotation = annotation;
            _annotationContextsByAnnotationTag[annotationTag] = context;
            _annotationTagsByAnnotation[annotation] = annotationTag;

            [(NSObject *)annotation addObserver:self forKeyPath:@"coordinates" options:0 context:(void *)(NSUInteger)annotationTag];
        }
        else if ( ! [annotation isKindOfClass:[MGLMultiPolyline class]]
                 && ![annotation isKindOfClass:[MGLMultiPolygon class]]
                 && ![annotation isKindOfClass:[MGLShapeCollection class]]
                 && ![annotation isKindOfClass:[MGLPointCollection class]])
        {
            MGLAnnotationView *annotationView;
            NSString *symbolName;
            NSValue *annotationValue = [NSValue valueWithNonretainedObject:annotation];

            if (delegateImplementsViewForAnnotation)
            {
                annotationView = [self annotationViewForAnnotation:annotation];
                if (annotationView)
                {
                    annotationViewsForAnnotation[annotationValue] = annotationView;
                    annotationView.annotation = annotation;
                    annotationView.center = MGLPointRounded([self convertCoordinate:annotation.coordinate toPointToView:self]);
                    [newAnnotationViews addObject:annotationView];

                    MGLAnnotationImage *annotationImage = self.invisibleAnnotationImage;
                    symbolName = annotationImage.styleIconIdentifier;
                    annotationImagesForAnnotation[annotationValue] = annotationImage;
                    if ( ! self.annotationImagesByIdentifier[annotationImage.reuseIdentifier])
                    {
                        [self installAnnotationImage:annotationImage];
                    }
                }
            }

            if ( ! annotationView) {
                MGLAnnotationImage *annotationImage;

                if (delegateImplementsImageForPoint)
                {
                    annotationImage = [self.delegate mapView:self imageForAnnotation:annotation];
                }
                if ( ! annotationImage)
                {
                    annotationImage = [self dequeueReusableAnnotationImageWithIdentifier:MGLDefaultStyleMarkerSymbolName];
                }
                if ( ! annotationImage)
                {
                    annotationImage = self.defaultAnnotationImage;
                }

                symbolName = annotationImage.styleIconIdentifier;

                if ( ! symbolName)
                {
                    symbolName = [MGLAnnotationSpritePrefix stringByAppendingString:annotationImage.reuseIdentifier];
                    annotationImage.styleIconIdentifier = symbolName;
                }
                if ( ! self.annotationImagesByIdentifier[annotationImage.reuseIdentifier])
                {
                    [self installAnnotationImage:annotationImage];
                }

                annotationImagesForAnnotation[annotationValue] = annotationImage;
            }

            MGLAnnotationTag annotationTag = self.mbglMap.addAnnotation(mbgl::SymbolAnnotation {
                MGLPointFromLocationCoordinate2D(annotation.coordinate),
                symbolName.UTF8String
            });

            MGLAnnotationContext context;
            context.annotation = annotation;
            MGLAnnotationImage *annotationImage = annotationImagesForAnnotation[annotationValue];
            context.imageReuseIdentifier = annotationImage.reuseIdentifier;

            if (annotationView) {
                context.annotationView = annotationView;
                context.viewReuseIdentifier = annotationView.reuseIdentifier;
            }

            _annotationTagsByAnnotation[annotation] = annotationTag;
            _annotationContextsByAnnotationTag[annotationTag] = context;

            if ([annotation isKindOfClass:[NSObject class]]) {
                MGLAssert(![annotation isKindOfClass:[MGLMultiPoint class]], @"Point annotation should not be MGLMultiPoint.");
                [(NSObject *)annotation addObserver:self forKeyPath:@"coordinate" options:0 context:(void *)(NSUInteger)annotationTag];
            }
        }
    }

    [self updateAnnotationContainerViewWithAnnotationViews:newAnnotationViews];

    [self didChangeValueForKey:@"annotations"];
    if (_isChangingAnnotationLayers)
    {
        [self.style willChangeValueForKey:@"layers"];
    }

    if ([self.delegate respondsToSelector:@selector(mapView:didAddAnnotationViews:)])
    {
        [self.delegate mapView:self didAddAnnotationViews:newAnnotationViews];
    }

    UIAccessibilityPostNotification(UIAccessibilityLayoutChangedNotification, nil);
}

- (void)updateAnnotationContainerViewWithAnnotationViews:(NSArray<MGLAnnotationView *> *)annotationViews
{
    if (annotationViews.count == 0) return;

    MGLAnnotationContainerView *newAnnotationContainerView;
    if (self.annotationContainerView)
    {
        // reload any previously added views
        newAnnotationContainerView = [MGLAnnotationContainerView annotationContainerViewWithAnnotationContainerView:self.annotationContainerView];
        [self.annotationContainerView removeFromSuperview];
    }
    else
    {
        newAnnotationContainerView = [[MGLAnnotationContainerView alloc] initWithFrame:self.bounds];
    }
    newAnnotationContainerView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    newAnnotationContainerView.contentMode = UIViewContentModeCenter;
    [newAnnotationContainerView addSubviews:annotationViews];
    [_glView insertSubview:newAnnotationContainerView atIndex:0];
    self.annotationContainerView = newAnnotationContainerView;
}

/// Initialize and return a default annotation image that depicts a round pin
/// rising from the center, with a shadow slightly below center. The alignment
/// rect therefore excludes the bottom half.
- (MGLAnnotationImage *)defaultAnnotationImage
{
    UIImage *image = [MGLMapView resourceImageNamed:MGLDefaultStyleMarkerSymbolName];
    image = [image imageWithAlignmentRectInsets:
             UIEdgeInsetsMake(0, 0, image.size.height / 2, 0)];
    MGLAnnotationImage *annotationImage = [MGLAnnotationImage annotationImageWithImage:image
                                                                       reuseIdentifier:MGLDefaultStyleMarkerSymbolName];
    annotationImage.styleIconIdentifier = [MGLAnnotationSpritePrefix stringByAppendingString:annotationImage.reuseIdentifier];
    return annotationImage;
}

- (MGLAnnotationImage *)invisibleAnnotationImage
{
    MGLAnnotationImage *annotationImage = [self dequeueReusableAnnotationImageWithIdentifier:MGLInvisibleStyleMarkerSymbolName];

    if (!annotationImage)
    {
        UIGraphicsBeginImageContext(CGSizeMake(1, 1));
        UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        annotationImage = [MGLAnnotationImage annotationImageWithImage:image
                                                                           reuseIdentifier:MGLInvisibleStyleMarkerSymbolName];
        annotationImage.styleIconIdentifier = [MGLAnnotationSpritePrefix stringByAppendingString:annotationImage.reuseIdentifier];
    }

    return annotationImage;
}

- (MGLAnnotationView *)annotationViewForAnnotation:(id<MGLAnnotation>)annotation
{
    MGLAnnotationView *annotationView = [self.delegate mapView:self viewForAnnotation:annotation];

    if (annotationView)
    {
        // Make sure that the annotation views are selected/deselected correctly because
        // annotations are not dismissed when they move out of the visible bounds
        BOOL isViewForSelectedAnnotation = self.selectedAnnotation == annotation;
        [annotationView setSelected:isViewForSelectedAnnotation];

        annotationView.annotation = annotation;
        annotationView.mapView = self;
        CGRect bounds = UIEdgeInsetsInsetRect({ CGPointZero, annotationView.frame.size }, annotationView.alignmentRectInsets);

        // Take any offset into consideration
        CGFloat adjustedAnnotationWidth = CGRectGetWidth(bounds) + fabs(annotationView.centerOffset.dx);
        CGFloat adjustedAnnotationHeight = CGRectGetHeight(bounds) + fabs(annotationView.centerOffset.dx);

        _largestAnnotationViewSize = CGSizeMake(MAX(_largestAnnotationViewSize.width, adjustedAnnotationWidth),
                                                MAX(_largestAnnotationViewSize.height, adjustedAnnotationHeight));

        _unionedAnnotationRepresentationSize = CGSizeMake(MAX(_unionedAnnotationRepresentationSize.width, _largestAnnotationViewSize.width),
                                                          MAX(_unionedAnnotationRepresentationSize.height, _largestAnnotationViewSize.height));
    }

    return annotationView;
}

- (nullable MGLAnnotationView *)viewForAnnotation:(id<MGLAnnotation>)annotation
{
    MGLLogDebug(@"Retrieving the annotation view for: %@", annotation);
    if (annotation == self.userLocation)
    {
        return self.userLocationAnnotationView;
    }
    MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
    if (annotationTag == MGLAnnotationTagNotFound) {
        return nil;
    }
    MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(annotationTag);
    return annotationContext.annotationView;
}

- (double)alphaForShapeAnnotation:(MGLShape *)annotation
{
    if (_delegateHasAlphasForShapeAnnotations)
    {
        return [self.delegate mapView:self alphaForShapeAnnotation:annotation];
    }
    return 1.0;
}

- (mbgl::Color)strokeColorForShapeAnnotation:(MGLShape *)annotation
{
    UIColor *color = (_delegateHasStrokeColorsForShapeAnnotations
                      ? [self.delegate mapView:self strokeColorForShapeAnnotation:annotation]
                      : self.tintColor);
    return color.mgl_color;
}

- (mbgl::Color)fillColorForPolygonAnnotation:(MGLPolygon *)annotation
{
    UIColor *color = (_delegateHasFillColorsForShapeAnnotations
                      ? [self.delegate mapView:self fillColorForPolygonAnnotation:annotation]
                      : self.tintColor);
    return color.mgl_color;
}

- (CGFloat)lineWidthForPolylineAnnotation:(MGLPolyline *)annotation
{
    if (_delegateHasLineWidthsForShapeAnnotations)
    {
        return [self.delegate mapView:self lineWidthForPolylineAnnotation:(MGLPolyline *)annotation];
    }
    return 3.0;
}

- (void)installAnnotationImage:(MGLAnnotationImage *)annotationImage
{
    NSString *iconIdentifier = annotationImage.styleIconIdentifier;
    self.annotationImagesByIdentifier[annotationImage.reuseIdentifier] = annotationImage;
    annotationImage.delegate = self;

    // add sprite
    self.mbglMap.addAnnotationImage([annotationImage.image mgl_styleImageWithIdentifier:iconIdentifier]);

    // Create a slop area with a “radius” equal in size to the annotation
    // image’s alignment rect, allowing the eventual tap to be on any point
    // within this image. Union this slop area with any existing slop areas.
    CGRect bounds = UIEdgeInsetsInsetRect({ CGPointZero, annotationImage.image.size },
                                          annotationImage.image.alignmentRectInsets);
    _unionedAnnotationRepresentationSize = CGSizeMake(MAX(_unionedAnnotationRepresentationSize.width, bounds.size.width),
                                                      MAX(_unionedAnnotationRepresentationSize.height, bounds.size.height));
}

- (void)removeAnnotation:(id <MGLAnnotation>)annotation
{
    MGLLogDebug(@"Removing annotation: %@", annotation);
    if ( ! annotation) return;

    // The core bulk deletion API is efficient with respect to indexing
    // and screen refreshes, thus we should defer to it even for
    // individual deletes.
    //
    [self removeAnnotations:@[ annotation ]];
}

- (void)removeAnnotations:(NSArray<id <MGLAnnotation>> *)annotations
{
    MGLLogDebug(@"Removing: %lu annotations", annotations.count);
    if ( ! annotations) return;

    [self willChangeValueForKey:@"annotations"];

    for (id <MGLAnnotation> annotation in annotations)
    {
        MGLAssert([annotation conformsToProtocol:@protocol(MGLAnnotation)], @"annotation should conform to MGLAnnotation");

        MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
        if (annotationTag == MGLAnnotationTagNotFound)
        {
            continue;
        }

        MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(annotationTag);
        MGLAnnotationView *annotationView = annotationContext.annotationView;

        if (annotationContext.viewReuseIdentifier)
        {
            NSMutableArray *annotationViewReuseQueue = [self annotationViewReuseQueueForIdentifier:annotationContext.viewReuseIdentifier];
            if (![annotationViewReuseQueue containsObject:annotationView])
            {
                [annotationViewReuseQueue removeObject:annotationView];
            }
        }

        annotationView.annotation = nil;
        [annotationView removeFromSuperview];
        [self.annotationContainerView.annotationViews removeObject:annotationView];

        if (annotationTag == _selectedAnnotationTag)
        {
            [self deselectAnnotation:annotation animated:NO];
        }

        _annotationContextsByAnnotationTag.erase(annotationTag);
        _annotationTagsByAnnotation.erase(annotation);

        if ([annotation isKindOfClass:[NSObject class]] && ![annotation isKindOfClass:[MGLMultiPoint class]])
        {
            [(NSObject *)annotation removeObserver:self forKeyPath:@"coordinate" context:(void *)(NSUInteger)annotationTag];
        }
        else if ([annotation isKindOfClass:[MGLMultiPoint class]])
        {
            [(NSObject *)annotation removeObserver:self forKeyPath:@"coordinates" context:(void *)(NSUInteger)annotationTag];
        }

        _isChangingAnnotationLayers = YES;
        self.mbglMap.removeAnnotation(annotationTag);
    }

    [self didChangeValueForKey:@"annotations"];
    UIAccessibilityPostNotification(UIAccessibilityLayoutChangedNotification, nil);
    if (_isChangingAnnotationLayers)
    {
        [self.style willChangeValueForKey:@"layers"];
    }
}

- (nonnull NSArray<id <MGLOverlay>> *)overlays
{
    if (self.annotations == nil) { return @[]; }

    NSMutableArray<id <MGLOverlay>> *mutableOverlays = [NSMutableArray array];

    [self.annotations enumerateObjectsUsingBlock:^(id<MGLAnnotation>  _Nonnull annotation, NSUInteger idx, BOOL * _Nonnull stop) {
        if ([annotation conformsToProtocol:@protocol(MGLOverlay)])
        {
            [mutableOverlays addObject:(id<MGLOverlay>)annotation];
        }
    }];

    return [NSArray arrayWithArray:mutableOverlays];
}

- (void)addOverlay:(id <MGLOverlay>)overlay
{
    MGLLogDebug(@"Adding overlay: %@", overlay);
    [self addOverlays:@[ overlay ]];
}

- (void)addOverlays:(NSArray<id <MGLOverlay>> *)overlays
{
    MGLLogDebug(@"Adding: %lu overlays", overlays.count);
#if DEBUG
    for (id <MGLOverlay> overlay in overlays)
    {
        MGLAssert([overlay conformsToProtocol:@protocol(MGLOverlay)], @"overlay should conform to MGLOverlay");
    }
#endif

    [self addAnnotations:overlays];
}

- (void)removeOverlay:(id <MGLOverlay>)overlay
{
    MGLLogDebug(@"Removing overlay: %@", overlay);
    [self removeOverlays:@[ overlay ]];
}

- (void)removeOverlays:(NSArray<id <MGLOverlay>> *)overlays
{
    MGLLogDebug(@"Removing: %lu overlays", overlays.count);
#if DEBUG
    for (id <MGLOverlay> overlay in overlays)
    {
        MGLAssert([overlay conformsToProtocol:@protocol(MGLOverlay)], @"overlay should conform to MGLOverlay");
    }
#endif

    [self removeAnnotations:overlays];
}

- (nullable MGLAnnotationImage *)dequeueReusableAnnotationImageWithIdentifier:(NSString *)identifier
{
    return self.annotationImagesByIdentifier[identifier];
}

- (nullable MGLAnnotationView *)dequeueReusableAnnotationViewWithIdentifier:(NSString *)identifier
{
    NSMutableArray *annotationViewReuseQueue = [self annotationViewReuseQueueForIdentifier:identifier];
    MGLAnnotationView *reusableView = annotationViewReuseQueue.firstObject;
    [reusableView prepareForReuse];
    [annotationViewReuseQueue removeObject:reusableView];

    return reusableView;
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
- (MGLAnnotationTag)annotationTagAtPoint:(CGPoint)point persistingResults:(BOOL)persist
{
    // Look for any annotation near the tap. An annotation is “near” if the
    // distance between its center and the tap is less than the maximum height
    // or width of an installed annotation image or annotation view.
    CGRect queryRect = CGRectInset({ point, CGSizeZero },
                                   -_unionedAnnotationRepresentationSize.width,
                                   -_unionedAnnotationRepresentationSize.height);
    queryRect = CGRectInset(queryRect, -MGLAnnotationImagePaddingForHitTest,
                            -MGLAnnotationImagePaddingForHitTest);
    std::vector<MGLAnnotationTag> nearbyAnnotations = [self annotationTagsInRect:queryRect];
    std::vector<MGLAnnotationTag> nearbyShapeAnnotations = [self shapeAnnotationTagsInRect:queryRect];
    
    if (nearbyShapeAnnotations.size()) {
        nearbyAnnotations.insert(nearbyAnnotations.end(), nearbyShapeAnnotations.begin(), nearbyShapeAnnotations.end());
    }

    if (nearbyAnnotations.size())
    {
        // Assume that the user is fat-fingering an annotation.
        CGRect hitRect = CGRectInset({ point, CGSizeZero },
                                     -MGLAnnotationImagePaddingForHitTest,
                                     -MGLAnnotationImagePaddingForHitTest);
        
        // Filter out any annotation whose image or view is unselectable or for which
        // hit testing fails.
        auto end = std::remove_if(nearbyAnnotations.begin(), nearbyAnnotations.end(), [&](const MGLAnnotationTag annotationTag) {
            id <MGLAnnotation> annotation = [self annotationWithTag:annotationTag];
            MGLAssert(annotation, @"Unknown annotation found nearby tap");
            if ( ! annotation)
            {
                return true;
            }
            
            MGLAnnotationContext annotationContext = _annotationContextsByAnnotationTag.at(annotationTag);
            CGRect annotationRect;
            
            MGLAnnotationView *annotationView = annotationContext.annotationView;
            
            if (annotationView)
            {
                if ( ! annotationView.enabled)
                {
                    return true;
                }

                CGPoint calloutAnchorPoint = MGLPointRounded([self convertCoordinate:annotation.coordinate toPointToView:self]);
                CGRect frame = CGRectInset({ calloutAnchorPoint, CGSizeZero }, -CGRectGetWidth(annotationView.frame) / 2, -CGRectGetHeight(annotationView.frame) / 2);

                // We need to take any offset into consideration. Note that a large offset will result in a
                // large value for `_unionedAnnotationRepresentationSize` (and thus a larger feature query rect).
                // Aim to keep the offset as small as possible.
                frame = CGRectOffset(frame, annotationView.centerOffset.dx, annotationView.centerOffset.dy);

                annotationRect = UIEdgeInsetsInsetRect(frame, annotationView.alignmentRectInsets);
            }
            else
            {
                if ([annotation isKindOfClass:[MGLMultiPoint class]])
                {
                    if ([self.delegate respondsToSelector:@selector(mapView:shapeAnnotationIsEnabled:)]) {
                        return !!(![self.delegate mapView:self shapeAnnotationIsEnabled:(MGLMultiPoint *)annotation]);
                    } else {
                        return false;
                    }
                }
                
                MGLAnnotationImage *annotationImage = [self imageOfAnnotationWithTag:annotationTag];
                if ( ! annotationImage.enabled)
                {
                    return true;
                }
                
                MGLAnnotationImage *fallbackAnnotationImage = [self dequeueReusableAnnotationImageWithIdentifier:MGLDefaultStyleMarkerSymbolName];
                UIImage *fallbackImage = fallbackAnnotationImage.image;
                
                annotationRect = [self frameOfImage:annotationImage.image ?: fallbackImage centeredAtCoordinate:annotation.coordinate];
            }
            
            // Filter out the annotation if the fattened finger didn’t land
            // within the image’s alignment rect.
            return !!!CGRectIntersectsRect(annotationRect, hitRect);
        });
        
        nearbyAnnotations.resize(std::distance(nearbyAnnotations.begin(), end));

    }

    MGLAnnotationTag hitAnnotationTag = MGLAnnotationTagNotFound;
    if (nearbyAnnotations.size())
    {
        // The first selection in the cycle should be the one nearest to the
        // tap. Also the annotation tags need to be stable in order to compare them with
        // the remembered tags _annotationsNearbyLastTap.
        CLLocationCoordinate2D currentCoordinate = [self convertPoint:point toCoordinateFromView:self];
        std::sort(nearbyAnnotations.begin(), nearbyAnnotations.end(), [&](const MGLAnnotationTag tagA, const MGLAnnotationTag tagB) {
            CLLocationCoordinate2D coordinateA = [[self annotationWithTag:tagA] coordinate];
            CLLocationCoordinate2D coordinateB = [[self annotationWithTag:tagB] coordinate];
            CLLocationDegrees deltaA = hypot(coordinateA.latitude - currentCoordinate.latitude,
                                             coordinateA.longitude - currentCoordinate.longitude);
            CLLocationDegrees deltaB = hypot(coordinateB.latitude - currentCoordinate.latitude,
                                             coordinateB.longitude - currentCoordinate.longitude);
            return deltaA < deltaB;
        });
        
        if (nearbyAnnotations == _annotationsNearbyLastTap)
        {
            // The last time we persisted a set of annotations, we had the same
            // set of annotations as we do now. Cycle through them.
            if (_selectedAnnotationTag == MGLAnnotationTagNotFound
                || _selectedAnnotationTag == nearbyAnnotations.back())
            {
                // Either no annotation is selected or the last annotation in
                // the set was selected. Wrap around to the first annotation in
                // the set.
                hitAnnotationTag = nearbyAnnotations.front();
            }
            else
            {
                auto result = std::find(nearbyAnnotations.begin(),
                                        nearbyAnnotations.end(),
                                        _selectedAnnotationTag);
                if (result == nearbyAnnotations.end())
                {
                    // An annotation from this set hasn’t been selected before.
                    // Select the first (nearest) one.
                    hitAnnotationTag = nearbyAnnotations.front();
                }
                else
                {
                    // Step to the next annotation in the set.
                    auto distance = std::distance(nearbyAnnotations.begin(), result);
                    hitAnnotationTag = nearbyAnnotations[distance + 1];
                }
            }
        }
        else
        {
            // Remember the nearby annotations for the next time this method is
            // called.
            if (persist)
            {
                _annotationsNearbyLastTap = nearbyAnnotations;
            }
            
            // Choose the first nearby annotation.
            if (nearbyAnnotations.size())
            {
                hitAnnotationTag = nearbyAnnotations.front();
            }
        }
    }

    return hitAnnotationTag;
}

/// Returns the tags of the annotations coincident with the given rectangle.
- (std::vector<MGLAnnotationTag>)annotationTagsInRect:(CGRect)rect
{
    return _rendererFrontend->getRenderer()->queryPointAnnotations({
        { CGRectGetMinX(rect), CGRectGetMinY(rect) },
        { CGRectGetMaxX(rect), CGRectGetMaxY(rect) },
    });
}

- (std::vector<MGLAnnotationTag>)shapeAnnotationTagsInRect:(CGRect)rect
{
    return _rendererFrontend->getRenderer()->queryShapeAnnotations({
        { CGRectGetMinX(rect), CGRectGetMinY(rect) },
        { CGRectGetMaxX(rect), CGRectGetMaxY(rect) },
    });
}


- (BOOL)isMovingAnnotationIntoViewSupportedForAnnotation:(id<MGLAnnotation>)annotation animated:(BOOL)animated {
    // Consider delegating
    return [annotation isKindOfClass:[MGLPointAnnotation class]];
}

- (id <MGLAnnotation>)selectedAnnotation
{
    if (_userLocationAnnotationIsSelected)
    {
        return self.userLocation;
    }

    if ( ! _annotationContextsByAnnotationTag.count(_selectedAnnotationTag) ||
        _selectedAnnotationTag == MGLAnnotationTagNotFound) {
        return nil;
    }

    MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(_selectedAnnotationTag);
    return annotationContext.annotation;
}

- (void)setSelectedAnnotation:(id <MGLAnnotation>)annotation
{
    MGLLogDebug(@"Selecting annotation: %@", annotation);
    [self willChangeValueForKey:@"selectedAnnotations"];
    _selectedAnnotationTag = [self annotationTagForAnnotation:annotation];
    _userLocationAnnotationIsSelected = annotation && annotation == self.userLocation;
    [self didChangeValueForKey:@"selectedAnnotations"];
}

- (NSArray<id <MGLAnnotation>> *)selectedAnnotations
{
    id <MGLAnnotation> selectedAnnotation = self.selectedAnnotation;
    return (selectedAnnotation ? @[ selectedAnnotation ] : @[]);
}

- (void)setSelectedAnnotations:(NSArray<id <MGLAnnotation>> *)selectedAnnotations
{
    MGLLogDebug(@"Selecting: %lu annotations", selectedAnnotations.count);
    if ( ! selectedAnnotations.count) return;

    id <MGLAnnotation> firstAnnotation = selectedAnnotations[0];

    MGLAssert([firstAnnotation conformsToProtocol:@protocol(MGLAnnotation)], @"annotation should conform to MGLAnnotation");

    if ([firstAnnotation isKindOfClass:[MGLMultiPoint class]]) return;

    [self selectAnnotation:firstAnnotation animated:YES];
}

- (void)selectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated
{
    [self selectAnnotation:annotation moveIntoView:animated animateSelection:animated];
}

- (void)selectAnnotation:(id <MGLAnnotation>)annotation moveIntoView:(BOOL)moveIntoView animateSelection:(BOOL)animateSelection
{
    MGLLogDebug(@"Selecting annotation: %@ moveIntoView: %@ animateSelection: %@", annotation, MGLStringFromBOOL(moveIntoView), MGLStringFromBOOL(animateSelection));
    CGRect positioningRect = [self positioningRectForAnnotation:annotation defaultCalloutPoint:CGPointZero];
    [self selectAnnotation:annotation moveIntoView:moveIntoView animateSelection:animateSelection calloutPositioningRect:positioningRect];
}

- (void)selectAnnotation:(id <MGLAnnotation>)annotation moveIntoView:(BOOL)moveIntoView animateSelection:(BOOL)animateSelection calloutPositioningRect:(CGRect)calloutPositioningRect
{
    if ( ! annotation) return;

    if (annotation == self.selectedAnnotation) return;

    [self deselectAnnotation:self.selectedAnnotation animated:NO];

    // Add the annotation to the map if it hasn’t been added yet.
    MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
    if (annotationTag == MGLAnnotationTagNotFound && annotation != self.userLocation)
    {
        [self addAnnotation:annotation];
        annotationTag = [self annotationTagForAnnotation:annotation];
        if (annotationTag == MGLAnnotationTagNotFound) return;
    }

    MGLAnnotationView *annotationView = nil;

    if (annotation != self.userLocation)
        if (annotationTag != MGLAnnotationTagNotFound) {
            MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(annotationTag);
            annotationView = annotationContext.annotationView;
            if (annotationView && annotationView.enabled) {
                // Annotations represented by views use the view frame as the positioning rect.
                calloutPositioningRect = annotationView.frame;
                [annotationView.superview bringSubviewToFront:annotationView];

                [annotationView setSelected:YES animated:animateSelection];
            }
        }

    self.selectedAnnotation = annotation;

    // Determine if we're allowed to move this offscreen annotation on screen, even though we've asked it to
    if (moveIntoView) {
        moveIntoView = [self isMovingAnnotationIntoViewSupportedForAnnotation:annotation animated:animateSelection];
    }

    // If we have an invalid positioning rect, we need to provide a suitable default.
    // This (currently) happens if you select an annotation that has NOT yet been
    // added. See https://github.com/mapbox/mapbox-gl-native/issues/11476
    if (CGRectIsNull(calloutPositioningRect)) {
        CLLocationCoordinate2D origin = annotation.coordinate;
        CGPoint originPoint = [self convertCoordinate:origin toPointToView:self];
        calloutPositioningRect = { .origin = originPoint, .size = CGSizeZero };
    }
    
    CGRect expandedPositioningRect = calloutPositioningRect;

    // Used for callout positioning, and moving offscreen annotations onscreen.
    CGRect constrainedRect = self.contentFrame;
    CGRect bounds = constrainedRect;

    BOOL expandedPositioningRectToMoveCalloutIntoViewWithMargins = NO;
    
    UIView <MGLCalloutView> *calloutView = nil;

    if ([annotation respondsToSelector:@selector(title)] &&
        annotation.title &&
        [self.delegate respondsToSelector:@selector(mapView:annotationCanShowCallout:)] &&
        [self.delegate mapView:self annotationCanShowCallout:annotation])
    {
        // build the callout
        if ([self.delegate respondsToSelector:@selector(mapView:calloutViewForAnnotation:)])
        {
            id providedCalloutView = [self.delegate mapView:self calloutViewForAnnotation:annotation];
            if (providedCalloutView) {
                if (![providedCalloutView isKindOfClass:[UIView class]]) {
                    [NSException raise:NSInvalidArgumentException format:@"Callout view must be a kind of UIView"];
                }
                MGLAssert([providedCalloutView conformsToProtocol:@protocol(MGLCalloutView)], @"callout view must conform to MGLCalloutView");
                calloutView = providedCalloutView;
            }
        }
        if (!calloutView)
        {
            calloutView = [self calloutViewForAnnotation:annotation];
        }
        self.calloutViewForSelectedAnnotation = calloutView;

        if (_userLocationAnnotationIsSelected)
        {
            calloutPositioningRect = [self.userLocationAnnotationView.layer.presentationLayer frame];

            CGRect implicitAnnotationFrame = [self.userLocationAnnotationView.layer.presentationLayer frame];
            CGRect explicitAnnotationFrame = self.userLocationAnnotationView.frame;
            _initialImplicitCalloutViewOffset = CGPointMake(CGRectGetMinX(explicitAnnotationFrame) - CGRectGetMinX(implicitAnnotationFrame),
                                                            CGRectGetMinY(explicitAnnotationFrame) - CGRectGetMinY(implicitAnnotationFrame));
        }

        // consult delegate for left and/or right accessory views
        if ([self.delegate respondsToSelector:@selector(mapView:leftCalloutAccessoryViewForAnnotation:)])
        {
            calloutView.leftAccessoryView = [self.delegate mapView:self leftCalloutAccessoryViewForAnnotation:annotation];

            if ([calloutView.leftAccessoryView isKindOfClass:[UIControl class]])
            {
                UITapGestureRecognizer *calloutAccessoryTap = [[UITapGestureRecognizer alloc] initWithTarget:self
                                                                                                      action:@selector(handleCalloutAccessoryTapGesture:)];

                [calloutView.leftAccessoryView addGestureRecognizer:calloutAccessoryTap];
            }
        }

        if ([self.delegate respondsToSelector:@selector(mapView:rightCalloutAccessoryViewForAnnotation:)])
        {
            calloutView.rightAccessoryView = [self.delegate mapView:self rightCalloutAccessoryViewForAnnotation:annotation];

            if ([calloutView.rightAccessoryView isKindOfClass:[UIControl class]])
            {
                UITapGestureRecognizer *calloutAccessoryTap = [[UITapGestureRecognizer alloc] initWithTarget:self
                                                                                                      action:@selector(handleCalloutAccessoryTapGesture:)];

                [calloutView.rightAccessoryView addGestureRecognizer:calloutAccessoryTap];
            }
        }

        // set annotation delegate to handle taps on the callout view
        calloutView.delegate = self;

        // If the callout view provides inset (outset) information, we can use it to expand our positioning
        // rect, which we then use to help move the annotation on-screen if want need to.
        if (moveIntoView && [calloutView respondsToSelector:@selector(marginInsetsHintForPresentationFromRect:)]) {
            UIEdgeInsets margins = [calloutView marginInsetsHintForPresentationFromRect:calloutPositioningRect];
            expandedPositioningRect = UIEdgeInsetsInsetRect(expandedPositioningRect, margins);
            expandedPositioningRectToMoveCalloutIntoViewWithMargins = YES;
        }
    }
    
    if (!expandedPositioningRectToMoveCalloutIntoViewWithMargins)
    {
        // We don't have a callout (OR our callout didn't implement
        // marginInsetsHintForPresentationFromRect: - in this case we need to
        // ensure that partially off-screen annotations are NOT moved into view.
        //
        // We may want to create (and fallback to) an `MGLMapViewDelegate` version
        // of the `-[MGLCalloutView marginInsetsHintForPresentationFromRect:]
        // protocol method.
        bounds = CGRectInset(bounds, -calloutPositioningRect.size.width, -calloutPositioningRect.size.height);
    }        

    if (moveIntoView)
    {
        moveIntoView = NO;

        // Any one of these cases should trigger a move onscreen
        CGFloat minX = CGRectGetMinX(expandedPositioningRect);
        
        if (minX < CGRectGetMinX(bounds)) {
            constrainedRect.origin.x = minX;
            moveIntoView = YES;
        }
        else {
            CGFloat maxX = CGRectGetMaxX(expandedPositioningRect);
            
            if (maxX > CGRectGetMaxX(bounds)) {
                constrainedRect.origin.x = maxX - CGRectGetWidth(constrainedRect);
                moveIntoView = YES;
            }
        }

        CGFloat minY = CGRectGetMinY(expandedPositioningRect);
        
        if (minY < CGRectGetMinY(bounds)) {
            constrainedRect.origin.y = minY;
            moveIntoView = YES;
        }
        else {
            CGFloat maxY = CGRectGetMaxY(expandedPositioningRect);
            
            if (maxY > CGRectGetMaxY(bounds)) {
                constrainedRect.origin.y = maxY - CGRectGetHeight(constrainedRect);
                moveIntoView = YES;
            }
        }
    }

    // Remember, calloutView can be nil here.
    [calloutView presentCalloutFromRect:calloutPositioningRect
                                 inView:self.glView
                      constrainedToRect:constrainedRect
                               animated:animateSelection];

    // Save the anchor coordinate
    if ([annotation isKindOfClass:[MGLPointAnnotation class]]) {
        self.anchorCoordinateForSelectedAnnotation = annotation.coordinate;
    }
    else {
        // This is used for features like polygons, so that if the map is dragged
        // the callout doesn't ping to its coordinate.
        CGPoint anchorPoint = CGPointMake(CGRectGetMidX(calloutPositioningRect), CGRectGetMidY(calloutPositioningRect));
        self.anchorCoordinateForSelectedAnnotation = [self convertPoint:anchorPoint toCoordinateFromView:self];
    }
        
    // notify delegate
    if ([self.delegate respondsToSelector:@selector(mapView:didSelectAnnotation:)])
    {
        [self.delegate mapView:self didSelectAnnotation:annotation];
    }

    if (annotationView && [self.delegate respondsToSelector:@selector(mapView:didSelectAnnotationView:)])
    {
        [self.delegate mapView:self didSelectAnnotationView:annotationView];
    }

    if (moveIntoView)
    {
        CGPoint center = CGPointMake(CGRectGetMidX(constrainedRect), CGRectGetMidY(constrainedRect));
        CLLocationCoordinate2D centerCoord = [self convertPoint:center toCoordinateFromView:self];
        [self setCenterCoordinate:centerCoord animated:animateSelection];
    }
}

- (MGLCompactCalloutView *)calloutViewForAnnotation:(id <MGLAnnotation>)annotation
{
    MGLCompactCalloutView *calloutView = [MGLCompactCalloutView platformCalloutView];
    calloutView.representedObject = annotation;
    calloutView.tintColor = self.tintColor;

    return calloutView;
}

/// Returns the rectangle that represents the annotation image of the annotation
/// with the given tag. This rectangle is fitted to the image’s alignment rect
/// and is appropriate for positioning a popover.
/// If a shape annotation is visible but its centroid is not, and a default point is specified,
/// the callout view is anchored to the default callout point.
- (CGRect)positioningRectForAnnotation:(id <MGLAnnotation>)annotation defaultCalloutPoint:(CGPoint)calloutPoint
{
    MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
    CGRect positioningRect = [self positioningRectForCalloutForAnnotationWithTag:annotationTag];

    if (CGRectIsNull(positioningRect)) {
        return positioningRect;
    }

    // For annotations which `coordinate` falls offscreen it will use the current tap point as anchor instead.
    if ( ! CGRectIntersectsRect(positioningRect, self.bounds) && annotation != self.userLocation)
    {
        if (!CGPointEqualToPoint(calloutPoint, CGPointZero)) {
            positioningRect = CGRectMake(calloutPoint.x, calloutPoint.y, positioningRect.size.width, positioningRect.size.height);
        }
    }
    
    return positioningRect;
}

/// Returns the rectangle that represents the annotation image of the annotation
/// with the given tag. This rectangle is fitted to the image’s alignment rect
/// and is appropriate for positioning a popover.
- (CGRect)positioningRectForCalloutForAnnotationWithTag:(MGLAnnotationTag)annotationTag
{
    id <MGLAnnotation> annotation = [self annotationWithTag:annotationTag];
    if ( ! annotation)
    {
        return CGRectNull;
    }
    
    CLLocationCoordinate2D coordinate;
    
    if ((annotation == self.selectedAnnotation) &&
        CLLocationCoordinate2DIsValid(self.anchorCoordinateForSelectedAnnotation)) {
        coordinate = self.anchorCoordinateForSelectedAnnotation;
    }
    else {
        coordinate = annotation.coordinate;
    }
    
    if ([annotation isKindOfClass:[MGLMultiPoint class]]) {
        CLLocationCoordinate2D origin = coordinate;
        CGPoint originPoint = [self convertCoordinate:origin toPointToView:self];
        return CGRectMake(originPoint.x, originPoint.y, MGLAnnotationImagePaddingForHitTest, MGLAnnotationImagePaddingForHitTest);
    }
    
    UIImage *image = [self imageOfAnnotationWithTag:annotationTag].image;
    if ( ! image)
    {
        image = [self dequeueReusableAnnotationImageWithIdentifier:MGLDefaultStyleMarkerSymbolName].image;
    }
    if ( ! image)
    {
        return CGRectZero;
    }

    CGRect positioningRect = [self frameOfImage:image centeredAtCoordinate:coordinate];
    positioningRect.origin.x -= 0.5;

    return CGRectInset(positioningRect, -MGLAnnotationImagePaddingForCallout,
                       -MGLAnnotationImagePaddingForCallout);
}

/// Returns the rectangle relative to the viewport that represents the given
/// image centered at the given coordinate.
- (CGRect)frameOfImage:(UIImage *)image centeredAtCoordinate:(CLLocationCoordinate2D)coordinate
{
    CGPoint calloutAnchorPoint = MGLPointRounded([self convertCoordinate:coordinate toPointToView:self]);
    CGRect frame = CGRectInset({ calloutAnchorPoint, CGSizeZero }, -image.size.width / 2, -image.size.height / 2);
    return UIEdgeInsetsInsetRect(frame, image.alignmentRectInsets);
}

/// Returns the annotation image assigned to the annotation with the given tag.
- (MGLAnnotationImage *)imageOfAnnotationWithTag:(MGLAnnotationTag)annotationTag
{
    if (annotationTag == MGLAnnotationTagNotFound
        || _annotationContextsByAnnotationTag.count(annotationTag) == 0)
    {
        return nil;
    }

    NSString *customSymbol = _annotationContextsByAnnotationTag.at(annotationTag).imageReuseIdentifier;
    NSString *symbolName = customSymbol.length ? customSymbol : MGLDefaultStyleMarkerSymbolName;

    return [self dequeueReusableAnnotationImageWithIdentifier:symbolName];
}

- (void)deselectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated
{
    if ( ! annotation) return;

    if (self.selectedAnnotation == annotation)
    {
        MGLLogDebug(@"Deselecting annotation: %@ animated: %@", annotation, MGLStringFromBOOL(animated));
        // dismiss popup
        [self.calloutViewForSelectedAnnotation dismissCalloutAnimated:animated];

        // deselect annotation view
        MGLAnnotationView *annotationView = nil;
        MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];

        if (annotationTag != MGLAnnotationTagNotFound)
        {
            MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(annotationTag);
            annotationView = annotationContext.annotationView;
            [annotationView setSelected:NO animated:animated];
        }

        // clean up
        self.calloutViewForSelectedAnnotation = nil;
        self.selectedAnnotation = nil;
        self.anchorCoordinateForSelectedAnnotation = kCLLocationCoordinate2DInvalid;

        // notify delegate
        if ([self.delegate respondsToSelector:@selector(mapView:didDeselectAnnotation:)])
        {
            [self.delegate mapView:self didDeselectAnnotation:annotation];
        }

        if (annotationView && [self.delegate respondsToSelector:@selector(mapView:didDeselectAnnotationView:)])
        {
            [self.delegate mapView:self didDeselectAnnotationView:annotationView];
        }
    }
}

- (void)calloutViewWillAppear:(UIView <MGLCalloutView> *)calloutView
{
    if (_userLocationAnnotationIsSelected ||
        CGPointEqualToPoint(_initialImplicitCalloutViewOffset, CGPointZero))
    {
        return;
    }

    __weak __typeof__(self) weakSelf = self;

    // The user location callout view initially points to the user location
    // annotation’s implicit (visual) frame, which is offset from the
    // annotation’s explicit frame. Now the callout view needs to rendezvous
    // with the explicit frame. Then,
    // -updateUserLocationAnnotationViewAnimatedWithDuration: will take over the
    // next time an updated location arrives.
    [UIView animateWithDuration:_userLocationAnimationCompletionDate.timeIntervalSinceNow
                          delay:0
                        options:(UIViewAnimationOptionCurveLinear |
                                 UIViewAnimationOptionAllowUserInteraction |
                                 UIViewAnimationOptionBeginFromCurrentState)
                     animations:^
     {
         __typeof__(self) strongSelf = weakSelf;
         if ( ! strongSelf)
         {
             return;
         }

         calloutView.frame = CGRectOffset(calloutView.frame,
                                          strongSelf->_initialImplicitCalloutViewOffset.x,
                                          strongSelf->_initialImplicitCalloutViewOffset.y);
         strongSelf->_initialImplicitCalloutViewOffset = CGPointZero;
     }
                     completion:NULL];
}

- (void)showAnnotations:(NSArray<id <MGLAnnotation>> *)annotations animated:(BOOL)animated
{
    MGLLogDebug(@"Showing: %lu annotations animated: %@", annotations.count, MGLStringFromBOOL(animated));
    CGFloat maximumPadding = 100;
    CGFloat yPadding = (self.frame.size.height / 5 <= maximumPadding) ? (self.frame.size.height / 5) : maximumPadding;
    CGFloat xPadding = (self.frame.size.width / 5 <= maximumPadding) ? (self.frame.size.width / 5) : maximumPadding;

    UIEdgeInsets edgeInsets = UIEdgeInsetsMake(yPadding, xPadding, yPadding, xPadding);

    [self showAnnotations:annotations edgePadding:edgeInsets animated:animated];
}

- (void)showAnnotations:(NSArray<id <MGLAnnotation>> *)annotations edgePadding:(UIEdgeInsets)insets animated:(BOOL)animated
{
    MGLLogDebug(@"Showing: %lu annotations edgePadding: %@ animated: %@", annotations.count, NSStringFromUIEdgeInsets(insets), MGLStringFromBOOL(animated));
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


#pragma mark Annotation Image Delegate

- (void)annotationImageNeedsRedisplay:(MGLAnnotationImage *)annotationImage
{
    NSString *reuseIdentifier = annotationImage.reuseIdentifier;
    NSString *iconIdentifier = annotationImage.styleIconIdentifier;
    NSString *fallbackReuseIdentifier = MGLDefaultStyleMarkerSymbolName;
    NSString *fallbackIconIdentifier = [MGLAnnotationSpritePrefix stringByAppendingString:fallbackReuseIdentifier];

    if (annotationImage.image)
    {
        // Add the new icon to the style.
        NSString *updatedIconIdentifier = [MGLAnnotationSpritePrefix stringByAppendingString:annotationImage.reuseIdentifier];
        annotationImage.styleIconIdentifier = updatedIconIdentifier;
        [self installAnnotationImage:annotationImage];

        if ([iconIdentifier isEqualToString:fallbackIconIdentifier])
        {
            // Update any annotations associated with the annotation image.
            [self applyIconIdentifier:updatedIconIdentifier toAnnotationsWithImageReuseIdentifier:reuseIdentifier];
        }
    }
    else
    {
        // Add the default icon to the style if necessary.
        annotationImage.styleIconIdentifier = fallbackIconIdentifier;
        if ( ! [self dequeueReusableAnnotationImageWithIdentifier:MGLDefaultStyleMarkerSymbolName])
        {
            [self installAnnotationImage:self.defaultAnnotationImage];
        }

        // Update any annotations associated with the annotation image.
        [self applyIconIdentifier:fallbackIconIdentifier toAnnotationsWithImageReuseIdentifier:reuseIdentifier];
    }
}

- (void)applyIconIdentifier:(NSString *)iconIdentifier toAnnotationsWithImageReuseIdentifier:(NSString *)reuseIdentifier
{
    for (auto &pair : _annotationContextsByAnnotationTag)
    {
        if ([pair.second.imageReuseIdentifier isEqualToString:reuseIdentifier])
        {
            const mbgl::Point<double> point = MGLPointFromLocationCoordinate2D(pair.second.annotation.coordinate);
            self.mbglMap.updateAnnotation(pair.first, mbgl::SymbolAnnotation { point, iconIdentifier.UTF8String ?: "" });
        }
    }
}

#pragma mark - User Location -

- (void)setLocationManager:(nullable id<MGLLocationManager>)locationManager
{
    MGLLogDebug(@"Setting locationManager: %@", locationManager);
    if (!locationManager) {
        locationManager = [[MGLCLLocationManager alloc] init];
    }
    [_locationManager stopUpdatingLocation];
    [_locationManager stopUpdatingHeading];
    _locationManager.delegate = nil;
    
    _locationManager = locationManager;
    _locationManager.delegate = self;
}

- (void)validateLocationServices
{
    BOOL shouldEnableLocationServices = self.showsUserLocation && !self.dormant;

    if (shouldEnableLocationServices)
    {
        if (self.locationManager.authorizationStatus == kCLAuthorizationStatusNotDetermined)
        {
            BOOL requiresWhenInUseUsageDescription = [NSProcessInfo.processInfo isOperatingSystemAtLeastVersion:(NSOperatingSystemVersion){11,0,0}];
            BOOL hasWhenInUseUsageDescription = !![[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSLocationWhenInUseUsageDescription"];
            BOOL hasAlwaysUsageDescription;
            if (requiresWhenInUseUsageDescription)
            {
                hasAlwaysUsageDescription = !![[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSLocationAlwaysAndWhenInUseUsageDescription"] && hasWhenInUseUsageDescription;
            }
            else
            {
                hasAlwaysUsageDescription = !![[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSLocationAlwaysUsageDescription"];
            }

            if (hasAlwaysUsageDescription)
            {
                [self.locationManager requestAlwaysAuthorization];
            }
            else if (hasWhenInUseUsageDescription)
            {
                [self.locationManager requestWhenInUseAuthorization];
            }
            else
            {
                NSString *suggestedUsageKeys = requiresWhenInUseUsageDescription ?
                    @"NSLocationWhenInUseUsageDescription and (optionally) NSLocationAlwaysAndWhenInUseUsageDescription" :
                    @"NSLocationWhenInUseUsageDescription and/or NSLocationAlwaysUsageDescription";
                [NSException raise:MGLMissingLocationServicesUsageDescriptionException
                            format:@"This app must have a value for %@ in its Info.plist.", suggestedUsageKeys];
            }
        }

        [self.locationManager startUpdatingLocation];

        [self validateUserHeadingUpdating];
    }
    else if ( ! shouldEnableLocationServices && self.locationManager)
    {
        [self.locationManager stopUpdatingLocation];
        [self.locationManager stopUpdatingHeading];
    }
}

- (void)setShowsUserLocation:(BOOL)showsUserLocation
{
    MGLLogDebug(@"Setting showsUserLocation: %@", MGLStringFromBOOL(showsUserLocation));
    if (showsUserLocation == _showsUserLocation || _isTargetingInterfaceBuilder) return;

    _showsUserLocation = showsUserLocation;

    if (showsUserLocation)
    {
        if ([self.delegate respondsToSelector:@selector(mapViewWillStartLocatingUser:)])
        {
            [self.delegate mapViewWillStartLocatingUser:self];
        }

        self.userLocation = [[MGLUserLocation alloc] initWithMapView:self];

        MGLUserLocationAnnotationView *userLocationAnnotationView;

        if ([self.delegate respondsToSelector:@selector(mapView:viewForAnnotation:)])
        {
            userLocationAnnotationView = (MGLUserLocationAnnotationView *)[self.delegate mapView:self viewForAnnotation:self.userLocation];
            if (userLocationAnnotationView && ! [userLocationAnnotationView isKindOfClass:MGLUserLocationAnnotationView.class])
            {
                [NSException raise:MGLUserLocationAnnotationTypeException
                            format:@"User location annotation view must be a kind of MGLUserLocationAnnotationView. %@", userLocationAnnotationView.debugDescription];
            }
        }

        self.userLocationAnnotationView = userLocationAnnotationView ?: [[MGLFaux3DUserLocationAnnotationView alloc] init];
        self.userLocationAnnotationView.mapView = self;
        self.userLocationAnnotationView.userLocation = self.userLocation;

        self.userLocationAnnotationView.autoresizingMask = (UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin |
                                                            UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin);

        [self validateLocationServices];
    }
    else
    {
        [self validateLocationServices];

        if ([self.delegate respondsToSelector:@selector(mapViewDidStopLocatingUser:)])
        {
            [self.delegate mapViewDidStopLocatingUser:self];
        }

        [self setUserTrackingMode:MGLUserTrackingModeNone animated:YES];

        [self.userLocationAnnotationView removeFromSuperview];
        self.userLocationAnnotationView = nil;
    }
}

- (void)setUserLocationAnnotationView:(MGLUserLocationAnnotationView *)newAnnotationView
{
    if ( ! [newAnnotationView isEqual:_userLocationAnnotationView])
    {
        _userLocationAnnotationView = newAnnotationView;
        [self updateUserLocationAnnotationView];
    }
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingUserLocation
{
    return [NSSet setWithObject:@"userLocationAnnotationView"];
}

- (BOOL)isUserLocationVisible
{
    if (self.userLocationAnnotationView)
    {
        CGPoint locationPoint = [self convertCoordinate:self.userLocation.coordinate toPointToView:self];

        CGRect locationRect = CGRectMake(locationPoint.x - self.userLocation.location.horizontalAccuracy,
                                         locationPoint.y - self.userLocation.location.horizontalAccuracy,
                                         self.userLocation.location.horizontalAccuracy * 2,
                                         self.userLocation.location.horizontalAccuracy * 2);

        return CGRectIntersectsRect([self bounds], locationRect);
    }

    return NO;
}

- (void)setUserTrackingMode:(MGLUserTrackingMode)mode
{
    MGLLogDebug(@"Setting userTrackingMode: %lu", mode);
    [self setUserTrackingMode:mode animated:YES];
}

- (void)setUserTrackingMode:(MGLUserTrackingMode)mode animated:(BOOL)animated
{
    MGLLogDebug(@"Setting userTrackingMode: %lu animated: %@", mode, MGLStringFromBOOL(animated));
    if (mode == _userTrackingMode) return;

    MGLUserTrackingMode oldMode = _userTrackingMode;
    [self willChangeValueForKey:@"userTrackingMode"];
    _userTrackingMode = mode;
    [self didChangeValueForKey:@"userTrackingMode"];

    switch (_userTrackingMode)
    {
        case MGLUserTrackingModeNone:
        {
            self.userTrackingState = MGLUserTrackingStatePossible;

            // Immediately update the annotation view; other cases update inside
            // the locationManager:didUpdateLocations: method.
            [self updateUserLocationAnnotationView];

            break;
        }
        case MGLUserTrackingModeFollow:
        case MGLUserTrackingModeFollowWithCourse:
        {
            self.userTrackingState = animated ? MGLUserTrackingStatePossible : MGLUserTrackingStateChanged;
            self.showsUserLocation = YES;

            break;
        }
        case MGLUserTrackingModeFollowWithHeading:
        {
            if (oldMode == MGLUserTrackingModeNone)
            {
                self.userTrackingState = animated ? MGLUserTrackingStatePossible : MGLUserTrackingStateChanged;
            }

            self.showsUserLocation = YES;

            if (self.zoomLevel < self.currentMinimumZoom)
            {
                [self setZoomLevel:self.currentMinimumZoom animated:YES];
            }

            break;
        }
    }

    if (_userTrackingMode != MGLUserTrackingModeNone)
    {
        CLLocation *location = self.userLocation.location;
        if (location && self.userLocationAnnotationView)
        {
            [self locationManager:self.locationManager didUpdateLocations:@[location] animated:animated];
        }
    }

    [self validateUserHeadingUpdating];

    if ([self.delegate respondsToSelector:@selector(mapView:didChangeUserTrackingMode:animated:)])
    {
        [self.delegate mapView:self didChangeUserTrackingMode:_userTrackingMode animated:animated];
    }
}

- (void)setUserLocationVerticalAlignment:(MGLAnnotationVerticalAlignment)alignment
{
    [self setUserLocationVerticalAlignment:alignment animated:YES];
}

- (void)setUserLocationVerticalAlignment:(MGLAnnotationVerticalAlignment)alignment animated:(BOOL)animated
{
    _userLocationVerticalAlignment = alignment;
    if (self.userTrackingMode != MGLUserTrackingModeNone)
    {
        CLLocation *location = self.userLocation.location;
        if (location)
        {
            [self locationManager:self.locationManager didUpdateLocations:@[location] animated:animated];
        }
    }
}

- (void)setTargetCoordinate:(CLLocationCoordinate2D)targetCoordinate
{
    MGLLogDebug(@"Setting targetCoordinate: %@", MGLStringFromCLLocationCoordinate2D(targetCoordinate));
    [self setTargetCoordinate:targetCoordinate animated:YES];
}

- (void)setTargetCoordinate:(CLLocationCoordinate2D)targetCoordinate animated:(BOOL)animated
{
    MGLLogDebug(@"Setting targetCoordinate: %@ animated: %@", MGLStringFromCLLocationCoordinate2D(targetCoordinate), MGLStringFromBOOL(animated));
    if (targetCoordinate.latitude != self.targetCoordinate.latitude
        || targetCoordinate.longitude != self.targetCoordinate.longitude)
    {
        _targetCoordinate = targetCoordinate;
        if (self.userTrackingMode == MGLUserTrackingModeFollowWithCourse)
        {
            self.userTrackingState = MGLUserTrackingStatePossible;

            CLLocation *location = self.userLocation.location;
            if (location)
            {
                [self locationManager:self.locationManager didUpdateLocations:@[location] animated:animated];
            }
        }
    }
}

- (void)setShowsUserHeadingIndicator:(BOOL)showsUserHeadingIndicator
{
    MGLLogDebug(@"Setting showsUserHeadingIndicator: %@", MGLStringFromBOOL(showsUserHeadingIndicator));
    _showsUserHeadingIndicator = showsUserHeadingIndicator;

    if (_showsUserHeadingIndicator)
    {
        self.showsUserLocation = YES;
    }
    [self validateUserHeadingUpdating];
}

- (void)validateUserHeadingUpdating
{
    BOOL canShowPermanentHeadingIndicator = self.showsUserHeadingIndicator && self.userTrackingMode != MGLUserTrackingModeFollowWithCourse;

    if (canShowPermanentHeadingIndicator || self.userTrackingMode == MGLUserTrackingModeFollowWithHeading)
    {
        [self updateHeadingForDeviceOrientation];
        [self.locationManager startUpdatingHeading];
    }
    else
    {
        [self.locationManager stopUpdatingHeading];
    }
}

- (void)locationManager:(id<MGLLocationManager>)manager didUpdateLocations:(NSArray *)locations
{
    [self locationManager:manager didUpdateLocations:locations animated:YES];
}

- (void)locationManager:(__unused id<MGLLocationManager>)manager didUpdateLocations:(NSArray *)locations animated:(BOOL)animated
{
    CLLocation *oldLocation = self.userLocation.location;
    CLLocation *newLocation = locations.lastObject;
    _distanceFromOldUserLocation = [newLocation distanceFromLocation:oldLocation];

    if ( ! _showsUserLocation || ! newLocation || ! CLLocationCoordinate2DIsValid(newLocation.coordinate)) return;

    if (! oldLocation || ! CLLocationCoordinate2DIsValid(oldLocation.coordinate) || [newLocation distanceFromLocation:oldLocation]
        || oldLocation.course != newLocation.course)
    {
        if ( ! oldLocation || ! CLLocationCoordinate2DIsValid(oldLocation.coordinate) || self.userTrackingState != MGLUserTrackingStateBegan)
        {
            self.userLocation.location = newLocation;
        }

        if ([self.delegate respondsToSelector:@selector(mapView:didUpdateUserLocation:)])
        {
            [self.delegate mapView:self didUpdateUserLocation:self.userLocation];
        }
    }

    [self didUpdateLocationWithUserTrackingAnimated:animated];

    NSTimeInterval duration = MGLAnimationDuration;
    if (oldLocation && ! CGPointEqualToPoint(self.userLocationAnnotationView.center, CGPointZero))
    {
        duration = MIN([newLocation.timestamp timeIntervalSinceDate:oldLocation.timestamp], MGLUserLocationAnimationDuration);
    }
    [self updateUserLocationAnnotationViewAnimatedWithDuration:duration];

    if (self.userTrackingMode == MGLUserTrackingModeNone &&
        self.userLocationAnnotationView.accessibilityElementIsFocused &&
        [UIApplication sharedApplication].applicationState == UIApplicationStateActive)
    {
        UIAccessibilityPostNotification(UIAccessibilityLayoutChangedNotification, self.userLocationAnnotationView);
    }
}

- (void)didUpdateLocationWithUserTrackingAnimated:(BOOL)animated
{
    CLLocation *location = self.userLocation.location;
    if ( ! _showsUserLocation || ! location
        || ! CLLocationCoordinate2DIsValid(location.coordinate)
        || self.userTrackingMode == MGLUserTrackingModeNone)
    {
        return;
    }

    // If the user location annotation is already where it’s supposed to be,
    // don’t change the viewport.
    CGPoint correctPoint = self.userLocationAnnotationViewCenter;
    CGPoint currentPoint = [self convertCoordinate:self.userLocation.coordinate toPointToView:self];
    if (std::abs(currentPoint.x - correctPoint.x) <= 1.0 && std::abs(currentPoint.y - correctPoint.y) <= 1.0
        && self.userTrackingMode != MGLUserTrackingModeFollowWithCourse)
    {
        return;
    }

    if (self.userTrackingMode == MGLUserTrackingModeFollowWithCourse
        && CLLocationCoordinate2DIsValid(self.targetCoordinate))
    {
        if (self.userTrackingState != MGLUserTrackingStateBegan)
        {
            // Keep both the user and the destination in view.
            [self didUpdateLocationWithTargetAnimated:animated];
        }
    }
    else if (self.userTrackingState == MGLUserTrackingStatePossible)
    {
        // The first location update is often a great distance away from the
        // current viewport, so fly there to provide additional context.
        [self didUpdateLocationSignificantlyAnimated:animated];
    }
    else if (self.userTrackingState == MGLUserTrackingStateChanged)
    {
        // Subsequent updates get a more subtle animation.
        [self didUpdateLocationIncrementallyAnimated:animated];
    }
    [self unrotateIfNeededAnimated:YES];
}

/// Changes the viewport based on an incremental location update.
- (void)didUpdateLocationIncrementallyAnimated:(BOOL)animated
{
    [self _setCenterCoordinate:self.userLocation.location.coordinate
                   edgePadding:self.edgePaddingForFollowing
                     zoomLevel:self.zoomLevel
                     direction:self.directionByFollowingWithCourse
                      duration:animated ? MGLUserLocationAnimationDuration : 0
       animationTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionLinear]
             completionHandler:NULL];
}

/// Changes the viewport based on a significant location update, such as the
/// first location update.
- (void)didUpdateLocationSignificantlyAnimated:(BOOL)animated
{
    
    if (_distanceFromOldUserLocation >= MGLDistanceThresholdForCameraPause) {
        self.userTrackingState = MGLUserTrackingStateBeginSignificantTransition;
    } else {
        self.userTrackingState = MGLUserTrackingStateBegan;
    }

    MGLMapCamera *camera = self.camera;
    camera.centerCoordinate = self.userLocation.location.coordinate;
    camera.heading = self.directionByFollowingWithCourse;
    if (self.zoomLevel < MGLMinimumZoomLevelForUserTracking)
    {
        camera.altitude = MGLAltitudeForZoomLevel(MGLDefaultZoomLevelForUserTracking,
                                                  camera.pitch,
                                                  camera.centerCoordinate.latitude,
                                                  self.frame.size);
    }

    __weak MGLMapView *weakSelf = self;
    [self _flyToCamera:camera
           edgePadding:self.edgePaddingForFollowing
          withDuration:animated ? -1 : 0
          peakAltitude:-1
     completionHandler:^{
        MGLMapView *strongSelf = weakSelf;
        if (strongSelf.userTrackingState == MGLUserTrackingStateBegan ||
            strongSelf.userTrackingState == MGLDistanceThresholdForCameraPause)
        {
            strongSelf.userTrackingState = MGLUserTrackingStateChanged;
        }
    }];
}

/// Changes the viewport based on a location update in the presence of a target
/// coordinate that must also be displayed on the map concurrently.
- (void)didUpdateLocationWithTargetAnimated:(BOOL)animated
{
    BOOL firstUpdate = self.userTrackingState == MGLUserTrackingStatePossible;
    void (^completion)(void);
    if (animated && firstUpdate)
    {
        self.userTrackingState = MGLUserTrackingStateBegan;
        __weak MGLMapView *weakSelf = self;
        completion = ^{
            MGLMapView *strongSelf = weakSelf;
            if (strongSelf.userTrackingState == MGLUserTrackingStateBegan)
            {
                strongSelf.userTrackingState = MGLUserTrackingStateChanged;
            }
        };
    }

    CLLocationCoordinate2D foci[] = {
        self.userLocation.location.coordinate,
        self.targetCoordinate,
    };
    UIEdgeInsets inset = self.edgePaddingForFollowingWithCourse;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    if (self.userLocationVerticalAlignment == MGLAnnotationVerticalAlignmentCenter)
    {
        inset.bottom = CGRectGetMaxY(self.bounds) - CGRectGetMidY(self.contentFrame);
    }
#pragma clang diagnostic pop

    [self _setVisibleCoordinates:foci
                           count:sizeof(foci) / sizeof(foci[0])
                     edgePadding:inset
                       direction:self.directionByFollowingWithCourse
                        duration:animated ? MGLUserLocationAnimationDuration : 0
         animationTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionLinear]
               completionHandler:completion];
}

/// Returns the edge padding to apply when moving the map to a tracked location.
- (UIEdgeInsets)edgePaddingForFollowing
{
    // Center on user location unless we're already centered there (or very close).
    CGPoint correctPoint = self.userLocationAnnotationViewCenter;

    // Shift the entire frame upward or downward to accommodate a shifted user
    // location annotation view.
    CGRect bounds = self.bounds;
    CGRect boundsAroundCorrectPoint = CGRectOffset(bounds,
                                                   correctPoint.x - CGRectGetMidX(bounds),
                                                   correctPoint.y - CGRectGetMidY(bounds));
    return UIEdgeInsetsMake(CGRectGetMinY(boundsAroundCorrectPoint) - CGRectGetMinY(bounds),
                            CGRectGetMaxX(boundsAroundCorrectPoint) - CGRectGetMaxX(bounds),
                            CGRectGetMaxY(bounds) - CGRectGetMaxY(boundsAroundCorrectPoint),
                            CGRectGetMaxX(bounds) - CGRectGetMaxX(boundsAroundCorrectPoint));
}

/// Returns the edge padding to apply during bifocal course tracking.
- (UIEdgeInsets)edgePaddingForFollowingWithCourse
{
    UIEdgeInsets inset = MGLUserLocationAnnotationViewInset;
    inset.top += CGRectGetHeight(self.userLocationAnnotationView.frame);
    inset.bottom += CGRectGetHeight(self.userLocationAnnotationView.frame);
    return inset;
}

/// Returns the direction the map should be turned to due to course tracking.
- (CLLocationDirection)directionByFollowingWithCourse
{
    CLLocationDirection direction = -1;
    if (self.userTrackingMode == MGLUserTrackingModeFollowWithCourse)
    {
        if (CLLocationCoordinate2DIsValid(self.targetCoordinate))
        {
            mbgl::LatLng userLatLng = MGLLatLngFromLocationCoordinate2D(self.userLocation.coordinate);
            mbgl::LatLng targetLatLng = MGLLatLngFromLocationCoordinate2D(self.targetCoordinate);
            mbgl::ProjectedMeters userMeters = mbgl::Projection::projectedMetersForLatLng(userLatLng);
            mbgl::ProjectedMeters targetMeters = mbgl::Projection::projectedMetersForLatLng(targetLatLng);
            double angle = atan2(targetMeters.easting() - userMeters.easting(),
                                 targetMeters.northing() - userMeters.northing());
            direction = mbgl::util::wrap(MGLDegreesFromRadians(angle), 0., 360.);
        }
        else
        {
            direction = self.userLocation.location.course;
        }

        if (direction >= 0)
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
            if (self.userLocationVerticalAlignment == MGLAnnotationVerticalAlignmentTop)
            {
                direction += 180;
            }
#pragma clang diagnostic pop
        }
    }
    return direction;
}

- (BOOL)locationManagerShouldDisplayHeadingCalibration:(id<MGLLocationManager>)manager
{
    if (self.displayHeadingCalibration) [self performSelector:@selector(dismissHeadingCalibrationDisplay:)
                                                      withObject:manager
                                                      afterDelay:10.0];

    return self.displayHeadingCalibration;
}

- (void)dismissHeadingCalibrationDisplay:(id<MGLLocationManager>)manager
{
    [manager dismissHeadingCalibrationDisplay];
}

- (void)locationManager:(__unused id<MGLLocationManager>)manager didUpdateHeading:(CLHeading *)newHeading
{
    if ( ! _showsUserLocation || self.pan.state == UIGestureRecognizerStateBegan || newHeading.headingAccuracy < 0) return;

    self.userLocation.heading = newHeading;

    if (self.showsUserHeadingIndicator || self.userTrackingMode == MGLUserTrackingModeFollowWithHeading)
    {
        [self updateUserLocationAnnotationView];
    }

    if ([self.delegate respondsToSelector:@selector(mapView:didUpdateUserLocation:)])
    {
        [self.delegate mapView:self didUpdateUserLocation:self.userLocation];

        if ( ! _showsUserLocation) return;
    }

    CLLocationDirection headingDirection = (newHeading.trueHeading >= 0 ? newHeading.trueHeading : newHeading.magneticHeading);

    if (headingDirection >= 0 && self.userTrackingMode == MGLUserTrackingModeFollowWithHeading
        && self.userTrackingState != MGLUserTrackingStateBegan)
    {
        [self _setDirection:headingDirection animated:YES];
    }
}

- (void)locationManager:(__unused id<MGLLocationManager>)manager didFailWithError:(NSError *)error
{
    if ([error code] == kCLErrorDenied)
    {
        self.userTrackingMode  = MGLUserTrackingModeNone;
        self.showsUserLocation = NO;

        if ([self.delegate respondsToSelector:@selector(mapView:didFailToLocateUserWithError:)])
        {
            [self.delegate mapView:self didFailToLocateUserWithError:error];
        }
    }
}

- (void)updateHeadingForDeviceOrientation
{
    if (self.locationManager)
    {
        // note that right/left device and interface orientations are opposites (see UIApplication.h)
        //
        CLDeviceOrientation orientation;
        switch ([[UIApplication sharedApplication] statusBarOrientation])
        {
            case (UIInterfaceOrientationLandscapeLeft):
            {
                orientation = CLDeviceOrientationLandscapeRight;
                break;
            }
            case (UIInterfaceOrientationLandscapeRight):
            {
                orientation = CLDeviceOrientationLandscapeLeft;
                break;
            }
            case (UIInterfaceOrientationPortraitUpsideDown):
            {
                orientation = CLDeviceOrientationPortraitUpsideDown;
                break;
            }
            case (UIInterfaceOrientationPortrait):
            default:
            {
                orientation = CLDeviceOrientationPortrait;
                break;
            }
        }

        // Setting the location manager's heading orientation causes it to send
        // a heading event, which in turn makes us redraw, which kicks off a
        // loop... so don't do that. rdar://34059173
        if (self.locationManager.headingOrientation != orientation)
        {
            self.locationManager.headingOrientation = orientation;
        }
    }
}

#pragma mark Data

- (NSArray<id <MGLFeature>> *)visibleFeaturesAtPoint:(CGPoint)point
{
    MGLLogDebug(@"Querying visibleFeaturesAtPoint: %@", NSStringFromCGPoint(point));
    return [self visibleFeaturesAtPoint:point inStyleLayersWithIdentifiers:nil];
}

- (NSArray<id <MGLFeature>> *)visibleFeaturesAtPoint:(CGPoint)point inStyleLayersWithIdentifiers:(NSSet<NSString *> *)styleLayerIdentifiers {
    MGLLogDebug(@"Querying visibleFeaturesAtPoint: %@ inStyleLayersWithIdentifiers: %@", NSStringFromCGPoint(point), styleLayerIdentifiers);
    return [self visibleFeaturesAtPoint:point inStyleLayersWithIdentifiers:styleLayerIdentifiers predicate:nil];
}

- (NSArray<id <MGLFeature>> *)visibleFeaturesAtPoint:(CGPoint)point inStyleLayersWithIdentifiers:(NSSet<NSString *> *)styleLayerIdentifiers predicate:(NSPredicate *)predicate
{
    MGLLogDebug(@"Querying visibleFeaturesAtPoint: %@ inStyleLayersWithIdentifiers: %@ predicate: %@", NSStringFromCGPoint(point), styleLayerIdentifiers, predicate);
    mbgl::ScreenCoordinate screenCoordinate = { point.x, point.y };

    mbgl::optional<std::vector<std::string>> optionalLayerIDs;
    if (styleLayerIdentifiers)
    {
        __block std::vector<std::string> layerIDs;
        layerIDs.reserve(styleLayerIdentifiers.count);
        [styleLayerIdentifiers enumerateObjectsUsingBlock:^(NSString * _Nonnull identifier, BOOL * _Nonnull stop)
        {
            layerIDs.push_back(identifier.UTF8String);
        }];
        optionalLayerIDs = layerIDs;
    }
    
    mbgl::optional<mbgl::style::Filter> optionalFilter;
    if (predicate) {
        optionalFilter = predicate.mgl_filter;
    }

    std::vector<mbgl::Feature> features = _rendererFrontend->getRenderer()->queryRenderedFeatures(screenCoordinate, { optionalLayerIDs, optionalFilter });
    return MGLFeaturesFromMBGLFeatures(features);
}

- (NSArray<id <MGLFeature>> *)visibleFeaturesInRect:(CGRect)rect {
    MGLLogDebug(@"Querying visibleFeaturesInRect: %@", NSStringFromCGRect(rect));
    return [self visibleFeaturesInRect:rect inStyleLayersWithIdentifiers:nil];
}

- (NSArray<id <MGLFeature>> *)visibleFeaturesInRect:(CGRect)rect inStyleLayersWithIdentifiers:(NSSet<NSString *> *)styleLayerIdentifiers {
    MGLLogDebug(@"Querying visibleFeaturesInRect: %@ inStyleLayersWithIdentifiers: %@", NSStringFromCGRect(rect), styleLayerIdentifiers);
    return [self visibleFeaturesInRect:rect inStyleLayersWithIdentifiers:styleLayerIdentifiers predicate:nil];
}

- (NSArray<id <MGLFeature>> *)visibleFeaturesInRect:(CGRect)rect inStyleLayersWithIdentifiers:(NSSet<NSString *> *)styleLayerIdentifiers predicate:(NSPredicate *)predicate {
    MGLLogDebug(@"Querying visibleFeaturesInRect: %@ inStyleLayersWithIdentifiers: %@ predicate: %@", NSStringFromCGRect(rect), styleLayerIdentifiers, predicate);
    mbgl::ScreenBox screenBox = {
        { CGRectGetMinX(rect), CGRectGetMinY(rect) },
        { CGRectGetMaxX(rect), CGRectGetMaxY(rect) },
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
    
    mbgl::optional<mbgl::style::Filter> optionalFilter;
    if (predicate) {
        optionalFilter = predicate.mgl_filter;
    }

    std::vector<mbgl::Feature> features = _rendererFrontend->getRenderer()->queryRenderedFeatures(screenBox, { optionalLayerIDs, optionalFilter });
    return MGLFeaturesFromMBGLFeatures(features);
}

#pragma mark - Utility -

- (void)animateWithDelay:(NSTimeInterval)delay animations:(void (^)(void))animations
{
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delay * NSEC_PER_SEC)), dispatch_get_main_queue(), animations);
}

- (CGFloat)currentMinimumZoom
{
    return fmaxf(self.mbglMap.getMinZoom(), MGLMinimumZoom);
}

- (BOOL)isRotationAllowed
{
    return (self.zoomLevel >= self.currentMinimumZoom);
}

- (void)unrotateIfNeededForGesture
{
    // Avoid contention with in-progress gestures.
    UIGestureRecognizerState state = self.pinch.state;
    if (self.direction != 0
        && state != UIGestureRecognizerStateBegan
        && state != UIGestureRecognizerStateChanged)
    {
        [self unrotateIfNeededAnimated:YES];

        // Snap to north.
        if ((self.direction < MGLToleranceForSnappingToNorth
             || self.direction > 360 - MGLToleranceForSnappingToNorth)
            && self.userTrackingMode != MGLUserTrackingModeFollowWithHeading
            && self.userTrackingMode != MGLUserTrackingModeFollowWithCourse)
        {
            [self resetNorthAnimated:YES];
        }
    }
}

/// Rotate back to true north if the map view is zoomed too far out.
- (void)unrotateIfNeededAnimated:(BOOL)animated
{
    if (self.direction != 0 && ! self.isRotationAllowed
        && self.userTrackingState != MGLUserTrackingStateBegan)
    {
        if (animated)
        {
            self.userInteractionEnabled = NO;

            __weak MGLMapView *weakSelf = self;

            [self animateWithDelay:0.1 animations:^
            {
                [weakSelf resetNorthAnimated:YES];

                [self animateWithDelay:MGLAnimationDuration animations:^
                {
                    weakSelf.userInteractionEnabled = YES;
                }];

            }];
        }
        else
        {
            [self resetNorthAnimated:NO];
        }
    }
}

- (void)cameraWillChangeAnimated:(BOOL)animated {
    if (!_mbglMap)
    {
        return;
    }

    if ( ! _userLocationAnnotationIsSelected
            || self.userTrackingMode == MGLUserTrackingModeNone
            || self.userTrackingState != MGLUserTrackingStateChanged)
    {
        UIView<MGLCalloutView> *calloutView = self.calloutViewForSelectedAnnotation;
        BOOL dismissesAutomatically = (calloutView
                && [calloutView respondsToSelector:@selector(dismissesAutomatically)]
                && calloutView.dismissesAutomatically);
        // dismissesAutomatically is an optional property and we want to dismiss
        // the callout view if it's unimplemented.
        if (dismissesAutomatically || (calloutView && ![calloutView respondsToSelector:@selector(dismissesAutomatically)]))
        {
            [self deselectAnnotation:self.selectedAnnotation animated:NO];
        }
    }

    if ( ! [self isSuppressingChangeDelimiters] )
    {
        if ([self.delegate respondsToSelector:@selector(mapView:regionWillChangeWithReason:animated:)])
        {
            [self.delegate mapView:self regionWillChangeWithReason:self.cameraChangeReasonBitmask animated:animated];
        }
        else if ([self.delegate respondsToSelector:@selector(mapView:regionWillChangeAnimated:)])
        {
            [self.delegate mapView:self regionWillChangeAnimated:animated];
        }
    }
}

- (void)cameraIsChanging {
    if (!_mbglMap)
    {
        return;
    }

    [self updateCompass];
    [self updateScaleBar];

    if ([self.delegate respondsToSelector:@selector(mapView:regionIsChangingWithReason:)])
    {
        [self.delegate mapView:self regionIsChangingWithReason:self.cameraChangeReasonBitmask];
    }
    else if ([self.delegate respondsToSelector:@selector(mapViewRegionIsChanging:)])
    {
        [self.delegate mapViewRegionIsChanging:self];
    }
}

- (void)cameraDidChangeAnimated:(BOOL)animated {
    if (!_mbglMap)
    {
        return;
    }

    [self updateCompass];
    [self updateScaleBar];

    if ( ! [self isSuppressingChangeDelimiters])
    {
        BOOL respondsToSelector = [self.delegate respondsToSelector:@selector(mapView:regionDidChangeAnimated:)];
        BOOL respondsToSelectorWithReason = [self.delegate respondsToSelector:@selector(mapView:regionDidChangeWithReason:animated:)];

        if ((respondsToSelector || respondsToSelectorWithReason) &&
            ([UIApplication sharedApplication].applicationState == UIApplicationStateActive))
        {
            _featureAccessibilityElements = nil;
            _visiblePlaceFeatures = nil;
            _visibleRoadFeatures = nil;
            if (_accessibilityValueAnnouncementIsPending) {
                _accessibilityValueAnnouncementIsPending = NO;
                [self performSelector:@selector(announceAccessibilityValue) withObject:nil afterDelay:0.1];
            } else {
                UIAccessibilityPostNotification(UIAccessibilityLayoutChangedNotification, nil);
            }
        }

        if (respondsToSelectorWithReason)
        {
            [self.delegate mapView:self regionDidChangeWithReason:self.cameraChangeReasonBitmask animated:animated];
        }
        else if (respondsToSelector)
        {
            [self.delegate mapView:self regionDidChangeAnimated:animated];
        }

        [self resetCameraChangeReason];
    }
}

- (void)announceAccessibilityValue
{
    UIAccessibilityPostNotification(UIAccessibilityAnnouncementNotification, self.accessibilityValue);
    UIAccessibilityPostNotification(UIAccessibilityLayoutChangedNotification, nil);
}

- (void)mapViewWillStartLoadingMap {
    if (!_mbglMap)
    {
        return;
    }

    if ([self.delegate respondsToSelector:@selector(mapViewWillStartLoadingMap:)])
    {
        [self.delegate mapViewWillStartLoadingMap:self];
    }
}

- (void)mapViewDidFinishLoadingMap {
    if (!_mbglMap)
    {
        return;
    }

    [self.style willChangeValueForKey:@"sources"];
    [self.style didChangeValueForKey:@"sources"];
    [self.style willChangeValueForKey:@"layers"];
    [self.style didChangeValueForKey:@"layers"];
    if ([self.delegate respondsToSelector:@selector(mapViewDidFinishLoadingMap:)])
    {
        [self.delegate mapViewDidFinishLoadingMap:self];
    }
}

- (void)mapViewDidFailLoadingMapWithError:(NSError *)error {
    if (!_mbglMap)
    {
        return;
    }

    if ([self.delegate respondsToSelector:@selector(mapViewDidFailLoadingMap:withError:)])
    {
        [self.delegate mapViewDidFailLoadingMap:self withError:error];
    }
}

- (void)mapViewWillStartRenderingFrame {
    if (!_mbglMap)
    {
        return;
    }

    if ([self.delegate respondsToSelector:@selector(mapViewWillStartRenderingFrame:)])
    {
        [self.delegate mapViewWillStartRenderingFrame:self];
    }
}

- (void)mapViewDidFinishRenderingFrameFullyRendered:(BOOL)fullyRendered {
    if (!_mbglMap)
    {
        return;
    }

    if (_isChangingAnnotationLayers)
    {
        _isChangingAnnotationLayers = NO;
        [self.style didChangeValueForKey:@"layers"];
    }

    if ([self.delegate respondsToSelector:@selector(mapViewDidFinishRenderingFrame:fullyRendered:)])
    {
        [self.delegate mapViewDidFinishRenderingFrame:self fullyRendered:fullyRendered];
    }
}

- (void)mapViewWillStartRenderingMap {
    if (!_mbglMap)
    {
        return;
    }

    if ([self.delegate respondsToSelector:@selector(mapViewWillStartRenderingMap:)])
    {
        [self.delegate mapViewWillStartRenderingMap:self];
    }
}

- (void)mapViewDidFinishRenderingMapFullyRendered:(BOOL)fullyRendered {
    if (!_mbglMap)
    {
        return;
    }
    
    UIAccessibilityPostNotification(UIAccessibilityLayoutChangedNotification, nil);

    if ([self.delegate respondsToSelector:@selector(mapViewDidFinishRenderingMap:fullyRendered:)])
    {
        [self.delegate mapViewDidFinishRenderingMap:self fullyRendered:fullyRendered];
    }
}

- (void)mapViewDidBecomeIdle {
    if (!_mbglMap) {
        return;
    }
    
    if ([self.delegate respondsToSelector:@selector(mapViewDidBecomeIdle:)]) {
        [self.delegate mapViewDidBecomeIdle:self];
    }
}

- (void)didFinishLoadingStyle {
    if (!_mbglMap)
    {
        return;
    }

    self.style = [[MGLStyle alloc] initWithRawStyle:&self.mbglMap.getStyle() mapView:self];
    if ([self.delegate respondsToSelector:@selector(mapView:didFinishLoadingStyle:)])
    {
        [self.delegate mapView:self didFinishLoadingStyle:self.style];
    }
}

- (void)updateUserLocationAnnotationView
{
    [self updateUserLocationAnnotationViewAnimatedWithDuration:0];
}

- (void)updateAnnotationViews
{
    BOOL delegateImplementsViewForAnnotation = [self.delegate respondsToSelector:@selector(mapView:viewForAnnotation:)];

    if (!delegateImplementsViewForAnnotation)
    {
        return;
    }

    // If the map is pitched consider the viewport to be exactly the same as the bounds.
    // Otherwise, add a small buffer.
    CGFloat largestWidth = MAX(_largestAnnotationViewSize.width, CGRectGetWidth(self.frame));
    CGFloat largestHeight = MAX(_largestAnnotationViewSize.height, CGRectGetHeight(self.frame));
    CGFloat widthAdjustment = self.camera.pitch > 0.0 ? 0.0 : -largestWidth * 2.0;
    CGFloat heightAdjustment = self.camera.pitch > 0.0 ? 0.0 : -largestHeight * 2.0;
    CGRect viewPort = CGRectInset(self.bounds, widthAdjustment, heightAdjustment);

    NSArray *visibleAnnotations = [self visibleAnnotationsInRect:viewPort];
    NSMutableArray *offscreenAnnotations = [self.annotations mutableCopy];
    [offscreenAnnotations removeObjectsInArray:visibleAnnotations];

    // Update the center of visible annotation views
    for (id<MGLAnnotation> annotation in visibleAnnotations)
    {
        // Defer to the shape/polygon styling delegate methods
        if ([annotation isKindOfClass:[MGLMultiPoint class]])
        {
            continue;
        }

        // Get the annotation tag then use it to get the context.
        MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
        MGLAssert(annotationTag != MGLAnnotationTagNotFound, @"-visibleAnnotationsInRect: returned unrecognized annotation");
        MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(annotationTag);

        MGLAnnotationView *annotationView = annotationContext.annotationView;
        if (!annotationView)
        {
            // This will dequeue views if the delegate implements the dequeue call
            annotationView = [self annotationViewForAnnotation:annotationContext.annotation];

            if (annotationView)
            {
                annotationView.mapView = self;
                annotationContext.annotationView = annotationView;

                // New annotation (created because there is nothing to dequeue) may not have been added to the
                // container view yet. Add them here.
                if (!annotationView.superview) {
                    [self.annotationContainerView insertSubview:annotationView atIndex:0];
                }
            }
        }

        if (annotationView)
        {
            annotationView.center = MGLPointRounded([self convertCoordinate:annotationContext.annotation.coordinate toPointToView:self]);
        }
    }

    MGLCoordinateBounds coordinateBounds = [self convertRect:viewPort toCoordinateBoundsFromView:self];

    // Enqueue (and move if required) offscreen annotation views
    for (id<MGLAnnotation> annotation in offscreenAnnotations)
    {
        // Defer to the shape/polygon styling delegate methods
        if ([annotation isKindOfClass:[MGLMultiPoint class]])
        {
            continue;
        }

        MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
        MGLAssert(annotationTag != MGLAnnotationTagNotFound, @"-visibleAnnotationsInRect: returned unrecognized annotation");
        MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(annotationTag);
        UIView *annotationView = annotationContext.annotationView;

        if (annotationView)
        {
            CLLocationCoordinate2D coordinate = annotation.coordinate;
            // Every so often (1 out of 1000 frames?) the mbgl query mechanism fails. This logic spot checks the
            // offscreenAnnotations values -- if they are actually still on screen then the view center is
            // moved and the enqueue operation is avoided. This allows us to keep the performance benefit of
            // using the mbgl query result. It also forces views that have just gone offscreen to be cleared
            // fully from view.
            if (MGLCoordinateInCoordinateBounds(coordinate, coordinateBounds))
            {
                annotationView.center = [self convertCoordinate:annotationContext.annotation.coordinate toPointToView:self];
            }
            else
            {
                if (annotationView.layer.animationKeys.count > 0) {
                    continue;
                }

                // Move the annotation view far out of view to the left
                CGPoint adjustedCenter = annotationView.center;
                adjustedCenter.x = -CGRectGetWidth(self.frame) * 10.0;
                annotationView.center = adjustedCenter;

                [self enqueueAnnotationViewForAnnotationContext:annotationContext];
            }
        }
    }
}

- (void)updateCalloutView
{
    UIView <MGLCalloutView> *calloutView = self.calloutViewForSelectedAnnotation;
    id <MGLAnnotation> annotation = calloutView.representedObject;

    BOOL isAnchoredToAnnotation = (calloutView
                                   && annotation
                                   && [calloutView respondsToSelector:@selector(isAnchoredToAnnotation)]
                                   && calloutView.isAnchoredToAnnotation);

    if (isAnchoredToAnnotation)
    {
        MGLAnnotationTag tag = [self annotationTagForAnnotation:annotation];
        MGLAnnotationView *annotationView = nil;

        if (tag != MGLAnnotationTagNotFound) {
            MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(tag);
            annotationView = annotationContext.annotationView;
        } else if (annotation == self.userLocation) {
            annotationView = self.userLocationAnnotationView;
        }

        CGRect positioningRect = annotationView ?
            annotationView.frame :
            [self positioningRectForCalloutForAnnotationWithTag:tag];

        MGLAssert( ! CGRectIsNull(positioningRect), @"Positioning rect should not be CGRectNull by this point");

        CGPoint centerPoint = CGPointMake(CGRectGetMidX(positioningRect), CGRectGetMinY(positioningRect));

        if ( ! CGPointEqualToPoint(calloutView.center, centerPoint)) {
            calloutView.center = centerPoint;
        }
    }
}

- (void)enqueueAnnotationViewForAnnotationContext:(MGLAnnotationContext &)annotationContext
{
    MGLAnnotationView *annotationView = annotationContext.annotationView;

    if (!annotationView) return;

    if (annotationContext.viewReuseIdentifier)
    {
        annotationView.annotation = nil;
        NSMutableArray *annotationViewReuseQueue = [self annotationViewReuseQueueForIdentifier:annotationContext.viewReuseIdentifier];
        if (![annotationViewReuseQueue containsObject:annotationView])
        {
            [annotationViewReuseQueue addObject:annotationView];
            annotationContext.annotationView = nil;
        }
    }
}

- (void)updateUserLocationAnnotationViewAnimatedWithDuration:(NSTimeInterval)duration
{
    MGLUserLocationAnnotationView *annotationView = self.userLocationAnnotationView;
    if ( ! CLLocationCoordinate2DIsValid(self.userLocation.coordinate)) {
        annotationView.hidden = YES;
        return;
    }

    CGPoint userPoint;
    if (self.userTrackingMode != MGLUserTrackingModeNone
        && self.userTrackingState == MGLUserTrackingStateChanged)
    {
        userPoint = self.userLocationAnnotationViewCenter;
    }
    else
    {
        userPoint = MGLPointRounded([self convertCoordinate:self.userLocation.coordinate toPointToView:self]);
    }

    if ( ! annotationView.superview)
    {
        [self.glView addSubview:annotationView];
        // Prevents the view from sliding in from the origin.
        annotationView.center = userPoint;
    }

    if (CGRectContainsPoint(CGRectInset(self.bounds, -MGLAnnotationUpdateViewportOutset.width,
        -MGLAnnotationUpdateViewportOutset.height), userPoint))
    {
        // Smoothly move the user location annotation view and callout view to
        // the new location.
        
        dispatch_block_t animation = ^{
            if (self.selectedAnnotation == self.userLocation)
            {
                UIView <MGLCalloutView> *calloutView = self.calloutViewForSelectedAnnotation;
                calloutView.frame = CGRectOffset(calloutView.frame,
                                                 userPoint.x - annotationView.center.x,
                                                 userPoint.y - annotationView.center.y);
            }
            annotationView.center = userPoint;
        };
        
        if (duration > 0) {
            [UIView animateWithDuration:duration
                                  delay:0
                                options:(UIViewAnimationOptionCurveLinear |
                                         UIViewAnimationOptionAllowUserInteraction |
                                         UIViewAnimationOptionBeginFromCurrentState)
                             animations:animation
                             completion:NULL];
        }
        else {
            animation();
        }
        _userLocationAnimationCompletionDate = [NSDate dateWithTimeIntervalSinceNow:duration];

        annotationView.hidden = NO;
        [annotationView update];
    }
    else
    {
        // User has moved far enough outside of the viewport that showing it or
        // its callout would be useless.
        annotationView.hidden = YES;

        if (_userLocationAnnotationIsSelected)
        {
            [self deselectAnnotation:self.selectedAnnotation animated:YES];
        }
    }
}

/// Intended center point of the user location annotation view with respect to
/// the overall map view (but respecting the content inset).
- (CGPoint)userLocationAnnotationViewCenter
{
    if ([self.delegate respondsToSelector:@selector(mapViewUserLocationAnchorPoint:)])
    {
        CGPoint anchorPoint = [self.delegate mapViewUserLocationAnchorPoint:self];
        return CGPointMake(anchorPoint.x + self.contentInset.left, anchorPoint.y + self.contentInset.top);
    }
    
    CGRect contentFrame = UIEdgeInsetsInsetRect(self.contentFrame, self.edgePaddingForFollowingWithCourse);
    
    if (CGRectIsEmpty(contentFrame))
    {
        contentFrame = self.contentFrame;
    }
    
    CGPoint center = CGPointMake(CGRectGetMidX(contentFrame), CGRectGetMidY(contentFrame));

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    switch (self.userLocationVerticalAlignment) {
        case MGLAnnotationVerticalAlignmentCenter:
            break;
        case MGLAnnotationVerticalAlignmentTop:
            center.y = CGRectGetMinY(contentFrame);
            break;
        case MGLAnnotationVerticalAlignmentBottom:
            center.y = CGRectGetMaxY(contentFrame);
            break;
    }
#pragma clang diagnostic pop
    
    return center;
}

- (void)updateCompass
{
    CLLocationDirection direction = self.direction;
    CLLocationDirection plateDirection = mbgl::util::wrap(-direction, 0., 360.);
    self.compassView.transform = CGAffineTransformMakeRotation(MGLRadiansFromDegrees(plateDirection));

    self.compassView.isAccessibilityElement = direction > 0;
    self.compassView.accessibilityValue = [_accessibilityCompassFormatter stringFromDirection:direction];

    if (direction > 0 && self.compassView.alpha < 1)
    {
        [UIView animateWithDuration:MGLAnimationDuration
                              delay:0
                            options:UIViewAnimationOptionBeginFromCurrentState
                         animations:^
                         {
                             self.compassView.alpha = 1;
                         }
                         completion:nil];
    }
    else if (direction == 0 && self.compassView.alpha > 0)
    {
        [UIView animateWithDuration:MGLAnimationDuration
                              delay:0
                            options:UIViewAnimationOptionBeginFromCurrentState
                         animations:^
                         {
                             self.compassView.alpha = 0;
                         }
                         completion:nil];
    }
}

- (void)updateScaleBar
{
    // Use the `hidden` property (instead of `self.showsScale`) so that we don't
    // break developers who still rely on the <4.0.0 approach of directly
    // setting this property.
    if ( ! self.scaleBar.hidden)
    {
        [(MGLScaleBar *)self.scaleBar setMetersPerPoint:[self metersPerPointAtLatitude:self.centerCoordinate.latitude]];
    }
}

+ (UIImage *)resourceImageNamed:(NSString *)imageName
{
    UIImage *image = [UIImage imageNamed:imageName
                                inBundle:[NSBundle mgl_frameworkBundle]
           compatibleWithTraitCollection:nil];

    if ( ! image)
    {
        [NSException raise:MGLResourceNotFoundException format:
         @"The resource named “%@” could not be found in the Mapbox framework bundle.", imageName];
    }

    return image;
}

- (BOOL)isFullyLoaded
{
    return self.mbglMap.isFullyLoaded();
}

- (void)prepareForInterfaceBuilder
{
    [super prepareForInterfaceBuilder];

    self.layer.borderColor = [UIColor colorWithRed:59/255.
                                             green:178/255.
                                              blue:208/255.
                                             alpha:0.8].CGColor;
    self.layer.borderWidth = 4;
    self.layer.backgroundColor = [UIColor whiteColor].CGColor;

    UIView *diagnosticView = [[UIView alloc] init];
    diagnosticView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:diagnosticView];

    // Headline
    UILabel *headlineLabel = [[UILabel alloc] init];
    headlineLabel.text = NSStringFromClass([self class]);
    headlineLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleHeadline];
    headlineLabel.textAlignment = NSTextAlignmentCenter;
    headlineLabel.numberOfLines = 1;
    headlineLabel.translatesAutoresizingMaskIntoConstraints = NO;
    [headlineLabel setContentCompressionResistancePriority:UILayoutPriorityDefaultLow
                                                   forAxis:UILayoutConstraintAxisHorizontal];
    [diagnosticView addSubview:headlineLabel];

    // Explanation
    UILabel *explanationLabel = [[UILabel alloc] init];
    explanationLabel.text = [NSString stringWithFormat:NSLocalizedStringWithDefaultValue(@"DESIGNABLE", nil, nil, @"To display a Mapbox-hosted map here, set %@ to your access token in %@\n\nFor detailed instructions, see:", @"Instructions in Interface Builder designable; {key}, {plist file name}"), @"MGLMapboxAccessToken", @"Info.plist"];
    explanationLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
    explanationLabel.numberOfLines = 0;
    explanationLabel.translatesAutoresizingMaskIntoConstraints = NO;
    [explanationLabel setContentCompressionResistancePriority:UILayoutPriorityDefaultLow
                                                      forAxis:UILayoutConstraintAxisHorizontal];
    [diagnosticView addSubview:explanationLabel];

    // Link
    UIButton *linkButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [linkButton setTitle:NSLocalizedStringWithDefaultValue(@"FIRST_STEPS_URL", nil, nil, @"docs.mapbox.com/help/tutorials/first-steps-ios-sdk", @"Setup documentation URL display string; keep as short as possible") forState:UIControlStateNormal];
    linkButton.translatesAutoresizingMaskIntoConstraints = NO;
    linkButton.titleLabel.numberOfLines = 0;
    [linkButton setContentCompressionResistancePriority:UILayoutPriorityDefaultLow
                                                forAxis:UILayoutConstraintAxisHorizontal];
    [diagnosticView addSubview:linkButton];

    // Constraints
    NSDictionary *views = @{
        @"container": diagnosticView,
        @"headline": headlineLabel,
        @"explanation": explanationLabel,
        @"link": linkButton,
    };
    [self addConstraint:
     [NSLayoutConstraint constraintWithItem:diagnosticView
                                  attribute:NSLayoutAttributeCenterYWithinMargins
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeCenterYWithinMargins
                                 multiplier:1
                                   constant:0]];
    [self addConstraint:
     [NSLayoutConstraint constraintWithItem:diagnosticView
                                  attribute:NSLayoutAttributeTopMargin
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeTopMargin
                                 multiplier:1
                                   constant:8]];
    [self addConstraint:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeBottomMargin
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:diagnosticView
                                  attribute:NSLayoutAttributeBottomMargin
                                 multiplier:1
                                   constant:8]];
    [self addConstraints:
     [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-[container(20@20)]-|"
                                             options:NSLayoutFormatAlignAllCenterY
                                             metrics:nil
                                               views:views]];
    [self addConstraints:
     [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[headline]-[explanation]-[link]|"
                                             options:0
                                             metrics:nil
                                               views:views]];
    [self addConstraints:
     [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[headline]|"
                                             options:0
                                             metrics:nil
                                               views:views]];
    [self addConstraints:
     [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[explanation]|"
                                             options:0
                                             metrics:nil
                                               views:views]];
    [self addConstraints:
     [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[link]|"
                                             options:0
                                             metrics:nil
                                               views:views]];
}

- (NSMutableArray<MGLAnnotationView *> *)annotationViewReuseQueueForIdentifier:(NSString *)identifier {
    if (!_annotationViewReuseQueueByIdentifier[identifier])
    {
        _annotationViewReuseQueueByIdentifier[identifier] = [NSMutableArray array];
    }

    return _annotationViewReuseQueueByIdentifier[identifier];
}

class MBGLView : public mbgl::RendererBackend, public mbgl::MapObserver
{
public:
    MBGLView(MGLMapView* nativeView_) : nativeView(nativeView_) {
    }

    /// This function is called before we start rendering, when iOS invokes our rendering method.
    /// iOS already sets the correct framebuffer and viewport for us, so we need to update the
    /// context state with the anticipated values.
    void updateAssumedState() override {
        assumeFramebufferBinding(ImplicitFramebufferBinding);
        assumeViewport(0, 0, nativeView.framebufferSize);
    }

    void bind() override {
        if (!implicitFramebufferBound()) {
            // Something modified our state, and we need to bind the original drawable again.
            // Doing this also sets the viewport to the full framebuffer.
            // Note that in reality, iOS does not use the Framebuffer 0 (it's typically 1), and we
            // only use this is a placeholder value.
            [nativeView.glView bindDrawable];
            updateAssumedState();
        } else {
            // Our framebuffer is still bound, but the viewport might have changed.
            setViewport(0, 0, nativeView.framebufferSize);
        }
    }

    mbgl::Size getFramebufferSize() const override {
        return nativeView.framebufferSize;
    }

    void onCameraWillChange(mbgl::MapObserver::CameraChangeMode mode) override {
        bool animated = mode == mbgl::MapObserver::CameraChangeMode::Animated;
        [nativeView cameraWillChangeAnimated:animated];
    }

    void onCameraIsChanging() override {
        [nativeView cameraIsChanging];
    }

    void onCameraDidChange(mbgl::MapObserver::CameraChangeMode mode) override {
        bool animated = mode == mbgl::MapObserver::CameraChangeMode::Animated;
        [nativeView cameraDidChangeAnimated:animated];
    }

    void onWillStartLoadingMap() override {
        [nativeView mapViewWillStartLoadingMap];
    }

    void onDidFinishLoadingMap() override {
        [nativeView mapViewDidFinishLoadingMap];
    }

    void onDidFailLoadingMap(std::exception_ptr exception) override {
        NSString *description;
        MGLErrorCode code;
        try {
            std::rethrow_exception(exception);
        } catch (const mbgl::util::StyleParseException&) {
            code = MGLErrorCodeParseStyleFailed;
            description = NSLocalizedStringWithDefaultValue(@"PARSE_STYLE_FAILED_DESC", nil, nil, @"The map failed to load because the style is corrupted.", @"User-friendly error description");
        } catch (const mbgl::util::StyleLoadException&) {
            code = MGLErrorCodeLoadStyleFailed;
            description = NSLocalizedStringWithDefaultValue(@"LOAD_STYLE_FAILED_DESC", nil, nil, @"The map failed to load because the style can't be loaded.", @"User-friendly error description");
        } catch (const mbgl::util::NotFoundException&) {
            code = MGLErrorCodeNotFound;
            description = NSLocalizedStringWithDefaultValue(@"STYLE_NOT_FOUND_DESC", nil, nil, @"The map failed to load because the style can’t be found or is incompatible.", @"User-friendly error description");
        } catch (...) {
            code = MGLErrorCodeUnknown;
            description = NSLocalizedStringWithDefaultValue(@"LOAD_MAP_FAILED_DESC", nil, nil, @"The map failed to load because an unknown error occurred.", @"User-friendly error description");
        }
        NSDictionary *userInfo = @{
            NSLocalizedDescriptionKey: description,
            NSLocalizedFailureReasonErrorKey: @(mbgl::util::toString(exception).c_str()),
        };
        NSError *error = [NSError errorWithDomain:MGLErrorDomain code:code userInfo:userInfo];
        [nativeView mapViewDidFailLoadingMapWithError:error];
    }

    void onWillStartRenderingFrame() override {
        [nativeView mapViewWillStartRenderingFrame];
    }

    void onDidFinishRenderingFrame(mbgl::MapObserver::RenderMode mode) override {
        bool fullyRendered = mode == mbgl::MapObserver::RenderMode::Full;
        [nativeView mapViewDidFinishRenderingFrameFullyRendered:fullyRendered];
    }

    void onWillStartRenderingMap() override {
        [nativeView mapViewWillStartRenderingMap];
    }

    void onDidFinishRenderingMap(mbgl::MapObserver::RenderMode mode) override {
        bool fullyRendered = mode == mbgl::MapObserver::RenderMode::Full;
        [nativeView mapViewDidFinishRenderingMapFullyRendered:fullyRendered];
    }

    void onDidBecomeIdle() override {
        [nativeView mapViewDidBecomeIdle];
    }
    
    void onDidFinishLoadingStyle() override {
        [nativeView didFinishLoadingStyle];
    }

    mbgl::gl::ProcAddress getExtensionFunctionPointer(const char* name) override {
        static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengles"));
        if (!framework) {
            throw std::runtime_error("Failed to load OpenGL framework.");
        }

        CFStringRef str =
            CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
        void* symbol = CFBundleGetFunctionPointerForName(framework, str);
        CFRelease(str);

        return reinterpret_cast<mbgl::gl::ProcAddress>(symbol);
    }

    void activate() override
    {
        if (activationCount++)
        {
            return;
        }

        [EAGLContext setCurrentContext:nativeView.context];
    }

    void deactivate() override
    {
        if (--activationCount)
        {
            return;
        }

        [EAGLContext setCurrentContext:nil];
    }

private:
    __weak MGLMapView *nativeView = nullptr;

    NSUInteger activationCount = 0;
};

@end

#pragma mark - IBAdditions methods

@implementation MGLMapView (IBAdditions)

+ (NSSet<NSString *> *)keyPathsForValuesAffectingStyleURL__
{
    return [NSSet setWithObject:@"styleURL"];
}

- (nullable NSString *)styleURL__
{
    return self.styleURL.absoluteString;
}

- (void)setStyleURL__:(nullable NSString *)URLString
{
    URLString = [URLString stringByTrimmingCharactersInSet:
                 [NSCharacterSet whitespaceAndNewlineCharacterSet]];
    NSURL *url = URLString.length ? [NSURL URLWithString:URLString] : nil;
    if (URLString.length && !url)
    {
        [NSException raise:MGLInvalidStyleURLException
                    format:@"“%@” is not a valid style URL.", URLString];
    }
    self.styleURL = url;
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingLatitude
{
    return [NSSet setWithObjects:@"centerCoordinate", @"camera", nil];
}

- (double)latitude
{
    return self.centerCoordinate.latitude;
}

- (void)setLatitude:(double)latitude
{
    if ( ! isnan(_pendingLongitude))
    {
        self.centerCoordinate = CLLocationCoordinate2DMake(latitude, _pendingLongitude);
        _pendingLatitude = NAN;
        _pendingLongitude = NAN;
    }
    else
    {
        // Not enough info to make a valid center coordinate yet. Stash this
        // latitude away until the longitude is set too.
        _pendingLatitude = latitude;
    }
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingLongitude
{
    return [NSSet setWithObjects:@"centerCoordinate", @"camera", nil];
}

- (double)longitude
{
    return self.centerCoordinate.longitude;
}

- (void)setLongitude:(double)longitude
{
    if ( ! isnan(_pendingLatitude))
    {
        self.centerCoordinate = CLLocationCoordinate2DMake(_pendingLatitude, longitude);
        _pendingLatitude = NAN;
        _pendingLongitude = NAN;
    }
    else
    {
        // Not enough info to make a valid center coordinate yet. Stash this
        // longitude away until the latitude is set too.
        _pendingLongitude = longitude;
    }
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingAllowsZooming
{
    return [NSSet setWithObject:@"zoomEnabled"];
}

- (BOOL)allowsZooming
{
    return self.zoomEnabled;
}

- (void)setAllowsZooming:(BOOL)allowsZooming
{
    MGLLogDebug(@"Setting allowsZooming: %@", MGLStringFromBOOL(allowsZooming));
    self.zoomEnabled = allowsZooming;
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingAllowsScrolling
{
    return [NSSet setWithObject:@"scrollEnabled"];
}

- (BOOL)allowsScrolling
{
    return self.scrollEnabled;
}

- (void)setAllowsScrolling:(BOOL)allowsScrolling
{
    MGLLogDebug(@"Setting allowsScrolling: %@", MGLStringFromBOOL(allowsScrolling));
    self.scrollEnabled = allowsScrolling;
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingAllowsRotating
{
    return [NSSet setWithObject:@"rotateEnabled"];
}

- (BOOL)allowsRotating
{
    return self.rotateEnabled;
}

- (void)setAllowsRotating:(BOOL)allowsRotating
{
    MGLLogDebug(@"Setting allowsRotating: %@", MGLStringFromBOOL(allowsRotating));
    self.rotateEnabled = allowsRotating;
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingAllowsTilting
{
    return [NSSet setWithObject:@"pitchEnabled"];
}

- (BOOL)allowsTilting
{
    return self.pitchEnabled;
}

- (void)setAllowsTilting:(BOOL)allowsTilting
{
    MGLLogDebug(@"Setting allowsTilting: %@", MGLStringFromBOOL(allowsTilting));
    self.pitchEnabled = allowsTilting;
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingShowsHeading
{
    return [NSSet setWithObject:@"showsUserHeadingIndicator"];
}

- (BOOL)showsHeading
{
    return self.showsUserHeadingIndicator;
}

- (void)setShowsHeading:(BOOL)showsHeading
{
    MGLLogDebug(@"Setting showsHeading: %@", MGLStringFromBOOL(showsHeading));
    self.showsUserHeadingIndicator = showsHeading;
}

@end
