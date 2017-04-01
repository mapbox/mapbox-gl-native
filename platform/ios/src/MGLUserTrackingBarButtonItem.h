//
//  MGLUserTrackingBarButtonItem.h
//  ios
//
//  Created by Eimantas Vaiciunas on 2017-04-01.
//  Copyright © 2017 Mapbox. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Mapbox/MGLMapView.h>

@class MGLMapView;

NS_ASSUME_NONNULL_BEGIN
MGL_EXPORT
@interface MGLUserTrackingBarButtonItem : UIBarButtonItem

- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_UNAVAILABLE;

- (instancetype)initWithMapView:(nullable MGLMapView *)mapView NS_DESIGNATED_INITIALIZER;

@property (nonatomic, readonly) MGLMapView *mapView;

@end

NS_ASSUME_NONNULL_END
