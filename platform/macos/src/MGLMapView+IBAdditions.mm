#import "MGLMapView+IBAdditions.h"

#import "MGLMapView_Private.hpp"

@implementation MGLMapView (IBAdditions)

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingStyleURL__ {
    return [NSSet setWithObject:@"styleURL"];
}

- (nullable NSString *)styleURL__ {
    return self.styleURL.absoluteString;
}

- (void)setStyleURL__:(nullable NSString *)URLString {
    URLString = [URLString stringByTrimmingCharactersInSet:
                 [NSCharacterSet whitespaceAndNewlineCharacterSet]];
    NSURL *url = URLString.length ? [NSURL URLWithString:URLString] : nil;
    if (URLString.length && !url) {
        [NSException raise:@"Invalid style URL"
                    format:@"“%@” is not a valid style URL.", URLString];
    }
    self.styleURL = url;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingLatitude {
    return [NSSet setWithObjects:@"centerCoordinate", @"camera", nil];
}

- (double)latitude {
    return self.centerCoordinate.latitude;
}

- (void)setLatitude:(double)latitude {
    if (!isnan(self.pendingLongitude)) {
        // With both components present, set the real center coordinate and
        // forget the pending parts.
        self.centerCoordinate = CLLocationCoordinate2DMake(latitude, self.pendingLongitude);
        self.pendingLatitude = NAN;
        self.pendingLongitude = NAN;
    } else {
        // Not enough info to make a valid center coordinate yet. Stash this
        // latitude away until the longitude is set too.
        self.pendingLatitude = latitude;
    }
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingLongitude {
    return [NSSet setWithObjects:@"centerCoordinate", @"camera", nil];
}

- (double)longitude {
    return self.centerCoordinate.longitude;
}

- (void)setLongitude:(double)longitude {
    if (!isnan(self.pendingLatitude)) {
        // With both components present, set the real center coordinate and
        // forget the pending parts.
        self.centerCoordinate = CLLocationCoordinate2DMake(self.pendingLatitude, longitude);
        self.pendingLatitude = NAN;
        self.pendingLongitude = NAN;
    } else {
        // Not enough info to make a valid center coordinate yet. Stash this
        // longitude away until the latitude is set too.
        self.pendingLongitude = longitude;
    }
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingAllowsZooming {
    return [NSSet setWithObject:@"zoomEnabled"];
}

- (BOOL)allowsZooming {
    return self.zoomEnabled;
}

- (void)setAllowsZooming:(BOOL)allowsZooming {
    self.zoomEnabled = allowsZooming;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingAllowsScrolling {
    return [NSSet setWithObject:@"scrollEnabled"];
}

- (BOOL)allowsScrolling {
    return self.scrollEnabled;
}

- (void)setAllowsScrolling:(BOOL)allowsScrolling {
    self.scrollEnabled = allowsScrolling;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingAllowsRotating {
    return [NSSet setWithObject:@"rotateEnabled"];
}

- (BOOL)allowsRotating {
    return self.rotateEnabled;
}

- (void)setAllowsRotating:(BOOL)allowsRotating {
    self.rotateEnabled = allowsRotating;
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingAllowsTilting {
    return [NSSet setWithObject:@"pitchEnabled"];
}

- (BOOL)allowsTilting {
    return self.pitchEnabled;
}

- (void)setAllowsTilting:(BOOL)allowsTilting {
    self.pitchEnabled = allowsTilting;
}

@end
