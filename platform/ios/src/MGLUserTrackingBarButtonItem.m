//
//  MGLUserTrackingBarButtonItem.m
//  ios
//
//  Created by Eimantas Vaiciunas on 2017-04-01.
//  Copyright © 2017 Mapbox. All rights reserved.
//

#import "MGLUserTrackingBarButtonItem.h"

NSString * const UserTrackingModeKeyPath = @"userTrackingMode";
NSUInteger const NumberOfTrackingModes = 4;

@interface MGLUserTrackingBarButtonItem ()

@property (nonatomic, readwrite) MGLMapView *mapView;

@end

@implementation MGLUserTrackingBarButtonItem

- (instancetype)initWithMapView:(MGLMapView *)mapView {
    self = [super init];

    if (self) {
        [self setImage:[UIImage imageNamed:@"TrackingLocationOffMask"]];
        self.mapView = mapView;
    }

    return self;
}

- (void)setMapView:(MGLMapView *)mapView {
    if (mapView == nil) {
        [self removeTrackingModeObservers];
        [self destroyAction];
    } else {
        [self addTrackingModeObserversToMapView:mapView];
        [self createAction];
    }
    _mapView = mapView;
}

- (void)removeTrackingModeObservers {
    [self.mapView removeObserver:self forKeyPath:UserTrackingModeKeyPath];
}

- (void)addTrackingModeObserversToMapView:(MGLMapView * _Nonnull)mapView {
    NSKeyValueObservingOptions observingOptions = (NSKeyValueObservingOptionInitial | NSKeyValueObservingOptionNew);
    [mapView addObserver:self
              forKeyPath:UserTrackingModeKeyPath
                 options:observingOptions
                 context:nil];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary<NSKeyValueChangeKey,id> *)change
                       context:(void *)context {
    if (![keyPath isEqualToString:UserTrackingModeKeyPath]) return;

    NSNumber *trackingMode = change[NSKeyValueChangeNewKey];
    MGLUserTrackingMode newMode = (MGLUserTrackingMode)[trackingMode integerValue];
    [self setImage:[self imageForTrackingMode:newMode]];
}

- (void)createAction {
    self.target = self;
    self.action = @selector(cycleTrackingMode:);
}

- (void)destroyAction {
    self.target = nil;
    self.action = nil;
}

- (void)cycleTrackingMode:(MGLUserTrackingBarButtonItem *)sender {
    MGLUserTrackingMode currentMode = self.mapView.userTrackingMode;
    MGLUserTrackingMode nextMode = [self nextTrackingModeAfter:currentMode];
    self.mapView.userTrackingMode = nextMode;
}

- (MGLUserTrackingMode)nextTrackingModeAfter:(MGLUserTrackingMode)currentMode {
    return (currentMode + 1) % NumberOfTrackingModes;
}

- (UIImage *)imageForTrackingMode:(MGLUserTrackingMode)trackingMode {
    switch(trackingMode) {
        case MGLUserTrackingModeFollow:
            return [UIImage imageNamed:@"TrackingLocationMask"];
        case MGLUserTrackingModeFollowWithHeading:
            return [UIImage imageNamed:@"TrackingHeadingMask"];
        case MGLUserTrackingModeFollowWithCourse:
            return [UIImage imageNamed:@"TrackingCourseMask"];
        case MGLUserTrackingModeNone:
        default:
            return [UIImage imageNamed:@"TrackingLocationOffMask"];
    }
}


@end
