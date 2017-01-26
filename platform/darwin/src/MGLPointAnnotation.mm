#import "MGLPointAnnotation.h"

#import "MGLShape_Private.h"
#import "NSCoder+MGLAdditions.h"

#import <mbgl/util/geometry.hpp>


@implementation MGLPointAnnotation

@synthesize coordinate;

+ (BOOL)supportsSecureCoding
{
    return YES;
}

- (instancetype)initWithCoder:(NSCoder *)coder
{
    if (self = [super initWithCoder:coder]) {
        self.coordinate = [coder decodeMGLCoordinateForKey:@"coordinate"];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [super encodeWithCoder:coder];
    [coder encodeMGLCoordinate:coordinate forKey:@"coordinate"];
}

- (BOOL)isEqual:(id)other
{
    if (other == self) return YES;
    if (![other isKindOfClass:[MGLPointAnnotation class]]) return NO;

    MGLPointAnnotation *otherAnnotation = other;
    return ([super isEqual:other]
            && self.coordinate.latitude == otherAnnotation.coordinate.latitude
            && self.coordinate.longitude == otherAnnotation.coordinate.longitude);
}

- (NSUInteger)hash
{
    return [super hash] + @(self.coordinate.latitude).hash + @(self.coordinate.longitude).hash;
}

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

- (mbgl::Geometry<double>)geometryObject
{
    mbgl::Point<double> point = { self.coordinate.longitude, self.coordinate.latitude };
    return point;
}

@end

