#import "MBXOrnamentsViewController.h"

@import Mapbox;

@interface MBXOrnamentsViewController ()<MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) NSTimer *timer;
@property (nonatomic) NSInteger currentPositionIndex;

@end

@implementation MBXOrnamentsViewController

- (void)setCurrentPositionIndex:(NSInteger)currentPositionIndex {
    NSArray *ornamentPositions = @[@[
                                       @(MGLOrnamentPositionTopLeft),
                                       @(MGLOrnamentPositionTopRight),
                                       @(MGLOrnamentPositionBottomRight),
                                       @(MGLOrnamentPositionBottomLeft)
                                       ],
                                   @[
                                       @(MGLOrnamentPositionTopRight),
                                       @(MGLOrnamentPositionBottomRight),
                                       @(MGLOrnamentPositionBottomLeft),
                                       @(MGLOrnamentPositionTopLeft)
                                       ],
                                   @[
                                       @(MGLOrnamentPositionBottomRight),
                                       @(MGLOrnamentPositionBottomLeft),
                                       @(MGLOrnamentPositionTopLeft),
                                       @(MGLOrnamentPositionTopRight)
                                       ],
                                   @[
                                       @(MGLOrnamentPositionBottomLeft),
                                       @(MGLOrnamentPositionTopLeft),
                                       @(MGLOrnamentPositionTopRight),
                                       @(MGLOrnamentPositionBottomRight)
                                       ],
                                   @[
                                       @(MGLOrnamentPositionTopLeft),
                                       @(MGLOrnamentPositionTopRight),
                                       @(MGLOrnamentPositionBottomRight),
                                       @(MGLOrnamentPositionBottomLeft)
                                       ],
                                   ];
    NSArray *currentPosition = ornamentPositions[currentPositionIndex];
    self.mapView.scaleBarPosition = [currentPosition[0] integerValue];
    self.mapView.compassViewPosition = [currentPosition[1] integerValue];
    self.mapView.logoViewPosition = [currentPosition[2] integerValue];
    self.mapView.attributionButtonPosition = [currentPosition[3] integerValue];
    
    _currentPositionIndex = currentPositionIndex;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"Ornaments";

    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.frame];
    mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [mapView setCenterCoordinate:CLLocationCoordinate2DMake(39.915143, 116.404053)
                       zoomLevel:16
                       direction:30
                        animated:NO];
    mapView.delegate = self;
    mapView.showsScale = YES;
    [self.view addSubview:mapView];

    self.mapView = mapView;
}

- (void)viewDidDisappear:(BOOL)animated {
    [self.timer invalidate];
    self.timer = nil;
}

- (void)viewDidAppear:(BOOL)animated {
    self.timer = [NSTimer scheduledTimerWithTimeInterval:1
                                                  target:self
                                                selector:@selector(onTimerTick)
                                                userInfo:nil
                                                 repeats:YES];
}

- (void)onTimerTick {
    self.currentPositionIndex ++;
    if (self.currentPositionIndex >= 4) {
        self.currentPositionIndex = 0;
    }
}

@end
