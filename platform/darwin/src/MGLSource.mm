#import "MGLSource.h"

#include <mbgl/style/source.hpp>

@interface MGLSource()
@end

@implementation MGLSource

- (instancetype)initWithSourceID:(NSString *)sourceID sourceType:(NSString *)sourceType
{
    self = [super init];
    if (self == nil) return nil;
    _sourceID = sourceID;
    _sourceType = sourceType;
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbgl_source
{
    [NSException raise:@"Subclasses must override this method" format:@""];
    return nil;
}

@end
