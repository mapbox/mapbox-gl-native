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
#include <llmr/map/tile.hpp>

NSString *const MBXNeedsRenderNotification = @"MBXNeedsRenderNotification";
NSString *const MBXUpdateActivityNotification = @"MBXUpdateActivityNotification";

@interface MBXViewController () <UIGestureRecognizerDelegate>

@property (nonatomic) EAGLContext *context;
@property (nonatomic) CGPoint center;
@property (nonatomic) CGFloat scale;
@property (nonatomic) CGFloat angle;
@property (nonatomic) CGFloat quickZoomStart;
@property (nonatomic) BOOL debug;
@property (nonatomic) UIView *palette;

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

            map.setup([[UIScreen mainScreen] scale]);

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
    view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
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

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateRender:) name:MBXNeedsRenderNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateNetworkActivity:) name:MBXUpdateActivityNotification object:nil];

    mapView = new MBXMapView();

    mapView->init();

    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];

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
    self.palette.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin;
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
    mapView->map.resetNorth();
}

- (void)resetPosition
{
    mapView->map.resetPosition();
}

- (void)toggleDebug
{
    mapView->map.toggleDebug();

    self.debug = ! self.debug;
}

- (void)toggleRaster
{
//    mapView->map.toggleRaster();
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    mapView->settings.sync();
}

- (void)updateRender:(NSNotification *)notification
{
    [self updateRender];
}

- (void)updateNetworkActivity:(NSNotification *)notification
{
    [[NSURLSession sharedSession] getTasksWithCompletionHandler:^(NSArray *dataTasks, NSArray *uploadTasks, NSArray *downloadTasks)
    {
        [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:([downloadTasks count] > 0)];
    }];
}

- (void)updateRender
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self];

    displayLink.paused = NO;

    [self performSelector:@selector(pauseRender) withObject:nil afterDelay:3.0];
}

- (void)pauseRender
{
    displayLink.paused = YES;

    mapView->settings.sync();
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

    [self updateRender];
}

- (void)handlePinchGesture:(UIPinchGestureRecognizer *)pinch
{
    mapView->map.cancelAnimations();

    if (pinch.state == UIGestureRecognizerStateBegan)
    {
        mapView->map.startScaling();

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
        mapView->map.stopScaling();

        if (fabsf(pinch.velocity) < 20)
            return;

        CGFloat finalZoom = log2f(mapView->map.getScale()) + (0.01 * pinch.velocity);

        double scale = mapView->map.getScale();
        double new_scale = powf(2, finalZoom);

        CGFloat duration = ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad ? 0.3 : 0.5);

        mapView->map.scaleBy(new_scale / scale, [pinch locationInView:pinch.view].x, [pinch locationInView:pinch.view].y, duration);
    }
    else if (pinch.state == UIGestureRecognizerStateCancelled)
    {
        mapView->map.stopScaling();
    }

    [self updateRender];
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    mapView->map.cancelAnimations();

    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        mapView->map.startRotating();

        self.angle = mapView->map.getAngle();
    }
    else if (rotate.state == UIGestureRecognizerStateChanged)
    {
        mapView->map.setAngle(self.angle + rotate.rotation, [rotate locationInView:rotate.view].x, [rotate locationInView:rotate.view].y);
    }
    else if (rotate.state == UIGestureRecognizerStateEnded || rotate.state == UIGestureRecognizerStateCancelled)
    {
        mapView->map.stopRotating();
    }

    [self updateRender];
}

- (void)handleSingleTapGesture:(UITapGestureRecognizer *)singleTap
{
    [self togglePalette];
}

- (void)handleDoubleTapGesture:(UITapGestureRecognizer *)doubleTap
{
    mapView->map.cancelAnimations();

    if (doubleTap.state == UIGestureRecognizerStateEnded)
        mapView->map.scaleBy(2, [doubleTap locationInView:doubleTap.view].x, [doubleTap locationInView:doubleTap.view].y, 0.5);

    [self updateRender];
}

- (void)handleTwoFingerTapGesture:(UITapGestureRecognizer *)twoFingerTap
{
    mapView->map.cancelAnimations();

    if (twoFingerTap.state == UIGestureRecognizerStateEnded)
        mapView->map.scaleBy(0.5, [twoFingerTap locationInView:twoFingerTap.view].x, [twoFingerTap locationInView:twoFingerTap.view].y, 0.5);

    [self updateRender];
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

    [self updateRender];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    NSArray *validSimultaneousGestures = @[ [UIPanGestureRecognizer class], [UIPinchGestureRecognizer class], [UIRotationGestureRecognizer class] ];

    return ([validSimultaneousGestures containsObject:[gestureRecognizer class]] && [validSimultaneousGestures containsObject:[otherGestureRecognizer class]]);
}

MBXMapView *mapView;
CADisplayLink *displayLink;

namespace llmr
{
    namespace platform
    {
        class Request
        {
            public:
                int16_t identifier;
                std::string original_url;
        };

        void restart()
        {
            [[NSNotificationCenter defaultCenter] postNotificationName:MBXNeedsRenderNotification object:nil];
        }

        Request *request_http(std::string url, std::function<void(Response&)> background_function, std::function<void()> foreground_callback)
        {
            NSURLSessionDataTask *task = [[NSURLSession sharedSession] dataTaskWithURL:[NSURL URLWithString:@(url.c_str())] completionHandler:^(NSData *data, NSURLResponse *response, NSError *error)
            {
                [[NSNotificationCenter defaultCenter] postNotificationName:MBXUpdateActivityNotification object:nil];

                Response res;

                if ( ! error && [response isKindOfClass:[NSHTTPURLResponse class]])
                {
                    res.code = [(NSHTTPURLResponse *)response statusCode];
                    res.body = { (const char *)[data bytes], [data length] };
                }

                background_function(res);

                dispatch_async(dispatch_get_main_queue(), ^(void)
                {
                    foreground_callback();
                });
            }];

            [task resume];

            [[NSNotificationCenter defaultCenter] postNotificationName:MBXUpdateActivityNotification object:nil];

            Request *req = new Request();

            req->identifier = task.taskIdentifier;
            req->original_url = url;

            return req;
        }

        void cancel_request_http(Request *request)
        {
            [[NSURLSession sharedSession] getTasksWithCompletionHandler:^(NSArray *dataTasks, NSArray *uploadTasks, NSArray *downloadTasks)
            {
                for (NSURLSessionDownloadTask *task in downloadTasks)
                {
                    if (task.taskIdentifier == request->identifier)
                    {
                        [task cancel];

                        [[NSNotificationCenter defaultCenter] postNotificationName:MBXUpdateActivityNotification object:nil];

                        return;
                    }
                }
            }];
        }

        double time()
        {
            return [displayLink timestamp];
        }
    }
}

@end