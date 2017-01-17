#import "NSCoder+MGLAdditions.h"

#import "NSArray+MGLAdditions.h"
#import "NSValue+MGLAdditions.h"

@implementation NSCoder (MGLAdditions)

- (void)mgl_encodeLocationCoordinates2D:(std::vector<CLLocationCoordinate2D>)coordinates forKey:(NSString *)key {
    [self encodeObject:[NSArray mgl_coordinatesFromCoordinates:coordinates] forKey:key];
}

- (std::vector<CLLocationCoordinate2D>)mgl_decodeLocationCoordinates2DForKey:(NSString *)key {
    NSArray *coordinates = [self decodeObjectOfClass:[NSArray class] forKey:key];
    return [coordinates mgl_coordinates];
}

- (void)encodeMGLCoordinate:(CLLocationCoordinate2D)coordinate forKey:(NSString *)key {
    [self encodeObject:@{@"latitude": @(coordinate.latitude), @"longitude": @(coordinate.longitude)} forKey:key];
}

- (CLLocationCoordinate2D)decodeMGLCoordinateForKey:(NSString *)key {
    NSDictionary *coordinate = [self decodeObjectForKey:key];
    return CLLocationCoordinate2DMake([coordinate[@"latitude"] doubleValue], [coordinate[@"longitude"] doubleValue]);
}

@end
