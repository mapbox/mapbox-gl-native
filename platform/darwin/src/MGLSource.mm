#import "MGLSource_Private.h"

#include <mbgl/style/source.hpp>

@implementation MGLSource

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super init]) {
        _identifier = identifier;
    }
    return self;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier];
}

@end
