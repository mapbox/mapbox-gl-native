//
//  MBXViewController.m
//  ios
//
//  Created by Justin R. Miller on 1/27/14.
//
//

#import "MBXViewController.h"

#import "MBXSettings.h"

#include <llmr/llmr.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/map/tile.hpp>

@interface MBXViewController ()

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
    [view bindDrawable];

    [EAGLContext setCurrentContext:self.context];

    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];

    mapView->init();

    UIPanGestureRecognizer *pan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
    [self.view addGestureRecognizer:pan];

    UITapGestureRecognizer *doubleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleTapGesture:)];
    doubleTap.numberOfTapsRequired = 2;
    [self.view addGestureRecognizer:doubleTap];

    UITapGestureRecognizer *twoFingerTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTwoFingerTapGesture:)];
    twoFingerTap.numberOfTouchesRequired = 2;
    [self.view addGestureRecognizer:twoFingerTap];

    UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
    [self.view addGestureRecognizer:longPress];

    UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)];
    [self.view addGestureRecognizer:pinch];

    UIRotationGestureRecognizer *rotate = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotateGesture:)];
    [self.view addGestureRecognizer:rotate];
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
        self.center = CGPointMake(0, 0);

    CGPoint delta = CGPointMake([pan translationInView:pan.view].x - self.center.x,
                                [pan translationInView:pan.view].y - self.center.y);

    mapView->map.moveBy(delta.x, delta.y);

    self.center = CGPointMake(self.center.x + delta.x, self.center.y + delta.y);
}

- (void)handleDoubleTapGesture:(UIPanGestureRecognizer *)doubleTap
{
    CGPoint gesturePoint = [doubleTap locationInView:doubleTap.view];

    mapView->map.scaleBy(2, gesturePoint.x, gesturePoint.y);
}

- (void)handleTwoFingerTapGesture:(UIPanGestureRecognizer *)twoFingerTap
{
    CGPoint gesturePoint = [twoFingerTap locationInView:twoFingerTap.view];

    mapView->map.scaleBy(0.5, gesturePoint.x, gesturePoint.y);
}

- (void)handleLongPressGesture:(UILongPressGestureRecognizer *)longPress
{
    mapView->map.resetNorth();
}

- (void)handlePinchGesture:(UIPinchGestureRecognizer *)pinch
{
    CGFloat scale = mapView->map.transform.getScale();

    if (pinch.state == UIGestureRecognizerStateBegan)
        self.zoom = log2f(scale);

    CGFloat newZoom = self.zoom + (pinch.scale > 1 ? ((pinch.scale / 10) * 1) : (((1 - pinch.scale) / 0.9) * -1));

    mapView->map.transform.setScale(powf(2, newZoom));
    mapView->map.update();
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    if (rotate.state == UIGestureRecognizerStateBegan)
        self.angle = mapView->map.transform.getAngle();

    mapView->map.transform.setAngle(self.angle + rotate.rotation);
    mapView->map.update();
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

        void request(void *, Tile::Ptr tile)
        {
            assert((bool)tile);

            NSString *urlTemplate = @"http://api.tiles.mapbox.com/v3/mapbox.mapbox-streets-v4/%d/%d/%d.vector.pbf";
            NSString *urlString = [NSString stringWithFormat:urlTemplate, tile->id.z, tile->id.x, tile->id.y];
            NSURL *url = [NSURL URLWithString:urlString];

            NSMutableURLRequest *urlRequest = [NSMutableURLRequest requestWithURL:url];

            if ( ! queue)
                queue = [NSOperationQueue new];

            [NSURLConnection sendAsynchronousRequest:urlRequest
                                               queue:queue
                                   completionHandler: ^(NSURLResponse * response, NSData * data, NSError * error)
            {
                if ( ! error)
                {
                    if ([(NSHTTPURLResponse*)response statusCode] == 200)
                    {
                        tile->setData((uint8_t *)[data bytes], [data length]);

                        if (tile->parse())
                        {
                            dispatch_async(dispatch_get_main_queue(), ^(void)
                            {
                                mapView->map.tileLoaded(tile);
                            });

                            return;
                        }
                    }
                }

                dispatch_async(dispatch_get_main_queue(), ^(void)
                {
                    mapView->map.tileFailed(tile);
                });
            }];
        }
        
        double time()
        {
            return [displayLink timestamp];
        }
    }
}

@end