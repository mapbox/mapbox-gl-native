#import "MGLMapView.h"
#import "MGLMapView+IBAdditions.h"

#import <mbgl/platform/log.hpp>
#import <mbgl/platform/gl.hpp>

#import <GLKit/GLKit.h>
#import <OpenGLES/EAGL.h>

#include <mbgl/mbgl.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/darwin/reachability.h>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/default/sqlite_cache.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/geo.hpp>

#import "MGLTypes.h"
#import "NSString+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "MGLAnnotation.h"
#import "MGLUserLocationAnnotationView.h"
#import "MGLUserLocation_Private.h"

#import "SMCalloutView.h"

#import "MGLMapboxEvents.h"

#import <algorithm>

// Returns the path to the default cache database on this system.
const std::string &defaultCacheDatabase() {
    static const std::string path = []() -> std::string {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        if ([paths count] == 0) {
            // Disable the cache if we don't have a location to write.
            return "";
        }

        NSString *libraryDirectory = [paths objectAtIndex:0];
        return [[libraryDirectory stringByAppendingPathComponent:@"cache.db"] UTF8String];
    }();
    return path;
}

static dispatch_once_t loadGLExtensions;

NSString *const MGLDefaultStyleName = @"Emerald";
NSString *const MGLStyleVersion = @"7";
NSString *const MGLDefaultStyleMarkerSymbolName = @"default_marker";
NSString *const MGLMapboxAccessTokenManagerURLDisplayString = @"mapbox.com/account/apps";

const NSTimeInterval MGLAnimationDuration = 0.3;
const CGSize MGLAnnotationUpdateViewportOutset = {150, 150};
const CGFloat MGLMinimumZoom = 3;

NSString *const MGLAnnotationIDKey = @"MGLAnnotationIDKey";

static NSURL *MGLURLForBundledStyleNamed(NSString *styleName)
{
    return [NSURL URLWithString:[NSString stringWithFormat:@"asset://styles/%@.json", styleName]];
}

#pragma mark - Private -

@interface MGLMapView () <UIGestureRecognizerDelegate, GLKViewDelegate, CLLocationManagerDelegate>

@property (nonatomic) EAGLContext *context;
@property (nonatomic) GLKView *glView;
@property (nonatomic) NSOperationQueue *regionChangeDelegateQueue;
@property (nonatomic) UIImageView *compass;
@property (nonatomic) UIImageView *logoBug;
@property (nonatomic) UIButton *attributionButton;
@property (nonatomic) UIPanGestureRecognizer *pan;
@property (nonatomic) UIPinchGestureRecognizer *pinch;
@property (nonatomic) UIRotationGestureRecognizer *rotate;
@property (nonatomic) UILongPressGestureRecognizer *quickZoom;
@property (nonatomic) NSMutableArray *bundledStyleURLs;
@property (nonatomic) NSMapTable *annotationIDsByAnnotation;
@property (nonatomic) std::vector<uint32_t> annotationsNearbyLastTap;
@property (nonatomic, weak) id <MGLAnnotation> selectedAnnotation;
@property (nonatomic) SMCalloutView *selectedAnnotationCalloutView;
@property (nonatomic) MGLUserLocationAnnotationView *userLocationAnnotationView;
@property (nonatomic) CLLocationManager *locationManager;
@property (nonatomic) CGPoint centerPoint;
@property (nonatomic) CGFloat scale;
@property (nonatomic) CGFloat angle;
@property (nonatomic) CGFloat quickZoomStart;
@property (nonatomic, getter=isAnimatingGesture) BOOL animatingGesture;
@property (nonatomic, readonly, getter=isRotationAllowed) BOOL rotationAllowed;

@end

@implementation MGLMapView
{
    BOOL _isTargetingInterfaceBuilder;
    CLLocationDegrees _pendingLatitude;
    CLLocationDegrees _pendingLongitude;
}

#pragma mark - Setup & Teardown -

@dynamic debugActive;

class MBGLView;

std::chrono::steady_clock::duration secondsAsDuration(float duration)
{
    return std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<float, std::chrono::seconds::period>(duration));
}

mbgl::Map *mbglMap = nullptr;
MBGLView *mbglView = nullptr;
mbgl::SQLiteCache *mbglFileCache = nullptr;
mbgl::DefaultFileSource *mbglFileSource = nullptr;

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];

    if (self && [self commonInit])
    {
        self.styleURL = nil;
        return self;
    }

    return nil;
}

- (instancetype)initWithFrame:(CGRect)frame accessToken:(NSString *)accessToken
{
    return [self initWithFrame:frame accessToken:accessToken styleURL:nil];
}

- (instancetype)initWithFrame:(CGRect)frame accessToken:(NSString *)accessToken styleURL:(NSURL *)styleURL
{
    self = [super initWithFrame:frame];

    if (self && [self commonInit])
    {
        self.accessToken = accessToken;
        self.styleURL = styleURL;
    }

    return self;
}

- (instancetype)initWithCoder:(NSCoder *)decoder
{
    self = [super initWithCoder:decoder];

    if (self && [self commonInit])
    {
        self.styleURL = nil;
        return self;
    }

    return nil;
}

- (NSString *)accessToken
{
    return @(mbglMap->getAccessToken().c_str()).mgl_stringOrNilIfEmpty;
}

- (void)setAccessToken:(NSString *)accessToken
{
    mbglMap->setAccessToken((std::string)[accessToken UTF8String]);
    [MGLMapboxEvents setToken:accessToken.mgl_stringOrNilIfEmpty];
}

+ (NSSet *)keyPathsForValuesAffectingStyleURL
{
    return [NSSet setWithObjects:@"mapID", @"accessToken", nil];
}

- (NSURL *)styleURL
{
    NSString *styleURLString = @(mbglMap->getStyleURL().c_str()).mgl_stringOrNilIfEmpty;
    return styleURLString ? [NSURL URLWithString:styleURLString] : nil;
}

- (void)setStyleURL:(NSURL *)styleURL
{
    if (_isTargetingInterfaceBuilder) return;
    
    if ( ! styleURL)
    {
        styleURL = MGLURLForBundledStyleNamed([NSString stringWithFormat:@"%@-v%@",
                                               MGLDefaultStyleName.lowercaseString,
                                               MGLStyleVersion]);
    }
    
    if ( ! [styleURL scheme])
    {
        // Assume a relative path into the developer’s bundle.
        styleURL = [[NSBundle mainBundle] URLForResource:styleURL.path withExtension:nil];
    }
    
    mbglMap->setStyleURL([[styleURL absoluteString] UTF8String]);
}

- (BOOL)commonInit
{
    _isTargetingInterfaceBuilder = NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent;
    
    // create context
    //
    _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if ( ! _context)
    {
        mbgl::Log::Error(mbgl::Event::Setup, "failed to create OpenGL ES context");

        return NO;
    }

    // setup accessibility
    //
    self.accessibilityLabel = @"Map";
    
    // metrics: initial setup
    NSString *appName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"];
    NSString *appVersion = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
    if (appName != nil) [MGLMapboxEvents setAppName:appName];
    if (appVersion != nil) [MGLMapboxEvents setAppVersion:appVersion];

    // create GL view
    //
    _glView = [[GLKView alloc] initWithFrame:self.bounds context:_context];
    _glView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    _glView.enableSetNeedsDisplay = YES;
    _glView.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    _glView.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    if ([UIScreen instancesRespondToSelector:@selector(nativeScale)]) {
        _glView.contentScaleFactor = [[UIScreen mainScreen] nativeScale];
    }
    _glView.delegate = self;
    [_glView bindDrawable];
    [self insertSubview:_glView atIndex:0];

    _glView.contentMode = UIViewContentModeCenter;
    [self setBackgroundColor:[UIColor clearColor]];

    // load extensions
    //
    dispatch_once(&loadGLExtensions, ^
    {
        const std::string extensions = (char *)glGetString(GL_EXTENSIONS);

        using namespace mbgl;

        if (extensions.find("GL_OES_vertex_array_object") != std::string::npos) {
            gl::BindVertexArray = glBindVertexArrayOES;
            gl::DeleteVertexArrays = glDeleteVertexArraysOES;
            gl::GenVertexArrays = glGenVertexArraysOES;
            gl::IsVertexArray = glIsVertexArrayOES;
        }

        if (extensions.find("GL_OES_packed_depth_stencil") != std::string::npos) {
            gl::isPackedDepthStencilSupported = YES;
        }

        if (extensions.find("GL_OES_depth24") != std::string::npos) {
            gl::isDepth24Supported = YES;
        }
    });

    // setup mbgl map
    //
    mbglView = new MBGLView(self);
    mbglFileCache  = new mbgl::SQLiteCache(defaultCacheDatabase());
    mbglFileSource = new mbgl::DefaultFileSource(mbglFileCache);
    mbglMap = new mbgl::Map(*mbglView, *mbglFileSource);
    mbglView->resize(self.bounds.size.width, self.bounds.size.height, _glView.contentScaleFactor, _glView.drawableWidth, _glView.drawableHeight);

    // Notify map object when network reachability status changes.
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(reachabilityChanged:)
                                                 name:kReachabilityChangedNotification
                                               object:nil];

    Reachability* reachability = [Reachability reachabilityForInternetConnection];
    [reachability startNotifier];

    // setup annotations
    //
    _annotationIDsByAnnotation = [NSMapTable mapTableWithKeyOptions:NSMapTableStrongMemory valueOptions:NSMapTableStrongMemory];
    std::string defaultSymbolName([MGLDefaultStyleMarkerSymbolName UTF8String]);
    mbglMap->setDefaultPointAnnotationSymbol(defaultSymbolName);

    // setup logo bug
    //
    _logoBug = [[UIImageView alloc] initWithImage:[MGLMapView resourceImageNamed:@"mapbox.png"]];
    _logoBug.accessibilityLabel = @"Mapbox logo";
    _logoBug.frame = CGRectMake(8, self.bounds.size.height - _logoBug.bounds.size.height - 4, _logoBug.bounds.size.width, _logoBug.bounds.size.height);
    _logoBug.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_logoBug];

    // setup attribution
    //
    _attributionButton = [UIButton buttonWithType:UIButtonTypeInfoLight];
    _attributionButton.accessibilityLabel = @"Attribution info";
    [_attributionButton addTarget:self action:@selector(showAttribution:) forControlEvents:UIControlEventTouchUpInside];
    _attributionButton.frame = CGRectMake(self.bounds.size.width - _attributionButton.bounds.size.width - 8, self.bounds.size.height - _attributionButton.bounds.size.height - 8, _attributionButton.bounds.size.width, _attributionButton.bounds.size.height);
    _attributionButton.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_attributionButton];

    // setup compass
    //
    _compass = [[UIImageView alloc] initWithImage:[MGLMapView resourceImageNamed:@"Compass.png"]];
    _compass.accessibilityLabel = @"Compass";
    UIImage *compassImage = [MGLMapView resourceImageNamed:@"Compass.png"];
    _compass.frame = CGRectMake(0, 0, compassImage.size.width, compassImage.size.height);
    _compass.alpha = 0;
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(self.bounds.size.width - compassImage.size.width - 5, 5, compassImage.size.width, compassImage.size.height)];
    [container addSubview:_compass];
    container.translatesAutoresizingMaskIntoConstraints = NO;
    [container addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleCompassTapGesture:)]];
    [self addSubview:container];

    self.viewControllerForLayoutGuides = nil;

    // setup interaction
    //
    _pan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
    _pan.delegate = self;
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

    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
    {
        _quickZoom = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleQuickZoomGesture:)];
        _quickZoom.numberOfTapsRequired = 1;
        _quickZoom.minimumPressDuration = 0.25;
        [self addGestureRecognizer:_quickZoom];
    }

    // observe app activity
    //
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appDidBackground:) name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillForeground:) name:UIApplicationWillEnterForegroundNotification object:nil];

    // set initial position
    //
    mbglMap->setLatLngZoom(mbgl::LatLng(0, 0), mbglMap->getMinZoom());
    _pendingLatitude = NAN;
    _pendingLongitude = NAN;

    // setup change delegate queue
    //
    _regionChangeDelegateQueue = [NSOperationQueue new];
    _regionChangeDelegateQueue.maxConcurrentOperationCount = 1;

    // start the main loop, but not on the IB canvas
    if ( ! _isTargetingInterfaceBuilder)
    {
        mbglMap->start();
    }

    // metrics: map load event
    const mbgl::LatLng latLng = mbglMap->getLatLng();
    const double zoom = mbglMap->getZoom();

    [MGLMapboxEvents pushEvent:MGLEventTypeMapLoad withAttributes:@{
        MGLEventKeyLatitude: @(latLng.latitude),
        MGLEventKeyLongitude: @(latLng.longitude),
        MGLEventKeyZoomLevel: @(zoom),
        MGLEventKeyPushEnabled: @([MGLMapboxEvents checkPushEnabled])
    }];

    return YES;
}

-(void)reachabilityChanged:(NSNotification*)notification
{
    Reachability *reachability = [notification object];
    if ([reachability isReachable]) {
        mbgl::NetworkStatus::Reachable();
    }
}

- (void)dealloc
{
    [_regionChangeDelegateQueue cancelAllOperations];

    [[NSNotificationCenter defaultCenter] removeObserver:self];

    if (mbglMap)
    {
        delete mbglMap;
        mbglMap = nullptr;
    }

    if (mbglFileSource)
    {
        delete mbglFileSource;
        mbglFileSource = nullptr;
    }

    if (mbglView)
    {
        delete mbglView;
        mbglView = nullptr;
    }

    if ([[EAGLContext currentContext] isEqual:_context])
    {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)setDelegate:(id<MGLMapViewDelegate>)delegate
{
    if (_delegate == delegate) return;
    
    _delegate = delegate;
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

- (void)didMoveToSuperview
{
    [self.compass.superview removeConstraints:self.compass.superview.constraints];
    [self.logoBug removeConstraints:self.logoBug.constraints];
    [self.attributionButton removeConstraints:self.attributionButton.constraints];

    [self setNeedsUpdateConstraints];
}

- (void)setViewControllerForLayoutGuides:(UIViewController *)viewController
{
    _viewControllerForLayoutGuides = viewController;

    [self.compass.superview removeConstraints:self.compass.superview.constraints];
    [self.logoBug removeConstraints:self.logoBug.constraints];
    [self.attributionButton removeConstraints:self.attributionButton.constraints];

    [self setNeedsUpdateConstraints];
}

- (void)updateConstraints
{
    // If we have a view controller reference, use its layout guides for our various top & bottom
    // views so they don't underlap navigation or tool bars. If we don't have a reference, apply
    // constraints against ourself to maintain (albeit less ideal) placement of the subviews.
    //
    NSString *topGuideFormatString    = (self.viewControllerForLayoutGuides ? @"[topLayoutGuide]"    : @"|");
    NSString *bottomGuideFormatString = (self.viewControllerForLayoutGuides ? @"[bottomLayoutGuide]" : @"|");

    id topGuideViewsObject            = (self.viewControllerForLayoutGuides ? (id)self.viewControllerForLayoutGuides.topLayoutGuide    : (id)@"");
    id bottomGuideViewsObject         = (self.viewControllerForLayoutGuides ? (id)self.viewControllerForLayoutGuides.bottomLayoutGuide : (id)@"");

    UIView *constraintParentView = (self.viewControllerForLayoutGuides.view ? self.viewControllerForLayoutGuides.view : self);

    // compass
    //
    UIView *compassContainer = self.compass.superview;

    [constraintParentView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:[NSString stringWithFormat:@"V:%@-topSpacing-[container]", topGuideFormatString]
                                                                                 options:0
                                                                                 metrics:@{ @"topSpacing"     : @(5) }
                                                                                   views:@{ @"topLayoutGuide" : topGuideViewsObject,
                                                                                            @"container"      : compassContainer }]];

    [constraintParentView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:[container]-rightSpacing-|"
                                                                                 options:0
                                                                                 metrics:@{ @"rightSpacing" : @(5) }
                                                                                   views:@{ @"container"    : compassContainer }]];

    [compassContainer addConstraint:[NSLayoutConstraint constraintWithItem:compassContainer
                                                                 attribute:NSLayoutAttributeWidth
                                                                 relatedBy:NSLayoutRelationEqual
                                                                    toItem:nil
                                                                 attribute:NSLayoutAttributeNotAnAttribute
                                                                multiplier:1
                                                                  constant:self.compass.image.size.width]];

    [compassContainer addConstraint:[NSLayoutConstraint constraintWithItem:compassContainer
                                                                 attribute:NSLayoutAttributeHeight
                                                                 relatedBy:NSLayoutRelationEqual
                                                                    toItem:nil
                                                                 attribute:NSLayoutAttributeNotAnAttribute
                                                                multiplier:1
                                                                  constant:self.compass.image.size.height]];

    // logo bug
    //
    [constraintParentView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:[NSString stringWithFormat:@"V:[logoBug]-bottomSpacing-%@", bottomGuideFormatString]
                                                                                 options:0
                                                                                 metrics:@{ @"bottomSpacing"     : @(4) }
                                                                                   views:@{ @"logoBug"           : self.logoBug,
                                                                                            @"bottomLayoutGuide" : bottomGuideViewsObject }]];

    [constraintParentView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|-leftSpacing-[logoBug]"
                                                                                 options:0
                                                                                 metrics:@{ @"leftSpacing"       : @(8) }
                                                                                   views:@{ @"logoBug"           : self.logoBug }]];

    // attribution button
    //
    [constraintParentView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:[NSString stringWithFormat:@"V:[attributionButton]-bottomSpacing-%@", bottomGuideFormatString]
                                                                                 options:0
                                                                                 metrics:@{ @"bottomSpacing"     : @(8) }
                                                                                   views:@{ @"attributionButton" : self.attributionButton,
                                                                                            @"bottomLayoutGuide" : bottomGuideViewsObject }]];

    [constraintParentView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:[attributionButton]-rightSpacing-|"
                                                                                 options:0
                                                                                 metrics:@{ @"rightSpacing"      : @(8) }
                                                                                   views:@{ @"attributionButton" : self.attributionButton }]];

    [super updateConstraints];
}

// This is the delegate of the GLKView object's display call.
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    mbglView->resize(rect.size.width, rect.size.height, view.contentScaleFactor, view.drawableWidth, view.drawableHeight);
    mbglMap->renderSync();
}

// This gets called when the view dimension changes, e.g. because the device is being rotated.
- (void)layoutSubviews
{
    [super layoutSubviews];
    
    if ( ! _isTargetingInterfaceBuilder)
    {
        mbglMap->triggerUpdate();
    }
}

#pragma mark - Life Cycle -

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

- (void)appDidBackground:(NSNotification *)notification
{
    [MGLMapboxEvents flush];
    
    mbglMap->stop();

    [self.glView deleteDrawable];
}

- (void)appWillForeground:(NSNotification *)notification
{
    [self.glView bindDrawable];

    mbglMap->start();
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

- (void)handleCompassTapGesture:(id)sender
{
    [self resetNorthAnimated:YES];

    if (self.userTrackingMode == MGLUserTrackingModeFollowWithHeading) self.userTrackingMode = MGLUserTrackingModeFollow;
}

#pragma clang diagnostic pop

- (void)handlePanGesture:(UIPanGestureRecognizer *)pan
{
    if ( ! self.isScrollEnabled) return;

    mbglMap->cancelTransitions();

    if (pan.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGesturePanStart forRecognizer:pan];

        mbglMap->setGestureInProgress(true);

        self.centerPoint = CGPointMake(0, 0);

        self.userTrackingMode = MGLUserTrackingModeNone;
    }
    else if (pan.state == UIGestureRecognizerStateChanged)
    {
        CGPoint delta = CGPointMake([pan translationInView:pan.view].x - self.centerPoint.x,
                                    [pan translationInView:pan.view].y - self.centerPoint.y);

        mbglMap->moveBy(delta.x, delta.y);

        self.centerPoint = CGPointMake(self.centerPoint.x + delta.x, self.centerPoint.y + delta.y);
        
        [self notifyMapChange:@(mbgl::MapChangeRegionDidChangeAnimated)];
    }
    else if (pan.state == UIGestureRecognizerStateEnded || pan.state == UIGestureRecognizerStateCancelled)
    {
        CGPoint velocity = [pan velocityInView:pan.view];
        CGFloat duration = 0;

        if ( ! CGPointEqualToPoint(velocity, CGPointZero))
        {
            CGFloat ease = 0.25;

            velocity.x = velocity.x * ease;
            velocity.y = velocity.y * ease;

            CGFloat speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            CGFloat deceleration = 2500;
            duration = speed / (deceleration * ease);
        }

        CGPoint offset = CGPointMake(velocity.x * duration / 2, velocity.y * duration / 2);

        mbglMap->moveBy(offset.x, offset.y, secondsAsDuration(duration));

        mbglMap->setGestureInProgress(false);

        if (duration)
        {
            self.animatingGesture = YES;

            __weak MGLMapView *weakSelf = self;

            [self animateWithDelay:duration animations:^
            {
                weakSelf.animatingGesture = NO;

                [weakSelf notifyMapChange:@(mbgl::MapChangeRegionDidChangeAnimated)];
            }];
        }
        else
        {
            [self notifyMapChange:@(mbgl::MapChangeRegionDidChange)];
        }

        // metrics: pan end
        CGPoint pointInView = CGPointMake([pan locationInView:pan.view].x, [pan locationInView:pan.view].y);
        CLLocationCoordinate2D panCoordinate = [self convertPoint:pointInView toCoordinateFromView:pan.view];
        double zoom = [self zoomLevel];

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

    if (mbglMap->getZoom() <= mbglMap->getMinZoom() && pinch.scale < 1) return;

    mbglMap->cancelTransitions();

    if (pinch.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGesturePinchStart forRecognizer:pinch];

        mbglMap->setGestureInProgress(true);

        self.scale = mbglMap->getScale();

        self.userTrackingMode = MGLUserTrackingModeNone;
    }
    else if (pinch.state == UIGestureRecognizerStateChanged)
    {
        CGFloat newScale = self.scale * pinch.scale;

        if (log2(newScale) < mbglMap->getMinZoom()) return;

        double scale = mbglMap->getScale();

        mbglMap->scaleBy(newScale / scale, [pinch locationInView:pinch.view].x, [pinch locationInView:pinch.view].y);
    }
    else if (pinch.state == UIGestureRecognizerStateEnded || pinch.state == UIGestureRecognizerStateCancelled)
    {
        mbglMap->setGestureInProgress(false);

        [self unrotateIfNeededAnimated:YES];

        [self notifyMapChange:@(mbgl::MapChangeRegionDidChange)];
    }
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    if ( ! self.isRotateEnabled) return;

    mbglMap->cancelTransitions();

    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGestureRotateStart forRecognizer:rotate];

        mbglMap->setGestureInProgress(true);

        self.angle = [MGLMapView degreesToRadians:mbglMap->getBearing()] * -1;

        self.userTrackingMode = MGLUserTrackingModeNone;
    }
    else if (rotate.state == UIGestureRecognizerStateChanged)
    {
        CGFloat newDegrees = [MGLMapView radiansToDegrees:(self.angle + rotate.rotation)] * -1;

        // constrain to +/-30 degrees when merely rotating like Apple does
        //
        if ( ! self.isRotationAllowed && std::abs(self.pinch.scale) < 10)
        {
            newDegrees = fminf(newDegrees,  30);
            newDegrees = fmaxf(newDegrees, -30);
        }

        mbglMap->setBearing(newDegrees,
                            [rotate locationInView:rotate.view].x,
                            [rotate locationInView:rotate.view].y);
    }
    else if (rotate.state == UIGestureRecognizerStateEnded || rotate.state == UIGestureRecognizerStateCancelled)
    {
        mbglMap->setGestureInProgress(false);

        [self unrotateIfNeededAnimated:YES];

        [self notifyMapChange:@(mbgl::MapChangeRegionDidChange)];
    }
}

- (void)handleSingleTapGesture:(UITapGestureRecognizer *)singleTap
{
    if (singleTap.state == UIGestureRecognizerStateEnded)
    {
        [self trackGestureEvent:MGLEventGestureSingleTap forRecognizer:singleTap];

        CGPoint tapPoint = [singleTap locationInView:self];

        if (self.userLocationVisible && ! [self.selectedAnnotation isEqual:self.userLocation])
        {
            CGRect userLocationRect = CGRectMake(tapPoint.x - 15, tapPoint.y - 15, 30, 30);

            if (CGRectContainsPoint(userLocationRect, [self convertCoordinate:self.userLocation.coordinate toPointToView:self]))
            {
                [self selectAnnotation:self.userLocation animated:YES];
                return;
            }
        }

        // tolerances based on touch size & typical marker aspect ratio
        CGFloat toleranceWidth  = 40;
        CGFloat toleranceHeight = 60;

        // setup a recognition area weighted 2/3 of the way above the point to account for average marker imagery
        CGRect tapRect = CGRectMake(tapPoint.x - toleranceWidth / 2, tapPoint.y - 2 * toleranceHeight / 3, toleranceWidth, toleranceHeight);
        CGPoint tapRectLowerLeft  = CGPointMake(tapRect.origin.x, tapRect.origin.y + tapRect.size.height);
        CGPoint tapRectUpperLeft  = CGPointMake(tapRect.origin.x, tapRect.origin.y);
        CGPoint tapRectUpperRight = CGPointMake(tapRect.origin.x + tapRect.size.width, tapRect.origin.y);
        CGPoint tapRectLowerRight = CGPointMake(tapRect.origin.x + tapRect.size.width, tapRect.origin.y + tapRect.size.height);

        // figure out what that means in coordinate space
        CLLocationCoordinate2D coordinate;
        mbgl::LatLngBounds tapBounds;

        coordinate = [self convertPoint:tapRectLowerLeft  toCoordinateFromView:self];
        tapBounds.extend(coordinateToLatLng(coordinate));

        coordinate = [self convertPoint:tapRectUpperLeft  toCoordinateFromView:self];
        tapBounds.extend(coordinateToLatLng(coordinate));

        coordinate = [self convertPoint:tapRectUpperRight toCoordinateFromView:self];
        tapBounds.extend(coordinateToLatLng(coordinate));

        coordinate = [self convertPoint:tapRectLowerRight toCoordinateFromView:self];
        tapBounds.extend(coordinateToLatLng(coordinate));

        // query for nearby annotations
        std::vector<uint32_t> nearbyAnnotations = mbglMap->getAnnotationsInBounds(tapBounds);

        int32_t newSelectedAnnotationID = -1;

        if (nearbyAnnotations.size())
        {
            // there is at least one nearby annotation; select one
            //
            // first, sort for comparison and iteration
            std::sort(nearbyAnnotations.begin(), nearbyAnnotations.end());

            if (nearbyAnnotations == self.annotationsNearbyLastTap)
            {
                // the selection candidates haven't changed; cycle through them
                if (self.selectedAnnotation &&
                    [[[self.annotationIDsByAnnotation objectForKey:self.selectedAnnotation]
                        objectForKey:MGLAnnotationIDKey] unsignedIntValue] == self.annotationsNearbyLastTap.back())
                {
                    // the selected annotation is the last in the set; cycle back to the first
                    // note: this could be the selected annotation if only one in set
                    newSelectedAnnotationID = self.annotationsNearbyLastTap.front();
                }
                else if (self.selectedAnnotation)
                {
                    // otherwise increment the selection through the candidates
                    uint32_t currentID = [[[self.annotationIDsByAnnotation objectForKey:self.selectedAnnotation] objectForKey:MGLAnnotationIDKey] unsignedIntValue];
                    auto result = std::find(self.annotationsNearbyLastTap.begin(), self.annotationsNearbyLastTap.end(), currentID);
                    auto distance = std::distance(self.annotationsNearbyLastTap.begin(), result);
                    newSelectedAnnotationID = self.annotationsNearbyLastTap[distance + 1];
                }
                else
                {
                    // no current selection; select the first one
                    newSelectedAnnotationID = self.annotationsNearbyLastTap.front();
                }
            }
            else
            {
                // start tracking a new set of nearby annotations
                self.annotationsNearbyLastTap = nearbyAnnotations;

                // select the first one
                newSelectedAnnotationID = self.annotationsNearbyLastTap.front();
            }
        }
        else
        {
            // there are no nearby annotations; deselect if necessary
            newSelectedAnnotationID = -1;
        }

        if (newSelectedAnnotationID >= 0)
        {
            // find & select model object for selection
            NSEnumerator *enumerator = self.annotationIDsByAnnotation.keyEnumerator;

            while (id <MGLAnnotation> annotation = enumerator.nextObject)
            {
                if ([[[self.annotationIDsByAnnotation objectForKey:annotation] objectForKey:MGLAnnotationIDKey] integerValue] == newSelectedAnnotationID)
                {
                    // only change selection status if not the currently selected annotation
                    if ( ! [annotation isEqual:self.selectedAnnotation])
                    {
                        [self selectAnnotation:annotation animated:YES];
                    }

                    // either way, we should stop enumerating
                    break;
                }
            }
        }
        else
        {
            // deselect any selected annotation
            if (self.selectedAnnotation) [self deselectAnnotation:self.selectedAnnotation animated:YES];
        }
    }
}

- (void)handleDoubleTapGesture:(UITapGestureRecognizer *)doubleTap
{
    if ( ! self.isZoomEnabled) return;

    mbglMap->cancelTransitions();

    if (doubleTap.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGestureDoubleTap forRecognizer:doubleTap];
    }
    else if (doubleTap.state == UIGestureRecognizerStateEnded)
    {
        CGPoint doubleTapPoint = [doubleTap locationInView:doubleTap.view];

        CGPoint zoomInPoint;

        if (self.userTrackingMode != MGLUserTrackingModeNone)
        {
            CGPoint userPoint = [self convertCoordinate:self.userLocation.coordinate toPointToView:self];
            CGRect userLocationRect = CGRectMake(userPoint.x - 40, userPoint.y - 40, 80, 80);
            if (CGRectContainsPoint(userLocationRect, doubleTapPoint))
            {
                zoomInPoint = userPoint;
            }
        }
        else
        {
            self.userTrackingMode = MGLUserTrackingModeNone;

            zoomInPoint = doubleTapPoint;
        }

        mbglMap->scaleBy(2, zoomInPoint.x, zoomInPoint.y, secondsAsDuration(MGLAnimationDuration));

        self.animatingGesture = YES;

        __weak MGLMapView *weakSelf = self;

        [self animateWithDelay:MGLAnimationDuration animations:^
        {
            weakSelf.animatingGesture = NO;

            [weakSelf unrotateIfNeededAnimated:YES];

            [weakSelf notifyMapChange:@(mbgl::MapChangeRegionDidChangeAnimated)];
        }];
    }
}

- (void)handleTwoFingerTapGesture:(UITapGestureRecognizer *)twoFingerTap
{
    if ( ! self.isZoomEnabled) return;

    if (mbglMap->getZoom() == mbglMap->getMinZoom()) return;

    mbglMap->cancelTransitions();

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

        mbglMap->scaleBy(0.5, zoomOutPoint.x, zoomOutPoint.y, secondsAsDuration(MGLAnimationDuration));

        self.animatingGesture = YES;

        __weak MGLMapView *weakSelf = self;

        [self animateWithDelay:MGLAnimationDuration animations:^
        {
            weakSelf.animatingGesture = NO;

            [weakSelf unrotateIfNeededAnimated:YES];

            [weakSelf notifyMapChange:@(mbgl::MapChangeRegionDidChangeAnimated)];
        }];
    }
}

- (void)handleQuickZoomGesture:(UILongPressGestureRecognizer *)quickZoom
{
    if ( ! self.isZoomEnabled) return;

    mbglMap->cancelTransitions();

    if (quickZoom.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGestureQuickZoom forRecognizer:quickZoom];

        self.scale = mbglMap->getScale();

        self.quickZoomStart = [quickZoom locationInView:quickZoom.view].y;

        self.userTrackingMode = MGLUserTrackingModeNone;
    }
    else if (quickZoom.state == UIGestureRecognizerStateChanged)
    {
        CGFloat distance = self.quickZoomStart - [quickZoom locationInView:quickZoom.view].y;

        CGFloat newZoom = log2f(self.scale) + (distance / 100);

        if (newZoom < mbglMap->getMinZoom()) return;

        mbglMap->scaleBy(powf(2, newZoom) / mbglMap->getScale(), self.bounds.size.width / 2, self.bounds.size.height / 2);
    }
    else if (quickZoom.state == UIGestureRecognizerStateEnded || quickZoom.state == UIGestureRecognizerStateCancelled)
    {
        [self unrotateIfNeededAnimated:YES];

        [self notifyMapChange:@(mbgl::MapChangeRegionDidChange)];
    }
}

- (void)handleCalloutAccessoryTapGesture:(UITapGestureRecognizer *)tap
{
    if ([self.delegate respondsToSelector:@selector(mapView:annotation:calloutAccessoryControlTapped:)])
    {
        [self.delegate mapView:self annotation:self.selectedAnnotation
            calloutAccessoryControlTapped:(UIControl *)tap.view];
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
    double zoom = [self zoomLevel];

    [MGLMapboxEvents pushEvent:MGLEventTypeMapTap withAttributes:@{
        MGLEventKeyLatitude: @(gestureCoordinate.latitude),
        MGLEventKeyLongitude: @(gestureCoordinate.longitude),
        MGLEventKeyZoomLevel: @(zoom),
        MGLEventKeyGestureID: gestureID
    }];
}

#pragma mark - Properties -

+ (NSSet *)keyPathsForValuesAffectingZoomEnabled
{
    return [NSSet setWithObject:@"allowsZooming"];
}

+ (NSSet *)keyPathsForValuesAffectingScrollEnabled
{
    return [NSSet setWithObject:@"allowsScrolling"];
}

+ (NSSet *)keyPathsForValuesAffectingRotateEnabled
{
    return [NSSet setWithObject:@"allowsRotating"];
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

- (void)showAttribution:(id)sender
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://www.mapbox.com/about/maps/"]];
}

#pragma clang diagnostic pop

- (void)setDebugActive:(BOOL)debugActive
{
    mbglMap->setDebug(debugActive);
}

- (BOOL)isDebugActive
{
    return mbglMap->getDebug();
}

- (void)resetNorth
{
    [self resetNorthAnimated:YES];
}

- (void)resetNorthAnimated:(BOOL)animated
{
    self.userTrackingMode = MGLUserTrackingModeNone;

    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    mbglMap->setBearing(0, secondsAsDuration(duration));

    [UIView animateWithDuration:duration
                     animations:^
                     {
                         self.compass.transform = CGAffineTransformIdentity;
                     }
                     completion:^(BOOL finished)
                     {
                         if (finished)
                         {
                             [self notifyMapChange:@(animated ? mbgl::MapChangeRegionDidChangeAnimated : mbgl::MapChangeRegionDidChange)];

                             [UIView animateWithDuration:MGLAnimationDuration
                                              animations:^
                                              {
                                                  self.compass.alpha = 0;
                                              }];
                         }
                     }];
}

- (void)resetPosition
{
    mbglMap->resetPosition();

    [self notifyMapChange:@(mbgl::MapChangeRegionDidChange)];
}

- (void)toggleDebug
{
    mbglMap->toggleDebug();
}

#pragma mark - Geography -

+ (NSSet *)keyPathsForValuesAffectingCenterCoordinate
{
    return [NSSet setWithObjects:@"latitude", @"longitude", nil];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)coordinate animated:(BOOL)animated preservingTracking:(BOOL)tracking
{
    self.userTrackingMode = (tracking ? self.userTrackingMode : MGLUserTrackingModeNone);

    [self setCenterCoordinate:coordinate animated:animated];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)coordinate animated:(BOOL)animated
{
    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    mbglMap->setLatLngZoom(coordinateToLatLng(coordinate),
                           fmaxf(mbglMap->getZoom(), self.currentMinimumZoom),
                           secondsAsDuration(duration));

    [self notifyMapChange:@(animated ? mbgl::MapChangeRegionDidChangeAnimated : mbgl::MapChangeRegionDidChange)];
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
{
    [self setCenterCoordinate:centerCoordinate animated:NO];
}

- (CLLocationCoordinate2D)centerCoordinate
{
    return latLngToCoordinate(mbglMap->getLatLng());
}

- (void)setCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate zoomLevel:(double)zoomLevel animated:(BOOL)animated
{
    self.userTrackingMode = MGLUserTrackingModeNone;

    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    mbglMap->setLatLngZoom(coordinateToLatLng(centerCoordinate), zoomLevel, secondsAsDuration(duration));

    [self unrotateIfNeededAnimated:animated];

    [self notifyMapChange:@(animated ? mbgl::MapChangeRegionDidChangeAnimated : mbgl::MapChangeRegionDidChange)];
}

- (double)zoomLevel
{
    return mbglMap->getZoom();
}

- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated
{
    self.userTrackingMode = MGLUserTrackingModeNone;

    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    mbglMap->setLatLngZoom(mbglMap->getLatLng(),
                           fmaxf(zoomLevel, self.currentMinimumZoom),
                           secondsAsDuration(duration));

    [self unrotateIfNeededAnimated:animated];

    [self notifyMapChange:@(animated ? mbgl::MapChangeRegionDidChangeAnimated : mbgl::MapChangeRegionDidChange)];
}

- (void)setZoomLevel:(double)zoomLevel
{
    [self setZoomLevel:zoomLevel animated:NO];
}

- (void)zoomToSouthWestCoordinate:(CLLocationCoordinate2D)southWestCoordinate northEastCoordinate:(CLLocationCoordinate2D)northEastCoordinate animated:(BOOL)animated
{
    // NOTE: does not disrupt tracking mode

    CLLocationCoordinate2D center = CLLocationCoordinate2DMake((northEastCoordinate.latitude + southWestCoordinate.latitude) / 2, (northEastCoordinate.longitude + southWestCoordinate.longitude) / 2);
    
    CGFloat scale = mbglMap->getScale();
    CGFloat scaleX = mbglMap->getState().getWidth() / (northEastCoordinate.longitude - southWestCoordinate.longitude);
    CGFloat scaleY = mbglMap->getState().getHeight() / (northEastCoordinate.latitude - southWestCoordinate.latitude);
    CGFloat minZoom = mbglMap->getMinZoom();
    CGFloat maxZoom = mbglMap->getMaxZoom();
    CGFloat zoomLevel = MAX(MIN(log(scale * MIN(scaleX, scaleY)) / log(2), maxZoom), minZoom);
    
    [self setCenterCoordinate:center zoomLevel:zoomLevel animated:animated];
}

- (CLLocationDirection)direction
{
    double direction = mbglMap->getBearing() * -1;

    while (direction > 360) direction -= 360;
    while (direction < 0) direction += 360;

    return direction;
}

- (void)setDirection:(CLLocationDirection)direction animated:(BOOL)animated
{
    if ( ! animated && ! self.rotationAllowed) return;

    self.userTrackingMode = MGLUserTrackingModeNone;

    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    mbglMap->setBearing(direction * -1, secondsAsDuration(duration));

    [self notifyMapChange:@(animated ? mbgl::MapChangeRegionDidChangeAnimated : mbgl::MapChangeRegionDidChange)];
}

- (void)setDirection:(CLLocationDirection)direction
{
    [self setDirection:direction animated:NO];
}

- (CLLocationCoordinate2D)convertPoint:(CGPoint)point toCoordinateFromView:(UIView *)view
{
    CGPoint convertedPoint = [self convertPoint:point fromView:view];

    // flip y coordinate for iOS view origin top left
    //
    convertedPoint.y = self.bounds.size.height - convertedPoint.y;

    return latLngToCoordinate(mbglMap->latLngForPixel(mbgl::vec2<double>(convertedPoint.x, convertedPoint.y)));
}

- (CGPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(UIView *)view
{
    mbgl::vec2<double> pixel = mbglMap->pixelForLatLng(coordinateToLatLng(coordinate));

    // flip y coordinate for iOS view origin in top left
    //
    pixel.y = self.bounds.size.height - pixel.y;

    return [self convertPoint:CGPointMake(pixel.x, pixel.y) toView:view];
}

- (CLLocationDistance)metersPerPixelAtLatitude:(CLLocationDegrees)latitude
{
    return mbglMap->getMetersPerPixelAtLatitude(latitude, self.zoomLevel);
}

mbgl::LatLng coordinateToLatLng(CLLocationCoordinate2D coordinate)
{
    return mbgl::LatLng(coordinate.latitude, coordinate.longitude);
}

CLLocationCoordinate2D latLngToCoordinate(mbgl::LatLng latLng)
{
    return CLLocationCoordinate2DMake(latLng.latitude, latLng.longitude);
}

- (mbgl::LatLngBounds)viewportBounds
{
    mbgl::LatLngBounds bounds;

    bounds.extend(coordinateToLatLng(
        [self convertPoint:CGPointMake(0, 0) toCoordinateFromView:self]));
    bounds.extend(coordinateToLatLng(
        [self convertPoint:CGPointMake(self.bounds.size.width, 0) toCoordinateFromView:self]));
    bounds.extend(coordinateToLatLng(
        [self convertPoint:CGPointMake(0, self.bounds.size.height) toCoordinateFromView:self]));
    bounds.extend(coordinateToLatLng(
        [self convertPoint:CGPointMake(self.bounds.size.width, self.bounds.size.height) toCoordinateFromView:self]));

    return bounds;
}

#pragma mark - Styling -

- (NSArray *)bundledStyleURLs
{
    if ( ! _bundledStyleURLs)
    {
        NSString *stylesPath = [[MGLMapView resourceBundlePath] stringByAppendingPathComponent:@"styles"];

        _bundledStyleURLs = [NSMutableArray array];

        NSArray *bundledStyleNamesWithExtensions = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:stylesPath error:nil];
        for (NSString *fileName in bundledStyleNamesWithExtensions)
        {
            [_bundledStyleURLs addObject:MGLURLForBundledStyleNamed([fileName stringByDeletingPathExtension])];
        }
    }

    return [NSArray arrayWithArray:_bundledStyleURLs];
}

+ (NSSet *)keyPathsForValuesAffectingMapID
{
    return [NSSet setWithObjects:@"styleURL", @"accessToken", nil];
}

- (NSString *)mapID
{
    NSURL *styleURL = self.styleURL;
    return [styleURL.scheme isEqualToString:@"mapbox"] ? styleURL.host.mgl_stringOrNilIfEmpty : nil;
}

- (void)setMapID:(NSString *)mapID
{
    if (mapID)
    {
        self.styleURL = [NSURL URLWithString:[NSString stringWithFormat:@"mapbox://%@", mapID]];
    }
    else
    {
        self.styleURL = nil;
    }
}

- (NSArray *)getAppliedStyleClasses
{
    NSMutableArray *returnArray = [NSMutableArray array];

    const std::vector<std::string> &appliedClasses = mbglMap->getClasses();

    for (auto class_it = appliedClasses.begin(); class_it != appliedClasses.end(); class_it++)
    {
        [returnArray addObject:@(class_it->c_str())];
    }

    return returnArray;
}

- (void)setStyleClasses:(NSArray *)appliedClasses
{
    [self setStyleClasses:appliedClasses transitionDuration:0];
}

- (void)setStyleClasses:(NSArray *)appliedClasses transitionDuration:(NSTimeInterval)transitionDuration
{
    std::vector<std::string> newAppliedClasses;

    for (NSString *appliedClass in appliedClasses)
    {
        newAppliedClasses.insert(newAppliedClasses.end(), [appliedClass UTF8String]);
    }

    mbglMap->setDefaultTransitionDuration(secondsAsDuration(transitionDuration));
    mbglMap->setClasses(newAppliedClasses);
}

#pragma mark - Annotations -

- (NSArray *)annotations
{
    if ([_annotationIDsByAnnotation count])
    {
        NSMutableArray *result = [NSMutableArray array];

        NSEnumerator *keyEnumerator = [_annotationIDsByAnnotation keyEnumerator];
        id <MGLAnnotation> annotation;

        while (annotation = [keyEnumerator nextObject])
        {
            [result addObject:annotation];
        }

        return [NSArray arrayWithArray:result];
    }

    return nil;
}

- (void)addAnnotation:(id <MGLAnnotation>)annotation
{
    if ( ! annotation) return;

    // The core bulk add API is efficient with respect to indexing and
    // screen refreshes, thus we should defer to it even for individual adds.
    //
    [self addAnnotations:@[ annotation ]];
}

- (void)addAnnotations:(NSArray *)annotations
{
    if ( ! annotations) return;

    std::vector<mbgl::LatLng> latLngs;
    latLngs.reserve(annotations.count);

    std::vector<std::string> symbols;
    symbols.reserve(annotations.count);

    BOOL delegateImplementsSymbolLookup = [self.delegate respondsToSelector:@selector(mapView:symbolNameForAnnotation:)];

    for (id <MGLAnnotation> annotation in annotations)
    {
        assert([annotation conformsToProtocol:@protocol(MGLAnnotation)]);

        latLngs.push_back(coordinateToLatLng(annotation.coordinate));

        NSString *symbolName = nil;

        if (delegateImplementsSymbolLookup)
        {
            symbolName = [self.delegate mapView:self symbolNameForAnnotation:annotation];
        }

        symbols.push_back((symbolName ? [symbolName UTF8String] : ""));
    }

    std::vector<uint32_t> annotationIDs = mbglMap->addPointAnnotations(latLngs, symbols);

    for (size_t i = 0; i < annotationIDs.size(); ++i)
    {
        [self.annotationIDsByAnnotation setObject:@{ MGLAnnotationIDKey : @(annotationIDs[i]) }
                                           forKey:annotations[i]];
    }
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

- (void)removeAnnotations:(NSArray *)annotations
{
    if ( ! annotations) return;

    std::vector<uint32_t> annotationIDsToRemove;
    annotationIDsToRemove.reserve(annotations.count);

    for (id <MGLAnnotation> annotation in annotations)
    {
        assert([annotation conformsToProtocol:@protocol(MGLAnnotation)]);

        annotationIDsToRemove.push_back([[[self.annotationIDsByAnnotation objectForKey:annotation] 
            objectForKey:MGLAnnotationIDKey] unsignedIntValue]);
        [self.annotationIDsByAnnotation removeObjectForKey:annotation];

        if (annotation == self.selectedAnnotation)
        {
            [self deselectAnnotation:annotation animated:NO];
        }
    }

    mbglMap->removeAnnotations(annotationIDsToRemove);
}

- (NSArray *)selectedAnnotations
{
    return (self.selectedAnnotation ? @[ self.selectedAnnotation ] : @[]);
}

- (void)setSelectedAnnotations:(NSArray *)selectedAnnotations
{
    if ( ! selectedAnnotations.count) return;

    id <MGLAnnotation> firstAnnotation = selectedAnnotations[0];

    assert([firstAnnotation conformsToProtocol:@protocol(MGLAnnotation)]);

    if ( ! [self viewportBounds].contains(coordinateToLatLng(firstAnnotation.coordinate))) return;

    [self selectAnnotation:firstAnnotation animated:NO];
}

- (void)selectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated
{
    if ( ! annotation) return;

    if ( ! [self viewportBounds].contains(coordinateToLatLng(annotation.coordinate))) return;
    
    if (annotation == self.selectedAnnotation) return;

    self.userTrackingMode = MGLUserTrackingModeNone;

    [self deselectAnnotation:self.selectedAnnotation animated:NO];

    self.selectedAnnotation = annotation;

    if (annotation.title && [self.delegate respondsToSelector:@selector(mapView:annotationCanShowCallout:)] &&
        [self.delegate mapView:self annotationCanShowCallout:annotation])
    {
        // build the callout
        self.selectedAnnotationCalloutView = [self calloutViewForAnnotation:annotation];

        CGRect calloutBounds;

        if ([annotation isEqual:self.userLocation])
        {
            CGPoint calloutAnchorPoint = [self convertCoordinate:annotation.coordinate toPointToView:self];
            calloutBounds = CGRectMake(calloutAnchorPoint.x - 1, calloutAnchorPoint.y - 13, 0, 0);
        }
        else
        {
            // determine symbol in use for point
            NSString *symbol = MGLDefaultStyleMarkerSymbolName;
            if ([self.delegate respondsToSelector:@selector(mapView:symbolNameForAnnotation:)])
            {
                symbol = [self.delegate mapView:self symbolNameForAnnotation:annotation];
            }
            std::string symbolName([symbol UTF8String]);

            // determine anchor point based on symbol
            CGPoint calloutAnchorPoint = [self convertCoordinate:annotation.coordinate toPointToView:self];
            double y = mbglMap->getTopOffsetPixelsForAnnotationSymbol(symbolName);
            calloutBounds = CGRectMake(calloutAnchorPoint.x - 1, calloutAnchorPoint.y + y, 0, 0);
        }

        // consult delegate for left and/or right accessory views
        if ([self.delegate respondsToSelector:@selector(mapView:leftCalloutAccessoryViewForAnnotation:)])
        {
            self.selectedAnnotationCalloutView.leftAccessoryView =
                [self.delegate mapView:self leftCalloutAccessoryViewForAnnotation:annotation];

            if ([self.selectedAnnotationCalloutView.leftAccessoryView isKindOfClass:[UIControl class]])
            {
                UITapGestureRecognizer *calloutAccessoryTap = [[UITapGestureRecognizer alloc] initWithTarget:self
                                                                  action:@selector(handleCalloutAccessoryTapGesture:)];

                [self.selectedAnnotationCalloutView.leftAccessoryView addGestureRecognizer:calloutAccessoryTap];
            }
        }

        if ([self.delegate respondsToSelector:@selector(mapView:rightCalloutAccessoryViewForAnnotation:)])
        {
            self.selectedAnnotationCalloutView.rightAccessoryView =
                [self.delegate mapView:self rightCalloutAccessoryViewForAnnotation:annotation];

            if ([self.selectedAnnotationCalloutView.rightAccessoryView isKindOfClass:[UIControl class]])
            {
                UITapGestureRecognizer *calloutAccessoryTap = [[UITapGestureRecognizer alloc] initWithTarget:self
                                                                  action:@selector(handleCalloutAccessoryTapGesture:)];

                [self.selectedAnnotationCalloutView.rightAccessoryView addGestureRecognizer:calloutAccessoryTap];
            }
        }

        // present popup
        [self.selectedAnnotationCalloutView presentCalloutFromRect:calloutBounds
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

- (void)deselectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated
{
    if ( ! annotation) return;

    if ([self.selectedAnnotation isEqual:annotation])
    {
        // dismiss popup
        [self.selectedAnnotationCalloutView dismissCalloutAnimated:animated];

        // clean up
        self.selectedAnnotationCalloutView = nil;
        self.selectedAnnotation = nil;
    }

    // notify delegate
    if ([self.delegate respondsToSelector:@selector(mapView:didDeselectAnnotation:)])
    {
        [self.delegate mapView:self didDeselectAnnotation:annotation];
    }
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
        
        self.locationManager = [CLLocationManager new];
        
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 80000
        // enable iOS 8+ location authorization API
        //
        if ([CLLocationManager instancesRespondToSelector:@selector(requestWhenInUseAuthorization)])
        {
            BOOL hasLocationDescription = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSLocationWhenInUseUsageDescription"] ||
                [[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSLocationAlwaysUsageDescription"];
            NSAssert(hasLocationDescription,
                @"For iOS 8 and above, your app must have a value for NSLocationWhenInUseUsageDescription or NSLocationAlwaysUsageDescription in its Info.plist");
            [self.locationManager requestWhenInUseAuthorization];
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

+ (NSSet *)keyPathsForValuesAffectingUserLocation
{
    return [NSSet setWithObject:@"userLocationAnnotationView"];
}

- (MGLUserLocation *)userLocation
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

    if (mode == MGLUserTrackingModeFollowWithHeading && ! CLLocationCoordinate2DIsValid(self.userLocation.coordinate))
    {
        mode = MGLUserTrackingModeNone;
    }

    _userTrackingMode = mode;

    switch (_userTrackingMode)
    {
        case MGLUserTrackingModeNone:
        default:
        {
            [self.locationManager stopUpdatingHeading];

            break;
        }
        case MGLUserTrackingModeFollow:
        {
            self.showsUserLocation = YES;

            [self.locationManager stopUpdatingHeading];

            if (self.userLocationAnnotationView)
            {
                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Wdeprecated-declarations"
                [self locationManager:self.locationManager didUpdateToLocation:self.userLocation.location fromLocation:self.userLocation.location];
                #pragma clang diagnostic pop
            }

            break;
        }
        case MGLUserTrackingModeFollowWithHeading:
        {
            self.showsUserLocation = YES;

            if (self.zoomLevel < self.currentMinimumZoom) [self setZoomLevel:self.currentMinimumZoom animated:YES];

            if (self.userLocationAnnotationView)
            {
                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Wdeprecated-declarations"
                [self locationManager:self.locationManager didUpdateToLocation:self.userLocation.location fromLocation:self.userLocation.location];
                #pragma clang diagnostic pop
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

- (void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation
{
    (void)manager;

    if ( ! _showsUserLocation || ! newLocation || ! CLLocationCoordinate2DIsValid(newLocation.coordinate)) return;

    if ([newLocation distanceFromLocation:oldLocation] || ! oldLocation)
    {
        self.userLocation.location = newLocation;

        // deselect user if applicable since we don't do callout tracking yet
        if ([self.selectedAnnotation isEqual:self.userLocation]) [self deselectAnnotation:self.userLocation animated:NO];

        if ([self.delegate respondsToSelector:@selector(mapView:didUpdateUserLocation:)])
        {
            [self.delegate mapView:self didUpdateUserLocation:self.userLocation];
        }
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
                [self setCenterCoordinate:self.userLocation.location.coordinate animated:YES preservingTracking:YES];
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
                    [self zoomToSouthWestCoordinate:desiredSouthWest northEastCoordinate:desiredNorthEast animated:YES];
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
    (void)manager;

    if (self.displayHeadingCalibration) [self.locationManager performSelector:@selector(dismissHeadingCalibrationDisplay)
                                                                   withObject:nil
                                                                   afterDelay:10.0];

    return self.displayHeadingCalibration;
}

- (void)locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)newHeading
{
    (void)manager;

    if ( ! _showsUserLocation || self.pan.state == UIGestureRecognizerStateBegan || newHeading.headingAccuracy < 0) return;

    self.userLocation.heading = newHeading;

    if ([self.delegate respondsToSelector:@selector(mapView:didUpdateUserLocation:)])
    {
        [self.delegate mapView:self didUpdateUserLocation:self.userLocation];

        if ( ! _showsUserLocation) return;
    }

    CLLocationDirection headingDirection = (newHeading.trueHeading > 0 ? newHeading.trueHeading : newHeading.magneticHeading);

    if (headingDirection > 0 && self.userTrackingMode == MGLUserTrackingModeFollowWithHeading)
    {
        mbglMap->setBearing(headingDirection, secondsAsDuration(MGLAnimationDuration));
    }
}

- (void)locationManager:(CLLocationManager *)manager didChangeAuthorizationStatus:(CLAuthorizationStatus)status
{
    (void)manager;

    if (status == kCLAuthorizationStatusDenied || status == kCLAuthorizationStatusRestricted)
    {
        self.userTrackingMode  = MGLUserTrackingModeNone;
        self.showsUserLocation = NO;
    }
}

- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error
{
    (void)manager;

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

+ (CGFloat)degreesToRadians:(CGFloat)degrees
{
    return degrees * M_PI / 180;
}

+ (CGFloat)radiansToDegrees:(CGFloat)radians
{
    return radians * 180 / M_PI;
}

- (void)animateWithDelay:(NSTimeInterval)delay animations:(void (^)(void))animations
{
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delay * NSEC_PER_SEC)), dispatch_get_main_queue(), animations);
}

- (CGFloat)currentMinimumZoom
{
    return fmaxf(mbglMap->getMinZoom(), MGLMinimumZoom);
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
            self.animatingGesture = YES;

            self.userInteractionEnabled = NO;

            __weak MGLMapView *weakSelf = self;

            [self animateWithDelay:0.1 animations:^
            {
                [weakSelf resetNorthAnimated:YES];

                [self animateWithDelay:MGLAnimationDuration animations:^
                {
                    weakSelf.userInteractionEnabled = YES;

                    self.animatingGesture = NO;
                }];

            }];
        }
        else
        {
            [self resetNorthAnimated:NO];
        }
    }
}

- (void)unsuspendRegionChangeDelegateQueue
{
    @synchronized (self.regionChangeDelegateQueue)
    {
        [self.regionChangeDelegateQueue setSuspended:NO];
    }
}

- (void)notifyMapChange:(NSNumber *)change
{
    switch ([change unsignedIntegerValue])
    {
        case mbgl::MapChangeRegionWillChange:
        case mbgl::MapChangeRegionWillChangeAnimated:
        {
            [self updateUserLocationAnnotationView];
            [self updateCompass];

            [self deselectAnnotation:self.selectedAnnotation animated:NO];

            BOOL animated = ([change unsignedIntegerValue] == mbgl::MapChangeRegionWillChangeAnimated);

            @synchronized (self.regionChangeDelegateQueue)
            {
                if ([self.regionChangeDelegateQueue operationCount] == 0)
                {
                    if ([self.delegate respondsToSelector:@selector(mapView:regionWillChangeAnimated:)])
                    {
                        [self.delegate mapView:self regionWillChangeAnimated:animated];
                    }
                }

                [self.regionChangeDelegateQueue setSuspended:YES];

                if ([self.regionChangeDelegateQueue operationCount] == 0)
                {
                    [self.regionChangeDelegateQueue addOperationWithBlock:^
                    {
                        dispatch_async(dispatch_get_main_queue(), ^
                        {
                            if ([self.delegate respondsToSelector:@selector(mapView:regionDidChangeAnimated:)])
                            {
                                [self.delegate mapView:self regionDidChangeAnimated:animated];
                            }
                        });
                    }];
                }
            }
            break;
        }
        case mbgl::MapChangeRegionIsChanging:
        {
            [self updateUserLocationAnnotationView];
            [self updateCompass];

            if ([self.delegate respondsToSelector:@selector(mapViewRegionIsChanging:)])
            {
                [self.delegate mapViewRegionIsChanging:self];
            }
        }
        case mbgl::MapChangeRegionDidChange:
        case mbgl::MapChangeRegionDidChangeAnimated:
        {
            [self updateUserLocationAnnotationView];
            [self updateCompass];

            if (self.pan.state       == UIGestureRecognizerStateChanged ||
                self.pinch.state     == UIGestureRecognizerStateChanged ||
                self.rotate.state    == UIGestureRecognizerStateChanged ||
                self.quickZoom.state == UIGestureRecognizerStateChanged) return;

            if (self.isAnimatingGesture) return;

            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(unsuspendRegionChangeDelegateQueue) object:nil];
            [self performSelector:@selector(unsuspendRegionChangeDelegateQueue) withObject:nil afterDelay:0];

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
            if ([self.delegate respondsToSelector:@selector(mapViewDidFailLoadingMap:withError::)])
            {
                [self.delegate mapViewDidFailLoadingMap:self withError:nil];
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
        {
            if ([self.delegate respondsToSelector:@selector(mapViewDidFinishRenderingMap:fullyRendered:)])
            {
                [self.delegate mapViewDidFinishRenderingMap:self fullyRendered:NO];
            }
            break;
        }
        case mbgl::MapChangeDidFinishRenderingMapFullyRendered:
        {
            if ([self.delegate respondsToSelector:@selector(mapViewDidFinishRenderingMap:fullyRendered:)])
            {
                [self.delegate mapViewDidFinishRenderingMap:self fullyRendered:YES];
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
    double degrees = mbglMap->getBearing() * -1;
    while (degrees >= 360) degrees -= 360;
    while (degrees < 0) degrees += 360;

    self.compass.transform = CGAffineTransformMakeRotation([MGLMapView degreesToRadians:degrees]);

    if (mbglMap->getBearing() && self.compass.alpha < 1)
    {
        [UIView animateWithDuration:MGLAnimationDuration
                              delay:0
                            options:UIViewAnimationOptionBeginFromCurrentState
                         animations:^
                         {
                             self.compass.alpha = 1;
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

    return [UIImage imageWithContentsOfFile:[MGLMapView pathForBundleResourceNamed:imageName ofType:nil inDirectory:@""]];
}

+ (NSString *)pathForBundleResourceNamed:(NSString *)name ofType:(NSString *)extension inDirectory:(NSString *)directory
{
    NSString *path = [[NSBundle bundleWithPath:[MGLMapView resourceBundlePath]] pathForResource:name ofType:extension inDirectory:directory];

    NSAssert(path, @"Resource not found in application.");

    return path;
}

+ (NSString *)resourceBundlePath
{
    NSString *resourceBundlePath = [[NSBundle bundleForClass:[MGLMapView class]] pathForResource:@"MapboxGL" ofType:@"bundle"];

    if ( ! resourceBundlePath) resourceBundlePath = [[NSBundle mainBundle] bundlePath];

    return resourceBundlePath;
}

- (void)invalidate
{
    assert([[NSThread currentThread] isMainThread]);

    [self.glView setNeedsDisplay];

    [self notifyMapChange:@(mbgl::MapChangeRegionIsChanging)];
}

- (void)prepareForInterfaceBuilder
{
    [super prepareForInterfaceBuilder];
    
    self.layer.borderColor = [UIColor colorWithWhite:184/255. alpha:1].CGColor;
    self.layer.borderWidth = 1;
    
    if (self.accessToken)
    {
        self.layer.backgroundColor = [UIColor colorWithRed:59/255.
                                                     green:178/255.
                                                      blue:208/255.
                                                     alpha:0.8].CGColor;
        
        UIImage *image = [[self class] resourceImageNamed:@"mapbox.png"];
        UIImageView *previewView = [[UIImageView alloc] initWithImage:image];
        previewView.translatesAutoresizingMaskIntoConstraints = NO;
        [self addSubview:previewView];
        [self addConstraint:
         [NSLayoutConstraint constraintWithItem:previewView
                                      attribute:NSLayoutAttributeCenterXWithinMargins
                                      relatedBy:NSLayoutRelationEqual
                                         toItem:self
                                      attribute:NSLayoutAttributeCenterXWithinMargins
                                     multiplier:1
                                       constant:0]];
        [self addConstraint:
         [NSLayoutConstraint constraintWithItem:previewView
                                      attribute:NSLayoutAttributeCenterYWithinMargins
                                      relatedBy:NSLayoutRelationEqual
                                         toItem:self
                                      attribute:NSLayoutAttributeCenterYWithinMargins
                                     multiplier:1
                                       constant:0]];
    }
    else
    {
        UIView *diagnosticView = [[UIView alloc] init];
        diagnosticView.translatesAutoresizingMaskIntoConstraints = NO;
        [self addSubview:diagnosticView];
        
        // Headline
        UILabel *headlineLabel = [[UILabel alloc] init];
        headlineLabel.text = @"No Access Token";
        headlineLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleHeadline];
        headlineLabel.textAlignment = NSTextAlignmentCenter;
        headlineLabel.numberOfLines = 1;
        headlineLabel.translatesAutoresizingMaskIntoConstraints = NO;
        [headlineLabel setContentCompressionResistancePriority:UILayoutPriorityDefaultLow
                                                       forAxis:UILayoutConstraintAxisHorizontal];
        [diagnosticView addSubview:headlineLabel];
        
        // Explanation
        UILabel *explanationLabel = [[UILabel alloc] init];
        explanationLabel.text = @"To display a map here, you must provide a Mapbox access token. Get an access token from:";
        explanationLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
        explanationLabel.numberOfLines = 0;
        explanationLabel.translatesAutoresizingMaskIntoConstraints = NO;
        [explanationLabel setContentCompressionResistancePriority:UILayoutPriorityDefaultLow
                                                          forAxis:UILayoutConstraintAxisHorizontal];
        [diagnosticView addSubview:explanationLabel];
        
        // Link
        UIButton *linkButton = [UIButton buttonWithType:UIButtonTypeSystem];
        [linkButton setTitle:MGLMapboxAccessTokenManagerURLDisplayString forState:UIControlStateNormal];
        linkButton.translatesAutoresizingMaskIntoConstraints = NO;
        [linkButton setContentCompressionResistancePriority:UILayoutPriorityDefaultLow
                                                    forAxis:UILayoutConstraintAxisHorizontal];
        [diagnosticView addSubview:linkButton];
        
        // More explanation
        UILabel *explanationLabel2 = [[UILabel alloc] init];
        explanationLabel2.text = @"and enter it into the Access Token field in the Attributes inspector.";
        explanationLabel2.font = [UIFont preferredFontForTextStyle:UIFontTextStyleBody];
        explanationLabel2.numberOfLines = 0;
        explanationLabel2.translatesAutoresizingMaskIntoConstraints = NO;
        [explanationLabel2 setContentCompressionResistancePriority:UILayoutPriorityDefaultLow
                                                           forAxis:UILayoutConstraintAxisHorizontal];
        [diagnosticView addSubview:explanationLabel2];
        
        // Constraints
        NSDictionary *views = @{
            @"container": diagnosticView,
            @"headline": headlineLabel,
            @"explanation": explanationLabel,
            @"link": linkButton,
            @"explanation2": explanationLabel2,
        };
        [self addConstraint:
         [NSLayoutConstraint constraintWithItem:diagnosticView
                                      attribute:NSLayoutAttributeCenterYWithinMargins
                                      relatedBy:NSLayoutRelationEqual
                                         toItem:self
                                      attribute:NSLayoutAttributeCenterYWithinMargins
                                     multiplier:1
                                       constant:0]];
        [self addConstraints:
         [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-[container(20@20)]-|"
                                                 options:NSLayoutFormatAlignAllCenterY
                                                 metrics:nil
                                                   views:views]];
        [self addConstraints:
         [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[headline]-[explanation]-[link]-[explanation2]|"
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
        [self addConstraints:
         [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[explanation2]|"
                                                 options:0
                                                 metrics:nil
                                                   views:views]];
    }
}

class MBGLView : public mbgl::View
{
    public:
        MBGLView(MGLMapView *nativeView_) : nativeView(nativeView_) {}
        virtual ~MBGLView() {}


    void notify() override
    {
        // no-op
    }

    void notifyMapChange(mbgl::MapChange change, std::chrono::steady_clock::duration delay = std::chrono::steady_clock::duration::zero()) override
    {
        if (delay != std::chrono::steady_clock::duration::zero())
        {
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, std::chrono::duration_cast<std::chrono::nanoseconds>(delay).count()), dispatch_get_main_queue(), ^
            {
                [nativeView performSelector:@selector(notifyMapChange:)
                                 withObject:@(change)
                                 afterDelay:0];
            });
        }
        else
        {
            assert([[NSThread currentThread] isMainThread]);

            [nativeView notifyMapChange:@(change)];
        }
    }

    void activate() override
    {
        [EAGLContext setCurrentContext:nativeView.context];
    }

    void deactivate() override
    {
        [EAGLContext setCurrentContext:nil];
    }

    void resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight) {
        View::resize(width, height, ratio, fbWidth, fbHeight);
    }

    void invalidate() override
    {
        [nativeView performSelectorOnMainThread:@selector(invalidate)
                                     withObject:nil
                                  waitUntilDone:NO];
    }

    private:
        __weak MGLMapView *nativeView = nullptr;
};

@end

@implementation MGLMapView (IBAdditions)

+ (NSSet *)keyPathsForValuesAffectingLatitude
{
    return [NSSet setWithObject:@"centerCoordinate"];
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

+ (NSSet *)keyPathsForValuesAffectingLongitude
{
    return [NSSet setWithObject:@"centerCoordinate"];
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

+ (NSSet *)keyPathsForValuesAffectingAllowsZooming
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

+ (NSSet *)keyPathsForValuesAffectingAllowsScrolling
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

+ (NSSet *)keyPathsForValuesAffectingAllowsRotating
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

@end
