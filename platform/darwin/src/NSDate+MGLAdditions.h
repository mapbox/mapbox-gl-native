#import <Foundation/Foundation.h>

#include <mbgl/mbgl.hpp>
#include <mbgl/util/chrono.hpp>

@interface NSDate (MGLAdditions)

/// Converts from a duration in seconds to a duration object usable in mbgl.
mbgl::Duration MGLDurationInSeconds(NSTimeInterval duration);

@end
