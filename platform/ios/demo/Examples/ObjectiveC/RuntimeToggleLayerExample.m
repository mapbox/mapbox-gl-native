#import "RuntimeToggleLayerExample.h"
@import Mapbox;

NSString *const MBXExampleRuntimeToggleLayer = @"RuntimeToggleLayerExample";

@interface RuntimeToggleLayerExample () <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) MGLStyleLayer *contoursLayer;
@end

@implementation RuntimeToggleLayerExample

- (void)viewDidLoad {
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    // Set the map's center coordinate
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(37.745395, -119.594421)
			    zoomLevel:11
			     animated:NO];

    [self.view addSubview:self.mapView];

    [self addToggleButton];

    // Set the delegate property of our map view to self after instantiating it
    self.mapView.delegate = self;
}

// Wait until the style is loaded before modifying the map style
- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    [self addLayer];
}

- (void)addLayer {
    MGLSource *source = [[MGLVectorSource alloc] initWithIdentifier:@"contours" configurationURL:[NSURL URLWithString:@"mapbox://mapbox.mapbox-terrain-v2"]];

    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"contours" source:source];
    layer.sourceLayerIdentifier = @"contour";
    layer.lineJoin = [MGLStyleValue valueWithRawValue:[NSValue valueWithMGLLineJoin:MGLLineJoinRound]];
    layer.lineCap = [MGLStyleValue valueWithRawValue:[NSValue valueWithMGLLineCap:MGLLineCapRound]];
    layer.lineColor = [MGLStyleValue valueWithRawValue:[UIColor brownColor]];
    layer.lineWidth = [MGLStyleValue valueWithRawValue:@1];

    [self.mapView.style addSource:source];

    MGLStyleLayer *waterLayer = [self.mapView.style layerWithIdentifier:@"water"];
    if (waterLayer != nil) {
	// You can insert a layer below an existing style layer
	[self.mapView.style insertLayer:layer belowLayer:waterLayer];
    } else {
	// or you can simply add it above all layers
	[self.mapView.style addLayer:layer];
    }

    self.contoursLayer = layer;

    [self showContours];
}

- (void)toggleLayer:(UIButton *)sender {
    sender.selected = !sender.selected;
    if (sender.selected) {
	[self showContours];
    } else {
	[self hideContours];
    }
}

- (void)showContours {
    [self.contoursLayer setVisible:YES];
}

- (void)hideContours {
    [self.contoursLayer setVisible:NO];
}

- (void)addToggleButton {
    UIButton *button = [UIButton buttonWithType:UIButtonTypeSystem];
    button.autoresizingMask = UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin;
    [button setTitle:@"Toggle Contours" forState:UIControlStateNormal];
    [button setSelected:YES];
    [button sizeToFit];
    button.center = CGPointMake(self.view.center.x, 0);
    button.frame = CGRectMake(button.frame.origin.x, self.view.frame.size.height - button.frame.size.height - 5, button.frame.size.width, button.frame.size.height);
    [button addTarget:self action:@selector(toggleLayer:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:button];
}

@end
