#import "MGLSource.h"

#include <mbgl/mbgl.hpp>
#include <mbgl/style/source.hpp>

@interface MGLSource (Private)

- (std::unique_ptr<mbgl::style::Source>)mbglSource;

@property (nonatomic) mbgl::style::Source *source;

@end
