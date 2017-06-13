#import "ClusteringExample.h"
@import Mapbox;

NSString *const MBXExampleClustering = @"ClusteringExample";

@interface ClusteringExample () <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) UIImage *icon;
@property (nonatomic) UILabel *popup;

@end

@implementation ClusteringExample

- (void)viewDidLoad {
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:[MGLStyle lightStyleURLWithVersion:9]];
    self.mapView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.mapView.tintColor = [UIColor darkGrayColor];
    self.mapView.delegate = self;
    [self.view addSubview:self.mapView];

    self.icon = [UIImage imageNamed:@"port"];
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    NSURL *url = [NSURL fileURLWithPath:[[NSBundle mainBundle] pathForResource:@"ports" ofType:@"geojson"]];

    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"clusteredPorts" URL:url options:@{
        MGLShapeSourceOptionClustered: @(YES),
        MGLShapeSourceOptionClusterRadius: @(self.icon.size.width)
    }];
    [style addSource:source];

    // Use a template image so that we can tint it with the `iconColor` runtime styling property.
    [style setImage:[self.icon imageWithRenderingMode:UIImageRenderingModeAlwaysTemplate] forName:@"icon"];

    // Show unclustered features as icons. The `cluster` attribute is built into clustering-enabled source features.
    MGLSymbolStyleLayer *ports = [[MGLSymbolStyleLayer alloc] initWithIdentifier:@"ports" source:source];
    ports.iconImageName = [MGLStyleValue valueWithRawValue:@"icon"];
    ports.iconColor = [MGLStyleValue valueWithRawValue:[[UIColor darkGrayColor] colorWithAlphaComponent:0.9]];
    ports.predicate = [NSPredicate predicateWithFormat:@"%K != YES", @"cluster"];
    [style addLayer:ports];

    // Color clustered features based on clustered point counts.
    NSDictionary *stops = @{ @20:  [MGLStyleValue valueWithRawValue:[UIColor lightGrayColor]],
                             @50:  [MGLStyleValue valueWithRawValue:[UIColor orangeColor]],
                             @100: [MGLStyleValue valueWithRawValue:[UIColor redColor]],
                             @200: [MGLStyleValue valueWithRawValue:[UIColor purpleColor]] };

    // Show clustered features as circles. The `point_count` attribute is built into clustering-enabled source features.
    MGLCircleStyleLayer *circlesLayer = [[MGLCircleStyleLayer alloc] initWithIdentifier:@"clusteredPorts" source:source];
    circlesLayer.circleRadius = [MGLStyleValue valueWithRawValue:@(self.icon.size.width / 2)];
    circlesLayer.circleOpacity = [MGLStyleValue valueWithRawValue:@0.75];
    circlesLayer.circleStrokeColor = [MGLStyleValue valueWithRawValue:[[UIColor whiteColor] colorWithAlphaComponent:0.75]];
    circlesLayer.circleStrokeWidth = [MGLStyleValue valueWithRawValue:@2];
    circlesLayer.circleColor = [MGLSourceStyleFunction
        functionWithInterpolationMode:MGLInterpolationModeInterval
                                stops:stops
                        attributeName:@"point_count"
                              options:nil];
    circlesLayer.predicate = [NSPredicate predicateWithFormat:@"%K == YES", @"cluster"];
    [style addLayer:circlesLayer];

    // Label cluster circles with a layer of text indicating feature count. Per text token convention, wrap the attribute in {}.
    MGLSymbolStyleLayer *numbersLayer = [[MGLSymbolStyleLayer alloc] initWithIdentifier:@"clusteredPortsNumbers" source:source];
    numbersLayer.textColor = [MGLStyleValue valueWithRawValue:[UIColor whiteColor]];
    numbersLayer.textFontSize = [MGLStyleValue valueWithRawValue:@(self.icon.size.width / 2)];
    numbersLayer.iconAllowsOverlap = [MGLStyleValue valueWithRawValue:@(YES)];
    numbersLayer.text = [MGLStyleValue valueWithRawValue:@"{point_count}"];
    numbersLayer.predicate = [NSPredicate predicateWithFormat:@"%K == YES", @"cluster"];
    [style addLayer:numbersLayer];

    // Add a tap gesture for zooming in to clusters or showing popups on individual features.
    [self.view addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap:)]];
}

- (void)mapViewRegionIsChanging:(MGLMapView *)mapView {
    [self showPopup:NO animated:NO];
}

- (void)handleTap:(UITapGestureRecognizer *)tap {
    if (tap.state == UIGestureRecognizerStateEnded) {
        CGPoint point = [tap locationInView:tap.view];
        CGFloat width = self.icon.size.width;
        CGRect rect = CGRectMake(point.x - width / 2, point.y - width / 2, width, width);

        // Find cluster circles and/or individual port icons in a touch-sized region around the tap.
        // In theory, we should only find either one cluster (since they don't overlap) or one port
        // (since overlapping ones would be clustered).
        NSArray *clusters = [self.mapView visibleFeaturesInRect:rect inStyleLayersWithIdentifiers:[NSSet setWithObject:@"clusteredPorts"]];
        NSArray *ports    = [self.mapView visibleFeaturesInRect:rect inStyleLayersWithIdentifiers:[NSSet setWithObject:@"ports"]];

        if (clusters.count) {
            [self showPopup:NO animated:YES];
            MGLPointFeature *cluster = (MGLPointFeature *)clusters.firstObject;
            [self.mapView setCenterCoordinate:cluster.coordinate zoomLevel:(self.mapView.zoomLevel + 1) animated:YES];
        } else if (ports.count) {
            MGLPointFeature *port = ((MGLPointFeature *)ports.firstObject);

            if (!self.popup) {
                self.popup = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, 100, 40)];
                self.popup.backgroundColor = [[UIColor whiteColor] colorWithAlphaComponent:0.9];
                self.popup.layer.cornerRadius = 4;
                self.popup.layer.masksToBounds = YES;
                self.popup.textAlignment = NSTextAlignmentCenter;
                self.popup.lineBreakMode = NSLineBreakByTruncatingTail;
                self.popup.font = [UIFont systemFontOfSize:16];
                self.popup.textColor = [UIColor blackColor];
                self.popup.alpha = 0;
                [self.view addSubview:self.popup];
            }

            self.popup.text = [NSString stringWithFormat:@"%@", [port attributeForKey:@"name"]];
            CGSize size = [self.popup.text sizeWithAttributes:@{ NSFontAttributeName: self.popup.font }];
            self.popup.bounds = CGRectInset(CGRectMake(0, 0, size.width, size.height), -10, -10);
            point = [self.mapView convertCoordinate:port.coordinate toPointToView:self.mapView];
            self.popup.center = CGPointMake(point.x, point.y - 50);

            if (self.popup.alpha < 1) {
                [self showPopup:YES animated:YES];
            }
        } else {
            [self showPopup:NO animated:YES];
        }
    }
}

- (void)showPopup:(BOOL)shouldShow animated:(BOOL)animated {
    CGFloat alpha = (shouldShow ? 1 : 0);
    if (animated) {
        __typeof__(self) __weak weakSelf = self;
        [UIView animateWithDuration:0.25 animations:^{
            weakSelf.popup.alpha = alpha;
        }];
    } else {
        self.popup.alpha = alpha;
    }
}

@end
