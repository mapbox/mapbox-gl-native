#import "MBXViewController.h"

#import "MBXSettings.h"

#import "../common/foundation_request.h"

#import <GLKit/GLKit.h>
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>

#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/map/tile.hpp>

@interface MBXViewController () <UIGestureRecognizerDelegate>

@property (nonatomic) EAGLContext *context;
@property (nonatomic) CGPoint center;
@property (nonatomic) CGFloat scale;
@property (nonatomic) CGFloat angle;
@property (nonatomic) CGFloat quickZoomStart;
@property (nonatomic) BOOL debug;
@property (nonatomic) UIView *palette;
@property (nonatomic) NSTimeInterval elapsed;

@end

@implementation MBXViewController

llmr::Map *map = nullptr;
llmr::Settings_iOS *settings = nullptr;
MBXViewController *view = nullptr;

#pragma mark - Setup

- (void)viewDidLoad
{
    [super viewDidLoad];

    view = self;

    self.preferredFramesPerSecond = 60;

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create OpenGL ES context");
    }

    GLKView *view = (GLKView *)self.view;

    view.context = self.context;
    view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat16;

    [view bindDrawable];

    [self setupMap];
    [self setupInteraction];
    [self setupDebugUI];
}

- (void)setupMap
{
    [EAGLContext setCurrentContext:self.context];

    settings = new llmr::Settings_iOS();
    settings->load();

    map = new llmr::Map(*settings);
    map->setup();
    map->loadSettings();

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateRender:) name:MBXNeedsRenderNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateNetworkActivity:) name:MBXUpdateActivityNotification object:nil];
}


- (void)setupInteraction
{
    UIPanGestureRecognizer *pan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
    pan.delegate = self;
    [self.view addGestureRecognizer:pan];

    UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)];
    pinch.delegate = self;
    [self.view addGestureRecognizer:pinch];

    UIRotationGestureRecognizer *rotate = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotateGesture:)];
    rotate.delegate = self;
    [self.view addGestureRecognizer:rotate];

    UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTapGesture:)];
    singleTap.numberOfTapsRequired = 1;
    [self.view addGestureRecognizer:singleTap];

    UITapGestureRecognizer *doubleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleTapGesture:)];
    doubleTap.numberOfTapsRequired = 2;
    [self.view addGestureRecognizer:doubleTap];

    [singleTap requireGestureRecognizerToFail:doubleTap];

    UITapGestureRecognizer *twoFingerTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTwoFingerTapGesture:)];
    twoFingerTap.numberOfTouchesRequired = 2;
    [self.view addGestureRecognizer:twoFingerTap];

    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
    {
        UILongPressGestureRecognizer *quickZoom = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleQuickZoomGesture:)];
        quickZoom.numberOfTapsRequired = 1;
        quickZoom.minimumPressDuration = 0.25;
        [self.view addGestureRecognizer:quickZoom];
    }
}

- (void)setupDebugUI
{
    NSArray *selectorNames = @[ @"unrotate", @"resetPosition", @"toggleDebug", @"toggleRaster" ];
    CGFloat buttonSize  = 40;
    CGFloat bufferSize  = 20;
    CGFloat alpha       = 0.75;
    CGFloat paletteWidth  = buttonSize + (2 * bufferSize);
    CGFloat paletteHeight = [selectorNames count] * (buttonSize + bufferSize) + bufferSize;
    self.palette = [[UIView alloc] initWithFrame:CGRectMake(self.view.bounds.size.width - paletteWidth,
                                                            (self.view.bounds.size.height - paletteHeight) / 2,
                                                            paletteWidth,
                                                            paletteHeight)];
    self.palette.backgroundColor = [UIColor colorWithWhite:0 alpha:alpha];
    self.palette.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin;
    self.palette.layer.cornerRadius = bufferSize;
    [self.view addSubview:self.palette];
    for (NSString *selectorName in selectorNames)
    {
        UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
        button.frame = CGRectMake(bufferSize,
                                  ([selectorNames indexOfObject:selectorName] * (buttonSize + bufferSize)) + bufferSize,
                                  buttonSize,
                                  buttonSize);
        [button setImage:[UIImage imageNamed:[selectorName stringByAppendingString:@".png"]] forState:UIControlStateNormal];
        [button addTarget:self action:NSSelectorFromString(selectorName) forControlEvents:UIControlEventTouchUpInside];
        [self.palette addSubview:button];
    }
}

- (NSUInteger)supportedInterfaceOrientations
{
    // We support everything; it's up to the embedding application to limit the orientations.
    return UIInterfaceOrientationMaskAll;
}

#pragma mark - NotificationCenter callbacks

- (void)updateRender:(NSNotification *)notification
{
    [self updateRender];
}

- (void)updateRender
{
    self.paused = NO;
}

- (void)updateNetworkActivity:(NSNotification *)notification
{
    [[NSURLSession sharedSession] getTasksWithCompletionHandler:^(NSArray *dataTasks, NSArray *uploadTasks, NSArray *downloadTasks)
     {
         [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:([downloadTasks count] > 0)];
     }];
}

#pragma mark - Rendering delegates

- (void)viewWillLayoutSubviews
{
    // This callback is called whenever the view dimensions change. We trigger an update to have a fresh render to display
    // while the view change is transitioned.
    [super viewWillLayoutSubviews];

    // TODO: Pause all current animations during animation. We don't want other events to fire while the view is rotated.

    [self updateRender];
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    self.elapsed = CACurrentMediaTime();

    // Render the map
    map->resize(rect.size.width, rect.size.height, view.contentScaleFactor, view.drawableWidth, view.drawableHeight);

    if (map->render() == false) {
        self.paused = YES;
        settings->sync();
    }
}

#pragma mark - Debugging UI

- (void)togglePalette
{
    if (self.palette.alpha < 1)
    {
        self.palette.userInteractionEnabled = YES;

        [UIView animateWithDuration:0.25 animations:^(void)
        {
            self.palette.alpha = 1;
        }];
    }
    else
    {
        self.palette.userInteractionEnabled = NO;

        [UIView animateWithDuration:0.25 animations:^(void)
        {
            self.palette.alpha = 0;
        }];
    }
}

- (void)unrotate
{
    map->resetNorth();
}

- (void)resetPosition
{
    map->resetPosition();
}

- (void)toggleDebug
{
    map->toggleDebug();

    self.debug = ! self.debug;
}

- (void)toggleRaster
{
    map->toggleRaster();
}

#pragma mark - Destruction

- (void)dealloc
{
    if (map) {
        delete map;
        map = nullptr;
    }
    if (settings) {
        settings->sync();
        delete settings;
        settings = nullptr;
    }

    [[NSNotificationCenter defaultCenter] removeObserver:self];

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

#pragma mark - UI gestures

- (void)handlePanGesture:(UIPanGestureRecognizer *)pan
{
    map->cancelAnimations();

    if (pan.state == UIGestureRecognizerStateBegan)
    {
        self.center = CGPointMake(0, 0);
    }
    else if (pan.state == UIGestureRecognizerStateChanged)
    {
        CGPoint delta = CGPointMake([pan translationInView:pan.view].x - self.center.x,
                                    [pan translationInView:pan.view].y - self.center.y);

        map->moveBy(delta.x, delta.y);

        self.center = CGPointMake(self.center.x + delta.x, self.center.y + delta.y);
    }
    else if (pan.state == UIGestureRecognizerStateEnded)
    {
        if ([pan velocityInView:pan.view].x < 50 && [pan velocityInView:pan.view].y < 50) {
            return;
        }

        CGPoint finalCenter = CGPointMake(self.center.x + (0.1 * [pan velocityInView:pan.view].x),
                                          self.center.y + (0.1 * [pan velocityInView:pan.view].y));

        CGFloat duration = ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad ? 0.3 : 0.5);

        map->moveBy(finalCenter.x - self.center.x, finalCenter.y - self.center.y, duration);
    }

    [self updateRender];
}

- (void)handlePinchGesture:(UIPinchGestureRecognizer *)pinch
{
    self.elapsed = CACurrentMediaTime();

    map->cancelAnimations();

    if (pinch.state == UIGestureRecognizerStateBegan)
    {
        map->startScaling();

        self.scale = map->getScale();
    }
    else if (pinch.state == UIGestureRecognizerStateChanged)
    {
        CGFloat tolerance  = 2.5;
        CGFloat adjustment = 0;

        if (pinch.scale > 1)
        {
            adjustment = (pinch.scale / tolerance) - (1 / tolerance);
        }
        else
        {
            adjustment = (-1 / pinch.scale) / tolerance + (1 / tolerance);
        }

        CGFloat newZoom = log2f(self.scale) + adjustment;

        CGPoint pt = [pinch locationInView:pinch.view];
        map->scaleBy(powf(2, newZoom) / map->getScale(), pt.x, pt.y);
    }
    else if (pinch.state == UIGestureRecognizerStateEnded)
    {
       map->stopScaling();

        if (fabsf(pinch.velocity) < 20) {
            return;
        }

        CGFloat finalZoom = log2f(map->getScale()) + (0.01 * pinch.velocity);

        double scale = map->getScale();
        double new_scale = powf(2, finalZoom);

        CGFloat duration = ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad ? 0.3 : 0.5);

        CGPoint pt = [pinch locationInView:pinch.view];
        map->scaleBy(new_scale / scale, pt.x, pt.y, duration);
    }
    else if (pinch.state == UIGestureRecognizerStateCancelled)
    {
        map->stopScaling();
    }

    [self updateRender];
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    self.elapsed = CACurrentMediaTime();

    map->cancelAnimations();

    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        map->startRotating();

        self.angle = map->getAngle();
    }
    else if (rotate.state == UIGestureRecognizerStateChanged)
    {
        CGPoint pt = [rotate locationInView:rotate.view];
        map->setAngle(self.angle + rotate.rotation, pt.x, pt.y);
    }
    else if (rotate.state == UIGestureRecognizerStateEnded || rotate.state == UIGestureRecognizerStateCancelled)
    {
        map->stopRotating();
    }

    [self updateRender];
}

- (void)handleSingleTapGesture:(UITapGestureRecognizer *)singleTap
{
    self.elapsed = CACurrentMediaTime();

    [self togglePalette];
}

- (void)handleDoubleTapGesture:(UITapGestureRecognizer *)doubleTap
{
    self.elapsed = CACurrentMediaTime();

    map->cancelAnimations();

    if (doubleTap.state == UIGestureRecognizerStateEnded) {
        CGPoint pt = [doubleTap locationInView:doubleTap.view];
        map->scaleBy(2, pt.x, pt.y, 0.5);
    }

    [self updateRender];
}

- (void)handleTwoFingerTapGesture:(UITapGestureRecognizer *)twoFingerTap
{
    self.elapsed = CACurrentMediaTime();

    map->cancelAnimations();

    if (twoFingerTap.state == UIGestureRecognizerStateEnded) {
        CGPoint pt = [twoFingerTap locationInView:twoFingerTap.view];
        map->scaleBy(0.5, pt.x, pt.y, 0.5);
    }

    [self updateRender];
}

- (void)handleQuickZoomGesture:(UILongPressGestureRecognizer *)quickZoom
{
    self.elapsed = CACurrentMediaTime();

    map->cancelAnimations();

    if (quickZoom.state == UIGestureRecognizerStateBegan)
    {
        self.scale = map->getScale();

        self.quickZoomStart = [quickZoom locationInView:quickZoom.view].y;
    }
    else if (quickZoom.state == UIGestureRecognizerStateChanged)
    {
        CGFloat distance = self.quickZoomStart - [quickZoom locationInView:quickZoom.view].y;

        CGFloat newZoom = log2f(self.scale) + (distance / 100);

        map->scaleBy(powf(2, newZoom) / map->getScale(), self.view.bounds.size.width / 2, self.view.bounds.size.height / 2);
    }

    [self updateRender];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    NSArray *validSimultaneousGestures = @[ [UIPanGestureRecognizer class], [UIPinchGestureRecognizer class], [UIRotationGestureRecognizer class] ];

    return ([validSimultaneousGestures containsObject:[gestureRecognizer class]] && [validSimultaneousGestures containsObject:[otherGestureRecognizer class]]);
}


namespace llmr {
    namespace platform {

        class Request {
        public:
            int16_t identifier;
            std::string original_url;
        };

        void restart() {
            [[NSNotificationCenter defaultCenter] postNotificationName:MBXNeedsRenderNotification object:nil];
        }

        double time() {
            return [view timeSinceFirstResume];
        }

        double elapsed() {
            return view.elapsed;
        }
    }
}

@end

