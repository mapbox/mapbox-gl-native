#import "MGLMapView.h"

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
#import "MGLStyleFunctionValue.h"
#import "MGLAnnotation.h"

#import "UIColor+MGLAdditions.h"
#import "NSArray+MGLAdditions.h"
#import "NSDictionary+MGLAdditions.h"

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
NSString *const MGLStyleVersion = @"v7";
NSString *const MGLDefaultStyleMarkerSymbolName = @"default_marker";

extern NSString *const MGLStyleKeyGeneric;
extern NSString *const MGLStyleKeyFill;
extern NSString *const MGLStyleKeyLine;
extern NSString *const MGLStyleKeyIcon;
extern NSString *const MGLStyleKeyText;
extern NSString *const MGLStyleKeyRaster;
extern NSString *const MGLStyleKeyComposite;
extern NSString *const MGLStyleKeyBackground;

extern NSString *const MGLStyleValueFunctionAllowed;

NSTimeInterval const MGLAnimationDuration = 0.3;

NSString *const MGLAnnotationIDKey = @"MGLAnnotationIDKey";

#pragma mark - Private -

@interface MGLMapView () <UIGestureRecognizerDelegate, GLKViewDelegate>

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
@property (nonatomic) NSMutableArray *bundledStyleNames;
@property (nonatomic) NSMapTable *annotationsStore;
@property (nonatomic) std::vector<uint32_t> annotationsNearbyLastTap;
@property (nonatomic, weak) id <MGLAnnotation> selectedAnnotation;
@property (nonatomic, readonly) NSDictionary *allowedStyleTypes;
@property (nonatomic) CGPoint centerPoint;
@property (nonatomic) CGFloat scale;
@property (nonatomic) CGFloat angle;
@property (nonatomic) CGFloat quickZoomStart;
@property (nonatomic, getter=isAnimatingGesture) BOOL animatingGesture;
@property (nonatomic, readonly, getter=isRotationAllowed) BOOL rotationAllowed;

@end

@interface MGLStyleFunctionValue (MGLMapViewFriend)

@property (nonatomic) NSString *functionType;
@property (nonatomic) NSDictionary *stops;
@property (nonatomic) CGFloat zBase;
@property (nonatomic) CGFloat val;
@property (nonatomic) CGFloat slope;
@property (nonatomic) CGFloat min;
@property (nonatomic) CGFloat max;
@property (nonatomic) CGFloat minimumZoom;
@property (nonatomic) CGFloat maximumZoom;

- (id)rawStyle;

@end

@implementation MGLMapView

@synthesize bundledStyleNames=_bundledStyleNames;

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

- (instancetype)initWithFrame:(CGRect)frame accessToken:(NSString *)accessToken styleJSON:(NSString *)styleJSON
{
    self = [super initWithFrame:frame];

    if (self && [self commonInit])
    {
        if (accessToken) [self setAccessToken:accessToken];

        if (styleJSON || accessToken)
        {
            // If style is set directly, pass it on. If not, if we have an access
            // token, we can pass nil and use the default style.
            //
            [self setStyleJSON:styleJSON];
        }
    }

    return self;
}

- (instancetype)initWithFrame:(CGRect)frame accessToken:(NSString *)accessToken bundledStyleNamed:(NSString *)styleName
{
    self = [super initWithFrame:frame];
    
    if (self && [self commonInit])
    {
        if (accessToken) [self setAccessToken:accessToken];
        if (styleName) [self useBundledStyleNamed:styleName];
    }
    
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame accessToken:(NSString *)accessToken
{
    return [self initWithFrame:frame accessToken:accessToken styleJSON:nil];
}

- (instancetype)initWithCoder:(NSCoder *)decoder
{
    self = [super initWithCoder:decoder];

    if (self && [self commonInit])
    {
        return self;
    }

    return nil;
}

- (void)setAccessToken:(NSString *)accessToken
{
    if (accessToken)
    {
        mbglMap->setAccessToken((std::string)[accessToken cStringUsingEncoding:[NSString defaultCStringEncoding]]);
    }
}

- (void)setStyleJSON:(NSString *)styleJSON
{
    if ( ! styleJSON)
    {
        [self useBundledStyleNamed:[[[MGLDefaultStyleName lowercaseString]
                                        stringByAppendingString:@"-"]
                                        stringByAppendingString:MGLStyleVersion]];
    }
    else
    {
        if ([@(mbglMap->getStyleJSON().c_str()) length]) mbglMap->stop();
        mbglMap->setStyleJSON((std::string)[styleJSON cStringUsingEncoding:[NSString defaultCStringEncoding]]);
        mbglMap->start();
    }
}

- (void)setStyleURL:(NSString *)filePathURL
{
    if ([@(mbglMap->getStyleJSON().c_str()) length]) mbglMap->stop();
    mbglMap->setStyleURL(std::string("asset://") + [filePathURL UTF8String]);
    mbglMap->start();
}

- (BOOL)commonInit
{
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
    [self addSubview:_glView];

    _glView.contentMode = UIViewContentModeCenter;
    [self setBackgroundColor:[UIColor clearColor]];

    // load extensions
    //
    dispatch_once(&loadGLExtensions, ^{
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
    _annotationsStore = [NSMapTable mapTableWithKeyOptions:NSMapTableStrongMemory valueOptions:NSMapTableStrongMemory];
    std::string defaultSymbolName([MGLDefaultStyleMarkerSymbolName cStringUsingEncoding:[NSString defaultCStringEncoding]]);
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

    // setup change delegate queue
    //
    _regionChangeDelegateQueue = [NSOperationQueue new];
    _regionChangeDelegateQueue.maxConcurrentOperationCount = 1;

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
    mbglMap->triggerUpdate();
}

#pragma mark - Life Cycle -

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

- (void)appDidBackground:(NSNotification *)notification
{
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
    for (UIView *subview in self.subviews)
    {
        if ([subview respondsToSelector:@selector(setTintColor:)])
        {
            subview.tintColor = self.tintColor;
        }
    }
}

#pragma mark - Gestures -

- (void)handleCompassTapGesture:(id)sender
{
    [self resetNorthAnimated:YES];
}

#pragma clang diagnostic pop

- (void)handlePanGesture:(UIPanGestureRecognizer *)pan
{
    if ( ! self.isScrollEnabled) return;

    mbglMap->cancelTransitions();

    if (pan.state == UIGestureRecognizerStateBegan)
    {
        self.centerPoint = CGPointMake(0, 0);
    }
    else if (pan.state == UIGestureRecognizerStateChanged)
    {
        CGPoint delta = CGPointMake([pan translationInView:pan.view].x - self.centerPoint.x,
                                    [pan translationInView:pan.view].y - self.centerPoint.y);

        mbglMap->moveBy(delta.x, delta.y);

        self.centerPoint = CGPointMake(self.centerPoint.x + delta.x, self.centerPoint.y + delta.y);
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
    }
}

- (void)handlePinchGesture:(UIPinchGestureRecognizer *)pinch
{
    if ( ! self.isZoomEnabled) return;

    if (mbglMap->getZoom() <= mbglMap->getMinZoom() && pinch.scale < 1) return;

    mbglMap->cancelTransitions();

    if (pinch.state == UIGestureRecognizerStateBegan)
    {
        mbglMap->startScaling();

        self.scale = mbglMap->getScale();
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
        mbglMap->stopScaling();

        [self unrotateIfNeededAnimated:YES];

        [self notifyMapChange:@(mbgl::MapChangeRegionDidChangeAnimated)];
    }
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    if ( ! self.isRotateEnabled) return;

    mbglMap->cancelTransitions();

    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        mbglMap->startRotating();

        self.angle = [MGLMapView degreesToRadians:mbglMap->getBearing()] * -1;
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
        mbglMap->stopRotating();

        [self unrotateIfNeededAnimated:YES];

        [self notifyMapChange:@(mbgl::MapChangeRegionDidChangeAnimated)];
    }
}

- (void)handleSingleTapGesture:(UITapGestureRecognizer *)singleTap
{
    CGPoint tapPoint = [singleTap locationInView:self];

    // tolerances based on touch size & typical marker aspect ratio
    CGFloat toleranceWidth  = 50;
    CGFloat toleranceHeight = 75;

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
    tapBounds.extend(mbgl::LatLng(coordinate.latitude, coordinate.longitude));

    coordinate = [self convertPoint:tapRectUpperLeft  toCoordinateFromView:self];
    tapBounds.extend(mbgl::LatLng(coordinate.latitude, coordinate.longitude));

    coordinate = [self convertPoint:tapRectUpperRight toCoordinateFromView:self];
    tapBounds.extend(mbgl::LatLng(coordinate.latitude, coordinate.longitude));

    coordinate = [self convertPoint:tapRectLowerRight toCoordinateFromView:self];
    tapBounds.extend(mbgl::LatLng(coordinate.latitude, coordinate.longitude));

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
                [[[self.annotationsStore objectForKey:self.selectedAnnotation]
                    objectForKey:MGLAnnotationIDKey] unsignedIntValue] == self.annotationsNearbyLastTap.back())
            {
                // the selected annotation is the last in the set; cycle back to the first
                // note: this could be the selected annotation if only one in set
                newSelectedAnnotationID = self.annotationsNearbyLastTap.front();
            }
            else if (self.selectedAnnotation)
            {
                // otherwise increment the selection through the candidates
                uint32_t currentID = [[[self.annotationsStore objectForKey:self.selectedAnnotation] objectForKey:MGLAnnotationIDKey] unsignedIntValue];
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
        NSEnumerator *enumerator = self.annotationsStore.keyEnumerator;

        while (id <MGLAnnotation> annotation = enumerator.nextObject)
        {
            if ([[[self.annotationsStore objectForKey:annotation] objectForKey:MGLAnnotationIDKey] integerValue] == newSelectedAnnotationID)
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

    NSLog(@"%i (%@)", newSelectedAnnotationID, self.selectedAnnotation.title);
}

- (void)handleDoubleTapGesture:(UITapGestureRecognizer *)doubleTap
{
    if ( ! self.isZoomEnabled) return;

    mbglMap->cancelTransitions();

    if (doubleTap.state == UIGestureRecognizerStateEnded)
    {
        mbglMap->scaleBy(2, [doubleTap locationInView:doubleTap.view].x, [doubleTap locationInView:doubleTap.view].y, secondsAsDuration(MGLAnimationDuration));

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

    if (twoFingerTap.state == UIGestureRecognizerStateEnded)
    {
        mbglMap->scaleBy(0.5, [twoFingerTap locationInView:twoFingerTap.view].x, [twoFingerTap locationInView:twoFingerTap.view].y, secondsAsDuration(MGLAnimationDuration));

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
        self.scale = mbglMap->getScale();

        self.quickZoomStart = [quickZoom locationInView:quickZoom.view].y;
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

        [self notifyMapChange:@(mbgl::MapChangeRegionDidChangeAnimated)];
    }
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    NSArray *validSimultaneousGestures = @[ self.pan, self.pinch, self.rotate ];

    return ([validSimultaneousGestures containsObject:gestureRecognizer] && [validSimultaneousGestures containsObject:otherGestureRecognizer]);
}

#pragma mark - Properties -

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
}

- (void)toggleDebug
{
    mbglMap->toggleDebug();
}

#pragma mark - Geography -

- (void)setCenterCoordinate:(CLLocationCoordinate2D)coordinate animated:(BOOL)animated
{
    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    mbglMap->setLatLng(coordinateToLatLng(coordinate), secondsAsDuration(duration));
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
    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    mbglMap->setLatLngZoom(coordinateToLatLng(centerCoordinate), zoomLevel, secondsAsDuration(duration));

    [self unrotateIfNeededAnimated:animated];
}

- (double)zoomLevel
{
    return mbglMap->getZoom();
}

- (void)setZoomLevel:(double)zoomLevel animated:(BOOL)animated
{
    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    mbglMap->setZoom(zoomLevel, secondsAsDuration(duration));

    [self unrotateIfNeededAnimated:animated];
}

- (void)setZoomLevel:(double)zoomLevel
{
    [self setZoomLevel:zoomLevel animated:NO];
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

    CGFloat duration = (animated ? MGLAnimationDuration : 0);

    mbglMap->setBearing(direction * -1, secondsAsDuration(duration));
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
    mbgl::vec2<double> pixel = mbglMap->pixelForLatLng(mbgl::LatLng(coordinate.latitude, coordinate.longitude));

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

- (NSDictionary *)getRawStyle
{
    const std::string styleJSON = mbglMap->getStyleJSON();

    return [NSJSONSerialization JSONObjectWithData:[@(styleJSON.c_str()) dataUsingEncoding:[NSString defaultCStringEncoding]] options:0 error:nil];
}

- (void)setRawStyle:(NSDictionary *)style
{
    NSData *data = [NSJSONSerialization dataWithJSONObject:style options:0 error:nil];

    [self setStyleJSON:[[NSString alloc] initWithData:data encoding:[NSString defaultCStringEncoding]]];
}

- (NSArray *)bundledStyleNames
{
    if (!_bundledStyleNames) {
        NSString *stylesPath = [[MGLMapView resourceBundlePath] stringByAppendingString:@"/styles"];

        _bundledStyleNames = [NSMutableArray array];

        NSArray *bundledStyleNamesWithExtensions = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:stylesPath error:nil];
        NSString *hybridStylePrefix = @"hybrid-";
        NSString *satelliteStylePrefix = @"satellite-";
        for (NSString *fileName in bundledStyleNamesWithExtensions) {
            NSString *styleName = [fileName stringByDeletingPathExtension];
            [_bundledStyleNames addObject:styleName];

            // Add satellite raster & "hybrid" (satellite raster + vector contours & labels)
            if ([styleName hasPrefix:satelliteStylePrefix]) {
                [_bundledStyleNames addObject:[hybridStylePrefix stringByAppendingString:[styleName substringFromIndex:[satelliteStylePrefix length]]]];
            }
        }
    }

    return [NSArray arrayWithArray:_bundledStyleNames];
}

- (void)useBundledStyleNamed:(NSString *)styleName
{
    NSString *hybridStylePrefix = @"hybrid-";
    BOOL isHybrid = [styleName hasPrefix:hybridStylePrefix];
    if (isHybrid) {
        styleName = [@"satellite-" stringByAppendingString:[styleName substringFromIndex:[hybridStylePrefix length]]];
    }
    [self setStyleURL:[NSString stringWithFormat:@"styles/%@.json", styleName]];
    if (isHybrid) {
        [self setStyleClasses:@[@"contours", @"labels"]];
    }
}

- (NSArray *)getStyleOrderedLayerNames
{
    return [[self getRawStyle] valueForKeyPath:@"layers.id"];
}

- (void)setStyleOrderedLayerNames:(NSArray *)orderedLayerNames
{
    NSMutableDictionary *style = [[self getRawStyle] deepMutableCopy];
    NSArray *oldLayers = style[@"layers"];
    NSMutableArray *newLayers = [NSMutableArray array];

    if ([orderedLayerNames count] != [[oldLayers valueForKeyPath:@"id"] count])
    {
        [NSException raise:@"invalid layer count"
                    format:@"new layer count (%lu) should equal existing layer count (%lu)",
                        (unsigned long)[orderedLayerNames count],
                        (unsigned long)[[oldLayers valueForKeyPath:@"id"] count]];
    }
    else
    {
        for (NSString *newLayerName in orderedLayerNames)
        {
            if ( ! [[oldLayers valueForKeyPath:@"id"] containsObject:newLayerName])
            {
                [NSException raise:@"invalid layer name"
                            format:@"layer name %@ unknown",
                                newLayerName];
            }
            else
            {
                NSDictionary *newLayer = [oldLayers objectAtIndex:[[oldLayers valueForKeyPath:@"id"] indexOfObject:newLayerName]];
                [newLayers addObject:newLayer];
            }
        }
    }

    [style setValue:newLayers forKey:@"layers"];

    [self setRawStyle:style];
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
        newAppliedClasses.insert(newAppliedClasses.end(), [appliedClass cStringUsingEncoding:[NSString defaultCStringEncoding]]);
    }

    mbglMap->setDefaultTransitionDuration(secondsAsDuration(transitionDuration));
    mbglMap->setClasses(newAppliedClasses);
}

- (NSString *)getKeyTypeForLayer:(NSString *)layerName
{
    NSDictionary *style = [self getRawStyle];

    NSString *bucketType;

    if ([layerName isEqualToString:@"background"])
    {
        bucketType = @"background";
    }
    else
    {
        for (NSDictionary *layer in style[@"structure"])
        {
            if ([layer[@"name"] isEqualToString:layerName])
            {
                bucketType = style[@"buckets"][layer[@"bucket"]][@"type"];
                break;
            }
        }
    }

    NSString *keyType;

    if ([bucketType isEqualToString:@"fill"])
    {
        keyType = MGLStyleKeyFill;
    }
    else if ([bucketType isEqualToString:@"line"])
    {
        keyType = MGLStyleKeyLine;
    }
    else if ([bucketType isEqualToString:@"point"])
    {
        keyType = MGLStyleKeyIcon;
    }
    else if ([bucketType isEqualToString:@"text"])
    {
        keyType = MGLStyleKeyText;
    }
    else if ([bucketType isEqualToString:@"raster"])
    {
        keyType = MGLStyleKeyRaster;
    }
    else if ([bucketType isEqualToString:@"composite"])
    {
        keyType = MGLStyleKeyComposite;
    }
    else if ([bucketType isEqualToString:@"background"])
    {
        keyType = MGLStyleKeyBackground;
    }
    else
    {
        [NSException raise:@"invalid bucket type"
                    format:@"bucket type %@ unknown",
                        bucketType];
    }

    return keyType;
}

- (NSDictionary *)getStyleDescriptionForLayer:(NSString *)layerName inClass:(NSString *)className
{
    NSDictionary *style = [self getRawStyle];

    if ( ! [[style valueForKeyPath:@"classes.name"] containsObject:className])
    {
        [NSException raise:@"invalid class name"
                    format:@"class name %@ unknown",
                        className];
    }

    NSUInteger classNumber = [[style valueForKeyPath:@"classes.name"] indexOfObject:className];

    if ( ! [[style[@"classes"][classNumber][@"layers"] allKeys] containsObject:layerName])
    {
        // layer specified in structure, but not styled
        //
        return nil;
    }

    NSDictionary *layerStyle = style[@"classes"][classNumber][@"layers"][layerName];

    NSMutableDictionary *styleDescription = [NSMutableDictionary dictionary];

    for (NSString *keyName in [layerStyle allKeys])
    {
        id value = layerStyle[keyName];

        while ([[style[@"constants"] allKeys] containsObject:value])
        {
            value = style[@"constants"][value];
        }

        if ([[self.allowedStyleTypes[MGLStyleKeyGeneric] allKeys] containsObject:keyName])
        {
            [styleDescription setValue:[self typedPropertyForKeyName:keyName
                                                              ofType:MGLStyleKeyGeneric
                                                           withValue:value]
                                forKey:keyName];
        }

        NSString *keyType = [self getKeyTypeForLayer:layerName];

        if ([[self.allowedStyleTypes[keyType] allKeys] containsObject:keyName])
        {
            [styleDescription setValue:[self typedPropertyForKeyName:keyName
                                                              ofType:keyType
                                                           withValue:value]
                                forKey:keyName];
        }
    }

    return styleDescription;
}

- (NSDictionary *)typedPropertyForKeyName:(NSString *)keyName ofType:(NSString *)keyType withValue:(id)value
{
    if ( ! [[self.allowedStyleTypes[keyType] allKeys] containsObject:keyName])
    {
        [NSException raise:@"invalid property name"
                    format:@"property name %@ unknown",
                        keyName];
    }

    NSArray *typeInfo = self.allowedStyleTypes[keyType][keyName];

    if ([value isKindOfClass:[NSArray class]] && ! [typeInfo containsObject:MGLStyleValueTypeColor])
    {
        if ([typeInfo containsObject:MGLStyleValueFunctionAllowed])
        {
            if ([[(NSArray *)value firstObject] isKindOfClass:[NSString class]])
            {
                NSString *functionType;

                if ([[(NSArray *)value firstObject] isEqualToString:@"linear"])
                {
                    functionType = MGLStyleValueTypeFunctionLinear;
                }
                else if ([[(NSArray *)value firstObject] isEqualToString:@"stops"])
                {
                    functionType = MGLStyleValueTypeFunctionStops;
                }
                else if ([[(NSArray *)value firstObject] isEqualToString:@"exponential"])
                {
                    functionType = MGLStyleValueTypeFunctionExponential;
                }
                else if ([[(NSArray *)value firstObject] isEqualToString:@"min"])
                {
                    functionType = MGLStyleValueTypeFunctionMinimumZoom;
                }
                else if ([[(NSArray *)value firstObject] isEqualToString:@"max"])
                {
                    functionType = MGLStyleValueTypeFunctionMaximumZoom;
                }

                if (functionType)
                {
                    return @{ @"type"  : functionType,
                              @"value" : value };
                }
            }
        }
        else if ([typeInfo containsObject:MGLStyleValueTypeNumberPair])
        {
            return @{ @"type"  : MGLStyleValueTypeNumberPair,
                      @"value" : value };
        }
    }
    else if ([typeInfo containsObject:MGLStyleValueTypeNumber])
    {
        return @{ @"type"  : MGLStyleValueTypeNumber,
                  @"value" : value };
    }
    else if ([typeInfo containsObject:MGLStyleValueTypeBoolean])
    {
        return @{ @"type"  : MGLStyleValueTypeBoolean,
                  @"value" : @([(NSString *)value boolValue]) };
    }
    else if ([typeInfo containsObject:MGLStyleValueTypeString])
    {
        return @{ @"type"  : MGLStyleValueTypeString,
                  @"value" : value };
    }
    else if ([typeInfo containsObject:MGLStyleValueTypeColor])
    {
        UIColor *color;

        if ([(NSString *)value hasPrefix:@"#"])
        {
            color = [UIColor colorWithHexString:value];
        }
        else if ([(NSString *)value hasPrefix:@"rgb"])
        {
            color = [UIColor colorWithRGBAString:value];
        }
        else if ([(NSString *)value hasPrefix:@"hsl"])
        {
            [NSException raise:@"invalid color format"
                        format:@"HSL color format not yet supported natively"];
        }
        else if ([value isKindOfClass:[NSArray class]] && [(NSArray *)value count] == 4)
        {
            color = [UIColor colorWithRed:[value[0] floatValue]
                                    green:[value[1] floatValue]
                                     blue:[value[2] floatValue]
                                    alpha:[value[3] floatValue]];
        }
        else if ([[UIColor class] respondsToSelector:NSSelectorFromString([NSString stringWithFormat:@"%@Color", [(NSString *)value lowercaseString]])])
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"

            color = [[UIColor class] performSelector:NSSelectorFromString([NSString stringWithFormat:@"%@Color", [(NSString *)value lowercaseString]])];

#pragma clang diagnostic pop
        }

        return @{ @"type"  : MGLStyleValueTypeColor,
                  @"value" : color };
    }

    return nil;
}

- (void)setStyleDescription:(NSDictionary *)styleDescription forLayer:(NSString *)layerName inClass:(NSString *)className
{
#pragma unused(className)

    NSMutableDictionary *convertedStyle = [NSMutableDictionary dictionary];

    for (NSString *key in [styleDescription allKeys])
    {
        NSArray *styleParameters = nil;

        if ([[self.allowedStyleTypes[MGLStyleKeyGeneric] allKeys] containsObject:key])
        {
            styleParameters = self.allowedStyleTypes[MGLStyleKeyGeneric][key];
        }
        else
        {
            NSString *keyType = [self getKeyTypeForLayer:layerName];

            if ([[self.allowedStyleTypes[keyType] allKeys] containsObject:key])
            {
                styleParameters = self.allowedStyleTypes[keyType][key];
            }
        }

        if (styleParameters)
        {
            if ([styleDescription[key][@"value"] isKindOfClass:[MGLStyleFunctionValue class]])
            {
                convertedStyle[key] = [(MGLStyleFunctionValue *)styleDescription[key][@"value"] rawStyle];
            }
            else if ([styleParameters containsObject:styleDescription[key][@"type"]])
            {
                NSString *valueType = styleDescription[key][@"type"];

                if ([valueType isEqualToString:MGLStyleValueTypeColor])
                {
                    convertedStyle[key] = [@"#" stringByAppendingString:[(UIColor *)styleDescription[key][@"value"] hexStringFromColor]];
                }
                else
                {
                    // the rest (bool/number/pair/string) are already JSON-convertible types
                    //
                    convertedStyle[key] = styleDescription[key][@"value"];
                }
            }
        }
        else
        {
            [NSException raise:@"invalid style description format"
                        format:@"unable to parse key '%@'",
                            key];
        }
    }

//    NSMutableDictionary *style = [[self getRawStyle] deepMutableCopy];
//
//    NSUInteger classIndex = [[[self getAllStyleClasses] valueForKey:@"name"] indexOfObject:className];
//
//    style[@"classes"][classIndex][@"layers"][layerName] = convertedStyle;
//
//    [self setRawStyle:style];
}

- (NSDictionary *)allowedStyleTypes
{
    static NSDictionary *MGLStyleAllowedTypes = @{
        MGLStyleKeyGeneric : @{
            @"enabled" : @[ MGLStyleValueTypeBoolean, MGLStyleValueFunctionAllowed ],
            @"translate" : @[ MGLStyleValueTypeNumberPair, MGLStyleValueFunctionAllowed ],
            @"translate-anchor" : @[ MGLStyleValueTypeString, MGLStyleValueFunctionAllowed ],
            @"opacity" : @[ MGLStyleValueTypeNumber, MGLStyleValueFunctionAllowed ],
            @"prerender" : @[ MGLStyleValueTypeBoolean ],
            @"prerender-buffer" : MGLStyleValueTypeNumber,
            @"prerender-size" : @[ MGLStyleValueTypeNumber ],
            @"prerender-blur" : @[ MGLStyleValueTypeNumber ] },
        MGLStyleKeyFill : @{
            @"color" : @[ MGLStyleValueTypeColor ],
            @"stroke" : @[ MGLStyleValueTypeColor ],
            @"antialias" : @[ MGLStyleValueTypeBoolean ],
            @"image" : @[ MGLStyleValueTypeString ] },
        MGLStyleKeyLine : @{
            @"color" : @[ MGLStyleValueTypeColor ],
            @"width" : @[ MGLStyleValueTypeNumber, MGLStyleValueFunctionAllowed ],
            @"dasharray" : @[ MGLStyleValueTypeNumberPair, MGLStyleValueFunctionAllowed ] },
        MGLStyleKeyIcon : @{
            @"color" : @[ MGLStyleValueTypeColor ],
            @"image" : @[ MGLStyleValueTypeString ],
            @"size" : @[ MGLStyleValueTypeNumber, MGLStyleValueFunctionAllowed ],
            @"radius" : @[ MGLStyleValueTypeNumber, MGLStyleValueFunctionAllowed],
            @"blur" : @[ MGLStyleValueTypeNumber, MGLStyleValueFunctionAllowed ] },
        MGLStyleKeyText : @{
            @"color" : @[ MGLStyleValueTypeColor ],
            @"stroke" : @[ MGLStyleValueTypeColor ],
            @"strokeWidth" : @[ MGLStyleValueTypeNumber, MGLStyleValueFunctionAllowed ],
            @"strokeBlur" : @[ MGLStyleValueTypeNumber, MGLStyleValueFunctionAllowed ],
            @"size" : @[ MGLStyleValueTypeNumber, MGLStyleValueFunctionAllowed ],
            @"rotate" : @[ MGLStyleValueTypeNumber, MGLStyleValueFunctionAllowed ],
            @"alwaysVisible" : @[ MGLStyleValueTypeBoolean ] },
        MGLStyleKeyRaster : @{},
        MGLStyleKeyComposite : @{},
        MGLStyleKeyBackground : @{
            @"color" : @[ MGLStyleValueTypeColor ] }
        };

    return MGLStyleAllowedTypes;
}

#pragma mark - Annotations -

- (NSArray *)annotations
{
    if ([_annotationsStore count])
    {
        NSMutableArray *result = [NSMutableArray array];

        NSEnumerator *keyEnumerator = [_annotationsStore keyEnumerator];
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

        symbols.push_back((symbolName ? [symbolName cStringUsingEncoding:[NSString defaultCStringEncoding]] : ""));
    }

    std::vector<uint32_t> annotationIDs = mbglMap->addPointAnnotations(latLngs, symbols);

    for (size_t i = 0; i < annotationIDs.size(); ++i)
    {
        [self.annotationsStore setObject:@{ MGLAnnotationIDKey : @(annotationIDs[i]) }
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

        annotationIDsToRemove.push_back([[[self.annotationsStore objectForKey:annotation] objectForKey:MGLAnnotationIDKey] unsignedIntValue]);
        [self.annotationsStore removeObjectForKey:annotation];
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

    self.selectedAnnotation = firstAnnotation;
}

- (void)selectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated
{
    (void)animated;

    if ( ! annotation) return;

    if ( ! [self viewportBounds].contains(coordinateToLatLng(annotation.coordinate))) return;

    self.selectedAnnotation = annotation;
}

- (void)deselectAnnotation:(id <MGLAnnotation>)annotation animated:(BOOL)animated
{
    (void)animated;

    if ( ! annotation) return;

    if ([self.selectedAnnotation isEqual:annotation]) self.selectedAnnotation = nil;
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

- (BOOL)isRotationAllowed
{
    return (self.zoomLevel > 3);
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
        case mbgl::MapChangeRegionDidChange:
        case mbgl::MapChangeRegionDidChangeAnimated:
        {
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
    // This is run in the main/UI thread.
    [self.glView setNeedsDisplay];
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
            dispatch_async(dispatch_get_main_queue(), ^
            {
                [nativeView performSelector:@selector(notifyMapChange:)
                                 withObject:@(change)
                                 afterDelay:0];
            });
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
