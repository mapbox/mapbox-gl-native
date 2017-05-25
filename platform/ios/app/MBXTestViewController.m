//
//  MBXTestViewController.m
//  ios
//
//  Created by Justin R. Miller on 5/25/17.
//  Copyright © 2017 Mapbox. All rights reserved.
//

#import "MBXTestViewController.h"

#import <Mapbox/Mapbox.h>

@interface MBXTestViewController ()

@property (nonatomic) MGLMapView *mapView;

@end

@implementation MBXTestViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.mapView = [[MGLMapView alloc] initWithFrame:self.view.bounds styleURL:[MGLStyle darkStyleURLWithVersion:9]];
    [self.view addSubview:self.mapView];

    NSLog(@"new map view: %p", self.mapView);

//    [self.mapView addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTap:)]];
//
//    double lat = 0;
//    double lon = 0;
//    NSMutableArray *annotations = [NSMutableArray array];
//    for (int i = 0; i < 10; i++)
//    {
//        MGLPointAnnotation *a = [MGLPointAnnotation new];
//        a.coordinate = CLLocationCoordinate2DMake(lat, lon);
//        [annotations addObject:a];
//        lat += 5;
//        lon += 10;
//    }
//    [self.mapView addAnnotations:annotations];
}

- (void)handleTap:(UITapGestureRecognizer *)tap
{
    self.navigationController.navigationBarHidden = !self.navigationController.navigationBarHidden;
}

@end
