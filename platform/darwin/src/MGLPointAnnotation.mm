#import "MGLPointAnnotation.h"

#import "MGLShape_Private.h"

#import <mbgl/util/geometry.hpp>


@implementation MGLPointAnnotation

@synthesize coordinate;

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; title = %@; subtitle = %@; coordinate = %f, %f>",
            NSStringFromClass([self class]), (void *)self,
            self.title ? [NSString stringWithFormat:@"\"%@\"", self.title] : self.title,
            self.subtitle ? [NSString stringWithFormat:@"\"%@\"", self.subtitle] : self.subtitle,
            coordinate.latitude, coordinate.longitude];
}

- (NSDictionary *)geoJSONDictionary
{
    return @{@"type": @"Point",
             @"coordinates": @[@(self.coordinate.longitude), @(self.coordinate.latitude)]};
}

- (mbgl::Feature)featureObject
{
    mbgl::Point<double> point = { self.coordinate.longitude, self.coordinate.latitude };
    return mbgl::Feature {point};
}

@end

