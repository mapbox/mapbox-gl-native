//
//  MBXTestViewController.h
//  Integration Test Harness
//
//  Created by Jordan on 5/15/19.
//  Copyright © 2019 Mapbox. All rights reserved.
//

#import "MGLMapViewIntegrationTest.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLTabBarControllerTests : MGLMapViewIntegrationTest

@property (nonatomic) UIViewController *viewController;
@property (nonatomic) UITabBarController *tabController;
@property (nonatomic) MGLMapView *mapView;
@end

NS_ASSUME_NONNULL_END
