#import "MGLMapView+IBAdditions.h"

#import "MGLStyle.h"

#import "MGLMapView_Private.h"

@implementation MGLMapView (IBAdditions)

+ (NSSet<NSString *> *)keyPathsForValuesAffectingStyleURL__ {
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
        [NSException raise:MGLInvalidStyleURLException
                    format:@"“%@” is not a valid style URL.", URLString];
    }
    self.styleURL = url;
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingLatitude {
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

+ (NSSet<NSString *> *)keyPathsForValuesAffectingLongitude {
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

+ (NSSet<NSString *> *)keyPathsForValuesAffectingAllowsZooming {
    return [NSSet setWithObject:@"zoomEnabled"];
}

- (BOOL)allowsZooming {
    return self.zoomEnabled;
}

- (void)setAllowsZooming:(BOOL)allowsZooming {
    self.zoomEnabled = allowsZooming;
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingAllowsScrolling {
    return [NSSet setWithObject:@"scrollEnabled"];
}

- (BOOL)allowsScrolling {
    return self.scrollEnabled;
}

- (void)setAllowsScrolling:(BOOL)allowsScrolling {
    self.scrollEnabled = allowsScrolling;
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingAllowsRotating {
    return [NSSet setWithObject:@"rotateEnabled"];
}

- (BOOL)allowsRotating {
    return self.rotateEnabled;
}

- (void)setAllowsRotating:(BOOL)allowsRotating {
    self.rotateEnabled = allowsRotating;
}

+ (NSSet<NSString *> *)keyPathsForValuesAffectingAllowsTilting {
    return [NSSet setWithObject:@"pitchEnabled"];
}

- (BOOL)allowsTilting {
    return self.pitchEnabled;
}

- (void)setAllowsTilting:(BOOL)allowsTilting {
    self.pitchEnabled = allowsTilting;
}

@end
