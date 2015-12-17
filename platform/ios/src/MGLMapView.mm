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
#include <mbgl/storage/sqlite_cache.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/default_styles.hpp>

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

#import "SMCalloutView.h"

#import <algorithm>
#import <cstdlib>
#import <map>
#import <unordered_set>

class MBGLView;
class MGLAnnotationContext;

NSString *const MGLMapboxSetupDocumentationURLDisplayString = @"mapbox.com/help/first-steps-ios-sdk";

const NSTimeInterval MGLAnimationDuration = 0.3;
const CGSize MGLAnnotationUpdateViewportOutset = {150, 150};
const CGFloat MGLMinimumZoom = 3;
const CGFloat MGLMinimumPitch = 0;
const CGFloat MGLMaximumPitch = 60;
const CLLocationDegrees MGLAngularFieldOfView = M_PI / 6.;
const NSUInteger MGLTargetFrameInterval = 1;  //Target FPS will be 60 divided by this value

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
                          SMCalloutViewDelegate,
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
@property (nonatomic) SMCalloutView *calloutViewForSelectedAnnotation;
@property (nonatomic) MGLUserLocationAnnotationView *userLocationAnnotationView;
@property (nonatomic) CLLocationManager *locationManager;
@property (nonatomic) CGPoint centerPoint;
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
    std::shared_ptr<mbgl::SQLiteCache> _mbglFileCache;
    mbgl::DefaultFileSource *_mbglFileSource;

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

std::chrono::steady_clock::duration MGLDurationInSeconds(float duration)
{
    return std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<float, std::chrono::seconds::period>(duration));
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
    _mbglFileCache = mbgl::SharedSQLiteCache::get([fileCachePath UTF8String]);
    _mbglFileSource = new mbgl::DefaultFileSource(_mbglFileCache.get());

    // setup mbgl map
    _mbglMap = new mbgl::Map(*_mbglView, *_mbglFileSource, mbgl::MapMode::Continuous);

    // setup refresh driver
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateFromDisplayLink)];
    _displayLink.frameInterval = MGLTargetFrameInterval;
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    _needsDisplayRefresh = YES;

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
    options.zoom = _mbglMap->getMinZoom();
    _mbglMap->jumpTo(options);
    _pendingLatitude = NAN;
    _pendingLongitude = NAN;

    // metrics: map load event
    mbgl::LatLng latLng = _mbglMap->getLatLng();
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
        [self updateUserLocationAnnotationView];
    }
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

- (void)invalidate
{
    MGLAssertIsMainThread();

    _needsDisplayRefresh = YES;
}

- (void)willTerminate
{
    MGLAssertIsMainThread();

    if ( ! self.isDormant)
    {
        self.dormant = YES;
        _mbglMap->pause();
        [self.glView deleteDrawable];
    }
}

- (void)sleepGL:(__unused NSNotification *)notification
{
    MGLAssertIsMainThread();

    if ( ! self.isDormant)
    {
        self.dormant = YES;

        [MGLMapboxEvents flush];

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

    if (self.isDormant && [UIApplication sharedApplication].applicationState != UIApplicationStateBackground)
    {
        self.dormant = NO;

        [self createGLView];
        [MGLMapboxEvents validate];

        self.glSnapshotView.hidden = YES;

        [self.glSnapshotView.subviews makeObjectsPerformSelector:@selector(removeFromSuperview)];

        [self.glView bindDrawable];

        _mbglMap->resume();
    }
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

        self.centerPoint = CGPointMake(0, 0);

        self.userTrackingMode = MGLUserTrackingModeNone;
        
        [self notifyGestureDidBegin];
    }
    else if (pan.state == UIGestureRecognizerStateChanged)
    {
        CGPoint delta = CGPointMake([pan translationInView:pan.view].x - self.centerPoint.x,
                                    [pan translationInView:pan.view].y - self.centerPoint.y);

        double flippedY = self.bounds.size.height - [pan locationInView:pan.view].y;
        _mbglMap->setLatLng(
            _mbglMap->latLngForPixel(mbgl::PrecisionPoint(
                [pan locationInView:pan.view].x - delta.x,
                flippedY + delta.y)),
            mbgl::PrecisionPoint(
                [pan locationInView:pan.view].x,
                flippedY));

        self.centerPoint = CGPointMake(self.centerPoint.x + delta.x, self.centerPoint.y + delta.y);

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

        CGFloat duration = UIScrollViewDecelerationRateNormal;
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

    if (pinch.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGesturePinchStart forRecognizer:pinch];

        self.scale = _mbglMap->getScale();

        self.userTrackingMode = MGLUserTrackingModeNone;
        
        [self notifyGestureDidBegin];
    }
    else if (pinch.state == UIGestureRecognizerStateChanged)
    {
        CGFloat newScale = self.scale * pinch.scale;

        if (log2(newScale) < _mbglMap->getMinZoom()) return;

        mbgl::PrecisionPoint center([pinch locationInView:pinch.view].x,
                                    [pinch locationInView:pinch.view].y);
        _mbglMap->setScale(newScale, center);

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
        CGFloat duration = velocity > 0 ? 1 : 0.25;

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
            CGPoint pinchCenter = [pinch locationInView:pinch.view];
            mbgl::PrecisionPoint center(pinchCenter.x, pinchCenter.y);
            _mbglMap->setScale(newScale, center, MGLDurationInSeconds(duration));
        }

        [self notifyGestureDidEndWithDrift:velocity];

        [self unrotateIfNeededAnimated:YES];
    }
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    if ( ! self.isRotateEnabled) return;

    _mbglMap->cancelTransitions();

    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGestureRotateStart forRecognizer:rotate];

        self.angle = MGLRadiansFromDegrees(_mbglMap->getBearing()) * -1;

        self.userTrackingMode = MGLUserTrackingModeNone;
        
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

        mbgl::PrecisionPoint center([rotate locationInView:rotate.view].x,
                                    self.bounds.size.height - [rotate locationInView:rotate.view].y);
        _mbglMap->setBearing(newDegrees, center);

        [self notifyMapChange:mbgl::MapChangeRegionIsChanging];
    }
    else if (rotate.state == UIGestureRecognizerStateEnded || rotate.state == UIGestureRecognizerStateCancelled)
    {
        CGFloat velocity = rotate.velocity;

        if (fabs(velocity) > 3)
        {
            CGFloat radians = self.angle + rotate.rotation;
            CGFloat duration = UIScrollViewDecelerationRateNormal;
            CGFloat newRadians = radians + velocity * duration * 0.1;
            CGFloat newDegrees = MGLDegreesFromRadians(newRadians) * -1;

            _mbglMap->setBearing(newDegrees, MGLDurationInSeconds(duration));

            [self notifyGestureDidEndWithDrift:YES];

            __weak MGLMapView *weakSelf = self;

            [self animateWithDelay:duration animations:^
             {
                 [weakSelf unrotateIfNeededAnimated:YES];
             }];
        }
        else
        {
            [self notifyGestureDidEndWithDrift:NO];

            [self unrotateIfNeededAnimated:YES];
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
        CGPoint doubleTapPoint = [doubleTap locationInView:doubleTap.view];

        CGPoint zoomInPoint = doubleTapPoint;
        CGPoint userPoint = [self convertCoordinate:self.userLocation.coordinate toPointToView:self];
        if (self.userTrackingMode != MGLUserTrackingModeNone)
        {
            CGRect userLocationRect = CGRectMake(userPoint.x - 40, userPoint.y - 40, 80, 80);
            if (CGRectContainsPoint(userLocationRect, doubleTapPoint))
            {
                zoomInPoint = userPoint;
            }
        }
        if ( ! CGPointEqualToPoint(zoomInPoint, userPoint))
        {
            self.userTrackingMode = MGLUserTrackingModeNone;
        }

        mbgl::PrecisionPoint center(zoomInPoint.x, zoomInPoint.y);
        _mbglMap->scaleBy(2, center, MGLDurationInSeconds(MGLAnimationDuration));

        __weak MGLMapView *weakSelf = self;

        [self animateWithDelay:MGLAnimationDuration animations:^
        {
            [weakSelf unrotateIfNeededAnimated:YES];
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
        CGPoint zoomOutPoint;

        if (self.userTrackingMode != MGLUserTrackingModeNone)
        {
            zoomOutPoint = self.center;
        }
        else
        {
            self.userTrackingMode = MGLUserTrackingModeNone;

            zoomOutPoint = CGPointMake([twoFingerTap locationInView:twoFingerTap.view].x, [twoFingerTap locationInView:twoFingerTap.view].y);
        }

        mbgl::PrecisionPoint center(zoomOutPoint.x, zoomOutPoint.y);
        _mbglMap->scaleBy(0.5, center, MGLDurationInSeconds(MGLAnimationDuration));

        __weak MGLMapView *weakSelf = self;

        [self animateWithDelay:MGLAnimationDuration animations:^
        {
            [weakSelf unrotateIfNeededAnimated:YES];
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

        mbgl::PrecisionPoint center(self.bounds.size.width / 2, self.bounds.size.height / 2);
        _mbglMap->scaleBy(powf(2, newZoom) / _mbglMap->getScale(), center);

        [self notifyMapChange:mbgl::MapChangeRegionIsChanging];
    }
    else if (quickZoom.state == UIGestureRecognizerStateEnded || quickZoom.state == UIGestureRecognizerStateCancelled)
    {
        [self notifyGestureDidEndWithDrift:NO];
        [self unrotateIfNeededAnimated:YES];
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

        CGFloat pitchNew = mbgl::util::clamp(currentPitch - (gestureDistance / slowdown), MGLMinimumPitch, MGLMaximumPitch);

        _mbglMap->setPitch(pitchNew);

        [self notifyMapChange:mbgl::MapChangeRegionIsChanging];
    }
    else if (twoFingerDrag.state == UIGestureRecognizerStateEnded || twoFingerDrag.state == UIGestureRecognizerStateCancelled)
    {
        [self notifyGestureDidEndWithDrift:NO];
        [self unrotateIfNeededAnimated:YES];
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

- (BOOL)calloutViewShouldHighlight:(__unused SMCalloutView *)calloutView
{
    return [self.delegate respondsToSelector:@selector(mapView:tapOnCalloutForAnnotation:)];
}

- (void)calloutViewClicked:(__unused SMCalloutView *)calloutView
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
                                 nil];

        // iOS 8+: add action that opens app's Settings.app panel, if applicable
        if (&UIApplicationOpenSettingsURLString != NULL && ! [MGLAccountManager mapboxMetricsEnabledSettingShownInApp])
        {
            [self.attributionSheet addButtonWithTitle:@"Adjust Privacy Settings"];
        }
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
    // skips to 4 because button is conditionally added after cancel (index 3)
    else if (buttonIndex == actionSheet.firstOtherButtonIndex + 4)
    {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString]];
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
    _mbglMap->cancelTransitions();
    _mbglMap->resetPosition();
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
    return MGLLocationCoordinate2DFromLatLng(_mbglMap->getLatLng());
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

    [self _setCenterCoordinate:centerCoordinate zoomLevel:zoomLevel direction:direction animated:animated completionHandler:completion];
}

- (void)_setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel direction:(CLLocationDirection)direction animated:(BOOL)animated completionHandler:(nullable void (^)(void))completion
{
    _mbglMap->cancelTransitions();
    
    NSTimeInterval duration = animated ? MGLAnimationDuration : 0;
    mbgl::CameraOptions options;
    options.center = MGLLatLngFromLocationCoordinate2D(centerCoordinate);
    options.zoom = fmaxf(zoomLevel, self.currentMinimumZoom);
    if (direction >= 0)
    {
        options.angle = MGLRadiansFromDegrees(-direction);
    }
    if (animated)
    {
        options.duration = MGLDurationInSeconds(duration);
        options.easing = MGLUnitBezierForMediaTimingFunction(nil);
    }
    if (completion)
    {
        options.transitionFinishFn = [completion]() {
            // Must run asynchronously after the transition is completely over.
            // Otherwise, a call to -setCenterCoordinate: within the completion
            // handler would reenter the completion handler’s caller.
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    _mbglMap->easeTo(options);

    [self unrotateIfNeededAnimated:animated];
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
    [self setCenterCoordinate:self.centerCoordinate zoomLevel:zoomLevel animated:animated];
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
    _mbglMap->cancelTransitions();
    
    // NOTE: does not disrupt tracking mode
    [self willChangeValueForKey:@"visibleCoordinateBounds"];
    mbgl::EdgeInsets mbglInsets = {insets.top, insets.left, insets.bottom, insets.right};
    mbgl::AnnotationSegment segment;
    segment.reserve(count);
    for (NSUInteger i = 0; i < count; i++)
    {
        segment.push_back({coordinates[i].latitude, coordinates[i].longitude});
    }
    mbgl::CameraOptions options = _mbglMap->cameraForLatLngs(segment, mbglInsets);
    if (direction >= 0)
    {
        options.angle = MGLRadiansFromDegrees(-direction);
    }
    if (duration > 0)
    {
        options.duration = MGLDurationInSeconds(duration);
        options.easing = MGLUnitBezierForMediaTimingFunction(function);
    }
    if (completion)
    {
        options.transitionFinishFn = [completion]() {
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    _mbglMap->easeTo(options);
    [self didChangeValueForKey:@"visibleCoordinateBounds"];

    [self unrotateIfNeededAnimated:duration > 0];
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

    if (self.userTrackingMode == MGLUserTrackingModeFollowWithHeading ||
        self.userTrackingMode == MGLUserTrackingModeFollowWithCourse)
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

    _mbglMap->setBearing(direction, MGLDurationInSeconds(duration));
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
    CGRect frame = self.frame;
    CGPoint edgePoint;
    // Constrain by the shorter of the two axes.
    if (frame.size.width > frame.size.height) // landscape
    {
        edgePoint = CGPointMake(0, frame.size.height / 2.);
    }
    else // portrait
    {
        edgePoint = CGPointMake(frame.size.width / 2., 0);
    }
    CLLocationCoordinate2D edgeCoordinate = [self convertPoint:edgePoint toCoordinateFromView:self];
    mbgl::ProjectedMeters edgeMeters = _mbglMap->projectedMetersForLatLng(MGLLatLngFromLocationCoordinate2D(edgeCoordinate));

    // Because we constrain the zoom level vertically in portrait orientation,
    // the visible medial span is affected by pitch: the distance from the
    // center point to the near edge is less than than distance from the center
    // point to the far edge. Average the two distances.
    mbgl::ProjectedMeters nearEdgeMeters;
    if (frame.size.width > frame.size.height)
    {
        nearEdgeMeters = edgeMeters;
    }
    else
    {
        CGPoint nearEdgePoint = CGPointMake(frame.size.width / 2., frame.size.height);
        CLLocationCoordinate2D nearEdgeCoordinate = [self convertPoint:nearEdgePoint toCoordinateFromView:self];
        nearEdgeMeters = _mbglMap->projectedMetersForLatLng(MGLLatLngFromLocationCoordinate2D(nearEdgeCoordinate));
    }

    // The opposite side is the distance between the center and one edge.
    CLLocationCoordinate2D centerCoordinate = self.centerCoordinate;
    mbgl::ProjectedMeters centerMeters = _mbglMap->projectedMetersForLatLng(MGLLatLngFromLocationCoordinate2D(centerCoordinate));
    CLLocationDistance centerToEdge = std::hypot(centerMeters.easting - edgeMeters.easting,
                                                 centerMeters.northing - edgeMeters.northing);
    CLLocationDistance centerToNearEdge = std::hypot(centerMeters.easting - nearEdgeMeters.easting,
                                                     centerMeters.northing - nearEdgeMeters.northing);
    CLLocationDistance altitude = (centerToEdge + centerToNearEdge) / 2 / std::tan(MGLAngularFieldOfView / 2.);

    CGFloat pitch = _mbglMap->getPitch();

    return [MGLMapCamera cameraLookingAtCenterCoordinate:centerCoordinate
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
    _mbglMap->cancelTransitions();
    mbgl::CameraOptions options = [self cameraOptionsObjectForAnimatingToCamera:camera];
    if (duration > 0)
    {
        options.duration = MGLDurationInSeconds(duration);
        options.easing = MGLUnitBezierForMediaTimingFunction(function);
    }
    if (completion)
    {
        options.transitionFinishFn = [completion]() {
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    
    [self willChangeValueForKey:@"camera"];
    _mbglMap->easeTo(options);
    [self didChangeValueForKey:@"camera"];
}

- (void)flyToCamera:(MGLMapCamera *)camera completionHandler:(nullable void (^)(void))completion
{
    [self flyToCamera:camera withDuration:0 completionHandler:completion];
}

- (void)flyToCamera:(MGLMapCamera *)camera withDuration:(NSTimeInterval)duration completionHandler:(nullable void (^)(void))completion
{
    _mbglMap->cancelTransitions();
    mbgl::CameraOptions options = [self cameraOptionsObjectForAnimatingToCamera:camera];
    if (duration > 0)
    {
        options.duration = MGLDurationInSeconds(duration);
    }
    if (completion)
    {
        options.transitionFinishFn = [completion]() {
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                completion();
            });
        };
    }
    
    [self willChangeValueForKey:@"camera"];
    _mbglMap->flyTo(options);
    [self didChangeValueForKey:@"camera"];
}

/// Returns a CameraOptions object that specifies parameters for animating to
/// the given camera.
- (mbgl::CameraOptions)cameraOptionsObjectForAnimatingToCamera:(MGLMapCamera *)camera {
    // The opposite side is the distance between the center and one edge.
    mbgl::LatLng centerLatLng = MGLLatLngFromLocationCoordinate2D(camera.centerCoordinate);
    mbgl::ProjectedMeters centerMeters = _mbglMap->projectedMetersForLatLng(centerLatLng);
    CLLocationDistance centerToEdge = camera.altitude * std::tan(MGLAngularFieldOfView / 2.);

    double angle = -1;
    if (camera.heading >= 0)
    {
        angle = MGLRadiansFromDegrees(-camera.heading);
    }
    double pitch = -1;
    if (camera.pitch >= 0)
    {
        pitch = MGLRadiansFromDegrees(mbgl::util::clamp(camera.pitch, MGLMinimumPitch, MGLMaximumPitch));
    }

    // Make a visible bounds that extends in the constrained direction (the
    // shorter of the two axes).
    CGRect frame = self.frame;
    mbgl::LatLng sw, ne;
    if (frame.size.width > frame.size.height) // landscape
    {
        sw = _mbglMap->latLngForProjectedMeters({
            centerMeters.northing - centerToEdge * std::sin(angle),
            centerMeters.easting - centerToEdge * std::cos(angle),
        });
        ne = _mbglMap->latLngForProjectedMeters({
            centerMeters.northing + centerToEdge * std::sin(angle),
            centerMeters.easting + centerToEdge * std::cos(angle),
        });
    }
    else // portrait
    {
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

    if (camera.heading >= 0)
    {
        options.angle = angle;
    }
    if (pitch >= 0)
    {
        options.pitch = pitch;
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
    
    // Flip y coordinate for iOS view origin in the top left corner.
    convertedPoint.y = self.bounds.size.height - convertedPoint.y;
    
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
    
    // Flip y coordinate for iOS view origin in the top left corner.
    pixel.y = self.bounds.size.height - pixel.y;
    
    return [self convertPoint:CGPointMake(pixel.x, pixel.y) toView:view];
}

- (MGLCoordinateBounds)convertRect:(CGRect)rect toCoordinateBoundsFromView:(nullable UIView *)view
{
    return MGLCoordinateBoundsFromLatLngBounds([self convertRect:rect toLatLngBoundsFromView:view]);
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

- (CLLocationDistance)metersPerPixelAtLatitude:(CLLocationDegrees)latitude
{
    return _mbglMap->getMetersPerPixelAtLatitude(latitude, self.zoomLevel);
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
    std::string pixels(width * height * 4, '\0');
    size_t bytesPerPixel = 4;
    size_t bytesPerRow = bytesPerPixel * width;
    size_t bitsPerComponent = 8;
    char *pixelData = const_cast<char *>(pixels.data());
    CGContextRef context = CGBitmapContextCreate(pixelData, width, height, bitsPerComponent, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);

    // add sprite
    auto cSpriteImage = std::make_shared<mbgl::SpriteImage>(
        uint16_t(annotationImage.image.size.width),
        uint16_t(annotationImage.image.size.height),
        float(annotationImage.image.scale),
        std::move(pixels));

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
        NSAssert(annotationTag != MGLAnnotationTagNotFound, @"No ID for annotation %@", annotation);
        annotationTagsToRemove.push_back(annotationTag);

        if (annotationTag == _selectedAnnotationTag)
        {
            [self deselectAnnotation:annotation animated:NO];
        }
        
        _annotationContextsByAnnotationTag.erase(annotationTag);
    }

    [self willChangeValueForKey:@"annotations"];
    _mbglMap->removeAnnotations(annotationTagsToRemove);
    [self didChangeValueForKey:@"annotations"];
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

    self.userTrackingMode = MGLUserTrackingModeNone;

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
        self.calloutViewForSelectedAnnotation = [self calloutViewForAnnotation:annotation];

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

- (SMCalloutView *)calloutViewForAnnotation:(id <MGLAnnotation>)annotation
{
    SMCalloutView *calloutView = [SMCalloutView platformCalloutView];

    if ([annotation respondsToSelector:@selector(title)]) calloutView.title = annotation.title;
    if ([annotation respondsToSelector:@selector(subtitle)]) calloutView.subtitle = annotation.subtitle;

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

    [self setVisibleCoordinateBounds:MGLCoordinateBoundsFromLatLngBounds(bounds)
                         edgePadding:UIEdgeInsetsMake(100, 100, 100, 100)
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
                [self locationManager:self.locationManager didUpdateLocations:@[self.userLocation.location]];
            }

            break;
        }
        case MGLUserTrackingModeFollowWithHeading:
        {
            self.showsUserLocation = YES;

            if (self.zoomLevel < self.currentMinimumZoom)
            {
                [self setZoomLevel:self.currentMinimumZoom animated:YES];
                _userTrackingMode = MGLUserTrackingModeFollowWithHeading; // reapply
            }

            if (self.userLocationAnnotationView)
            {
                [self locationManager:self.locationManager didUpdateLocations:@[self.userLocation.location]];
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

- (void)locationManager:(__unused CLLocationManager *)manager didUpdateLocations:(NSArray *)locations
{
    CLLocation *oldLocation = self.userLocation.location;
    CLLocation *newLocation = locations.lastObject;

    if ( ! _showsUserLocation || ! newLocation || ! CLLocationCoordinate2DIsValid(newLocation.coordinate)) return;

    if (! oldLocation || ! CLLocationCoordinate2DIsValid(oldLocation.coordinate) || [newLocation distanceFromLocation:oldLocation])
    {
        self.userLocation.location = newLocation;

        // deselect user if applicable since we don't do callout tracking yet
        if ([self.selectedAnnotation isEqual:self.userLocation]) [self deselectAnnotation:self.userLocation animated:NO];

        if ([self.delegate respondsToSelector:@selector(mapView:didUpdateUserLocation:)])
        {
            [self.delegate mapView:self didUpdateUserLocation:self.userLocation];
        }
    }

    CLLocationDirection course = self.userLocation.location.course;
    if (course < 0 || self.userTrackingMode != MGLUserTrackingModeFollowWithCourse)
    {
        course = -1;
    }

    if (self.userTrackingMode != MGLUserTrackingModeNone)
    {
        // center on user location unless we're already centered there (or very close)
        //
        CGPoint mapCenterPoint    = [self convertPoint:self.center fromView:self.superview];
        CGPoint userLocationPoint = [self convertCoordinate:self.userLocation.coordinate toPointToView:self];

        if (std::abs(userLocationPoint.x - mapCenterPoint.x) > 1.0 || std::abs(userLocationPoint.y - mapCenterPoint.y) > 1.0)
        {
            if (round(self.zoomLevel) >= 10)
            {
                // at sufficient detail, just re-center the map; don't zoom
                //
                [self _setCenterCoordinate:self.userLocation.location.coordinate zoomLevel:self.zoomLevel direction:course animated:YES completionHandler:NULL];
            }
            else
            {
                // otherwise re-center and zoom in to near accuracy confidence
                //
                float delta = (newLocation.horizontalAccuracy / 110000) * 1.2; // approx. meter per degree latitude, plus some margin

                CLLocationCoordinate2D desiredSouthWest = CLLocationCoordinate2DMake(newLocation.coordinate.latitude  - delta,
                                                                                     newLocation.coordinate.longitude - delta);

                CLLocationCoordinate2D desiredNorthEast = CLLocationCoordinate2DMake(newLocation.coordinate.latitude  + delta,
                                                                                     newLocation.coordinate.longitude + delta);

                CGFloat pixelRadius = fminf(self.bounds.size.width, self.bounds.size.height) / 2;

                CLLocationCoordinate2D actualSouthWest = [self convertPoint:CGPointMake(userLocationPoint.x - pixelRadius,
                                                                                        userLocationPoint.y - pixelRadius)
                                                       toCoordinateFromView:self];

                CLLocationCoordinate2D actualNorthEast = [self convertPoint:CGPointMake(userLocationPoint.x + pixelRadius,
                                                                                        userLocationPoint.y + pixelRadius)
                                                       toCoordinateFromView:self];

                if (desiredNorthEast.latitude  != actualNorthEast.latitude  ||
                    desiredNorthEast.longitude != actualNorthEast.longitude ||
                    desiredSouthWest.latitude  != actualSouthWest.latitude  ||
                    desiredSouthWest.longitude != actualSouthWest.longitude)
                {
                    // assumes we won't disrupt tracking mode
                    [self setVisibleCoordinateBounds:MGLCoordinateBoundsMake(desiredSouthWest, desiredNorthEast) edgePadding:UIEdgeInsetsZero direction:course animated:YES];
                }
            }
        }
    }

    self.userLocationAnnotationView.haloLayer.hidden = ! CLLocationCoordinate2DIsValid(self.userLocation.coordinate) ||
        newLocation.horizontalAccuracy > 10;

    [self updateUserLocationAnnotationView];
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

    if (headingDirection >= 0 && self.userTrackingMode == MGLUserTrackingModeFollowWithHeading)
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

// correct rotations to north as needed
//
- (void)unrotateIfNeededAnimated:(BOOL)animated
{
    // don't worry about it in the midst of pinch or rotate gestures
    //
    if (self.pinch.state  == UIGestureRecognizerStateChanged || self.rotate.state == UIGestureRecognizerStateChanged) return;

    // but otherwise, do
    //
    if (self.direction != 0 && ! self.isRotationAllowed)
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
            [self deselectAnnotation:self.selectedAnnotation animated:NO];

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
    if ( ! CLLocationCoordinate2DIsValid(self.userLocation.coordinate)) {
        self.userLocationAnnotationView.layer.hidden = YES;
        return;
    }

    if ( ! self.userLocationAnnotationView.superview) [self.glView addSubview:self.userLocationAnnotationView];

    CGPoint userPoint = [self convertCoordinate:self.userLocation.coordinate toPointToView:self];

    if (CGRectContainsPoint(CGRectInset(self.bounds, -MGLAnnotationUpdateViewportOutset.width,
        -MGLAnnotationUpdateViewportOutset.height), userPoint))
    {
        self.userLocationAnnotationView.center = userPoint;

        self.userLocationAnnotationView.layer.hidden = NO;

        [self.userLocationAnnotationView setupLayers];
    }
    else
    {
        self.userLocationAnnotationView.layer.hidden = YES;
    }
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
    if ( ! [[imageName pathExtension] length])
    {
        imageName = [imageName stringByAppendingString:@".png"];
    }

    return [UIImage imageWithContentsOfFile:[self pathForBundleResourceNamed:imageName ofType:nil inDirectory:@""]];
}

+ (NSString *)pathForBundleResourceNamed:(NSString *)name ofType:(NSString *)extension inDirectory:(NSString *)directory
{
    NSString *path = [[NSBundle bundleWithPath:[NSBundle mgl_resourceBundlePath]] pathForResource:name ofType:extension inDirectory:directory];

    if (!path)
    {
        [NSException raise:@"Resource not found" format:
         @"The resource named “%@” could not be found in the Mapbox resource bundle.", name];
    }

    return path;
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
    explanationLabel.text = (@"To display a Mapbox-hosted map here:\n\n"
                             @"1. Set MGLMapboxAccessToken to your access token in Info.plist\n"
                             @"2. Add a Settings bundle that allows the user to turn Mapbox Metrics on and off\n\n"
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
        [nativeView performSelectorOnMainThread:@selector(invalidate)
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

- (void)setCustomStyleLayersNeedDisplay
{
    _mbglMap->update(mbgl::Update::Repaint);
}

@end
