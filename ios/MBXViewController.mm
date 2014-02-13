//
//  MBXViewController.m
//  ios
//
//  Created by Justin R. Miller on 1/27/14.
//
//

#import "MBXViewController.h"

#import "MBXSettings.h"

#import <GLKit/GLKit.h>
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>

#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>

NSString *const MBXNeedsRenderNotification = @"MBXNeedsRenderNotification";
NSString *const MBXUpdateActivityNotification = @"MBXUpdateActivityNotification";

@interface MBXViewController () <UIGestureRecognizerDelegate>

@property (nonatomic) EAGLContext *context;
@property (nonatomic) NSUInteger activityCount;
@property (nonatomic) CGPoint center;
@property (nonatomic) CGFloat scale;
@property (nonatomic) CGFloat angle;
@property (nonatomic) CGFloat quickZoomStart;
@property (nonatomic) BOOL debug;

@end

@implementation MBXViewController

class MBXMapView
{
    public:
        MBXMapView() : settings(), map(settings)
        {
        }

        ~MBXMapView()
        {
        }

        void init()
        {
            settings.load();

            map.setup();

            CGRect frame = [[UIScreen mainScreen] bounds];
            map.resize(frame.size.width, frame.size.height, frame.size.width, frame.size.height);

            map.loadSettings();
        }

    public:
        llmr::Settings_iOS settings;
        llmr::Map map;
};

- (void)loadView
{
    [super loadView];

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if ( ! self.context)
    {
        NSLog(@"Failed to initialize OpenGL ES context");
        return;
    }

    GLKView *view = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds] context:self.context];

    view.enableSetNeedsDisplay = NO;
    view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    [view bindDrawable];

    [EAGLContext setCurrentContext:self.context];

    self.view = view;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    UIPanGestureRecognizer *pan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
    pan.delegate = self;
    [self.view addGestureRecognizer:pan];

    UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)];
    pinch.delegate = self;
    [self.view addGestureRecognizer:pinch];

    UIRotationGestureRecognizer *rotate = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotateGesture:)];
    rotate.delegate = self;
    [self.view addGestureRecognizer:rotate];

    UITapGestureRecognizer *doubleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleTapGesture:)];
    doubleTap.numberOfTapsRequired = 2;
    [self.view addGestureRecognizer:doubleTap];

    UITapGestureRecognizer *twoFingerTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTwoFingerTapGesture:)];
    twoFingerTap.numberOfTouchesRequired = 2;
    [self.view addGestureRecognizer:twoFingerTap];

    UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
    [self.view addGestureRecognizer:longPress];

    UILongPressGestureRecognizer *twoFingerLongPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleTwoFingerLongPressGesture:)];
    twoFingerLongPress.numberOfTouchesRequired = 2;
    [self.view addGestureRecognizer:twoFingerLongPress];

    UILongPressGestureRecognizer *threeFingerLongPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleThreeFingerLongPressGesture:)];
    threeFingerLongPress.numberOfTouchesRequired = 3;
    [self.view addGestureRecognizer:threeFingerLongPress];

    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)
    {
        UILongPressGestureRecognizer *quickZoom = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleQuickZoomGesture:)];
        quickZoom.numberOfTapsRequired = 1;
        quickZoom.minimumPressDuration = 0.25;
        [self.view addGestureRecognizer:quickZoom];
    }

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(startRender:) name:MBXNeedsRenderNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateNetworkActivity:) name:MBXUpdateActivityNotification object:nil];

    mapView = new MBXMapView();

    mapView->init();

    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)startRender:(NSNotification *)notification
{
    [self startRender];
}

- (void)updateNetworkActivity:(NSNotification *)notification
{
    NSInteger input = [[notification userInfo][@"count"] integerValue];

    self.activityCount += input;

    [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:(self.activityCount > 0)];
}

- (void)startRender
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self];

    displayLink.paused = NO;

    [self performSelector:@selector(stopRender) withObject:nil afterDelay:3.0];
}

- (void)stopRender
{
    displayLink.paused = YES;
}

- (void)render:(id)sender
{
    mapView->map.render();

    static double frames  = 0;
    static double elapsed = 0;

    frames++;

    double current = [[NSDate date] timeIntervalSince1970];

    if (current - elapsed >= 1)
    {
        if (self.debug)
            NSLog(@"FPS: %f", frames / (current - elapsed));

        elapsed = current;
        frames = 0;
    }

    [(GLKView *)self.view display];
}

- (void)handlePanGesture:(UIPanGestureRecognizer *)pan
{
    mapView->map.cancelAnimations();

    if (pan.state == UIGestureRecognizerStateBegan)
    {
        self.center = CGPointMake(0, 0);
    }
    else if (pan.state == UIGestureRecognizerStateChanged)
    {
        CGPoint delta = CGPointMake([pan translationInView:pan.view].x - self.center.x,
                                    [pan translationInView:pan.view].y - self.center.y);

        mapView->map.moveBy(delta.x, delta.y);

        self.center = CGPointMake(self.center.x + delta.x, self.center.y + delta.y);
    }
    else if (pan.state == UIGestureRecognizerStateEnded)
    {
        if ([pan velocityInView:pan.view].x < 50 && [pan velocityInView:pan.view].y < 50)
            return;

        CGPoint finalCenter = CGPointMake(self.center.x + (0.1 * [pan velocityInView:pan.view].x),
                                          self.center.y + (0.1 * [pan velocityInView:pan.view].y));

        CGFloat duration = ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad ? 0.3 : 0.5);

        mapView->map.moveBy(finalCenter.x - self.center.x, finalCenter.y - self.center.y, duration);
    }

    [self startRender];
}

- (void)handlePinchGesture:(UIPinchGestureRecognizer *)pinch
{
    mapView->map.cancelAnimations();

    if (pinch.state == UIGestureRecognizerStateBegan)
    {
        self.scale = mapView->map.getScale();
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

        mapView->map.scaleBy(powf(2, newZoom) / mapView->map.getScale(), [pinch locationInView:pinch.view].x, [pinch locationInView:pinch.view].y);
    }
    else if (pinch.state == UIGestureRecognizerStateEnded)
    {
        if (fabsf(pinch.velocity) < 20)
            return;

        CGFloat finalZoom = log2f(mapView->map.getScale()) + (0.01 * pinch.velocity);

        double scale = mapView->map.getScale();
        double new_scale = powf(2, finalZoom);

        CGFloat duration = ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad ? 0.3 : 0.5);

        mapView->map.scaleBy(new_scale / scale, [pinch locationInView:pinch.view].x, [pinch locationInView:pinch.view].y, duration);
    }

    [self startRender];
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    mapView->map.cancelAnimations();

    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        self.angle = mapView->map.getAngle();
    }
    else if (rotate.state == UIGestureRecognizerStateChanged)
    {
        mapView->map.setAngle(self.angle + rotate.rotation, [rotate locationInView:rotate.view].x, [rotate locationInView:rotate.view].y);
    }

    [self startRender];
}

- (void)handleDoubleTapGesture:(UITapGestureRecognizer *)doubleTap
{
    mapView->map.cancelAnimations();

    if (doubleTap.state == UIGestureRecognizerStateEnded)
        mapView->map.scaleBy(2, [doubleTap locationInView:doubleTap.view].x, [doubleTap locationInView:doubleTap.view].y, 0.5);

    [self startRender];
}

- (void)handleTwoFingerTapGesture:(UITapGestureRecognizer *)twoFingerTap
{
    mapView->map.cancelAnimations();

    if (twoFingerTap.state == UIGestureRecognizerStateEnded)
        mapView->map.scaleBy(0.5, [twoFingerTap locationInView:twoFingerTap.view].x, [twoFingerTap locationInView:twoFingerTap.view].y, 0.5);

    [self startRender];
}

- (void)handleLongPressGesture:(UILongPressGestureRecognizer *)longPress
{
    mapView->map.cancelAnimations();

    if (longPress.state == UIGestureRecognizerStateBegan)
        mapView->map.resetNorth();

    [self startRender];
}

- (void)handleTwoFingerLongPressGesture:(UILongPressGestureRecognizer *)twoFingerLongPress
{
    mapView->map.cancelAnimations();

    if (twoFingerLongPress.state == UIGestureRecognizerStateBegan)
        mapView->map.resetPosition();

    [self startRender];
}

- (void)handleThreeFingerLongPressGesture:(UILongPressGestureRecognizer *)threeFingerLongPress
{
    mapView->map.cancelAnimations();

    if (threeFingerLongPress.state == UIGestureRecognizerStateBegan)
    {
        mapView->map.toggleDebug();

        self.debug = ! self.debug;
    }

    [self startRender];
}

- (void)handleQuickZoomGesture:(UILongPressGestureRecognizer *)quickZoom
{
    mapView->map.cancelAnimations();

    if (quickZoom.state == UIGestureRecognizerStateBegan)
    {
        self.scale = mapView->map.getScale();

        self.quickZoomStart = [quickZoom locationInView:quickZoom.view].y;
    }
    else if (quickZoom.state == UIGestureRecognizerStateChanged)
    {
        CGFloat distance = self.quickZoomStart - [quickZoom locationInView:quickZoom.view].y;

        CGFloat newZoom = log2f(self.scale) + (distance / 100);

        mapView->map.scaleBy(powf(2, newZoom) / mapView->map.getScale(), self.view.bounds.size.width / 2, self.view.bounds.size.height / 2);
    }

    [self startRender];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    NSArray *validSimultaneousGestures = @[ [UIPanGestureRecognizer class], [UIPinchGestureRecognizer class], [UIRotationGestureRecognizer class] ];

    return ([validSimultaneousGestures containsObject:[gestureRecognizer class]] && [validSimultaneousGestures containsObject:[otherGestureRecognizer class]]);
}

CADisplayLink *displayLink;
MBXMapView *mapView;
NSOperationQueue *queue = NULL;

namespace llmr
{
    namespace platform
    {
        void restart(void *)
        {
        }

        void request_http(std::string url, std::function<void(Response&)> func, std::function<void()> cb)
        {
            [[NSNotificationCenter defaultCenter] postNotificationName:MBXUpdateActivityNotification object:nil userInfo:[NSDictionary dictionaryWithObject:@1 forKey:@"count"]];

            if (!queue) {
                queue = [[NSOperationQueue alloc] init];
            }

            NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];

            [NSURLConnection sendAsynchronousRequest:urlRequest
                                               queue:queue
                                   completionHandler:^(NSURLResponse *response, NSData *data, NSError *error)
            {
                Response res;

                if (error == nil)
                {
                    res.code = [(NSHTTPURLResponse *)response statusCode];
                    res.body = { (const char *)[data bytes], [data length] };
                }

                func(res);

                dispatch_async(dispatch_get_main_queue(), ^(void) {
                   cb();
                });

                [[NSNotificationCenter defaultCenter] postNotificationName:MBXUpdateActivityNotification object:nil userInfo:[NSDictionary dictionaryWithObject:@(-1) forKey:@"count"]];
                [[NSNotificationCenter defaultCenter] postNotificationName:MBXNeedsRenderNotification object:nil];
            }];
        }

        double time()
        {
            return [displayLink timestamp];
        }
    }
}

@end