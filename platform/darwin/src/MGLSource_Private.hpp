#import "MGLSource.h"

#include <mbgl/mbgl.hpp>
#include <mbgl/style/source.hpp>

@interface MGLSource (Private)

- (std::unique_ptr<mbgl::style::Source>)mbgl_source;

@end
