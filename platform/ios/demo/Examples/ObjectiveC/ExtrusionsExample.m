//
//  ExtrusionsExample.m
//  Examples
//
//  Created by Jordan Kiley on 5/11/17.
//  Copyright © 2017 Mapbox. All rights reserved.
//

#import "ExtrusionsExample.h"

@import Mapbox;

NSString *const MBXExample3DExtrusions = @"ExtrusionsExample";

@interface ExtrusionsExample () <MGLMapViewDelegate>

@end

@implementation ExtrusionsExample

- (void)viewDidLoad {
    [super viewDidLoad];
    
    MGLMapView *mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:[MGLStyle lightStyleURLWithVersion:9]];
    
    // Center the map on the Colosseum in Rome, Italy.
    
    // Center the map view on the Colosseum in Rome, Italy and set the camera's pitch and distance.
    mapView.camera = [MGLMapCamera cameraLookingAtCenterCoordinate:CLLocationCoordinate2DMake(41.8902, 12.4922) fromDistance:600 pitch:60 heading:0];
    mapView.delegate = self;
    
    [self.view addSubview:mapView];
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    
    // Access the Mapbox Streets source and use it to create a `MGLFillExtrusionStyleLayer`. The source identifier is `composite`. Use the `sources` property on a style to verify source identifiers.
    MGLSource *source = [style sourceWithIdentifier:@"composite"];
    MGLFillExtrusionStyleLayer *layer = [[MGLFillExtrusionStyleLayer alloc] initWithIdentifier:@"buildings" source:source];
    layer.sourceLayerIdentifier = @"building";
    
    // Filter out buildings that should not extrude.
    layer.predicate = [NSPredicate predicateWithFormat:@"extrude == 'true' AND height >= 0"];
    
    // Set the fill extrusion height to the value for the building height attribute.
    layer.fillExtrusionHeight = [MGLStyleValue valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"height" options:nil];
    layer.fillExtrusionBase =[MGLStyleValue valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"min_height" options:nil];
    layer.fillExtrusionOpacity = [MGLStyleValue valueWithRawValue:@0.75];
    layer.fillExtrusionColor = [MGLStyleValue valueWithRawValue:[UIColor whiteColor]];
    
    // Insert the fill extrusion layer below a POI label layer. If you aren’t sure what the layer is called, you can view the style in Mapbox Studio or iterate over the style’s layers property, printing out each layer’s identifier.
    MGLStyleLayer *symbolLayer = [style layerWithIdentifier:@"poi-scalerank3"];
    [style insertLayer:layer belowLayer:symbolLayer];
}

@end
