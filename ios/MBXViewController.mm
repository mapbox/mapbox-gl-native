#import "MBXViewController.h"

#import "MBXSettings.h"

#import "../common/foundation_request.h"

#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>
#import <GLKit/GLKit.h>

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

@end

@implementation MBXViewController

class View;

llmr::Map *map = nullptr;
View *viewObj = nullptr;
llmr::Settings_iOS *settings = nullptr;
MBXViewController *viewController = nullptr;

#pragma mark - Setup

- (void)loadView
{
    [super loadView];

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context)
    {
        NSLog(@"Failed to create OpenGL ES context");
    }

    GLKView *view = [[GLKView alloc] initWithFrame:[[UIScreen mainScreen] bounds] context:self.context];
    view.enableSetNeedsDisplay = NO;
    view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    [view bindDrawable];

    self.view = view;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    [self setupMap];
    [self setupInteraction];
    [self setupDebugUI];

    map->start();
}

- (void)setupMap
{
    settings = new llmr::Settings_iOS();
    settings->load();

    viewObj = new View(self);

    map = new llmr::Map(*viewObj);
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

#pragma mark - Rendering delegates

- (void)swap
{
    if (map->needsSwap())
    {
        [(GLKView *)self.view display];
        map->swapped();
    }
}

- (void)viewWillLayoutSubviews
{
    [super viewWillLayoutSubviews];

    GLKView *view = (GLKView *)self.view;
    CGRect rect = [view bounds];
    map->resize(rect.size.width, rect.size.height, view.contentScaleFactor, view.drawableWidth, view.drawableHeight);
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
    if (map)
    {
        delete map;
        map = nullptr;
    }

    if (viewObj)
    {
        delete viewObj;
        viewObj = nullptr;
    }

    if (settings)
    {
        settings->sync();
        delete settings;
        settings = nullptr;
    }

    if ([[EAGLContext currentContext] isEqual:self.context])
    {
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
        if ([pan velocityInView:pan.view].x < 50 && [pan velocityInView:pan.view].y < 50)
        {
            return;
        }

        CGPoint finalCenter = CGPointMake(self.center.x + (0.1 * [pan velocityInView:pan.view].x),
                                          self.center.y + (0.1 * [pan velocityInView:pan.view].y));

        CGFloat duration = ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad ? 0.3 : 0.5);

        map->moveBy(finalCenter.x - self.center.x, finalCenter.y - self.center.y, duration);
    }
}

- (void)handlePinchGesture:(UIPinchGestureRecognizer *)pinch
{
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

        map->scaleBy(powf(2, newZoom) / map->getScale(), [pinch locationInView:pinch.view].x, [pinch locationInView:pinch.view].y);
    }
    else if (pinch.state == UIGestureRecognizerStateEnded)
    {
       map->stopScaling();

        if (fabsf(pinch.velocity) < 20)
        {
            return;
        }

        CGFloat finalZoom = log2f(map->getScale()) + (0.01 * pinch.velocity);

        double scale = map->getScale();
        double new_scale = powf(2, finalZoom);

        CGFloat duration = ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad ? 0.3 : 0.5);

        map->scaleBy(new_scale / scale, [pinch locationInView:pinch.view].x, [pinch locationInView:pinch.view].y, duration);
    }
    else if (pinch.state == UIGestureRecognizerStateCancelled)
    {
        map->stopScaling();
    }
}

- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate
{
    map->cancelAnimations();

    if (rotate.state == UIGestureRecognizerStateBegan)
    {
        map->startRotating();

        self.angle = map->getAngle();
    }
    else if (rotate.state == UIGestureRecognizerStateChanged)
    {
        map->setAngle(self.angle + rotate.rotation, [rotate locationInView:rotate.view].x, [rotate locationInView:rotate.view].y);
    }
    else if (rotate.state == UIGestureRecognizerStateEnded || rotate.state == UIGestureRecognizerStateCancelled)
    {
        map->stopRotating();
    }
}

- (void)handleSingleTapGesture:(UITapGestureRecognizer *)singleTap
{
    if (singleTap.state == UIGestureRecognizerStateEnded)
    {
        [self togglePalette];
    }
}

- (void)handleDoubleTapGesture:(UITapGestureRecognizer *)doubleTap
{
    map->cancelAnimations();

    if (doubleTap.state == UIGestureRecognizerStateEnded)
    {
        map->scaleBy(2, [doubleTap locationInView:doubleTap.view].x, [doubleTap locationInView:doubleTap.view].y, 0.5);
    }
}

- (void)handleTwoFingerTapGesture:(UITapGestureRecognizer *)twoFingerTap
{
    map->cancelAnimations();

    if (twoFingerTap.state == UIGestureRecognizerStateEnded)
    {
        map->scaleBy(0.5, [twoFingerTap locationInView:twoFingerTap.view].x, [twoFingerTap locationInView:twoFingerTap.view].y, 0.5);
    }
}

- (void)handleQuickZoomGesture:(UILongPressGestureRecognizer *)quickZoom
{
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
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    NSArray *validSimultaneousGestures = @[ [UIPanGestureRecognizer class], [UIPinchGestureRecognizer class], [UIRotationGestureRecognizer class] ];

    return ([validSimultaneousGestures containsObject:[gestureRecognizer class]] && [validSimultaneousGestures containsObject:[otherGestureRecognizer class]]);
}

class View : public llmr::View {
public:
    View(MBXViewController *controller) : controller(controller) {}
    virtual ~View() {}

    void make_active() {
        [EAGLContext setCurrentContext:controller.context];
    }

    void swap() {
        [controller performSelectorOnMainThread:@selector(swap) withObject:nil waitUntilDone:NO];
    }

private:
    MBXViewController *controller = nullptr;
};

@end
