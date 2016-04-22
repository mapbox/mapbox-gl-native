#import "NSValue+MGLAdditions.h"

@implementation NSValue (MGLAdditions)

#pragma mark Geometry

+ (instancetype)valueWithMGLCoordinate:(CLLocationCoordinate2D)coordinate {
    return [self valueWithBytes:&coordinate objCType:@encode(CLLocationCoordinate2D)];
}

- (CLLocationCoordinate2D)MGLCoordinateValue {
    CLLocationCoordinate2D coordinate;
    [self getValue:&coordinate];
    return coordinate;
}

+ (instancetype)valueWithMGLCoordinateSpan:(MGLCoordinateSpan)span {
    return [self valueWithBytes:&span objCType:@encode(MGLCoordinateSpan)];
}

- (MGLCoordinateSpan)MGLCoordinateSpanValue {
    MGLCoordinateSpan span;
    [self getValue:&span];
    return span;
}

+ (instancetype)valueWithMGLCoordinateBounds:(MGLCoordinateBounds)bounds {
    return [self valueWithBytes:&bounds objCType:@encode(MGLCoordinateBounds)];
}

- (MGLCoordinateBounds)MGLCoordinateBoundsValue {
    MGLCoordinateBounds bounds;
    [self getValue:&bounds];
    return bounds;
}

#pragma mark Offline maps

+ (NSValue *)valueWithMGLOfflinePackProgress:(MGLOfflinePackProgress)progress {
    return [NSValue value:&progress withObjCType:@encode(MGLOfflinePackProgress)];
}

- (MGLOfflinePackProgress)MGLOfflinePackProgressValue {
    MGLOfflinePackProgress progress;
    [self getValue:&progress];
    return progress;
}

@end
