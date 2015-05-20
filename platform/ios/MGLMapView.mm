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
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/constants.hpp>

#import "MapboxGL.h"

#import "NSBundle+MGLAdditions.h"
#import "NSString+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSException+MGLAdditions.h"
#import "MGLUserLocationAnnotationView.h"
#import "MGLUserLocation_Private.h"
#import "MGLFileCache.h"
#import "MGLAccountManager_Private.h"
#import "MGLMapboxEvents.h"

#import "SMCalloutView.h"

#import <algorithm>

class MBGLView;

NSString *const MGLDefaultStyleName = @"mapbox-streets";
NSString *const MGLStyleVersion = @"7";
NSString *const MGLDefaultStyleMarkerSymbolName = @"default_marker";
NSString *const MGLMapboxSetupDocumentationURLDisplayString = @"mapbox.com/guides/first-steps-gl-ios";

const NSTimeInterval MGLAnimationDuration = 0.3;
const CGSize MGLAnnotationUpdateViewportOutset = {150, 150};
const CGFloat MGLMinimumZoom = 3;

NSString *const MGLAnnotationIDKey = @"MGLAnnotationIDKey";

static NSURL *MGLURLForBundledStyleNamed(NSString *styleName)
{
    return [NSURL URLWithString:[NSString stringWithFormat:@"asset://styles/%@.json", styleName]];
}

CGFloat MGLRadiansFromDegrees(CLLocationDegrees degrees)
{
    return degrees * M_PI / 180;
}

CLLocationDegrees MGLDegreesFromRadians(CGFloat radians)
{
    return radians * 180 / M_PI;
}

#pragma mark - Private -

@interface MGLMapView () <UIGestureRecognizerDelegate, GLKViewDelegate, CLLocationManagerDelegate, UIActionSheetDelegate>

@property (nonatomic) EAGLContext *context;
@property (nonatomic) GLKView *glView;
@property (nonatomic) UIImageView *glSnapshotView;
@property (nonatomic) NSOperationQueue *regionChangeDelegateQueue;
@property (nonatomic) UIImageView *compass;
@property (nonatomic) UIImageView *logoBug;
@property (nonatomic) UIButton *attributionButton;
@property (nonatomic) UIActionSheet *attributionSheet;
@property (nonatomic) UIPanGestureRecognizer *pan;
@property (nonatomic) UIPinchGestureRecognizer *pinch;
@property (nonatomic) UIRotationGestureRecognizer *rotate;
@property (nonatomic) UILongPressGestureRecognizer *quickZoom;
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
@property (nonatomic, getter=isDormant) BOOL dormant;
@property (nonatomic, getter=isAnimatingGesture) BOOL animatingGesture;
@property (nonatomic, readonly, getter=isRotationAllowed) BOOL rotationAllowed;

@end

@implementation MGLMapView
{
    mbgl::Map *_mbglMap;
    MBGLView *_mbglView;
    mbgl::DefaultFileSource *_mbglFileSource;
    
    NS_MUTABLE_ARRAY_OF(NSURL *) *_bundledStyleURLs;

    BOOL _isTargetingInterfaceBuilder;
    CLLocationDegrees _pendingLatitude;
    CLLocationDegrees _pendingLongitude;
}

#pragma mark - Setup & Teardown -

@dynamic debugActive;

std::chrono::steady_clock::duration secondsAsDuration(float duration)
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

- (nullable NSString *)accessToken
{
    [NSException raise:@"Method unavailable" format:
     @"%s has been removed. "
     @"Use +[MGLAccountManager accessToken] or get MGLMapboxAccessToken from the Info.plist.",
     __PRETTY_FUNCTION__];
    return nil;
}

- (void)setAccessToken:(nullable NSString *)accessToken
{
    [NSException raise:@"Method unavailable" format:
     @"%s has been replaced by +[MGLAccountManager setAccessToken:].\n\n"
     @"If you previously set this access token in a storyboard inspectable, select the MGLMapView in Interface Builder and delete the “accessToken” entry from the User Defined Runtime Attributes section of the Identity inspector. "
     @"Then go to the Info.plist file and set MGLMapboxAccessToken to “%@”.",
     __PRETTY_FUNCTION__, accessToken];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingStyleURL
{
    return [NSSet setWithObject:@"styleID"];
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
        styleURL = MGLURLForBundledStyleNamed([NSString stringWithFormat:@"%@-v%@",
                                               MGLDefaultStyleName.lowercaseString,
                                               MGLStyleVersion]);
    }

    if ( ! [styleURL scheme])
    {
        // Assume a relative path into the developer’s bundle.
        styleURL = [[NSBundle mainBundle] URLForResource:styleURL.path withExtension:nil];
    }

    _mbglMap->setStyleURL([[styleURL absoluteString] UTF8String]);
}

- (void)commonInit
{
    _isTargetingInterfaceBuilder = NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent;

    // create context
    //
    _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    NSAssert(_context, @"Failed to create OpenGL ES context.");

    // setup accessibility
    //
    self.accessibilityLabel = @"Map";

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

    self.backgroundColor = [UIColor clearColor];
    self.clipsToBounds = YES;

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

    // setup mbgl map
    //
    _mbglView = new MBGLView(self);
    _mbglFileSource = new mbgl::DefaultFileSource([MGLFileCache obtainSharedCacheWithObject:self]);

    // Start paused on the IB canvas
    _mbglMap = new mbgl::Map(*_mbglView, *_mbglFileSource, mbgl::MapMode::Continuous);
    if (_isTargetingInterfaceBuilder) {
        _mbglMap->pause();
    }
    _mbglMap->resize(self.bounds.size.width, self.bounds.size.height, _glView.contentScaleFactor);

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
    [reachability startNotifier];

    // setup annotations
    //
    _annotationIDsByAnnotation = [NSMapTable mapTableWithKeyOptions:NSMapTableStrongMemory valueOptions:NSMapTableStrongMemory];
    std::string defaultSymbolName([MGLDefaultStyleMarkerSymbolName UTF8String]);
    _mbglMap->setDefaultPointAnnotationSymbol(defaultSymbolName);

    // setup logo bug
    //
    UIImage *logo = [[MGLMapView resourceImageNamed:@"mapbox.png"] imageWithAlignmentRectInsets:UIEdgeInsetsMake(1.5, 4, 3.5, 2)];
    _logoBug = [[UIImageView alloc] initWithImage:logo];
    _logoBug.accessibilityLabel = @"Mapbox logo";
    _logoBug.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_logoBug];

    // setup attribution
    //
    _attributionButton = [UIButton buttonWithType:UIButtonTypeInfoLight];
    _attributionButton.accessibilityLabel = @"Attribution info";
    [_attributionButton addTarget:self action:@selector(showAttribution) forControlEvents:UIControlEventTouchUpInside];
    _attributionButton.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_attributionButton];
    
    _attributionSheet = [[UIActionSheet alloc] initWithTitle:@"Mapbox GL for iOS"
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
        [_attributionSheet addButtonWithTitle:@"Adjust Privacy Settings"];
    }

    // setup compass
    //
    _compass = [[UIImageView alloc] initWithImage:[MGLMapView resourceImageNamed:@"Compass.png"]];
    _compass.accessibilityLabel = @"Compass";
    UIImage *compassImage = [MGLMapView resourceImageNamed:@"Compass.png"];
    _compass.frame = CGRectMake(0, 0, compassImage.size.width, compassImage.size.height);
    _compass.alpha = 0;
    UIView *container = [[UIView alloc] initWithFrame:CGRectZero];
    [container addSubview:_compass];
    container.translatesAutoresizingMaskIntoConstraints = NO;
    [container addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleCompassTapGesture:)]];
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

    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
    {
        _quickZoom = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleQuickZoomGesture:)];
        _quickZoom.numberOfTapsRequired = 1;
        _quickZoom.minimumPressDuration = 0.25;
        [self addGestureRecognizer:_quickZoom];
    }

    // observe app activity
    //
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sleepGL:) name:UIApplicationWillResignActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sleepGL:) name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(wakeGL:) name:UIApplicationWillEnterForegroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(wakeGL:) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveMemoryWarning) name:UIApplicationDidReceiveMemoryWarningNotification object:nil];

    // set initial position
    //
    _mbglMap->setLatLngZoom(mbgl::LatLng(0, 0), _mbglMap->getMinZoom());
    _pendingLatitude = NAN;
    _pendingLongitude = NAN;

    // setup change delegate queue
    //
    _regionChangeDelegateQueue = [NSOperationQueue new];
    _regionChangeDelegateQueue.maxConcurrentOperationCount = 1;

    // metrics: map load event
    const mbgl::LatLng latLng = _mbglMap->getLatLng();
    const double zoom = _mbglMap->getZoom();

    [MGLMapboxEvents pushEvent:MGLEventTypeMapLoad withAttributes:@{
        MGLEventKeyLatitude: @(latLng.latitude),
        MGLEventKeyLongitude: @(latLng.longitude),
        MGLEventKeyZoomLevel: @(zoom),
        MGLEventKeyPushEnabled: @([MGLMapboxEvents checkPushEnabled])
    }];
}

-(void)reachabilityChanged:(NSNotification*)notification
{
    MGLReachability *reachability = [notification object];
    if ([reachability isReachable]) {
        mbgl::NetworkStatus::Reachable();
    }
}

- (void)dealloc
{
    [_regionChangeDelegateQueue cancelAllOperations];

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

    [MGLFileCache releaseSharedCacheForObject:self];

    if (_mbglView)
    {
        delete _mbglView;
        _mbglView = nullptr;
    }

    if ([[EAGLContext currentContext] isEqual:_context])
    {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)setDelegate:(nullable id<MGLMapViewDelegate>)delegate
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
    UIView *compassContainer = self.compass.superview;

    if (viewController)
    {
        [constraintParentView addConstraint:
         [NSLayoutConstraint constraintWithItem:compassContainer
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationGreaterThanOrEqual
                                         toItem:viewController.topLayoutGuide
                                      attribute:NSLayoutAttributeBottom
                                     multiplier:1
                                       constant:5]];
    }
    [constraintParentView addConstraint:
     [NSLayoutConstraint constraintWithItem:compassContainer
                                  attribute:NSLayoutAttributeTop
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeTop
                                 multiplier:1
                                   constant:5]];

    [constraintParentView addConstraint:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeTrailing
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:compassContainer
                                  attribute:NSLayoutAttributeTrailing
                                 multiplier:1
                                   constant:5]];

    [compassContainer addConstraint:
     [NSLayoutConstraint constraintWithItem:compassContainer
                                  attribute:NSLayoutAttributeWidth
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:nil
                                  attribute:NSLayoutAttributeNotAnAttribute
                                 multiplier:1
                                   constant:self.compass.image.size.width]];

    [compassContainer addConstraint:
     [NSLayoutConstraint constraintWithItem:compassContainer
                                  attribute:NSLayoutAttributeHeight
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:nil
                                  attribute:NSLayoutAttributeNotAnAttribute
                                 multiplier:1
                                   constant:self.compass.image.size.height]];

    // logo bug
    //
    if (viewController)
    {
        [constraintParentView addConstraint:
         [NSLayoutConstraint constraintWithItem:viewController.bottomLayoutGuide
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationGreaterThanOrEqual
                                         toItem:self.logoBug
                                      attribute:NSLayoutAttributeBaseline
                                     multiplier:1
                                       constant:8]];
    }
    [constraintParentView addConstraint:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeBottom
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self.logoBug
                                  attribute:NSLayoutAttributeBaseline
                                 multiplier:1
                                   constant:8]];

    [constraintParentView addConstraint:
     [NSLayoutConstraint constraintWithItem:self.logoBug
                                  attribute:NSLayoutAttributeLeading
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:self
                                  attribute:NSLayoutAttributeLeading
                                 multiplier:1
                                   constant:8]];

    // attribution button
    //
    if (viewController)
    {
        [constraintParentView addConstraint:
         [NSLayoutConstraint constraintWithItem:viewController.bottomLayoutGuide
                                      attribute:NSLayoutAttributeTop
                                      relatedBy:NSLayoutRelationGreaterThanOrEqual
                                         toItem:self.attributionButton
                                      attribute:NSLayoutAttributeBaseline
                                     multiplier:1
                                       constant:8]];
    }
    [constraintParentView addConstraint:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeBottom
                                  relatedBy:NSLayoutRelationGreaterThanOrEqual
                                     toItem:self.attributionButton
                                  attribute:NSLayoutAttributeBaseline
                                 multiplier:1
                                   constant:8]];

    [constraintParentView addConstraint:
     [NSLayoutConstraint constraintWithItem:self
                                  attribute:NSLayoutAttributeTrailing
                                  relatedBy:NSLayoutRelationEqual
                                     toItem:self.attributionButton
                                  attribute:NSLayoutAttributeTrailing
                                 multiplier:1
                                   constant:8]];

    [super updateConstraints];
}

// This is the delegate of the GLKView object's display call.
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if ( ! self.isDormant)
    {
        [self notifyMapChange:@(mbgl::MapChangeWillStartRenderingMap)];

        _mbglMap->resize(rect.size.width, rect.size.height, view.contentScaleFactor);

        CGFloat zoomFactor   = _mbglMap->getMaxZoom() - _mbglMap->getMinZoom() + 1;
        CGFloat cpuFactor    = (CGFloat)[[NSProcessInfo processInfo] processorCount];
        CGFloat memoryFactor = (CGFloat)[[NSProcessInfo processInfo] physicalMemory] / 1000 / 1000 / 1000;
        CGFloat sizeFactor   = ((CGFloat)_mbglMap->getWidth()  / mbgl::util::tileSize) *
                               ((CGFloat)_mbglMap->getHeight() / mbgl::util::tileSize);

        NSUInteger cacheSize = zoomFactor * cpuFactor * memoryFactor * sizeFactor * 0.5;

        _mbglMap->setSourceTileCacheSize(cacheSize);

        _mbglMap->renderSync();

        [self notifyMapChange:@(mbgl::MapChangeDidFinishRenderingMap)];
    }
}

// This gets called when the view dimension changes, e.g. because the device is being rotated.
- (void)layoutSubviews
{
    [super layoutSubviews];

    if ( ! _isTargetingInterfaceBuilder)
    {
        _mbglMap->update();
    }
    
    if (self.attributionSheet.visible)
    {
        [self.attributionSheet dismissWithClickedButtonIndex:self.attributionSheet.cancelButtonIndex animated:YES];
    }

    if (self.compass.alpha)
    {
        [self updateHeadingForDeviceOrientation];
        [self updateCompass];
    }
}

#pragma mark - Life Cycle -

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
            [self insertSubview:self.glSnapshotView aboveSubview:self.glView];
        }

        self.glSnapshotView.image = self.glView.snapshot;
        self.glSnapshotView.hidden = NO;

        if (_mbglMap->getDebug() && [self.glSnapshotView.subviews count] == 0)
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

    if (self.isDormant)
    {
        self.dormant = NO;
        
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

    if (self.userTrackingMode == MGLUserTrackingModeFollowWithHeading) self.userTrackingMode = MGLUserTrackingModeFollow;
}

- (void)touchesBegan:(__unused NS_SET_OF(UITouch *) *)touches withEvent:(__unused UIEvent *)event
{
    _mbglMap->cancelTransitions();
    _mbglMap->setGestureInProgress(false);
    self.animatingGesture = NO;
}

- (void)handlePanGesture:(UIPanGestureRecognizer *)pan
{
    if ( ! self.isScrollEnabled) return;

    _mbglMap->cancelTransitions();

    if (pan.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGesturePanStart forRecognizer:pan];

        _mbglMap->setGestureInProgress(true);

        self.centerPoint = CGPointMake(0, 0);

        self.userTrackingMode = MGLUserTrackingModeNone;
    }
    else if (pan.state == UIGestureRecognizerStateChanged)
    {
        CGPoint delta = CGPointMake([pan translationInView:pan.view].x - self.centerPoint.x,
                                    [pan translationInView:pan.view].y - self.centerPoint.y);

        _mbglMap->moveBy(delta.x, delta.y);

        self.centerPoint = CGPointMake(self.centerPoint.x + delta.x, self.centerPoint.y + delta.y);

        [self notifyMapChange:@(mbgl::MapChangeRegionDidChangeAnimated)];
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
        if ( ! CGPointEqualToPoint(velocity, CGPointZero))
        {
            CGPoint offset = CGPointMake(velocity.x * duration / 4, velocity.y * duration / 4);
            _mbglMap->moveBy(offset.x, offset.y, secondsAsDuration(duration));
        }

        _mbglMap->setGestureInProgress(false);

        if ( ! CGPointEqualToPoint(velocity, CGPointZero))
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

    if (_mbglMap->getZoom() <= _mbglMap->getMinZoom() && pinch.scale < 1) return;

    _mbglMap->cancelTransitions();

    if (pinch.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGesturePinchStart forRecognizer:pinch];

        _mbglMap->setGestureInProgress(true);

        self.scale = _mbglMap->getScale();

        self.userTrackingMode = MGLUserTrackingModeNone;
    }
    else if (pinch.state == UIGestureRecognizerStateChanged)
    {
        CGFloat newScale = self.scale * pinch.scale;

        if (log2(newScale) < _mbglMap->getMinZoom()) return;

        _mbglMap->setScale(newScale, [pinch locationInView:pinch.view].x, [pinch locationInView:pinch.view].y);
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
            _mbglMap->setScale(newScale, pinchCenter.x, pinchCenter.y, secondsAsDuration(duration));
        }

        _mbglMap->setGestureInProgress(false);

        [self unrotateIfNeededAnimated:YES];

        if (velocity)
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
    }
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    if ( ! self.isRotateEnabled) return;

    _mbglMap->cancelTransitions();

    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGestureRotateStart forRecognizer:rotate];

        _mbglMap->setGestureInProgress(true);

        self.angle = MGLRadiansFromDegrees(_mbglMap->getBearing()) * -1;

        self.userTrackingMode = MGLUserTrackingModeNone;
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

        _mbglMap->setBearing(newDegrees,
                            [rotate locationInView:rotate.view].x,
                            [rotate locationInView:rotate.view].y);
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

            _mbglMap->setBearing(newDegrees, secondsAsDuration(duration));

            _mbglMap->setGestureInProgress(false);

            self.animatingGesture = YES;

            __weak MGLMapView *weakSelf = self;

            [self animateWithDelay:duration animations:^
             {
                 weakSelf.animatingGesture = NO;

                 [weakSelf unrotateIfNeededAnimated:YES];

                 [weakSelf notifyMapChange:@(mbgl::MapChangeRegionDidChangeAnimated)];
             }];
        }
        else
        {
            _mbglMap->setGestureInProgress(false);

            [self unrotateIfNeededAnimated:YES];

            [self notifyMapChange:@(mbgl::MapChangeRegionDidChange)];
        }
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
        tapBounds.extend(MGLLatLngFromLocationCoordinate2D(coordinate));

        coordinate = [self convertPoint:tapRectUpperLeft  toCoordinateFromView:self];
        tapBounds.extend(MGLLatLngFromLocationCoordinate2D(coordinate));

        coordinate = [self convertPoint:tapRectUpperRight toCoordinateFromView:self];
        tapBounds.extend(MGLLatLngFromLocationCoordinate2D(coordinate));

        coordinate = [self convertPoint:tapRectLowerRight toCoordinateFromView:self];
        tapBounds.extend(MGLLatLngFromLocationCoordinate2D(coordinate));

        // query for nearby annotations
        std::vector<uint32_t> nearbyAnnotations = _mbglMap->getAnnotationsInBounds(tapBounds, mbgl::AnnotationType::Point);

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

        _mbglMap->scaleBy(2, zoomInPoint.x, zoomInPoint.y, secondsAsDuration(MGLAnimationDuration));

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

        _mbglMap->scaleBy(0.5, zoomOutPoint.x, zoomOutPoint.y, secondsAsDuration(MGLAnimationDuration));

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

    _mbglMap->cancelTransitions();

    if (quickZoom.state == UIGestureRecognizerStateBegan)
    {
        [self trackGestureEvent:MGLEventGestureQuickZoom forRecognizer:quickZoom];

        self.scale = _mbglMap->getScale();

        self.quickZoomStart = [quickZoom locationInView:quickZoom.view].y;

        self.userTrackingMode = MGLUserTrackingModeNone;
    }
    else if (quickZoom.state == UIGestureRecognizerStateChanged)
    {
        CGFloat distance = self.quickZoomStart - [quickZoom locationInView:quickZoom.view].y;

        CGFloat newZoom = log2f(self.scale) + (distance / 100);

        if (newZoom < _mbglMap->getMinZoom()) return;

        _mbglMap->scaleBy(powf(2, newZoom) / _mbglMap->getScale(), self.bounds.size.width / 2, self.bounds.size.height / 2);
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
        NSAssert([tap.view isKindOfClass:[UIControl class]], @"Tapped view %@ is not a UIControl", tap.view);
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

#pragma mark - Attribution -

- (void)showAttribution
{
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

- (void)setDebugActive:(BOOL)debugActive
{
    _mbglMap->setDebug(debugActive);
}

- (BOOL)isDebugActive
{
    return _mbglMap->getDebug();
}

- (void)resetNorth
{
    [self resetNorthAnimated:YES];
}

- (void)resetNorthAnimated:(BOOL)animated
{
    self.userTrackingMode = MGLUserTrackingModeNone;

    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    _mbglMap->setBearing(0, secondsAsDuration(duration));

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
                         }
                     }];
}

- (void)resetPosition
{
    _mbglMap->resetPosition();

    [self notifyMapChange:@(mbgl::MapChangeRegionDidChange)];
}

- (void)toggleDebug
{
    _mbglMap->toggleDebug();
}

- (void)emptyMemoryCache
{
    _mbglMap->onLowMemory();
}

#pragma mark - Geography -

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCenterCoordinate
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

    _mbglMap->setLatLngZoom(MGLLatLngFromLocationCoordinate2D(coordinate),
                            fmaxf(_mbglMap->getZoom(), self.currentMinimumZoom),
                            secondsAsDuration(duration));

    [self notifyMapChange:@(animated ? mbgl::MapChangeRegionDidChangeAnimated : mbgl::MapChangeRegionDidChange)];
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
    self.userTrackingMode = MGLUserTrackingModeNone;

    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    _mbglMap->setLatLngZoom(MGLLatLngFromLocationCoordinate2D(centerCoordinate), zoomLevel, secondsAsDuration(duration));

    [self unrotateIfNeededAnimated:animated];

    [self notifyMapChange:@(animated ? mbgl::MapChangeRegionDidChangeAnimated : mbgl::MapChangeRegionDidChange)];
}

- (double)zoomLevel
{
    return _mbglMap->getZoom();
}

- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated
{
    self.userTrackingMode = MGLUserTrackingModeNone;

    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    _mbglMap->setLatLngZoom(_mbglMap->getLatLng(),
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

    CGFloat scale = _mbglMap->getScale();
    CGFloat scaleX = _mbglMap->getWidth() / (northEastCoordinate.longitude - southWestCoordinate.longitude);
    CGFloat scaleY = _mbglMap->getHeight() / (northEastCoordinate.latitude - southWestCoordinate.latitude);
    CGFloat minZoom = _mbglMap->getMinZoom();
    CGFloat maxZoom = _mbglMap->getMaxZoom();
    CGFloat zoomLevel = MAX(MIN(log(scale * MIN(scaleX, scaleY)) / log(2), maxZoom), minZoom);

    [self setCenterCoordinate:center zoomLevel:zoomLevel animated:animated];
}

- (CLLocationDirection)direction
{
    double direction = _mbglMap->getBearing();

    while (direction > 360) direction -= 360;
    while (direction < 0) direction += 360;

    return direction;
}

- (void)setDirection:(CLLocationDirection)direction animated:(BOOL)animated
{
    if ( ! animated && ! self.rotationAllowed) return;

    self.userTrackingMode = MGLUserTrackingModeNone;

    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    _mbglMap->setBearing(direction, secondsAsDuration(duration));

    [self notifyMapChange:@(animated ? mbgl::MapChangeRegionDidChangeAnimated : mbgl::MapChangeRegionDidChange)];
}

- (void)setDirection:(CLLocationDirection)direction
{
    [self setDirection:direction animated:NO];
}

- (CLLocationCoordinate2D)convertPoint:(CGPoint)point toCoordinateFromView:(nullable UIView *)view
{
    CGPoint convertedPoint = [self convertPoint:point fromView:view];

    // flip y coordinate for iOS view origin top left
    //
    convertedPoint.y = self.bounds.size.height - convertedPoint.y;

    return MGLLocationCoordinate2DFromLatLng(_mbglMap->latLngForPixel(mbgl::vec2<double>(convertedPoint.x, convertedPoint.y)));
}

- (CGPoint)convertCoordinate:(CLLocationCoordinate2D)coordinate toPointToView:(nullable UIView *)view
{
    mbgl::vec2<double> pixel = _mbglMap->pixelForLatLng(MGLLatLngFromLocationCoordinate2D(coordinate));

    // flip y coordinate for iOS view origin in top left
    //
    pixel.y = self.bounds.size.height - pixel.y;

    return [self convertPoint:CGPointMake(pixel.x, pixel.y) toView:view];
}

- (CLLocationDistance)metersPerPixelAtLatitude:(CLLocationDegrees)latitude
{
    return _mbglMap->getMetersPerPixelAtLatitude(latitude, self.zoomLevel);
}

mbgl::LatLng MGLLatLngFromLocationCoordinate2D(CLLocationCoordinate2D coordinate)
{
    return mbgl::LatLng(coordinate.latitude, coordinate.longitude);
}

CLLocationCoordinate2D MGLLocationCoordinate2DFromLatLng(mbgl::LatLng latLng)
{
    return CLLocationCoordinate2DMake(latLng.latitude, latLng.longitude);
}

- (mbgl::LatLngBounds)viewportBounds
{
    mbgl::LatLngBounds bounds;

    bounds.extend(MGLLatLngFromLocationCoordinate2D(
        [self convertPoint:CGPointMake(0, 0) toCoordinateFromView:self]));
    bounds.extend(MGLLatLngFromLocationCoordinate2D(
        [self convertPoint:CGPointMake(self.bounds.size.width, 0) toCoordinateFromView:self]));
    bounds.extend(MGLLatLngFromLocationCoordinate2D(
        [self convertPoint:CGPointMake(0, self.bounds.size.height) toCoordinateFromView:self]));
    bounds.extend(MGLLatLngFromLocationCoordinate2D(
        [self convertPoint:CGPointMake(self.bounds.size.width, self.bounds.size.height) toCoordinateFromView:self]));

    return bounds;
}

#pragma mark - Styling -

- (NS_ARRAY_OF(NSURL *) *)bundledStyleURLs
{
    if ( ! _bundledStyleURLs)
    {
        NSString *stylesPath = [[NSBundle mgl_resourceBundlePath] stringByAppendingPathComponent:@"styles"];

        _bundledStyleURLs = [NSMutableArray array];

        NSArray *bundledStyleNamesWithExtensions = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:stylesPath error:nil];
        for (NSString *fileName in bundledStyleNamesWithExtensions)
        {
            [_bundledStyleURLs addObject:MGLURLForBundledStyleNamed([fileName stringByDeletingPathExtension])];
        }
    }

    return [NSArray arrayWithArray:_bundledStyleURLs];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingStyleID
{
    return [NSSet setWithObject:@"styleURL"];
}

- (nullable NSString *)styleID
{
    NSURL *styleURL = self.styleURL;
    return [styleURL.scheme isEqualToString:@"mapbox"] ? styleURL.host.mgl_stringOrNilIfEmpty : nil;
}

- (void)setStyleID:(nullable NSString *)styleID
{
    self.styleURL = styleID ? [NSURL URLWithString:[NSString stringWithFormat:@"mapbox://%@", styleID]] : nil;
}

- (nullable NSString *)mapID
{
    [NSException raise:@"Method unavailable" format:
     @"%s has been renamed -[MGLMapView styleID].",
     __PRETTY_FUNCTION__];
    return nil;
}

- (void)setMapID:(nullable NSString *)mapID
{
    [NSException raise:@"Method unavailable" format:
     @"%s has been renamed -[MGLMapView setStyleID:].\n\n"
     @"If you previously set this map ID in a storyboard inspectable, select the MGLMapView in Interface Builder and delete the “mapID” entry from the User Defined Runtime Attributes section of the Identity inspector. "
     @"Then go to the Attributes inspector and enter “%@” into the “Style ID” field.",
     __PRETTY_FUNCTION__, mapID];
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

    _mbglMap->setDefaultTransitionDuration(secondsAsDuration(transitionDuration));
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

- (void)addAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations
{
    if ( ! annotations) return;

    std::vector<mbgl::LatLng> points;
    std::vector<std::string> symbols;

    std::vector<mbgl::AnnotationSegments> shapes;
    std::vector<mbgl::StyleProperties> shapesProperties;

    BOOL delegateImplementsSymbolLookup = [self.delegate respondsToSelector:@selector(mapView:symbolNameForAnnotation:)];
    BOOL delegateImplementsAlphaForShape = [self.delegate respondsToSelector:@selector(mapView:alphaForShapeAnnotation:)];
    BOOL delegateImplementsStrokeColorForShape = [self.delegate respondsToSelector:@selector(mapView:strokeColorForShapeAnnotation:)];
    BOOL delegateImplementsFillColorForPolygon = [self.delegate respondsToSelector:@selector(mapView:fillColorForPolygonAnnotation:)];
    BOOL delegateImplementsLineWidthForPolyline = [self.delegate respondsToSelector:@selector(mapView:lineWidthForPolylineAnnotation:)];

    for (id <MGLAnnotation> annotation in annotations)
    {
        assert([annotation conformsToProtocol:@protocol(MGLAnnotation)]);

        if ([annotation isKindOfClass:[MGLMultiPoint class]])
        {
            CGFloat alpha = (delegateImplementsAlphaForShape ?
                                [self.delegate mapView:self alphaForShapeAnnotation:annotation] :
                                1.0);

            UIColor *strokeColor = (delegateImplementsStrokeColorForShape ?
                                [self.delegate mapView:self strokeColorForShapeAnnotation:annotation] :
                                [UIColor blackColor]);

            assert(strokeColor);

            CGFloat r,g,b,a;
            [strokeColor getRed:&r green:&g blue:&b alpha:&a];
            mbgl::Color strokeNativeColor({{ (float)r, (float)g, (float)b, (float)a }});

            mbgl::StyleProperties shapeProperties;

            if ([annotation isKindOfClass:[MGLPolyline class]])
            {
                CGFloat lineWidth = (delegateImplementsLineWidthForPolyline ?
                                [self.delegate mapView:self lineWidthForPolylineAnnotation:(MGLPolyline *)annotation] :
                                3.0);

                mbgl::LineProperties lineProperties;
                lineProperties.opacity = alpha;
                lineProperties.color = strokeNativeColor;
                lineProperties.width = lineWidth;
                shapeProperties.set<mbgl::LineProperties>(lineProperties);

            }
            else if ([annotation isKindOfClass:[MGLPolygon class]])
            {
                UIColor *fillColor = (delegateImplementsFillColorForPolygon ?
                                [self.delegate mapView:self fillColorForPolygonAnnotation:(MGLPolygon *)annotation] :
                                [UIColor blueColor]);

                assert(fillColor);

                [fillColor getRed:&r green:&g blue:&b alpha:&a];
                mbgl::Color fillNativeColor({{ (float)r, (float)g, (float)b, (float)a }});

                mbgl::FillProperties fillProperties;
                fillProperties.opacity = alpha;
                fillProperties.stroke_color = strokeNativeColor;
                fillProperties.fill_color = fillNativeColor;
                shapeProperties.set<mbgl::FillProperties>(fillProperties);
            }
            else
            {
                [[NSException exceptionWithName:@"MGLUnknownShapeClassException"
                                         reason:[NSString stringWithFormat:@"%@ is an unknown shape class", [annotation class]]
                                       userInfo:nil] raise];
            }

            shapesProperties.push_back(shapeProperties);

            NSUInteger count = [(MGLMultiPoint *)annotation pointCount];

            CLLocationCoordinate2D *coordinates = (CLLocationCoordinate2D *)malloc(count * sizeof(CLLocationCoordinate2D));
            [(MGLMultiPoint *)annotation getCoordinates:coordinates range:NSMakeRange(0, count)];

            mbgl::AnnotationSegment shape;
            shape.reserve(count);

            for (NSUInteger i = 0; i < count; i++)
            {
                shape.push_back(mbgl::LatLng(coordinates[i].latitude, coordinates[i].longitude));
            }

            free(coordinates);

            shapes.push_back({{ shape }});
        }
        else
        {
            points.push_back(MGLLatLngFromLocationCoordinate2D(annotation.coordinate));

            NSString *symbolName = nil;

            if (delegateImplementsSymbolLookup)
            {
                symbolName = [self.delegate mapView:self symbolNameForAnnotation:annotation];
            }

            symbols.push_back((symbolName ? [symbolName UTF8String] : ""));
        }
    }

    if (points.size())
    {
        std::vector<uint32_t> pointAnnotationIDs = _mbglMap->addPointAnnotations(points, symbols);

        for (size_t i = 0; i < pointAnnotationIDs.size(); ++i)
        {
            [self.annotationIDsByAnnotation setObject:@{ MGLAnnotationIDKey : @(pointAnnotationIDs[i]) }
                                               forKey:annotations[i]];
        }
    }

    if (shapes.size())
    {
        std::vector<uint32_t> shapeAnnotationIDs = _mbglMap->addShapeAnnotations(shapes, shapesProperties);

        for (size_t i = 0; i < shapeAnnotationIDs.size(); ++i)
        {
            [self.annotationIDsByAnnotation setObject:@{ MGLAnnotationIDKey : @(shapeAnnotationIDs[i]) }
                                               forKey:annotations[i]];
        }
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

- (void)removeAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)annotations
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

    _mbglMap->removeAnnotations(annotationIDsToRemove);
}

- (NS_ARRAY_OF(id <MGLAnnotation>) *)selectedAnnotations
{
    return (self.selectedAnnotation ? @[ self.selectedAnnotation ] : @[]);
}

- (void)setSelectedAnnotations:(NS_ARRAY_OF(id <MGLAnnotation>) *)selectedAnnotations
{
    if ( ! selectedAnnotations.count) return;

    id <MGLAnnotation> firstAnnotation = selectedAnnotations[0];

    assert([firstAnnotation conformsToProtocol:@protocol(MGLAnnotation)]);

    if ([firstAnnotation isKindOfClass:[MGLMultiPoint class]]) return;

    if ( ! [self viewportBounds].contains(MGLLatLngFromLocationCoordinate2D(firstAnnotation.coordinate))) return;

    [self selectAnnotation:firstAnnotation animated:NO];
}

- (void)selectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated
{
    if ( ! annotation) return;

    if ([annotation isKindOfClass:[MGLMultiPoint class]]) return;

    if ( ! [self viewportBounds].contains(MGLLatLngFromLocationCoordinate2D(annotation.coordinate))) return;

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
            double y = _mbglMap->getTopOffsetPixelsForAnnotationSymbol(symbolName);
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

        // notify delegate
        if ([self.delegate respondsToSelector:@selector(mapView:didDeselectAnnotation:)])
        {
            [self.delegate mapView:self didDeselectAnnotation:annotation];
        }
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

- (void)locationManager:(__unused CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation
{
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

    CLLocationDirection headingDirection = (newHeading.trueHeading > 0 ? newHeading.trueHeading : newHeading.magneticHeading);

    if (headingDirection > 0 && self.userTrackingMode == MGLUserTrackingModeFollowWithHeading)
    {
        _mbglMap->setBearing(headingDirection, secondsAsDuration(MGLAnimationDuration));
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
    double degrees = _mbglMap->getBearing() * -1;
    while (degrees >= 360) degrees -= 360;
    while (degrees < 0) degrees += 360;

    self.compass.transform = CGAffineTransformMakeRotation(MGLRadiansFromDegrees(degrees));

    if (_mbglMap->getBearing() && self.compass.alpha < 1)
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
    else if (_mbglMap->getBearing() == 0 && self.compass.alpha > 0)
    {
        [UIView animateWithDuration:MGLAnimationDuration
                              delay:0
                            options:UIViewAnimationOptionBeginFromCurrentState
                         animations:^
                         {
                             self.compass.alpha = 0;
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
         @"The resource named “%@” could not be found in the Mapbox GL resource bundle.", name];
    }

    return path;
}

- (void)invalidate
{
    MGLAssertIsMainThread();

    [self.glView setNeedsDisplay];

    [self notifyMapChange:@(mbgl::MapChangeRegionIsChanging)];
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

    void invalidate(std::function<void()>) override
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

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingLatitude
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

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingLongitude
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

- (void)didReceiveMemoryWarning
{
    _mbglMap->onLowMemory();
}

@end
