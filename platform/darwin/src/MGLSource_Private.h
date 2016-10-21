#import "MGLSource.h"

#include <mbgl/mbgl.hpp>
#include <mbgl/style/source.hpp>

@interface MGLSource (Private)

@property (nonatomic) mbgl::style::Source *rawSource;
@property (nonatomic) std::unique_ptr<mbgl::style::Source> source;

@end
