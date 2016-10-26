#import "MGLSource_Private.h"

#include <mbgl/style/source.hpp>

@interface MGLSource ()

@property (nonatomic) mbgl::style::Source *source;

@end

@implementation MGLSource

- (instancetype)initWithIdentifier:(NSString *)identifier {
    if (self = [super init]) {
        _identifier = identifier;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbglSource {
    [NSException raise:@"MGLAbstractClassException" format:
     @"The source %@ cannot be added to the style. "
     @"Make sure the source was created as a member of a concrete subclass of MGLSource.",
     NSStringFromClass(self)];
    return nil;
}

@end
