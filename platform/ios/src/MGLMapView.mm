#import "MGLMapView.h"
#import "MGLMapView+IBAdditions.h"
#import "MGLMapView+MGLCustomStyleLayerAdditions.h"

#import <mbgl/platform/log.hpp>
#import <mbgl/platform/gl.hpp>

#import <GLKit/GLKit.h>
#import <OpenGLES/EAGL.h>

#include <mbgl/mbgl.hpp>
#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/annotation/shape_annotation.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/darwin/reachability.h>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/chrono.hpp>

#import "Mapbox.h"
#import "../../darwin/MGLGeometry_Private.h"
#import "../../darwin/MGLMultiPoint_Private.h"

#import "NSBundle+MGLAdditions.h"
#import "NSString+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSException+MGLAdditions.h"
#import "MGLUserLocationAnnotationView.h"
#import "MGLUserLocation_Private.h"
#import "MGLAccountManager_Private.h"
#import "MGLAnnotationImage_Private.h"
#import "MGLMapboxEvents.h"
#import "MGLCompactCalloutView.h"

#import <algorithm>
#import <cstdlib>
#import <map>
#import <unordered_set>

class MBGLView;
class MGLAnnotationContext;

/// Indicates the manner in which the map view is tracking the user location.
typedef NS_ENUM(NSUInteger, MGLUserTrackingState) {
    /// The map view is not yet tracking the user location.
    MGLUserTrackingStatePossible = 0,
    /// The map view has begun to move to the first reported user location.
    MGLUserTrackingStateBegan,
    /// The map view has finished moving to the first reported user location.
    MGLUserTrackingStateChanged,
};

NSString *const MGLMapboxSetupDocumentationURLDisplayString = @"mapbox.com/help/first-steps-ios-sdk";

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

const NSUInteger MGLTargetFrameInterval = 1;  //Target FPS will be 60 divided by this value

/// Tolerance for snapping to true north, measured in degrees in either direction.
const CLLocationDirection MGLToleranceForSnappingToNorth = 7;

/// Reuse identifier and file name of the default point annotation image.
static NSString * const MGLDefaultStyleMarkerSymbolName = @"default_marker";

/// Prefix that denotes a sprite installed by MGLMapView, to avoid collisions
/// with style-defined sprites.
NSString *const MGLAnnotationSpritePrefix = @"com.mapbox.sprites.";

/// Slop area around the hit testing point, allowing for imprecise annotation selection.
const CGFloat MGLAnnotationImagePaddingForHitTest = 5;

/// Distance from the callout’s anchor point to the annotation it points to.
const CGFloat MGLAnnotationImagePaddingForCallout = 1;

/// Unique identifier representing a single annotation in mbgl.
typedef uint32_t MGLAnnotationTag;

/// An indication that the requested annotation was not found or is nonexistent.
enum { MGLAnnotationTagNotFound = UINT32_MAX };

/// Mapping from an annotation tag to metadata about that annotation, including
/// the annotation itself.
typedef std::map<MGLAnnotationTag, MGLAnnotationContext> MGLAnnotationContextMap;

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

mbgl::Color MGLColorObjectFromUIColor(UIColor *color)
{
    if (!color)
    {
        return {{ 0, 0, 0, 0 }};
    }
    CGFloat r, g, b, a;
    [color getRed:&r green:&g blue:&b alpha:&a];
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

#pragma mark - Private -

@interface MGLMapView () <UIGestureRecognizerDelegate,
                          GLKViewDelegate,
                          CLLocationManagerDelegate,
                          UIActionSheetDelegate,
                          MGLCalloutViewDelegate,
                          UIAlertViewDelegate,
                          MGLMultiPointDelegate,
                          MGLAnnotationImageDelegate>

@property (nonatomic) EAGLContext *context;
@property (nonatomic) GLKView *glView;
@property (nonatomic) UIImageView *glSnapshotView;
@property (nonatomic, readwrite) UIImageView *compassView;
@property (nonatomic, readwrite) UIImageView *logoView;
@property (nonatomic) NS_MUTABLE_ARRAY_OF(NSLayoutConstraint *) *logoViewConstraints;
@property (nonatomic, readwrite) UIButton *attributionButton;
@property (nonatomic) NS_MUTABLE_ARRAY_OF(NSLayoutConstraint *) *attributionButtonConstraints;
@property (nonatomic) UIActionSheet *attributionSheet;
@property (nonatomic) UIPanGestureRecognizer *pan;
@property (nonatomic) UIPinchGestureRecognizer *pinch;
@property (nonatomic) UIRotationGestureRecognizer *rotate;
@property (nonatomic) UILongPressGestureRecognizer *quickZoom;
@property (nonatomic) UIPanGestureRecognizer *twoFingerDrag;
/// Mapping from reusable identifiers to annotation images.
@property (nonatomic) NS_MUTABLE_DICTIONARY_OF(NSString *, MGLAnnotationImage *) *annotationImagesByIdentifier;
/// Currently shown popover representing the selected annotation.
@property (nonatomic) UIView<MGLCalloutView> *calloutViewForSelectedAnnotation;
@property (nonatomic) MGLUserLocationAnnotationView *userLocationAnnotationView;
/// Indicates how thoroughly the map view is tracking the user location.
@property (nonatomic) MGLUserTrackingState userTrackingState;
@property (nonatomic) CLLocationManager *locationManager;
@property (nonatomic) CGFloat scale;
@property (nonatomic) CGFloat angle;
@property (nonatomic) CGFloat quickZoomStart;
@property (nonatomic, getter=isDormant) BOOL dormant;
@property (nonatomic, readonly, getter=isRotationAllowed) BOOL rotationAllowed;

@end

@implementation MGLMapView
{
    mbgl::Map *_mbglMap;
    MBGLView *_mbglView;
    mbgl::DefaultFileSource *_mbglFileSource;
    
    BOOL _opaque;

    NS_MUTABLE_ARRAY_OF(NSURL *) *_bundledStyleURLs;
    
    MGLAnnotationContextMap _annotationContextsByAnnotationTag;
    /// Tag of the selected annotation. If the user location annotation is selected, this ivar is set to `MGLAnnotationTagNotFound`.
    MGLAnnotationTag _selectedAnnotationTag;
    BOOL _userLocationAnnotationIsSelected;
    /// Size of the rectangle formed by unioning the maximum slop area around every annotation image.
    CGSize _unionedAnnotationImageSize;
    std::vector<MGLAnnotationTag> _annotationsNearbyLastTap;

    BOOL _isWaitingForRedundantReachableNotification;
    BOOL _isTargetingInterfaceBuilder;

    CLLocationDegrees _pendingLatitude;
    CLLocationDegrees _pendingLongitude;

    CADisplayLink *_displayLink;
    BOOL _needsDisplayRefresh;
    
    NSUInteger _changeDelimiterSuppressionDepth;
    
    BOOL _delegateHasAlphasForShapeAnnotations;
    BOOL _delegateHasStrokeColorsForShapeAnnotations;
    BOOL _delegateHasFillColorsForShapeAnnotations;
    BOOL _delegateHasLineWidthsForShapeAnnotations;
}

#pragma mark - Setup & Teardown -

@dynamic debugActive;
mbgl::Duration MGLDurationInSeconds(NSTimeInterval duration)
{
    return std::chrono::duration_cast<mbgl::Duration>(std::chrono::duration<NSTimeInterval>(duration));
}

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        [self commonInit];
        self.styleURL = nil;
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame styleURL:(nullable NSURL *)styleURL
{
    if (self = [super initWithFrame:frame])
    {
        [self commonInit];
        self.styleURL = styleURL;
    }
    return self;
}

- (instancetype)initWithCoder:(nonnull NSCoder *)decoder
{
    if (self = [super initWithCoder:decoder])
    {
        [self commonInit];
        self.styleURL = nil;
    }
    return self;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingStyleURL
{
    return [NSSet setWithObjects:@"styleURL__", nil];
}

- (nonnull NSURL *)styleURL
{
    NSString *styleURLString = @(_mbglMap->getStyleURL().c_str()).mgl_stringOrNilIfEmpty;
    NSAssert(styleURLString || _isTargetingInterfaceBuilder, @"Invalid style URL string %@", styleURLString);
    return styleURLString ? [NSURL URLWithString:styleURLString] : nil;
}

- (void)setStyleURL:(nullable NSURL *)styleURL
{
    if (_isTargetingInterfaceBuilder) return;

    if ( ! styleURL)
    {
        styleURL = [MGLStyle streetsStyleURL];
    }

    if ( ! [styleURL scheme])
    {
        // Assume a relative path into the application bundle.
        styleURL = [NSURL URLWithString:[@"asset://" stringByAppendingString:[styleURL absoluteString]]];
    }

    _mbglMap->setStyleURL([[styleURL absoluteString] UTF8String]);
}

- (void)commonInit
{
    _isTargetingInterfaceBuilder = NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent;
    _opaque = YES;

    BOOL background = [UIApplication sharedApplication].applicationState == UIApplicationStateBackground;
    if (!background)
    {
        [self createGLView];
    }

    self.accessibilityLabel = @"Map";
    self.backgroundColor = [UIColor clearColor];
    self.clipsToBounds = YES;

    // setup mbgl view
    const float scaleFactor = [UIScreen instancesRespondToSelector:@selector(nativeScale)] ? [[UIScreen mainScreen] nativeScale] : [[UIScreen mainScreen] scale];
    _mbglView = new MBGLView(self, scaleFactor);

    // setup mbgl cache & file source
    NSString *fileCachePath = @"";
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    if ([paths count] != 0) {
        NSString *libraryDirectory = [paths objectAtIndex:0];
        fileCachePath = [libraryDirectory stringByAppendingPathComponent:@"cache.db"];
    }
    _mbglFileSource = new mbgl::DefaultFileSource([fileCachePath UTF8String], [[[[NSBundle mainBundle] resourceURL] path] UTF8String]);

    // setup mbgl map
    _mbglMap = new mbgl::Map(*_mbglView, *_mbglFileSource, mbgl::MapMode::Continuous);

    // start paused if in IB
    if (_isTargetingInterfaceBuilder || background) {
        self.dormant = YES;
        _mbglMap->pause();
    }

    // Observe for changes to the global access token (and find out the current one).
    [[MGLAccountManager sharedManager] addObserver:self
                                        forKeyPath:@"accessToken"
                                           options:(NSKeyValueObservingOptionInitial |
                                                    NSKeyValueObservingOptionNew)
                                           context:NULL];

    // Notify map object when network reachability status changes.
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(reachabilityChanged:)
                                                 name:kMGLReachabilityChangedNotification
                                               object:nil];

    MGLReachability* reachability = [MGLReachability reachabilityForInternetConnection];
    if ([reachability isReachable])
    {
        _isWaitingForRedundantReachableNotification = YES;
    }
    [reachability startNotifier];
    
    // Set up annotation management and selection state.
    _annotationImagesByIdentifier = [NSMutableDictionary dictionary];
    _annotationContextsByAnnotationTag = {};
    _selectedAnnotationTag = MGLAnnotationTagNotFound;
    _annotationsNearbyLastTap = {};

    // setup logo bug
    //
    UIImage *logo = [[MGLMapView resourceImageNamed:@"mapbox.png"] imageWithAlignmentRectInsets:UIEdgeInsetsMake(1.5, 4, 3.5, 2)];
    _logoView = [[UIImageView alloc] initWithImage:logo];
    _logoView.accessibilityLabel = @"Mapbox logo";
    _logoView.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_logoView];
    _logoViewConstraints = [NSMutableArray array];

    // setup attribution
    //
    _attributionButton = [UIButton buttonWithType:UIButtonTypeInfoLight];
    _attributionButton.accessibilityLabel = @"Attribution info";
    [_attributionButton addTarget:self action:@selector(showAttribution) forControlEvents:UIControlEventTouchUpInside];
    _attributionButton.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_attributionButton];
    _attributionButtonConstraints = [NSMutableArray array];
    [_attributionButton addObserver:self forKeyPath:@"hidden" options:NSKeyValueObservingOptionNew context:NULL];

    // setup compass
    //
    _compassView = [[UIImageView alloc] initWithImage:[MGLMapView resourceImageNamed:@"Compass.png"]];
    _compassView.accessibilityLabel = @"Compass";
    _compassView.frame = CGRectMake(0, 0, _compassView.image.size.width, _compassView.image.size.height);
    _compassView.alpha = 0;
    _compassView.userInteractionEnabled = YES;
    [_compassView addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleCompassTapGesture:)]];
    UIView *container = [[UIView alloc] initWithFrame:CGRectZero];
    [container addSubview:_compassView];
    container.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:container];

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

    UITapGestureRecognizer *doubleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleTapGesture:)];
    doubleTap.numberOfTapsRequired = 2;
    [self addGestureRecognizer:doubleTap];

    UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTapGesture:)];
    [singleTap requireGestureRecognizerToFail:doubleTap];
    [self addGestureRecognizer:singleTap];

    UITapGestureRecognizer *twoFingerTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTwoFingerTapGesture:)];
    twoFingerTap.numberOfTouchesRequired = 2;
    [twoFingerTap requireGestureRecognizerToFail:_pinch];
    [twoFingerTap requireGestureRecognizerToFail:_rotate];
    [self addGestureRecognizer:twoFingerTap];

    _twoFingerDrag = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleTwoFingerDragGesture:)];
    _twoFingerDrag.minimumNumberOfTouches = 2;
    _twoFingerDrag.maximumNumberOfTouches = 2;
    _twoFingerDrag.delegate = self;
    [_twoFingerDrag requireGestureRecognizerToFail:twoFingerTap];
    [_twoFingerDrag requireGestureRecognizerToFail:_pan];
    [self addGestureRecognizer:_twoFingerDrag];
    _pitchEnabled = YES;

    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
    {
        _quickZoom = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleQuickZoomGesture:)];
        _quickZoom.numberOfTapsRequired = 1;
        _quickZoom.minimumPressDuration = 0;
        [_quickZoom requireGestureRecognizerToFail:doubleTap];
        [self addGestureRecognizer:_quickZoom];
    }

    // observe app activity
    //
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willTerminate) name:UIApplicationWillTerminateNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sleepGL:) name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(wakeGL:) name:UIApplicationWillEnterForegroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(wakeGL:) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveMemoryWarning) name:UIApplicationDidReceiveMemoryWarningNotification object:nil];

    // set initial position
    //
    mbgl::CameraOptions options;
    options.center = mbgl::LatLng(0, 0);
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    options.padding = padding;
    options.zoom = 0;
    _mbglMap->jumpTo(options);
    _pendingLatitude = NAN;
    _pendingLongitude = NAN;
    _targetCoordinate = kCLLocationCoordinate2DInvalid;

    // metrics: map load event
    mbgl::LatLng latLng = _mbglMap->getLatLng(padding);
    int zoom = round(_mbglMap->getZoom());

    [MGLMapboxEvents pushEvent:MGLEventTypeMapLoad withAttributes:@{
        MGLEventKeyLatitude: @(latLng.latitude),
        MGLEventKeyLongitude: @(latLng.longitude),
        MGLEventKeyZoomLevel: @(zoom),
        MGLEventKeyPushEnabled: @([MGLMapboxEvents checkPushEnabled])
    }];
}

- (void)createGLView
{
    if (_context) return;

    // create context
    //
    _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    NSAssert(_context, @"Failed to create OpenGL ES context.");

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
    [_glView bindDrawable];
    [self insertSubview:_glView atIndex:0];

    _glView.contentMode = UIViewContentModeCenter;

    // load extensions
    //
    mbgl::gl::InitializeExtensions([](const char * name) {
        static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengles"));
        if (!framework) {
            throw std::runtime_error("Failed to load OpenGL framework.");
        }

        CFStringRef str = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
        void* symbol = CFBundleGetFunctionPointerForName(framework, str);
        CFRelease(str);

        return reinterpret_cast<mbgl::gl::glProc>(symbol);
    });
}

- (void)reachabilityChanged:(NSNotification *)notification
{
    MGLReachability *reachability = [notification object];
    if ( ! _isWaitingForRedundantReachableNotification && [reachability isReachable])
    {
        mbgl::NetworkStatus::Reachable();
    }
    _isWaitingForRedundantReachableNotification = NO;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [[MGLAccountManager sharedManager] removeObserver:self forKeyPath:@"accessToken"];
    [_attributionButton removeObserver:self forKeyPath:@"hidden"];
    
    [self validateDisplayLink];

    if (_mbglMap)
    {
        delete _mbglMap;
        _mbglMap = nullptr;
    }

    if (_mbglFileSource)
    {
        delete _mbglFileSource;
        _mbglFileSource = nullptr;
    }

    if (_mbglView)
    {
        delete _mbglView;
        _mbglView = nullptr;
    }

    if ([[EAGLContext currentContext] isEqual:_context])
    {
        [EAGLContext setCurrentContext:nil];
    }

    [self.logoViewConstraints removeAllObjects];
    self.logoViewConstraints = nil;
    [self.attributionButtonConstraints removeAllObjects];
    self.attributionButtonConstraints = nil;
}

- (void)setDelegate:(nullable id<MGLMapViewDelegate>)delegate
{
    if (_delegate == delegate) return;

    _delegate = delegate;

    if ([delegate respondsToSelector:@selector(mapView:symbolNameForAnnotation:)])
    {
        [NSException raise:@"Method unavailable" format:
         @"-mapView:symbolNameForAnnotation: has been removed from the MGLMapViewDelegate protocol, but %@ still implements it. "
         @"Implement -[%@ mapView:imageForAnnotation:] instead.",
         NSStringFromClass([delegate class]), NSStringFromClass([delegate class])];
    }
    
    _delegateHasAlphasForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:alphaForShapeAnnotation:)];
    _delegateHasStrokeColorsForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:strokeColorForShapeAnnotation:)];
    _delegateHasFillColorsForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:fillColorForPolygonAnnotation:)];
    _delegateHasLineWidthsForShapeAnnotations = [_delegate respondsToSelector:@selector(mapView:lineWidthForPolylineAnnotation:)];
}

- (void)didReceiveMemoryWarning
{
    _mbglMap->onLowMemory();
}

#pragma mark - Layout -

- (void)setFrame:(CGRect)frame
{
    [super setFrame:frame];

    [self setNeedsLayout];
}

- (void)setBounds:(CGRect)bounds
{
    [super setBounds:bounds];

    [self setNeedsLayout];
}

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

- (void)updateConstraints
{
    // If we have a view controller reference, use its layout guides for our various top & bottom
    // views so they don't underlap navigation or tool bars. If we don't have a reference, apply
    // constraints against ourself to maintain (albeit less ideal) placement of the subviews.
    //
    UIViewController *viewController = self.viewControllerForLayoutGuides;
    UIView *constraintParentView = (viewController.view ? viewController.view : self);

    // compass
    //
    UIView *compassContainer = self.compassView.superview;
    [compassContainer removeConstraints:compassContainer.constraints];

    NSMutableArray *compassContainerConstraints = [NSMutableArray array];
    if (viewController)
    {
        [compassContainerConstraints addObject:
         [NSLayoutConstraint constraintWithItem:compassContainer
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationGreaterThanOrEqual
                                         toItem:viewController.topLayoutGuide
                                      attribute:NSLayoutAttributeBottom
                                     multiplier:1
                                       constant:5]];
    }
    [compassContainerConstraints addObject:
     [NSLayoutConstraint constraintWithItem:compassContainer
                                  attribute:NSLayoutAttributeTop
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeTop
                                 multiplier:1
                                   constant:5]];

    [compassContainerConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeTrailing
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:compassContainer
                                  attribute:NSLayoutAttributeTrailing
                                 multiplier:1
                                   constant:5]];

    [compassContainerConstraints addObject:
     [NSLayoutConstraint constraintWithItem:compassContainer
                                  attribute:NSLayoutAttributeWidth
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:nil
                                  attribute:NSLayoutAttributeNotAnAttribute
                                 multiplier:1
                                   constant:self.compassView.image.size.width]];

    [compassContainerConstraints addObject:
     [NSLayoutConstraint constraintWithItem:compassContainer
                                  attribute:NSLayoutAttributeHeight
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:nil
                                  attribute:NSLayoutAttributeNotAnAttribute
                                 multiplier:1
                                   constant:self.compassView.image.size.height]];
    [constraintParentView addConstraints:compassContainerConstraints];

    // logo bug
    //
    [self.logoView removeConstraints:self.logoViewConstraints];
    [self.logoViewConstraints removeAllObjects];
    if (viewController)
    {
        [self.logoViewConstraints addObject:
         [NSLayoutConstraint constraintWithItem:viewController.bottomLayoutGuide
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationGreaterThanOrEqual
                                         toItem:self.logoView
                                      attribute:NSLayoutAttributeBaseline
                                     multiplier:1
                                       constant:8]];
    }
    [self.logoViewConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeBottom
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self.logoView
                                  attribute:NSLayoutAttributeBaseline
                                 multiplier:1
                                   constant:8]];

    [self.logoViewConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self.logoView
                                  attribute:NSLayoutAttributeLeading
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeLeading
                                 multiplier:1
                                   constant:8]];
    [constraintParentView addConstraints:self.logoViewConstraints];

    // attribution button
    //
    [self.attributionButton removeConstraints:self.attributionButtonConstraints];
    [self.attributionButtonConstraints removeAllObjects];
    if (viewController)
    {
        [self.attributionButtonConstraints addObject:
         [NSLayoutConstraint constraintWithItem:viewController.bottomLayoutGuide
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationGreaterThanOrEqual
                                         toItem:self.attributionButton
                                      attribute:NSLayoutAttributeBaseline
                                     multiplier:1
                                       constant:8]];
    }
    [self.attributionButtonConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeBottom
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self.attributionButton
                                  attribute:NSLayoutAttributeBaseline
                                 multiplier:1
                                   constant:8]];

    [self.attributionButtonConstraints addObject:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeTrailing
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:self.attributionButton
                                  attribute:NSLayoutAttributeTrailing
                                 multiplier:1
                                   constant:8]];
    [constraintParentView addConstraints:self.attributionButtonConstraints];

    [super updateConstraints];
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
    if ( ! self.isDormant)
    {
        CGFloat zoomFactor   = _mbglMap->getMaxZoom() - _mbglMap->getMinZoom() + 1;
        CGFloat cpuFactor    = (CGFloat)[[NSProcessInfo processInfo] processorCount];
        CGFloat memoryFactor = (CGFloat)[[NSProcessInfo processInfo] physicalMemory] / 1000 / 1000 / 1000;
        CGFloat sizeFactor   = ((CGFloat)_mbglMap->getWidth()  / mbgl::util::tileSize) *
                               ((CGFloat)_mbglMap->getHeight() / mbgl::util::tileSize);

        NSUInteger cacheSize = zoomFactor * cpuFactor * memoryFactor * sizeFactor * 0.5;

        _mbglMap->setSourceTileCacheSize(cacheSize);

        _mbglMap->renderSync();

        [self updateUserLocationAnnotationView];
    }
}

// This gets called when the view dimension changes, e.g. because the device is being rotated.
- (void)layoutSubviews
{
    [super layoutSubviews];
    
    [self adjustContentInset];

    if ( ! _isTargetingInterfaceBuilder)
    {
        _mbglMap->update(mbgl::Update::Dimensions);
    }

    if (self.attributionSheet.visible)
    {
        [self.attributionSheet dismissWithClickedButtonIndex:self.attributionSheet.cancelButtonIndex animated:YES];
    }

    if (self.compassView.alpha)
    {
        [self updateHeadingForDeviceOrientation];
        [self updateCompass];
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
    self.contentInset = contentInset;
}

- (void)setContentInset:(UIEdgeInsets)contentInset
{
    [self setContentInset:contentInset animated:NO];
}

- (void)setContentInset:(UIEdgeInsets)contentInset animated:(BOOL)animated
{
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

        [self.glView display];
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

    if ( ! self.isDormant)
    {
        [self validateDisplayLink];
        self.dormant = YES;
        _mbglMap->pause();
        [self.glView deleteDrawable];
    }
}

- (void)validateDisplayLink
{
    BOOL isVisible = self.superview && self.window;
    if (isVisible && ! _displayLink)
    {
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateFromDisplayLink)];
        _displayLink.frameInterval = MGLTargetFrameInterval;
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        _needsDisplayRefresh = YES;
    }
    else if ( ! isVisible && _displayLink)
    {
        [_displayLink invalidate];
        _displayLink = nil;
    }
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

- (void)sleepGL:(__unused NSNotification *)notification
{
    MGLAssertIsMainThread();

    if ( ! self.dormant)
    {
        self.dormant = YES;

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

        if (_mbglMap->getDebug() != mbgl::MapDebugOptions::NoDebug && [self.glSnapshotView.subviews count] == 0)
        {
            UIView *snapshotTint = [[UIView alloc] initWithFrame:self.glSnapshotView.bounds];
            snapshotTint.autoresizingMask = self.glSnapshotView.autoresizingMask;
            snapshotTint.backgroundColor = [[UIColor redColor] colorWithAlphaComponent:0.25];
            [self.glSnapshotView addSubview:snapshotTint];
        }

        _mbglMap->pause();

        [self.glView deleteDrawable];
    }
}

- (void)wakeGL:(__unused NSNotification *)notification
{
    MGLAssertIsMainThread();

    if (self.dormant && [UIApplication sharedApplication].applicationState != UIApplicationStateBackground)
    {
        self.dormant = NO;

        [self createGLView];
        [MGLMapboxEvents validate];

        self.glSnapshotView.hidden = YES;

        [self.glSnapshotView.subviews makeObjectsPerformSelector:@selector(removeFromSuperview)];

        [self.glView bindDrawable];

        _mbglMap->resume();
        
        _displayLink.paused = NO;
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
    if ([view respondsToSelector:@selector(setTintColor:)]) view.tintColor = self.tintColor;

    for (UIView *subview in view.subviews) [self updateTintColorForView:subview];
}

#pragma mark - Gestures -

- (void)handleCompassTapGesture:(__unused id)sender
{
    [self resetNorthAnimated:YES];

    if (self.userTrackingMode == MGLUserTrackingModeFollowWithHeading ||
        self.userTrackingMode == MGLUserTrackingModeFollowWithCourse)
    {
        self.userTrackingMode = MGLUserTrackingModeFollow;
    }
}

- (void)touchesBegan:(__unused NS_SET_OF(UITouch *) *)touches withEvent:(__unused UIEvent *)event
{
    _changeDelimiterSuppressionDepth = 0;
    _mbglMap->setGestureInProgress(false);
    _mbglMap->cancelTransitions();
}

- (void)notifyGestureDidBegin {
    [self notifyMapChange:mbgl::MapChangeRegionWillChange];
    _mbglMap->setGestureInProgress(true);
    _changeDelimiterSuppressionDepth++;
}

- (void)notifyGestureDidEndWithDrift:(BOOL)drift {
    _changeDelimiterSuppressionDepth--;
    NSAssert(_changeDelimiterSuppressionDepth >= 0,
             @"Unbalanced change delimiter suppression/unsuppression");
    if (_changeDelimiterSuppressionDepth == 0) {
        _mbglMap->setGestureInProgress(false);
    }
    if ( ! drift)
    {
        [self notifyMapChange:mbgl::MapChangeRegionDidChange];
    }
}

- (BOOL)isSuppressingChangeDelimiters {
    return _changeDelimiterSuppressionDepth > 0;
}

- (void)handlePanGesture:(UIPanGestureRecognizer *)pan
{
    if ( ! self.isScrollEnabled) return;

    _mbglMap->cancelTransitions();

    if (pan.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGesturePanStart forRecognizer:pan];

        self.userTrackingMode = MGLUserTrackingModeNone;
        
        [self notifyGestureDidBegin];
    }
    else if (pan.state == UIGestureRecognizerStateChanged)
    {
        CGPoint delta = [pan translationInView:pan.view];
        _mbglMap->moveBy({ delta.x, delta.y });
        [pan setTranslation:CGPointZero inView:pan.view];

        [self notifyMapChange:mbgl::MapChangeRegionIsChanging];
    }
    else if (pan.state == UIGestureRecognizerStateEnded || pan.state == UIGestureRecognizerStateCancelled)
    {
        CGPoint velocity = [pan velocityInView:pan.view];
        if (sqrtf(velocity.x * velocity.x + velocity.y * velocity.y) < 100)
        {
            // Not enough velocity to overcome friction
            velocity = CGPointZero;
        }

        NSTimeInterval duration = UIScrollViewDecelerationRateNormal;
        BOOL drift = ! CGPointEqualToPoint(velocity, CGPointZero);
        if (drift)
        {
            CGPoint offset = CGPointMake(velocity.x * duration / 4, velocity.y * duration / 4);
            _mbglMap->moveBy({ offset.x, offset.y }, MGLDurationInSeconds(duration));
        }

        [self notifyGestureDidEndWithDrift:drift];

        // metrics: pan end
        CGPoint pointInView = CGPointMake([pan locationInView:pan.view].x, [pan locationInView:pan.view].y);
        CLLocationCoordinate2D panCoordinate = [self convertPoint:pointInView toCoordinateFromView:pan.view];
        int zoom = round([self zoomLevel]);

        [MGLMapboxEvents pushEvent:MGLEventTypeMapDragEnd withAttributes:@{
            MGLEventKeyLatitude: @(panCoordinate.latitude),
            MGLEventKeyLongitude: @(panCoordinate.longitude),
            MGLEventKeyZoomLevel: @(zoom)
        }];
    }
}

- (void)handlePinchGesture:(UIPinchGestureRecognizer *)pinch
{
    if ( ! self.isZoomEnabled) return;

    if (_mbglMap->getZoom() <= _mbglMap->getMinZoom() && pinch.scale < 1) return;

    _mbglMap->cancelTransitions();
    
    CGPoint centerPoint = [pinch locationInView:pinch.view];
    if (self.userTrackingMode != MGLUserTrackingModeNone)
    {
        centerPoint = self.userLocationAnnotationViewCenter;
    }

    if (pinch.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGesturePinchStart forRecognizer:pinch];

        self.scale = _mbglMap->getScale();
        
        [self notifyGestureDidBegin];
    }
    else if (pinch.state == UIGestureRecognizerStateChanged)
    {
        CGFloat newScale = self.scale * pinch.scale;

        if (log2(newScale) < _mbglMap->getMinZoom()) return;
        
        _mbglMap->setScale(newScale, { centerPoint.x, centerPoint.y });

        [self notifyMapChange:mbgl::MapChangeRegionIsChanging];
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

        NSTimeInterval duration = velocity > 0 ? 1 : 0.25;

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

        if (newScale <= 0 || log2(newScale) < _mbglMap->getMinZoom())
        {
            velocity = 0;
        }

        if (velocity)
        {
            _mbglMap->setScale(newScale, { centerPoint.x, centerPoint.y }, MGLDurationInSeconds(duration));
        }

        [self notifyGestureDidEndWithDrift:velocity];

        [self unrotateIfNeededForGesture];
    }
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    if ( ! self.isRotateEnabled) return;

    _mbglMap->cancelTransitions();
    
    CGPoint centerPoint = [rotate locationInView:rotate.view];
    if (self.userTrackingMode != MGLUserTrackingModeNone)
    {
        centerPoint = self.userLocationAnnotationViewCenter;
    }

    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGestureRotateStart forRecognizer:rotate];

        self.angle = MGLRadiansFromDegrees(_mbglMap->getBearing()) * -1;

        if (self.userTrackingMode != MGLUserTrackingModeNone)
        {
            self.userTrackingMode = MGLUserTrackingModeFollow;
        }
        
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
        
        _mbglMap->setBearing(newDegrees, { centerPoint.x, centerPoint.y });

        [self notifyMapChange:mbgl::MapChangeRegionIsChanging];
    }
    else if (rotate.state == UIGestureRecognizerStateEnded || rotate.state == UIGestureRecognizerStateCancelled)
    {
        CGFloat velocity = rotate.velocity;

        if (fabs(velocity) > 3)
        {
            CGFloat radians = self.angle + rotate.rotation;
            NSTimeInterval duration = UIScrollViewDecelerationRateNormal;
            CGFloat newRadians = radians + velocity * duration * 0.1;
            CGFloat newDegrees = MGLDegreesFromRadians(newRadians) * -1;

            _mbglMap->setBearing(newDegrees, { centerPoint.x, centerPoint.y }, MGLDurationInSeconds(duration));

            [self notifyGestureDidEndWithDrift:YES];

            __weak MGLMapView *weakSelf = self;

            [self animateWithDelay:duration animations:^
             {
                 [weakSelf unrotateIfNeededForGesture];
             }];
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
    if (singleTap.state != UIGestureRecognizerStateEnded)
    {
        return;
    }
    [self trackGestureEvent:MGLEventGestureSingleTap forRecognizer:singleTap];

    CGPoint tapPoint = [singleTap locationInView:self];

    if (self.userLocationVisible)
    {
        // Assume that the user is fat-fingering an annotation.
        CGRect hitRect = CGRectInset({ tapPoint, CGSizeZero },
                                     -MGLAnnotationImagePaddingForHitTest,
                                     -MGLAnnotationImagePaddingForHitTest);

        if (CGRectIntersectsRect(hitRect, self.userLocationAnnotationView.frame))
        {
            if ( ! _userLocationAnnotationIsSelected)
            {
                [self selectAnnotation:self.userLocation animated:YES];
            }
            return;
        }
    }
    
    MGLAnnotationTag hitAnnotationTag = [self annotationTagAtPoint:tapPoint persistingResults:YES];
    if (hitAnnotationTag != MGLAnnotationTagNotFound)
    {
        if (hitAnnotationTag != _selectedAnnotationTag)
        {
            id <MGLAnnotation> annotation = [self annotationWithTag:hitAnnotationTag];
            NSAssert(annotation, @"Cannot select nonexistent annotation with tag %u", hitAnnotationTag);
            [self selectAnnotation:annotation animated:YES];
        }
    }
    else
    {
        [self deselectAnnotation:self.selectedAnnotation animated:YES];
    }
}

- (void)handleDoubleTapGesture:(UITapGestureRecognizer *)doubleTap
{
    if ( ! self.isZoomEnabled) return;

    _mbglMap->cancelTransitions();

    if (doubleTap.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGestureDoubleTap forRecognizer:doubleTap];
    }
    else if (doubleTap.state == UIGestureRecognizerStateEnded)
    {
        CGPoint gesturePoint = [doubleTap locationInView:doubleTap.view];
        if (self.userTrackingMode != MGLUserTrackingModeNone)
        {
            gesturePoint = self.userLocationAnnotationViewCenter;
        }

        mbgl::PrecisionPoint center(gesturePoint.x, gesturePoint.y);
        _mbglMap->scaleBy(2, center, MGLDurationInSeconds(MGLAnimationDuration));

        __weak MGLMapView *weakSelf = self;

        [self animateWithDelay:MGLAnimationDuration animations:^
        {
            [weakSelf unrotateIfNeededForGesture];
        }];
    }
}

- (void)handleTwoFingerTapGesture:(UITapGestureRecognizer *)twoFingerTap
{
    if ( ! self.isZoomEnabled) return;

    if (_mbglMap->getZoom() == _mbglMap->getMinZoom()) return;

    _mbglMap->cancelTransitions();

    if (twoFingerTap.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGestureTwoFingerSingleTap forRecognizer:twoFingerTap];
    }
    else if (twoFingerTap.state == UIGestureRecognizerStateEnded)
    {
        CGPoint gesturePoint = [twoFingerTap locationInView:twoFingerTap.view];
        if (self.userTrackingMode != MGLUserTrackingModeNone)
        {
            gesturePoint = self.userLocationAnnotationViewCenter;
        }

        mbgl::PrecisionPoint center(gesturePoint.x, gesturePoint.y);
        _mbglMap->scaleBy(0.5, center, MGLDurationInSeconds(MGLAnimationDuration));

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

    _mbglMap->cancelTransitions();

    if (quickZoom.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGestureQuickZoom forRecognizer:quickZoom];

        self.scale = _mbglMap->getScale();

        self.quickZoomStart = [quickZoom locationInView:quickZoom.view].y;
        
        [self notifyGestureDidBegin];
    }
    else if (quickZoom.state == UIGestureRecognizerStateChanged)
    {
        CGFloat distance = [quickZoom locationInView:quickZoom.view].y - self.quickZoomStart;

        CGFloat newZoom = log2f(self.scale) + (distance / 75);

        if (newZoom < _mbglMap->getMinZoom()) return;
        
        CGPoint centerPoint = self.contentCenter;
        if (self.userTrackingMode != MGLUserTrackingModeNone)
        {
            centerPoint = self.userLocationAnnotationViewCenter;
        }
        _mbglMap->scaleBy(powf(2, newZoom) / _mbglMap->getScale(),
                          { centerPoint.x, centerPoint.y });

        [self notifyMapChange:mbgl::MapChangeRegionIsChanging];
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

    _mbglMap->cancelTransitions();

    if (twoFingerDrag.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGesturePitchStart forRecognizer:twoFingerDrag];
        [self notifyGestureDidBegin];
    }
    else if (twoFingerDrag.state == UIGestureRecognizerStateBegan || twoFingerDrag.state == UIGestureRecognizerStateChanged)
    {
        CGFloat gestureDistance = CGPoint([twoFingerDrag translationInView:twoFingerDrag.view]).y;
        CGFloat currentPitch = _mbglMap->getPitch();
        CGFloat slowdown = 20.0;

        CGFloat pitchNew = currentPitch - (gestureDistance / slowdown);

        _mbglMap->setPitch(pitchNew);

        [self notifyMapChange:mbgl::MapChangeRegionIsChanging];
    }
    else if (twoFingerDrag.state == UIGestureRecognizerStateEnded || twoFingerDrag.state == UIGestureRecognizerStateCancelled)
    {
        [self notifyGestureDidEndWithDrift:NO];
        [self unrotateIfNeededForGesture];
    }

}

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    if ([gestureRecognizer isKindOfClass:[UIPanGestureRecognizer class]])
    {
        UIPanGestureRecognizer *panGesture = (UIPanGestureRecognizer *)gestureRecognizer;

        if (panGesture.minimumNumberOfTouches == 2)
        {
            CGPoint velocity = [panGesture velocityInView:panGesture.view];
            double gestureAngle = MGLDegreesFromRadians(atan(velocity.y / velocity.x));
            double horizontalToleranceDegrees = 20.0;

            // cancel if gesture angle is not 90º±20º (more or less vertical)
            if ( ! (fabs((fabs(gestureAngle) - 90.0)) < horizontalToleranceDegrees))
            {
                return NO;
            }
        }
    }
    return YES;
}

- (void)handleCalloutAccessoryTapGesture:(UITapGestureRecognizer *)tap
{
    if ([self.delegate respondsToSelector:@selector(mapView:annotation:calloutAccessoryControlTapped:)])
    {
        NSAssert([tap.view isKindOfClass:[UIControl class]], @"Tapped view %@ is not a UIControl", tap.view);
        [self.delegate mapView:self annotation:self.selectedAnnotation
            calloutAccessoryControlTapped:(UIControl *)tap.view];
    }
}

- (BOOL)calloutViewShouldHighlight:(__unused MGLCompactCalloutView *)calloutView
{
    return [self.delegate respondsToSelector:@selector(mapView:tapOnCalloutForAnnotation:)];
}

- (void)calloutViewTapped:(__unused MGLCompactCalloutView *)calloutView
{
    if ([self.delegate respondsToSelector:@selector(mapView:tapOnCalloutForAnnotation:)])
    {
        [self.delegate mapView:self tapOnCalloutForAnnotation:self.selectedAnnotation];
    }
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    NSArray *validSimultaneousGestures = @[ self.pan, self.pinch, self.rotate ];

    return ([validSimultaneousGestures containsObject:gestureRecognizer] && [validSimultaneousGestures containsObject:otherGestureRecognizer]);
}

- (void)trackGestureEvent:(NSString *)gestureID forRecognizer:(UIGestureRecognizer *)recognizer
{
    CGPoint pointInView = CGPointMake([recognizer locationInView:recognizer.view].x, [recognizer locationInView:recognizer.view].y);
    CLLocationCoordinate2D gestureCoordinate = [self convertPoint:pointInView toCoordinateFromView:recognizer.view];
    int zoom = round([self zoomLevel]);

    [MGLMapboxEvents pushEvent:MGLEventTypeMapTap withAttributes:@{
        MGLEventKeyLatitude: @(gestureCoordinate.latitude),
        MGLEventKeyLongitude: @(gestureCoordinate.longitude),
        MGLEventKeyZoomLevel: @(zoom),
        MGLEventKeyGestureID: gestureID
    }];
}

#pragma mark - Attribution -

- (void)showAttribution
{
    if ( ! self.attributionSheet)
    {
        self.attributionSheet = [[UIActionSheet alloc] initWithTitle:@"Mapbox iOS SDK"
                                                            delegate:self
                                                   cancelButtonTitle:@"Cancel"
                                              destructiveButtonTitle:nil
                                                   otherButtonTitles:
                                 @"© Mapbox",
                                 @"© OpenStreetMap",
                                 @"Improve This Map",
                                 @"Mapbox Telemetry",
                                 nil];

    }
    
    [self.attributionSheet showFromRect:self.attributionButton.frame inView:self animated:YES];
}

- (void)actionSheet:(UIActionSheet *)actionSheet didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == actionSheet.firstOtherButtonIndex)
    {
        [[UIApplication sharedApplication] openURL:
         [NSURL URLWithString:@"https://www.mapbox.com/about/maps/"]];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 1)
    {
        [[UIApplication sharedApplication] openURL:
         [NSURL URLWithString:@"http://www.openstreetmap.org/about/"]];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 2)
    {
        NSString *feedbackURL = [NSString stringWithFormat:@"https://www.mapbox.com/map-feedback/#/%.5f/%.5f/%i",
                                 self.longitude, self.latitude, (int)round(self.zoomLevel)];
        [[UIApplication sharedApplication] openURL:
         [NSURL URLWithString:feedbackURL]];
    }
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 3)
    {
        NSString *message;
        NSString *participate;
        NSString *optOut;

        if ([[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsEnabled"])
        {
            message = @"You are helping to make OpenStreetMap and Mapbox maps better by contributing anonymous usage data.";
            participate = @"Keep Participating";
            optOut = @"Stop Participating";
        }
        else
        {
            message = @"You can help make OpenStreetMap and Mapbox maps better by contributing anonymous usage data.";
            participate = @"Participate";
            optOut = @"Don’t Participate";
        }

        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Make Mapbox Maps Better"
                                                        message:message
                                                       delegate:self
                                              cancelButtonTitle:participate
                                              otherButtonTitles:@"Tell Me More", optOut, nil];
        [alert show];
    }
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == alertView.cancelButtonIndex)
    {
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"MGLMapboxMetricsEnabled"];
    }
    else if (buttonIndex == alertView.firstOtherButtonIndex)
    {
        [[UIApplication sharedApplication] openURL:
         [NSURL URLWithString:@"https://mapbox.com/telemetry/"]];
    }
    else if (buttonIndex == alertView.firstOtherButtonIndex + 1)
    {
        [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"MGLMapboxMetricsEnabled"];
    }
}

#pragma mark - Properties -

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(__unused void *)context
{
    // Synchronize mbgl::Map’s access token with the global one in MGLAccountManager.
    if ([keyPath isEqualToString:@"accessToken"] && object == [MGLAccountManager sharedManager])
    {
        NSString *accessToken = change[NSKeyValueChangeNewKey];
        if (![accessToken isKindOfClass:[NSNull class]]) {
            _mbglFileSource->setAccessToken((std::string)[accessToken UTF8String]);
        }
    }
    else if ([keyPath isEqualToString:@"hidden"] && object == _attributionButton)
    {
        NSNumber *hiddenNumber = change[NSKeyValueChangeNewKey];
        BOOL attributionButtonWasHidden = [hiddenNumber boolValue];
        if (attributionButtonWasHidden)
        {
            [MGLMapboxEvents ensureMetricsOptoutExists];
        }
    }
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingZoomEnabled
{
    return [NSSet setWithObject:@"allowsZooming"];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingScrollEnabled
{
    return [NSSet setWithObject:@"allowsScrolling"];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingRotateEnabled
{
    return [NSSet setWithObject:@"allowsRotating"];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingPitchEnabled
{
    return [NSSet setWithObject:@"allowsTilting"];
}

- (void)setDebugActive:(BOOL)debugActive
{
    _mbglMap->setDebug(debugActive ? mbgl::MapDebugOptions::TileBorders
                                   | mbgl::MapDebugOptions::ParseStatus
                                   | mbgl::MapDebugOptions::Collision
                                   : mbgl::MapDebugOptions::NoDebug);
}

- (BOOL)isDebugActive
{
    return (_mbglMap->getDebug() != mbgl::MapDebugOptions::NoDebug);
}

- (void)resetNorth
{
    [self resetNorthAnimated:YES];
}

- (void)resetNorthAnimated:(BOOL)animated
{
    [self setDirection:0 animated:animated];
}

- (void)resetPosition
{
    MGLMapCamera *camera = [MGLMapCamera camera];
    camera.altitude = MGLAltitudeForZoomLevel(0, 0, 0, self.frame.size);
    self.camera = camera;
}

- (void)cycleDebugOptions
{
    _mbglMap->cycleDebugOptions();
}

- (void)emptyMemoryCache
{
    _mbglMap->onLowMemory();
}

#pragma mark - Geography -

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCenterCoordinate
{
    return [NSSet setWithObjects:@"latitude", @"longitude", @"camera", nil];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)coordinate animated:(BOOL)animated
{
    [self setCenterCoordinate:coordinate zoomLevel:self.zoomLevel animated:animated];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
{
    [self setCenterCoordinate:centerCoordinate animated:NO];
}

- (CLLocationCoordinate2D)centerCoordinate
{
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    return MGLLocationCoordinate2DFromLatLng(_mbglMap->getLatLng(padding));
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel animated:(BOOL)animated
{
    [self setCenterCoordinate:centerCoordinate zoomLevel:zoomLevel direction:self.direction animated:animated];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction animated:(BOOL)animated {
    [self setCenterCoordinate:centerCoordinate zoomLevel:zoomLevel direction:direction animated:animated completionHandler:NULL];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction animated:(BOOL)animated completionHandler:(nullable void (^)(void))completion
{
    self.userTrackingMode = MGLUserTrackingModeNone;

    [self _setCenterCoordinate:centerCoordinate edgePadding:self.contentInset zoomLevel:zoomLevel direction:direction duration:animated ? MGLAnimationDuration : 0 animationTimingFunction:nil completionHandler:completion];
}

- (void)_setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate animated:(BOOL)animated {
    [self _setCenterCoordinate:centerCoordinate edgePadding:self.contentInset zoomLevel:self.zoomLevel direction:self.direction duration:animated ? MGLAnimationDuration : 0 animationTimingFunction:nil completionHandler:NULL];
}

- (void)_setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate edgePadding:(UIEdgeInsets)insets zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction duration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion
{
    _mbglMap->cancelTransitions();
    
    mbgl::CameraOptions cameraOptions;
    cameraOptions.center = MGLLatLngFromLocationCoordinate2D(centerCoordinate);
    cameraOptions.padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    cameraOptions.zoom = zoomLevel;
    if (direction >= 0)
    {
        cameraOptions.angle = MGLRadiansFromDegrees(-direction);
    }
    
    mbgl::AnimationOptions animationOptions;
    if (duration)
    {
        animationOptions.duration = MGLDurationInSeconds(duration);
        animationOptions.easing = MGLUnitBezierForMediaTimingFunction(function);
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
    _mbglMap->easeTo(cameraOptions, animationOptions);
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingZoomLevel
{
    return [NSSet setWithObject:@"camera"];
}

- (double)zoomLevel
{
    return _mbglMap->getZoom();
}

- (void)setZoomLevel:(double)zoomLevel
{
    [self setZoomLevel:zoomLevel animated:NO];
}

- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated
{
    if (zoomLevel == self.zoomLevel) return;
    _mbglMap->cancelTransitions();
    
    CGFloat duration = animated ? MGLAnimationDuration : 0;
    
    _mbglMap->setZoom(zoomLevel,
                      MGLEdgeInsetsFromNSEdgeInsets(self.contentInset),
                      MGLDurationInSeconds(duration));
}

- (MGLCoordinateBounds)visibleCoordinateBounds
{
    return [self convertRect:self.bounds toCoordinateBoundsFromView:self];
}

- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds
{
    [self setVisibleCoordinateBounds:bounds animated:NO];
}

- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds animated:(BOOL)animated
{
    [self setVisibleCoordinateBounds:bounds edgePadding:UIEdgeInsetsZero animated:animated];
}

- (void)setVisibleCoordinateBounds:(MGLCoordinateBounds)bounds edgePadding:(UIEdgeInsets)insets animated:(BOOL)animated
{
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

- (void)setVisibleCoordinates:(CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets animated:(BOOL)animated
{
    [self setVisibleCoordinates:coordinates count:count edgePadding:insets direction:self.direction animated:animated];
}

- (void)setVisibleCoordinates:(CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction animated:(BOOL)animated
{
    [self setVisibleCoordinates:coordinates count:count edgePadding:insets direction:direction duration:animated ? MGLAnimationDuration : 0 animationTimingFunction:nil];
}

- (void)setVisibleCoordinates:(CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction duration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function {
    [self setVisibleCoordinates:coordinates count:count edgePadding:insets direction:direction duration:duration animationTimingFunction:function completionHandler:NULL];
}

- (void)setVisibleCoordinates:(CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction duration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion
{
    self.userTrackingMode = MGLUserTrackingModeNone;
    [self _setVisibleCoordinates:coordinates count:count edgePadding:insets direction:direction duration:duration animationTimingFunction:function completionHandler:completion];
}

- (void)_setVisibleCoordinates:(CLLocationCoordinate2D *)coordinates count:(NSUInteger)count edgePadding:(UIEdgeInsets)insets direction:(CLLocationDirection)direction duration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion
{
    _mbglMap->cancelTransitions();
    
    [self willChangeValueForKey:@"visibleCoordinateBounds"];
    mbgl::EdgeInsets padding = MGLEdgeInsetsFromNSEdgeInsets(insets);
    padding += MGLEdgeInsetsFromNSEdgeInsets(self.contentInset);
    mbgl::AnnotationSegment segment;
    segment.reserve(count);
    for (NSUInteger i = 0; i < count; i++)
    {
        segment.push_back({coordinates[i].latitude, coordinates[i].longitude});
    }
    
    mbgl::CameraOptions cameraOptions = _mbglMap->cameraForLatLngs(segment, padding);
    if (direction >= 0)
    {
        cameraOptions.angle = MGLRadiansFromDegrees(-direction);
    }
    
    mbgl::AnimationOptions animationOptions;
    if (duration > 0)
    {
        animationOptions.duration = MGLDurationInSeconds(duration);
        animationOptions.easing = MGLUnitBezierForMediaTimingFunction(function);
    }
    if (completion)
    {
        animationOptions.transitionFinishFn = [completion]() {
            dispatch_async(dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    _mbglMap->easeTo(cameraOptions, animationOptions);
    [self didChangeValueForKey:@"visibleCoordinateBounds"];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingDirection
{
    return [NSSet setWithObject:@"camera"];
}

- (CLLocationDirection)direction
{
    return mbgl::util::wrap(_mbglMap->getBearing(), 0., 360.);
}

- (void)setDirection:(CLLocationDirection)direction animated:(BOOL)animated
{
    if ( ! animated && ! self.rotationAllowed) return;

    if (self.userTrackingMode == MGLUserTrackingModeFollowWithHeading)
    {
        self.userTrackingMode = MGLUserTrackingModeFollow;
    }
    
    [self _setDirection:direction animated:animated];
}

- (void)_setDirection:(CLLocationDirection)direction animated:(BOOL)animated
{
    if (direction == self.direction) return;
    _mbglMap->cancelTransitions();

    CGFloat duration = animated ? MGLAnimationDuration : 0;
    
    if (self.userTrackingMode == MGLUserTrackingModeNone)
    {
        _mbglMap->setBearing(direction,
                             MGLEdgeInsetsFromNSEdgeInsets(self.contentInset),
                             MGLDurationInSeconds(duration));
    }
    else
    {
        CGPoint centerPoint = self.userLocationAnnotationViewCenter;
        _mbglMap->setBearing(direction, { centerPoint.x, centerPoint.y },
                             MGLDurationInSeconds(duration));
    }
}

- (void)setDirection:(CLLocationDirection)direction
{
    [self setDirection:direction animated:NO];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingPitch
{
    return [NSSet setWithObject:@"camera"];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCamera
{
    return [NSSet setWithObjects:@"longitude", @"latitude", @"centerCoordinate", @"zoomLevel", @"direction", nil];
}

- (MGLMapCamera *)camera
{
    CGFloat pitch = _mbglMap->getPitch();
    CLLocationDistance altitude = MGLAltitudeForZoomLevel(self.zoomLevel, pitch,
                                                          self.centerCoordinate.latitude,
                                                          self.frame.size);
    return [MGLMapCamera cameraLookingAtCenterCoordinate:self.centerCoordinate
                                            fromDistance:altitude
                                                   pitch:pitch
                                                 heading:self.direction];
}

- (void)setCamera:(MGLMapCamera *)camera
{
    [self setCamera:camera animated:NO];
}

- (void)setCamera:(MGLMapCamera *)camera animated:(BOOL)animated
{
    [self setCamera:camera withDuration:animated ? MGLAnimationDuration : 0 animationTimingFunction:nil];
}

- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function
{
    [self setCamera:camera withDuration:duration animationTimingFunction:function completionHandler:NULL];
}

- (void)setCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration animationTimingFunction:(nullable CAMediaTimingFunction *)function completionHandler:(nullable void (^)(void))completion
{
    self.userTrackingMode = MGLUserTrackingModeNone;
    _mbglMap->cancelTransitions();
    if ([self.camera isEqual:camera])
    {
        return;
    }

    mbgl::CameraOptions cameraOptions = [self cameraOptionsObjectForAnimatingToCamera:camera edgePadding:self.contentInset];
    mbgl::AnimationOptions animationOptions;
    if (duration > 0)
    {
        animationOptions.duration = MGLDurationInSeconds(duration);
        animationOptions.easing = MGLUnitBezierForMediaTimingFunction(function);
    }
    if (completion)
    {
        animationOptions.transitionFinishFn = [completion]() {
            dispatch_async(dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    
    [self willChangeValueForKey:@"camera"];
    _mbglMap->easeTo(cameraOptions, animationOptions);
    [self didChangeValueForKey:@"camera"];
}

- (void)flyToCamera:(MGLMapCamera *)camera completionHandler:(nullable void (^)(void))completion
{
    [self flyToCamera:camera withDuration:-1 completionHandler:completion];
}

- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration completionHandler:(nullable void (^)(void))completion
{
    [self flyToCamera:camera withDuration:duration peakAltitude:-1 completionHandler:completion];
}

- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration peakAltitude:(CLLocationDistance)peakAltitude completionHandler:(nullable void (^)(void))completion
{
    self.userTrackingMode = MGLUserTrackingModeNone;
    
    [self _flyToCamera:camera edgePadding:self.contentInset withDuration:duration peakAltitude:peakAltitude completionHandler:completion];
}

- (void)_flyToCamera:(MGLMapCamera *)camera edgePadding:(UIEdgeInsets)insets withDuration:(NSTimeInterval)duration peakAltitude:(CLLocationDistance)peakAltitude completionHandler:(nullable void (^)(void))completion
{
    _mbglMap->cancelTransitions();
    if ([self.camera isEqual:camera])
    {
        return;
    }

    mbgl::CameraOptions cameraOptions = [self cameraOptionsObjectForAnimatingToCamera:camera edgePadding:insets];
    mbgl::AnimationOptions animationOptions;
    if (duration >= 0)
    {
        animationOptions.duration = MGLDurationInSeconds(duration);
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
    
    [self willChangeValueForKey:@"camera"];
    _mbglMap->flyTo(cameraOptions, animationOptions);
    [self didChangeValueForKey:@"camera"];
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
        options.angle = MGLRadiansFromDegrees(-camera.heading);
    }
    if (camera.pitch >= 0)
    {
        options.pitch = MGLRadiansFromDegrees(camera.pitch);
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
    return _mbglMap->latLngForPixel(mbgl::PrecisionPoint(convertedPoint.x, convertedPoint.y));
}

- (CGPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(nullable UIView *)view
{
    return [self convertLatLng:MGLLatLngFromLocationCoordinate2D(coordinate) toPointToView:view];
}

/// Converts a geographic coordinate to a point in the view’s coordinate system.
- (CGPoint)convertLatLng:(mbgl::LatLng)latLng toPointToView:(nullable UIView *)view
{
    mbgl::vec2<double> pixel = _mbglMap->pixelForLatLng(latLng);
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
    CGRect rect = { [self convertLatLng:bounds.sw toPointToView:view], CGSizeZero };
    rect = MGLExtendRect(rect, [self convertLatLng:bounds.ne toPointToView:view]);
    return rect;
}

/// Converts a rectangle in the given view’s coordinate system to a geographic
/// bounding box.
- (mbgl::LatLngBounds)convertRect:(CGRect)rect toLatLngBoundsFromView:(nullable UIView *)view
{
    mbgl::LatLngBounds bounds = mbgl::LatLngBounds::getExtendable();
    bounds.extend([self convertPoint:rect.origin toLatLngFromView:view]);
    bounds.extend([self convertPoint:{ CGRectGetMaxX(rect), CGRectGetMinY(rect) } toLatLngFromView:view]);
    bounds.extend([self convertPoint:{ CGRectGetMaxX(rect), CGRectGetMaxY(rect) } toLatLngFromView:view]);
    bounds.extend([self convertPoint:{ CGRectGetMinX(rect), CGRectGetMaxY(rect) } toLatLngFromView:view]);
    
    // The world is wrapping if a point just outside the bounds is also within
    // the rect.
    mbgl::LatLng outsideLatLng;
    if (bounds.sw.longitude > -180)
    {
        outsideLatLng = {
            (bounds.sw.latitude + bounds.ne.latitude) / 2,
            bounds.sw.longitude - 1,
        };
    }
    else if (bounds.ne.longitude < 180)
    {
        outsideLatLng = {
            (bounds.sw.latitude + bounds.ne.latitude) / 2,
            bounds.ne.longitude + 1,
        };
    }
    
    // If the world is wrapping, extend the bounds to cover all longitudes.
    if (CGRectContainsPoint(rect, [self convertLatLng:outsideLatLng toPointToView:view]))
    {
        bounds.sw.longitude = -180;
        bounds.ne.longitude = 180;
    }
    
    return bounds;
}

- (CLLocationDistance)metersPerPointAtLatitude:(CLLocationDegrees)latitude
{
    return _mbglMap->getMetersPerPixelAtLatitude(latitude, self.zoomLevel);
}

- (CLLocationDistance)metersPerPixelAtLatitude:(CLLocationDegrees)latitude
{
    return [self metersPerPointAtLatitude:latitude];
}

#pragma mark - Styling -

- (NS_ARRAY_OF(NSURL *) *)bundledStyleURLs
{
    if ( ! _bundledStyleURLs)
    {
        _bundledStyleURLs = [NSMutableArray array];
        for (NSUInteger i = 0; i < mbgl::util::default_styles::numOrderedStyles; i++)
        {
            NSURL *styleURL = [NSURL URLWithString:@(mbgl::util::default_styles::orderedStyles[i].url)];
            [_bundledStyleURLs addObject:styleURL];
        }
    }

    return [NSArray arrayWithArray:_bundledStyleURLs];
}

- (nullable NSString *)styleID
{
    [NSException raise:@"Method unavailable" format:
     @"%s has been replaced by -[MGLMapView styleURL].",
     __PRETTY_FUNCTION__];
    return nil;
}

- (void)setStyleID:(nullable NSString *)styleID
{
    [NSException raise:@"Method unavailable" format:
     @"%s has been replaced by -[MGLMapView setStyleURL:].\n\n"
     @"If you previously set this style ID in a storyboard inspectable, select the MGLMapView in Interface Builder and delete the “styleID” entry from the User Defined Runtime Attributes section of the Identity inspector. "
     @"Then go to the Attributes inspector and enter “mapbox://styles/%@” into the “Style URL” field.",
     __PRETTY_FUNCTION__, styleID];
}

- (NS_ARRAY_OF(NSString *) *)styleClasses
{
    NSMutableArray *returnArray = [NSMutableArray array];

    const std::vector<std::string> &appliedClasses = _mbglMap->getClasses();

    for (auto class_it = appliedClasses.begin(); class_it != appliedClasses.end(); class_it++)
    {
        [returnArray addObject:@(class_it->c_str())];
    }

    return returnArray;
}

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses
{
    [self setStyleClasses:appliedClasses transitionDuration:0];
}

- (void)setStyleClasses:(NS_ARRAY_OF(NSString *) *)appliedClasses transitionDuration:(NSTimeInterval)transitionDuration
{
    std::vector<std::string> newAppliedClasses;

    for (NSString *appliedClass in appliedClasses)
    {
        newAppliedClasses.insert(newAppliedClasses.end(), [appliedClass UTF8String]);
    }

    _mbglMap->setDefaultTransitionDuration(MGLDurationInSeconds(transitionDuration));
    _mbglMap->setClasses(newAppliedClasses);
}

- (BOOL)hasStyleClass:(NSString *)styleClass
{
    return styleClass && _mbglMap->hasClass([styleClass UTF8String]);
}

- (void)addStyleClass:(NSString *)styleClass
{
    if (styleClass)
    {
        _mbglMap->addClass([styleClass UTF8String]);
    }
}

- (void)removeStyleClass:(NSString *)styleClass
{
    if (styleClass)
    {
        _mbglMap->removeClass([styleClass UTF8String]);
    }
}

#pragma mark - Annotations -

- (nullable NS_ARRAY_OF(id <MGLAnnotation>) *)annotations
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
    return [NSArray arrayWithObjects:&annotations[0] count:annotations.size()];
}

/// Returns the annotation assigned the given tag. Cheap.
- (id <MGLAnnotation>)annotationWithTag:(MGLAnnotationTag)tag
{
    if ( ! _annotationContextsByAnnotationTag.count(tag))
    {
        return nil;
    }
    
    MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag[tag];
    return annotationContext.annotation;
}

/// Returns the annotation tag assigned to the given annotation. Relatively expensive.
- (MGLAnnotationTag)annotationTagForAnnotation:(id <MGLAnnotation>)annotation
{
    if ( ! annotation || annotation == self.userLocation)
    {
        return MGLAnnotationTagNotFound;
    }
    
    for (auto &pair : _annotationContextsByAnnotationTag)
    {
        if (pair.second.annotation == annotation)
        {
            return pair.first;
        }
    }
    return MGLAnnotationTagNotFound;
}

- (void)addAnnotation:(id <MGLAnnotation>)annotation
{
    if ( ! annotation) return;

    // The core bulk add API is efficient with respect to indexing and
    // screen refreshes, thus we should defer to it even for individual adds.
    //
    [self addAnnotations:@[ annotation ]];
}

- (void)addAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations
{
    if ( ! annotations) return;
    [self willChangeValueForKey:@"annotations"];

    std::vector<mbgl::PointAnnotation> points;
    std::vector<mbgl::ShapeAnnotation> shapes;

    BOOL delegateImplementsImageForPoint = [self.delegate respondsToSelector:@selector(mapView:imageForAnnotation:)];

    for (id <MGLAnnotation> annotation in annotations)
    {
        NSAssert([annotation conformsToProtocol:@protocol(MGLAnnotation)], @"annotation should conform to MGLAnnotation");

        if ([annotation isKindOfClass:[MGLMultiPoint class]])
        {
            [(MGLMultiPoint *)annotation addShapeAnnotationObjectToCollection:shapes withDelegate:self];
        }
        else
        {
            MGLAnnotationImage *annotationImage = delegateImplementsImageForPoint ? [self.delegate mapView:self imageForAnnotation:annotation] : nil;
            if ( ! annotationImage)
            {
                annotationImage = [self dequeueReusableAnnotationImageWithIdentifier:MGLDefaultStyleMarkerSymbolName];
            }
            if ( ! annotationImage)
            {
                // Create a default annotation image that depicts a round pin
                // rising from the center, with a shadow slightly below center.
                // The alignment rect therefore excludes the bottom half.
                UIImage *defaultAnnotationImage = [MGLMapView resourceImageNamed:MGLDefaultStyleMarkerSymbolName];
                defaultAnnotationImage = [defaultAnnotationImage imageWithAlignmentRectInsets:
                                          UIEdgeInsetsMake(0, 0, defaultAnnotationImage.size.height / 2, 0)];
                annotationImage = [MGLAnnotationImage annotationImageWithImage:defaultAnnotationImage
                                                               reuseIdentifier:MGLDefaultStyleMarkerSymbolName];
            }
            
            if ( ! self.annotationImagesByIdentifier[annotationImage.reuseIdentifier])
            {
                self.annotationImagesByIdentifier[annotationImage.reuseIdentifier] = annotationImage;
                [self installAnnotationImage:annotationImage];
                annotationImage.delegate = self;
            }

            NSString *symbolName = [MGLAnnotationSpritePrefix stringByAppendingString:annotationImage.reuseIdentifier];

            points.emplace_back(MGLLatLngFromLocationCoordinate2D(annotation.coordinate), symbolName ? [symbolName UTF8String] : "");
        }
    }

    if (points.size())
    {
        std::vector<MGLAnnotationTag> pointAnnotationTags = _mbglMap->addPointAnnotations(points);
        
        for (size_t i = 0; i < pointAnnotationTags.size(); ++i)
        {
            MGLAnnotationContext context;
            context.annotation = annotations[i];
            context.symbolIdentifier = @(points[i].icon.c_str());
            _annotationContextsByAnnotationTag[pointAnnotationTags[i]] = context;
        }
    }

    if (shapes.size())
    {
        std::vector<MGLAnnotationTag> shapeAnnotationTags = _mbglMap->addShapeAnnotations(shapes);
        
        for (size_t i = 0; i < shapeAnnotationTags.size(); ++i)
        {
            MGLAnnotationContext context;
            context.annotation = annotations[i];
            _annotationContextsByAnnotationTag[shapeAnnotationTags[i]] = context;
        }
    }
    
    [self didChangeValueForKey:@"annotations"];
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
                      : [UIColor blackColor]);
    return MGLColorObjectFromUIColor(color);
}

- (mbgl::Color)fillColorForPolygonAnnotation:(MGLPolygon *)annotation
{
    UIColor *color = (_delegateHasFillColorsForShapeAnnotations
                      ? [self.delegate mapView:self fillColorForPolygonAnnotation:annotation]
                      : [UIColor blueColor]);
    return MGLColorObjectFromUIColor(color);
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
    // retrieve pixels
    CGImageRef image = annotationImage.image.CGImage;
    size_t width = CGImageGetWidth(image);
    size_t height = CGImageGetHeight(image);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    mbgl::PremultipliedImage cPremultipliedImage(width, height);
    size_t bytesPerPixel = 4;
    size_t bytesPerRow = bytesPerPixel * width;
    size_t bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(cPremultipliedImage.data.get(), width, height, bitsPerComponent, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);

    // add sprite
    auto cSpriteImage = std::make_shared<mbgl::SpriteImage>(
        std::move(cPremultipliedImage), 
        float(annotationImage.image.scale));

    // sprite upload
    NSString *symbolName = [MGLAnnotationSpritePrefix stringByAppendingString:annotationImage.reuseIdentifier];
    _mbglMap->addAnnotationIcon(symbolName.UTF8String, cSpriteImage);
    
    // Create a slop area with a “radius” equal in size to the annotation
    // image’s alignment rect, allowing the eventual tap to be on any point
    // within this image. Union this slop area with any existing slop areas.
    CGRect bounds = UIEdgeInsetsInsetRect({ CGPointZero, annotationImage.image.size },
                                          annotationImage.image.alignmentRectInsets);
    _unionedAnnotationImageSize = CGSizeMake(MAX(_unionedAnnotationImageSize.width, bounds.size.width),
                                             MAX(_unionedAnnotationImageSize.height, bounds.size.height));
}

- (void)removeAnnotation:(id <MGLAnnotation>)annotation
{
    if ( ! annotation) return;

    // The core bulk deletion API is efficient with respect to indexing
    // and screen refreshes, thus we should defer to it even for
    // individual deletes.
    //
    [self removeAnnotations:@[ annotation ]];
}

- (void)removeAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations
{
    if ( ! annotations) return;

    std::vector<MGLAnnotationTag> annotationTagsToRemove;
    annotationTagsToRemove.reserve(annotations.count);

    for (id <MGLAnnotation> annotation in annotations)
    {
        NSAssert([annotation conformsToProtocol:@protocol(MGLAnnotation)], @"annotation should conform to MGLAnnotation");

        MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
        if (annotationTag == MGLAnnotationTagNotFound)
        {
            continue;
        }
        annotationTagsToRemove.push_back(annotationTag);

        if (annotationTag == _selectedAnnotationTag)
        {
            [self deselectAnnotation:annotation animated:NO];
        }
        
        _annotationContextsByAnnotationTag.erase(annotationTag);
    }

    if ( ! annotationTagsToRemove.empty())
    {
        [self willChangeValueForKey:@"annotations"];
        _mbglMap->removeAnnotations(annotationTagsToRemove);
        [self didChangeValueForKey:@"annotations"];
    }
}

- (void)addOverlay:(id <MGLOverlay>)overlay
{
    [self addOverlays:@[ overlay ]];
}

- (void)addOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays
{
    for (id <MGLOverlay> overlay in overlays)
    {
        NSAssert([overlay conformsToProtocol:@protocol(MGLOverlay)], @"overlay should conform to MGLOverlay");
    }

    [self addAnnotations:overlays];
}

- (void)removeOverlay:(id <MGLOverlay>)overlay
{
    [self removeOverlays:@[ overlay ]];
}

- (void)removeOverlays:(NS_ARRAY_OF(id <MGLOverlay>) *)overlays
{
    for (id <MGLOverlay> overlay in overlays)
    {
        NSAssert([overlay conformsToProtocol:@protocol(MGLOverlay)], @"overlay should conform to MGLOverlay");
    }

    [self removeAnnotations:overlays];
}

- (nullable MGLAnnotationImage *)dequeueReusableAnnotationImageWithIdentifier:(NSString *)identifier
{
    // This prefix is used to avoid collisions with style-defined sprites in
    // mbgl, but reusable identifiers are never prefixed.
    if ([identifier hasPrefix:MGLAnnotationSpritePrefix])
    {
        identifier = [identifier substringFromIndex:MGLAnnotationSpritePrefix.length];
    }
    return self.annotationImagesByIdentifier[identifier];
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
    // or width of an installed annotation image.
    CGRect queryRect = CGRectInset({ point, CGSizeZero },
                                   -_unionedAnnotationImageSize.width,
                                   -_unionedAnnotationImageSize.height);
    queryRect = CGRectInset(queryRect, -MGLAnnotationImagePaddingForHitTest,
                            -MGLAnnotationImagePaddingForHitTest);
    std::vector<MGLAnnotationTag> nearbyAnnotations = [self annotationTagsInRect:queryRect];
    
    if (nearbyAnnotations.size())
    {
        // Assume that the user is fat-fingering an annotation.
        CGRect hitRect = CGRectInset({ point, CGSizeZero },
                                     -MGLAnnotationImagePaddingForHitTest,
                                     -MGLAnnotationImagePaddingForHitTest);
        
        // Filter out any annotation whose image is unselectable or for which
        // hit testing fails.
        std::remove_if(nearbyAnnotations.begin(), nearbyAnnotations.end(), [&](const MGLAnnotationTag annotationTag)
        {
            id <MGLAnnotation> annotation = [self annotationWithTag:annotationTag];
            NSAssert(annotation, @"Unknown annotation found nearby tap");
            
            MGLAnnotationImage *annotationImage = [self imageOfAnnotationWithTag:annotationTag];
            if ( ! annotationImage.enabled)
            {
                return true;
            }
            
            // Filter out the annotation if the fattened finger didn’t land
            // within the image’s alignment rect.
            CGRect annotationRect = [self frameOfImage:annotationImage.image
                                  centeredAtCoordinate:annotation.coordinate];
            return !!!CGRectIntersectsRect(annotationRect, hitRect);
        });
    }
    
    MGLAnnotationTag hitAnnotationTag = MGLAnnotationTagNotFound;
    if (nearbyAnnotations.size())
    {
        // The annotation tags need to be stable in order to compare them with
        // the remembered tags.
        std::sort(nearbyAnnotations.begin(), nearbyAnnotations.end());
        
        if (nearbyAnnotations == _annotationsNearbyLastTap)
        {
            // The first selection in the cycle should be the one nearest to the
            // tap.
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
            
            // The last time we persisted a set of annotations, we had the same
            // set of annotations as we do now. Cycle through them.
            if (_selectedAnnotationTag == MGLAnnotationTagNotFound
                || _selectedAnnotationTag == _annotationsNearbyLastTap.back())
            {
                // Either an annotation from this set hasn’t been selected
                // before or the last annotation in the set was selected. Wrap
                // around to the first annotation in the set.
                hitAnnotationTag = _annotationsNearbyLastTap.front();
            }
            else
            {
                // Step to the next annotation in the set.
                auto result = std::find(_annotationsNearbyLastTap.begin(),
                                        _annotationsNearbyLastTap.end(),
                                        _selectedAnnotationTag);
                auto distance = std::distance(_annotationsNearbyLastTap.begin(), result);
                hitAnnotationTag = _annotationsNearbyLastTap[distance + 1];
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
            if (_annotationsNearbyLastTap.size())
            {
                hitAnnotationTag = _annotationsNearbyLastTap.front();
            }
        }
    }
    
    return hitAnnotationTag;
}

/// Returns the tags of the annotations coincident with the given rectangle.
- (std::vector<MGLAnnotationTag>)annotationTagsInRect:(CGRect)rect
{
    mbgl::LatLngBounds queryBounds = [self convertRect:rect toLatLngBoundsFromView:self];
    return _mbglMap->getPointAnnotationsInBounds(queryBounds);
}

- (id <MGLAnnotation>)selectedAnnotation
{
    if (_userLocationAnnotationIsSelected)
    {
        return self.userLocation;
    }
    if ( ! _annotationContextsByAnnotationTag.count(_selectedAnnotationTag))
    {
        return nil;
    }
    MGLAnnotationContext &annotationContext = _annotationContextsByAnnotationTag.at(_selectedAnnotationTag);
    return annotationContext.annotation;
}

- (void)setSelectedAnnotation:(id <MGLAnnotation>)annotation
{
    [self willChangeValueForKey:@"selectedAnnotations"];
    _selectedAnnotationTag = [self annotationTagForAnnotation:annotation];
    _userLocationAnnotationIsSelected = annotation && annotation == self.userLocation;
    [self didChangeValueForKey:@"selectedAnnotations"];
}

- (NS_ARRAY_OF(id <MGLAnnotation>) *)selectedAnnotations
{
    return (self.selectedAnnotation ? @[ self.selectedAnnotation ] : @[]);
}

- (void)setSelectedAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)selectedAnnotations
{
    if ( ! selectedAnnotations.count) return;

    id <MGLAnnotation> firstAnnotation = selectedAnnotations[0];

    NSAssert([firstAnnotation conformsToProtocol:@protocol(MGLAnnotation)], @"annotation should conform to MGLAnnotation");

    if ([firstAnnotation isKindOfClass:[MGLMultiPoint class]]) return;

    // Select the annotation if it’s visible.
    if (MGLCoordinateInCoordinateBounds(firstAnnotation.coordinate, self.visibleCoordinateBounds))
    {
        [self selectAnnotation:firstAnnotation animated:NO];
    }
}

- (void)selectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated
{
    if ( ! annotation) return;

    if ([annotation isKindOfClass:[MGLMultiPoint class]]) return;

    if (annotation == self.selectedAnnotation) return;

    if (annotation != self.userLocation)
    {
        self.userTrackingMode = MGLUserTrackingModeNone;
    }

    [self deselectAnnotation:self.selectedAnnotation animated:NO];
    
    // Add the annotation to the map if it hasn’t been added yet.
    MGLAnnotationTag annotationTag = [self annotationTagForAnnotation:annotation];
    if (annotationTag == MGLAnnotationTagNotFound && annotation != self.userLocation)
    {
        [self addAnnotation:annotation];
    }
    
    // The annotation can’t be selected if no part of it is hittable.
    CGRect positioningRect = [self positioningRectForCalloutForAnnotationWithTag:annotationTag];
    if ( ! CGRectIntersectsRect(positioningRect, self.bounds) && annotation != self.userLocation)
    {
        return;
    }

    self.selectedAnnotation = annotation;

    if ([annotation respondsToSelector:@selector(title)] &&
        annotation.title &&
        [self.delegate respondsToSelector:@selector(mapView:annotationCanShowCallout:)] &&
        [self.delegate mapView:self annotationCanShowCallout:annotation])
    {
        // build the callout
        if ([self.delegate respondsToSelector:@selector(mapView:calloutViewForAnnotation:)])
        {
            self.calloutViewForSelectedAnnotation = [self.delegate mapView:self calloutViewForAnnotation:annotation];
        }
        if (!self.calloutViewForSelectedAnnotation)
        {
            self.calloutViewForSelectedAnnotation = [self calloutViewForAnnotation:annotation];
        }

        if (_userLocationAnnotationIsSelected)
        {
            positioningRect = CGRectInset(self.userLocationAnnotationView.frame,
                                          -MGLAnnotationImagePaddingForCallout,
                                          -MGLAnnotationImagePaddingForCallout);
        }

        // consult delegate for left and/or right accessory views
        if ([self.delegate respondsToSelector:@selector(mapView:leftCalloutAccessoryViewForAnnotation:)])
        {
            self.calloutViewForSelectedAnnotation.leftAccessoryView =
                [self.delegate mapView:self leftCalloutAccessoryViewForAnnotation:annotation];

            if ([self.calloutViewForSelectedAnnotation.leftAccessoryView isKindOfClass:[UIControl class]])
            {
                UITapGestureRecognizer *calloutAccessoryTap = [[UITapGestureRecognizer alloc] initWithTarget:self
                                                                  action:@selector(handleCalloutAccessoryTapGesture:)];

                [self.calloutViewForSelectedAnnotation.leftAccessoryView addGestureRecognizer:calloutAccessoryTap];
            }
        }

        if ([self.delegate respondsToSelector:@selector(mapView:rightCalloutAccessoryViewForAnnotation:)])
        {
            self.calloutViewForSelectedAnnotation.rightAccessoryView =
                [self.delegate mapView:self rightCalloutAccessoryViewForAnnotation:annotation];

            if ([self.calloutViewForSelectedAnnotation.rightAccessoryView isKindOfClass:[UIControl class]])
            {
                UITapGestureRecognizer *calloutAccessoryTap = [[UITapGestureRecognizer alloc] initWithTarget:self
                                                                  action:@selector(handleCalloutAccessoryTapGesture:)];

                [self.calloutViewForSelectedAnnotation.rightAccessoryView addGestureRecognizer:calloutAccessoryTap];
            }
        }

        // set annotation delegate to handle taps on the callout view
        self.calloutViewForSelectedAnnotation.delegate = self;

        // present popup
        [self.calloutViewForSelectedAnnotation presentCalloutFromRect:positioningRect
                                                               inView:self.glView
                                                    constrainedToView:self.glView
                                                             animated:animated];
    }

    // notify delegate
    if ([self.delegate respondsToSelector:@selector(mapView:didSelectAnnotation:)])
    {
        [self.delegate mapView:self didSelectAnnotation:annotation];
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
- (CGRect)positioningRectForCalloutForAnnotationWithTag:(MGLAnnotationTag)annotationTag
{
    id <MGLAnnotation> annotation = [self annotationWithTag:annotationTag];
    if ( ! annotation)
    {
        return CGRectZero;
    }
    UIImage *image = [self imageOfAnnotationWithTag:annotationTag].image;
    if ( ! image)
    {
        return CGRectZero;
    }
    
    CGRect positioningRect = [self frameOfImage:image centeredAtCoordinate:annotation.coordinate];
    positioningRect.origin.x -= 0.5;
    return CGRectInset(positioningRect, -MGLAnnotationImagePaddingForCallout,
                       -MGLAnnotationImagePaddingForCallout);
}

/// Returns the rectangle relative to the viewport that represents the given
/// image centered at the given coordinate.
- (CGRect)frameOfImage:(UIImage *)image centeredAtCoordinate:(CLLocationCoordinate2D)coordinate
{
    CGPoint calloutAnchorPoint = [self convertCoordinate:coordinate toPointToView:self];
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
    
    NSString *customSymbol = _annotationContextsByAnnotationTag.at(annotationTag).symbolIdentifier;
    NSString *symbolName = customSymbol.length ? customSymbol : MGLDefaultStyleMarkerSymbolName;
    
    return [self dequeueReusableAnnotationImageWithIdentifier:symbolName];
}

- (void)deselectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated
{
    if ( ! annotation) return;

    if ([self.selectedAnnotation isEqual:annotation])
    {
        // dismiss popup
        [self.calloutViewForSelectedAnnotation dismissCalloutAnimated:animated];

        // clean up
        self.calloutViewForSelectedAnnotation = nil;
        self.selectedAnnotation = nil;

        // notify delegate
        if ([self.delegate respondsToSelector:@selector(mapView:didDeselectAnnotation:)])
        {
            [self.delegate mapView:self didDeselectAnnotation:annotation];
        }
    }
}

- (void)showAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations animated:(BOOL)animated
{
    if ( ! annotations || ! annotations.count) return;

    mbgl::LatLngBounds bounds = mbgl::LatLngBounds::getExtendable();

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

    CGFloat defaultPadding = 100;
    CGFloat yPadding = (self.frame.size.height / 2 <= defaultPadding) ? (self.frame.size.height / 5) : defaultPadding;
    CGFloat xPadding = (self.frame.size.width / 2 <= defaultPadding) ? (self.frame.size.width / 5) : defaultPadding;

    [self setVisibleCoordinateBounds:MGLCoordinateBoundsFromLatLngBounds(bounds)
                         edgePadding:UIEdgeInsetsMake(yPadding, xPadding, yPadding, xPadding)
                            animated:animated];
}

#pragma mark Annotation Image Delegate

- (void)annotationImageNeedsRedisplay:(MGLAnnotationImage *)annotationImage
{
    // remove sprite
    NSString *symbolName = [MGLAnnotationSpritePrefix stringByAppendingString:annotationImage.reuseIdentifier];
    _mbglMap->removeAnnotationIcon(symbolName.UTF8String);
    [self installAnnotationImage:annotationImage];
    _mbglMap->update(mbgl::Update::Annotations);
}

#pragma mark - User Location -

- (void)setShowsUserLocation:(BOOL)showsUserLocation
{
    if (showsUserLocation == _showsUserLocation) return;

    _showsUserLocation = showsUserLocation;

    if (showsUserLocation)
    {
        if ([self.delegate respondsToSelector:@selector(mapViewWillStartLocatingUser:)])
        {
            [self.delegate mapViewWillStartLocatingUser:self];
        }

        self.userLocationAnnotationView = [[MGLUserLocationAnnotationView alloc] initInMapView:self];
        self.userLocationAnnotationView.autoresizingMask = (UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin |
                                                            UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin);

        self.locationManager = [CLLocationManager new];

#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 80000
        // enable iOS 8+ location authorization API
        //
        if ([CLLocationManager instancesRespondToSelector:@selector(requestWhenInUseAuthorization)])
        {
            BOOL hasLocationDescription = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSLocationWhenInUseUsageDescription"] ||
                [[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSLocationAlwaysUsageDescription"];
            if (!hasLocationDescription)
            {
                [NSException raise:@"Missing Location Services usage description" format:
                 @"In iOS 8 and above, this app must have a value for NSLocationWhenInUseUsageDescription or NSLocationAlwaysUsageDescription in its Info.plist."];
            }
            // request location permissions, if both keys exist ask for less permissive
            if ([[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSLocationWhenInUseUsageDescription"])
            {
                [self.locationManager requestWhenInUseAuthorization];
            }
            else if ([[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSLocationAlwaysUsageDescription"])
            {
                [self.locationManager requestAlwaysAuthorization];
            }
        }
#endif

        self.locationManager.headingFilter = 5.0;
        self.locationManager.delegate = self;
        [self.locationManager startUpdatingLocation];
    }
    else
    {
        [self.locationManager stopUpdatingLocation];
        [self.locationManager stopUpdatingHeading];
        self.locationManager.delegate = nil;
        self.locationManager = nil;

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

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingUserLocation
{
    return [NSSet setWithObject:@"userLocationAnnotationView"];
}

- (nullable MGLUserLocation *)userLocation
{
    return self.userLocationAnnotationView.annotation;
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
    [self setUserTrackingMode:mode animated:YES];
}

- (void)setUserTrackingMode:(MGLUserTrackingMode)mode animated:(BOOL)animated
{
    if (mode == _userTrackingMode) return;

    if ((mode == MGLUserTrackingModeFollowWithHeading || mode == MGLUserTrackingModeFollowWithCourse) &&
        ! CLLocationCoordinate2DIsValid(self.userLocation.coordinate))
    {
        mode = MGLUserTrackingModeNone;
    }

    _userTrackingMode = mode;
    
    self.userTrackingState = animated ? MGLUserTrackingStatePossible : MGLUserTrackingStateChanged;

    switch (_userTrackingMode)
    {
        case MGLUserTrackingModeNone:
        {
            [self.locationManager stopUpdatingHeading];

            // Immediately update the annotation view; other cases update inside
            // the locationManager:didUpdateLocations: method.
            [self updateUserLocationAnnotationView];

            break;
        }
        case MGLUserTrackingModeFollow:
        case MGLUserTrackingModeFollowWithCourse:
        {
            self.showsUserLocation = YES;

            [self.locationManager stopUpdatingHeading];

            if (self.userLocationAnnotationView)
            {
                [self locationManager:self.locationManager didUpdateLocations:@[self.userLocation.location] animated:animated];
            }

            break;
        }
        case MGLUserTrackingModeFollowWithHeading:
        {
            self.showsUserLocation = YES;

            if (self.zoomLevel < self.currentMinimumZoom)
            {
                [self setZoomLevel:self.currentMinimumZoom animated:YES];
            }

            if (self.userLocationAnnotationView)
            {
                [self locationManager:self.locationManager didUpdateLocations:@[self.userLocation.location] animated:animated];
            }

            [self updateHeadingForDeviceOrientation];

            [self.locationManager startUpdatingHeading];

            break;
        }
    }

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
        [self locationManager:self.locationManager didUpdateLocations:@[self.userLocation.location] animated:animated];
    }
}

- (void)setTargetCoordinate:(CLLocationCoordinate2D)targetCoordinate
{
    [self setTargetCoordinate:targetCoordinate animated:YES];
}

- (void)setTargetCoordinate:(CLLocationCoordinate2D)targetCoordinate animated:(BOOL)animated
{
    if (targetCoordinate.latitude != self.targetCoordinate.latitude
        || targetCoordinate.longitude != self.targetCoordinate.longitude)
    {
        _targetCoordinate = targetCoordinate;
        if (CLLocationCoordinate2DIsValid(targetCoordinate)
            && self.userTrackingMode == MGLUserTrackingModeFollowWithCourse
            && self.userLocation.location)
        {
            [self locationManager:self.locationManager didUpdateLocations:@[self.userLocation.location] animated:animated];
        }
    }
}

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations
{
    [self locationManager:manager didUpdateLocations:locations animated:YES];
}

- (void)locationManager:(__unused CLLocationManager *)manager didUpdateLocations:(NSArray *)locations animated:(BOOL)animated
{
    CLLocation *oldLocation = self.userLocation.location;
    CLLocation *newLocation = locations.lastObject;

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

    self.userLocationAnnotationView.haloLayer.hidden = ! CLLocationCoordinate2DIsValid(self.userLocation.coordinate) ||
        newLocation.horizontalAccuracy > 10;

    [self updateUserLocationAnnotationView];
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
        && self.userTrackingState != MGLUserTrackingStateBegan
        && CLLocationCoordinate2DIsValid(self.targetCoordinate))
    {
        // Keep both the user and the destination in view.
        [self didUpdateLocationWithTargetAnimated:animated];
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
    self.userTrackingState = MGLUserTrackingStateBegan;
    
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
        if (strongSelf.userTrackingState == MGLUserTrackingStateBegan)
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
    if (firstUpdate)
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
    if (self.userLocationVerticalAlignment == MGLAnnotationVerticalAlignmentCenter)
    {
        inset.bottom = CGRectGetMaxY(self.bounds) - CGRectGetMidY(self.contentFrame);
    }
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
    return UIEdgeInsetsMake(CGRectGetMinY(boundsAroundCorrectPoint) - CGRectGetMinY(bounds), 0,
                            CGRectGetMaxY(bounds) - CGRectGetMaxY(boundsAroundCorrectPoint), 0);
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
            double angle = atan2(targetMeters.easting - userMeters.easting,
                                 targetMeters.northing - userMeters.northing);
            direction = mbgl::util::wrap(MGLDegreesFromRadians(angle), 0., 360.);
        }
        else
        {
            direction = self.userLocation.location.course;
        }
        
        if (direction >= 0)
        {
            if (self.userLocationVerticalAlignment == MGLAnnotationVerticalAlignmentTop)
            {
                direction += 180;
            }
        }
    }
    return direction;
}

- (BOOL)locationManagerShouldDisplayHeadingCalibration:(CLLocationManager *)manager
{
    if (self.displayHeadingCalibration) [manager performSelector:@selector(dismissHeadingCalibrationDisplay)
                                                      withObject:nil
                                                      afterDelay:10.0];

    return self.displayHeadingCalibration;
}

- (void)locationManager:(__unused CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading
{
    if ( ! _showsUserLocation || self.pan.state == UIGestureRecognizerStateBegan || newHeading.headingAccuracy < 0) return;

    self.userLocation.heading = newHeading;

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

- (void)locationManager:(__unused CLLocationManager *)manager didFailWithError:(NSError *)error
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
        switch ([[UIApplication sharedApplication] statusBarOrientation])
        {
            case (UIInterfaceOrientationLandscapeLeft):
            {
                self.locationManager.headingOrientation = CLDeviceOrientationLandscapeRight;
                break;
            }
            case (UIInterfaceOrientationLandscapeRight):
            {
                self.locationManager.headingOrientation = CLDeviceOrientationLandscapeLeft;
                break;
            }
            case (UIInterfaceOrientationPortraitUpsideDown):
            {
                self.locationManager.headingOrientation = CLDeviceOrientationPortraitUpsideDown;
                break;
            }
            case (UIInterfaceOrientationPortrait):
            default:
            {
                self.locationManager.headingOrientation = CLDeviceOrientationPortrait;
                break;
            }
        }
    }
}

#pragma mark - Utility -

- (void)animateWithDelay:(NSTimeInterval)delay animations:(void (^)(void))animations
{
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delay * NSEC_PER_SEC)), dispatch_get_main_queue(), animations);
}

- (CGFloat)currentMinimumZoom
{
    return fmaxf(_mbglMap->getMinZoom(), MGLMinimumZoom);
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

- (void)notifyMapChange:(mbgl::MapChange)change
{
    // Ignore map updates when the Map object isn't set.
    if (!_mbglMap) {
        return;
    }

    switch (change)
    {
        case mbgl::MapChangeRegionWillChange:
        case mbgl::MapChangeRegionWillChangeAnimated:
        {
            if ( ! _userLocationAnnotationIsSelected
                || self.userTrackingMode == MGLUserTrackingModeNone
                || self.userTrackingState != MGLUserTrackingStateChanged)
            {
                [self deselectAnnotation:self.selectedAnnotation animated:NO];
            }

            if ( ! [self isSuppressingChangeDelimiters] && [self.delegate respondsToSelector:@selector(mapView:regionWillChangeAnimated:)])
            {
                BOOL animated = change == mbgl::MapChangeRegionWillChangeAnimated;
                [self.delegate mapView:self regionWillChangeAnimated:animated];
            }
            break;
        }
        case mbgl::MapChangeRegionIsChanging:
        {
            [self updateCompass];

            if ([self.delegate respondsToSelector:@selector(mapViewRegionIsChanging:)])
            {
                [self.delegate mapViewRegionIsChanging:self];
            }
            break;
        }
        case mbgl::MapChangeRegionDidChange:
        case mbgl::MapChangeRegionDidChangeAnimated:
        {
            [self updateCompass];

            if ( ! [self isSuppressingChangeDelimiters] && [self.delegate respondsToSelector:@selector(mapView:regionDidChangeAnimated:)])
            {
                BOOL animated = change == mbgl::MapChangeRegionDidChangeAnimated;
                [self.delegate mapView:self regionDidChangeAnimated:animated];
            }
            break;
        }
        case mbgl::MapChangeWillStartLoadingMap:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewWillStartLoadingMap:)])
            {
                [self.delegate mapViewWillStartLoadingMap:self];
            }
            break;
        }
        case mbgl::MapChangeDidFinishLoadingMap:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewDidFinishLoadingMap:)])
            {
                [self.delegate mapViewDidFinishLoadingMap:self];
            }
            break;
        }
        case mbgl::MapChangeDidFailLoadingMap:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewDidFailLoadingMap:withError:)])
            {
                NSError *error = [NSError errorWithDomain:MGLErrorDomain code:0 userInfo:nil];
                [self.delegate mapViewDidFailLoadingMap:self withError:error];
            }
            break;
        }
        case mbgl::MapChangeWillStartRenderingMap:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewWillStartRenderingMap:)])
            {
                [self.delegate mapViewWillStartRenderingMap:self];
            }
            break;
        }
        case mbgl::MapChangeDidFinishRenderingMap:
        case mbgl::MapChangeDidFinishRenderingMapFullyRendered:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewDidFinishRenderingMap:fullyRendered:)])
            {
                [self.delegate mapViewDidFinishRenderingMap:self fullyRendered:(change == mbgl::MapChangeDidFinishRenderingMapFullyRendered)];
            }
            break;
        }
        case mbgl::MapChangeWillStartRenderingFrame:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewWillStartRenderingFrame:)])
            {
                [self.delegate mapViewWillStartRenderingFrame:self];
            }
            break;
        }
        case mbgl::MapChangeDidFinishRenderingFrame:
        case mbgl::MapChangeDidFinishRenderingFrameFullyRendered:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewDidFinishRenderingFrame:fullyRendered:)])
            {
                [self.delegate mapViewDidFinishRenderingFrame:self fullyRendered:(change == mbgl::MapChangeDidFinishRenderingFrameFullyRendered)];
            }
            break;
        }
    }
}

- (void)updateUserLocationAnnotationView
{
    MGLUserLocationAnnotationView *annotationView = self.userLocationAnnotationView;
    if ( ! CLLocationCoordinate2DIsValid(self.userLocation.coordinate)) {
        annotationView.hidden = YES;
        return;
    }

    if ( ! annotationView.superview) [self.glView addSubview:annotationView];

    CGPoint userPoint;
    if (self.userTrackingMode != MGLUserTrackingModeNone
        && self.userTrackingState == MGLUserTrackingStateChanged)
    {
        userPoint = self.userLocationAnnotationViewCenter;
    }
    else
    {
        userPoint = [self convertCoordinate:self.userLocation.coordinate toPointToView:self];
    }

    if (CGRectContainsPoint(CGRectInset(self.bounds, -MGLAnnotationUpdateViewportOutset.width,
        -MGLAnnotationUpdateViewportOutset.height), userPoint))
    {
        annotationView.center = userPoint;
        annotationView.hidden = NO;
        [annotationView setupLayers];
        
        if (_userLocationAnnotationIsSelected)
        {
            // Ensure the callout view still points to its annotation.
            UIView <MGLCalloutView> *calloutView = self.calloutViewForSelectedAnnotation;
            CGRect calloutFrame = calloutView.frame;
            calloutFrame.origin.x = annotationView.center.x - CGRectGetWidth(calloutFrame) / 2;
            calloutFrame.origin.y = CGRectGetMinY(annotationView.frame) - CGRectGetHeight(calloutFrame);
            if ( ! CGRectEqualToRect(calloutView.frame, calloutFrame))
            {
                calloutView.frame = calloutFrame;
            }
        }
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
    CGRect contentFrame = UIEdgeInsetsInsetRect(self.contentFrame, self.edgePaddingForFollowingWithCourse);
    if (CGRectIsEmpty(contentFrame))
    {
        contentFrame = self.contentFrame;
    }
    CGPoint center = CGPointMake(CGRectGetMidX(contentFrame), CGRectGetMidY(contentFrame));
    
    // When tracking course, it’s more important to see the road ahead, so
    // weight the user dot down towards the bottom.
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
    
    return center;
}

- (void)updateCompass
{
    CLLocationDirection degrees = mbgl::util::wrap(-self.direction, 0., 360.);

    self.compassView.transform = CGAffineTransformMakeRotation(MGLRadiansFromDegrees(degrees));

    if (_mbglMap->getBearing() && self.compassView.alpha < 1)
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
    else if (_mbglMap->getBearing() == 0 && self.compassView.alpha > 0)
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

+ (UIImage *)resourceImageNamed:(NSString *)imageName
{
    NSString *extension = imageName.pathExtension.length ? imageName.pathExtension : @"png";
    NSBundle *bundle = [NSBundle mgl_frameworkBundle];
    NSString *path = [bundle pathForResource:imageName.stringByDeletingPathExtension
                                      ofType:extension];
    if ( ! path)
    {
        [NSException raise:@"Resource not found" format:
         @"The resource named “%@” could not be found in the Mapbox framework bundle.", imageName];
    }
    
    return [UIImage imageWithContentsOfFile:path];
}

- (BOOL)isFullyLoaded
{
    return _mbglMap->isFullyLoaded();
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
    headlineLabel.text = @"MGLMapView";
    headlineLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleHeadline];
    headlineLabel.textAlignment = NSTextAlignmentCenter;
    headlineLabel.numberOfLines = 1;
    headlineLabel.translatesAutoresizingMaskIntoConstraints = NO;
    [headlineLabel setContentCompressionResistancePriority:UILayoutPriorityDefaultLow
                                                   forAxis:UILayoutConstraintAxisHorizontal];
    [diagnosticView addSubview:headlineLabel];

    // Explanation
    UILabel *explanationLabel = [[UILabel alloc] init];
    explanationLabel.text = (@"To display a Mapbox-hosted map here, set MGLMapboxAccessToken to your access token in Info.plist\n\n"
                             @"For detailed instructions, see:");
    explanationLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
    explanationLabel.numberOfLines = 0;
    explanationLabel.translatesAutoresizingMaskIntoConstraints = NO;
    [explanationLabel setContentCompressionResistancePriority:UILayoutPriorityDefaultLow
                                                      forAxis:UILayoutConstraintAxisHorizontal];
    [diagnosticView addSubview:explanationLabel];

    // Link
    UIButton *linkButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [linkButton setTitle:MGLMapboxSetupDocumentationURLDisplayString forState:UIControlStateNormal];
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

class MBGLView : public mbgl::View
{
    public:
        MBGLView(MGLMapView* nativeView_, const float scaleFactor_)
            : nativeView(nativeView_), scaleFactor(scaleFactor_) {
        }
        virtual ~MBGLView() {}


    float getPixelRatio() const override {
        return scaleFactor;
    }

    std::array<uint16_t, 2> getSize() const override {
        return {{ static_cast<uint16_t>([nativeView bounds].size.width),
                static_cast<uint16_t>([nativeView bounds].size.height) }};
    }

    std::array<uint16_t, 2> getFramebufferSize() const override {
        return {{ static_cast<uint16_t>([[nativeView glView] drawableWidth]),
                  static_cast<uint16_t>([[nativeView glView] drawableHeight]) }};
    }

    void notify() override
    {
        // no-op
    }

    void notifyMapChange(mbgl::MapChange change) override
    {
        assert([[NSThread currentThread] isMainThread]);
        [nativeView notifyMapChange:change];
    }

    void activate() override
    {
        [EAGLContext setCurrentContext:nativeView.context];
    }

    void deactivate() override
    {
        [EAGLContext setCurrentContext:nil];
    }

    void invalidate() override
    {
        [nativeView performSelectorOnMainThread:@selector(setNeedsGLDisplay)
                                     withObject:nil
                                  waitUntilDone:NO];
    }

    void beforeRender() override
    {
        // no-op
    }

    void afterRender() override
    {
        // no-op
    }

    private:
        __weak MGLMapView *nativeView = nullptr;
        const float scaleFactor;
};

@end

#pragma mark - IBAdditions methods

@implementation MGLMapView (IBAdditions)

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingStyleURL__
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
        [NSException raise:@"Invalid style URL" format:
         @"“%@” is not a valid style URL.", URLString];
    }
    self.styleURL = url;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingLatitude
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

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingLongitude
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

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingAllowsZooming
{
    return [NSSet setWithObject:@"zoomEnabled"];
}

- (BOOL)allowsZooming
{
    return self.zoomEnabled;
}

- (void)setAllowsZooming:(BOOL)allowsZooming
{
    self.zoomEnabled = allowsZooming;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingAllowsScrolling
{
    return [NSSet setWithObject:@"scrollEnabled"];
}

- (BOOL)allowsScrolling
{
    return self.scrollEnabled;
}

- (void)setAllowsScrolling:(BOOL)allowsScrolling
{
    self.scrollEnabled = allowsScrolling;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingAllowsRotating
{
    return [NSSet setWithObject:@"rotateEnabled"];
}

- (BOOL)allowsRotating
{
    return self.rotateEnabled;
}

- (void)setAllowsRotating:(BOOL)allowsRotating
{
    self.rotateEnabled = allowsRotating;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingAllowsTilting
{
    return [NSSet setWithObject:@"pitchEnabled"];
}

- (BOOL)allowsTilting
{
    return self.pitchEnabled;
}

- (void)setAllowsTilting:(BOOL)allowsTilting
{
    self.pitchEnabled = allowsTilting;
}

@end

#pragma mark - MGLCustomStyleLayerAdditions methods

class MGLCustomStyleLayerHandlers
{
public:
    MGLCustomStyleLayerHandlers(MGLCustomStyleLayerPreparationHandler p,
                                MGLCustomStyleLayerDrawingHandler d,
                                MGLCustomStyleLayerCompletionHandler f)
    : prepare(p), draw(d), finish(f) {}
    
    MGLCustomStyleLayerPreparationHandler prepare;
    MGLCustomStyleLayerDrawingHandler draw;
    MGLCustomStyleLayerCompletionHandler finish;
};

void MGLPrepareCustomStyleLayer(void *context)
{
    MGLCustomStyleLayerPreparationHandler prepare = reinterpret_cast<MGLCustomStyleLayerHandlers *>(context)->prepare;
    if (prepare)
    {
        prepare();
    }
}

void MGLDrawCustomStyleLayer(void *context, const mbgl::CustomLayerRenderParameters &params)
{
    CGSize size = CGSizeMake(params.width, params.height);
    CLLocationCoordinate2D centerCoordinate = CLLocationCoordinate2DMake(params.latitude, params.longitude);
    double zoomLevel = params.zoom;
    CLLocationDirection direction = mbgl::util::wrap(params.bearing, 0., 360.);
    CGFloat pitch = params.pitch;
    CGFloat perspectiveSkew = params.altitude;
    MGLCustomStyleLayerDrawingHandler draw = reinterpret_cast<MGLCustomStyleLayerHandlers *>(context)->draw;
    if (draw)
    {
        draw(size, centerCoordinate, zoomLevel, direction, pitch, perspectiveSkew);
    }
}

void MGLFinishCustomStyleLayer(void *context)
{
    MGLCustomStyleLayerHandlers *handlers = reinterpret_cast<MGLCustomStyleLayerHandlers *>(context);
    MGLCustomStyleLayerCompletionHandler finish = handlers->finish;
    if (finish)
    {
        finish();
    }
    delete handlers;
}

@implementation MGLMapView (MGLCustomStyleLayerAdditions)

- (void)insertCustomStyleLayerWithIdentifier:(NSString *)identifier preparationHandler:(void (^)())preparation drawingHandler:(MGLCustomStyleLayerDrawingHandler)drawing completionHandler:(void (^)())completion belowStyleLayerWithIdentifier:(nullable NSString *)otherIdentifier
{
    NSAssert(identifier, @"Style layer needs an identifier");
    MGLCustomStyleLayerHandlers *context = new MGLCustomStyleLayerHandlers(preparation, drawing, completion);
    _mbglMap->addCustomLayer(identifier.UTF8String, MGLPrepareCustomStyleLayer,
                             MGLDrawCustomStyleLayer, MGLFinishCustomStyleLayer,
                             context, otherIdentifier.UTF8String);
}

- (void)removeCustomStyleLayerWithIdentifier:(NSString *)identifier
{
    _mbglMap->removeCustomLayer(identifier.UTF8String);
}

- (void)setCustomStyleLayersNeedDisplay
{
    _mbglMap->update(mbgl::Update::Repaint);
}

@end
