//
//  MBXTestMapView.h
//  Integration Test Harness
//
//  Created by Jordan on 5/23/19.
//  Copyright © 2019 Mapbox. All rights reserved.
//

#import <Mapbox/Mapbox.h>

NS_ASSUME_NONNULL_BEGIN

@interface MBXTestMapView : MGLMapView
- (void)updateFromDisplayLink:(CADisplayLink *)displayLink;
@end

NS_ASSUME_NONNULL_END
