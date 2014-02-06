//
//  MBXViewController.m
//  ios
//
//  Created by Justin R. Miller on 1/27/14.
//
//

#import "MBXViewController.h"

#import "MBXSettings.h"

#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>

#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>

@interface MBXViewController () <UIGestureRecognizerDelegate>

@property (nonatomic) EAGLContext *context;
@property (nonatomic) CGPoint center;
@property (nonatomic) CGFloat zoom;
@property (nonatomic) CGFloat angle;

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

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if ( ! self.context)
    {
        NSLog(@"Failed to initialize OpenGL ES context");
        return;
    }

    mapView = new MBXMapView();

    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    self.preferredFramesPerSecond = 60;
    [view bindDrawable];

    [EAGLContext setCurrentContext:self.context];

    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];

    mapView->init();

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
        NSLog(@"FPS: %f", frames / (current - elapsed));
        elapsed = current;
        frames = 0;
    }
}

- (void)handlePanGesture:(UIPanGestureRecognizer *)pan
{
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
}

- (void)handlePinchGesture:(UIPinchGestureRecognizer *)pinch
{
    CGFloat scale = mapView->map.getScale();

    if (pinch.state == UIGestureRecognizerStateBegan)
    {
        self.zoom = log2f(scale);
    }
    else if (pinch.state == UIGestureRecognizerStateChanged)
    {
        CGFloat tolerance = 2;

        CGFloat newZoom = self.zoom + (pinch.scale > 1 ? (pinch.scale / tolerance) : ((1 - pinch.scale) * -tolerance));

        double scale = mapView->map.getScale();
        double new_scale = powf(2, newZoom);
        mapView->map.scaleBy(new_scale / scale, [pinch locationInView:pinch.view].x, [pinch locationInView:pinch.view].y);
    }
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        self.angle = mapView->map.getAngle();
    }
    else if (rotate.state == UIGestureRecognizerStateChanged)
    {
        mapView->map.setAngle(self.angle + rotate.rotation, [rotate locationInView:rotate.view].x, [rotate locationInView:rotate.view].y);
    }
}

- (void)handleDoubleTapGesture:(UITapGestureRecognizer *)doubleTap
{
    if (doubleTap.state == UIGestureRecognizerStateEnded)
        mapView->map.scaleBy(2, [doubleTap locationInView:doubleTap.view].x, [doubleTap locationInView:doubleTap.view].y, 0.5);
}

- (void)handleTwoFingerTapGesture:(UITapGestureRecognizer *)twoFingerTap
{
    if (twoFingerTap.state == UIGestureRecognizerStateEnded)
        mapView->map.scaleBy(0.5, [twoFingerTap locationInView:twoFingerTap.view].x, [twoFingerTap locationInView:twoFingerTap.view].y, 0.5);
}

- (void)handleLongPressGesture:(UILongPressGestureRecognizer *)longPress
{
    if (longPress.state == UIGestureRecognizerStateBegan)
        mapView->map.resetNorth();
}

- (void)handleTwoFingerLongPressGesture:(UILongPressGestureRecognizer *)twoFingerLongPress
{
    if (twoFingerLongPress.state == UIGestureRecognizerStateBegan)
        mapView->map.resetPosition();
}

- (void)handleThreeFingerLongPressGesture:(UILongPressGestureRecognizer *)threeFingerLongPress
{
    if (threeFingerLongPress.state == UIGestureRecognizerStateBegan)
        mapView->map.toggleDebug();
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    NSArray *validSimultaneousGestures = @[ [UIPanGestureRecognizer class], [UIPinchGestureRecognizer class], [UIRotationGestureRecognizer class] ];

    return ([validSimultaneousGestures containsObject:[gestureRecognizer class]] && [validSimultaneousGestures containsObject:[otherGestureRecognizer class]]);
}

CADisplayLink *displayLink;
MBXMapView *mapView;
NSOperationQueue *queue;

namespace llmr
{
    namespace platform
    {
        void restart(void *)
        {
        }

        void async(std::function<void()> fn, std::function<void()> cb)
        {
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void)
            {
                fn();

                dispatch_async(dispatch_get_main_queue(), ^(void)
                {
                    cb();
                });
            });
        }

        void request_http(std::string url, std::function<void(Response&)> func)
        {
            NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];

            [NSURLConnection sendAsynchronousRequest:urlRequest
                                               queue:[NSOperationQueue mainQueue]
                                   completionHandler: ^(NSURLResponse *response, NSData *data, NSError *error)
             {
                 if ( ! error)
                 {
                     Response res;

                     res.code = [(NSHTTPURLResponse *)response statusCode];
                     res.body = { (const char *)[data bytes], [data length] };

                     func(res);
                 }
                 else
                 {
                     Response res;

                     func(res);
                 }
            }];
        }

        void request_http(std::string url, std::function<void(Response&)> func, std::function<void()> cb)
        {
            NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];

            [NSURLConnection sendAsynchronousRequest:urlRequest
                                               queue:[NSOperationQueue mainQueue]
                                   completionHandler:^(NSURLResponse *response, NSData *data, NSError *error)
            {
                 Response res;

                 if (error == nil)
                 {
                     res.code = [(NSHTTPURLResponse *)response statusCode];
                     res.body = { (const char *)[data bytes], [data length] };
                 }
                 
                 func(res);

                 cb();
             }];
        }

        double time()
        {
            return [displayLink timestamp];
        }
    }
}

@end