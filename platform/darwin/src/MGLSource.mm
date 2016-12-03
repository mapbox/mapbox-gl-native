#import "MGLSource_Private.h"

#include <mbgl/style/source.hpp>

@interface MGLSource ()

// Even though this class is abstract, MGLStyle uses it to represent some
// special internal source types like mbgl::AnnotationSource.
@property (nonatomic) mbgl::style::Source *rawSource;

@end

@implementation MGLSource

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super init]) {
        _identifier = identifier;
    }
    return self;
}

- (void)addToMapView:(MGLMapView *)mapView {
    [NSException raise:NSInvalidArgumentException format:
     @"The source %@ cannot be added to the style. "
     @"Make sure the source was created as a member of a concrete subclass of MGLSource.",
     self];
}

- (void)removeFromMapView:(MGLMapView *)mapView {
    [NSException raise:NSInvalidArgumentException format:
     @"The source %@ cannot be removed from the style. "
     @"Make sure the source was created as a member of a concrete subclass of MGLSource.",
     self];
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier];
}

@end
