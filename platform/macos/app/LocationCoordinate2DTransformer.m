#import "LocationCoordinate2DTransformer.h"

#import <Mapbox/Mapbox.h>

@implementation LocationCoordinate2DTransformer {
    MGLCoordinateFormatter *_coordinateFormatter;
}

+ (Class)transformedValueClass {
    return [NSString class];
}

+ (BOOL)allowsReverseTransformation {
    return NO;
}

- (instancetype)init {
    if (self = [super init]) {
        _coordinateFormatter = [[MGLCoordinateFormatter alloc] init];
    }
    return self;
}

- (id)transformedValue:(id)value {
    if (![value isKindOfClass:[NSValue class]]) {
        return nil;
    }
    return [_coordinateFormatter stringForObjectValue:value].capitalizedString;
}

@end
