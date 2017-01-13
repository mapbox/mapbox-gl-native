#import <Foundation/Foundation.h>

#include <mbgl/util/chrono.hpp>

@interface NSDate (MGLAdditions)

/// Converts from a duration in seconds to a duration object usable in mbgl.
mbgl::Duration MGLDurationInSeconds(NSTimeInterval duration);

/// Converts from an mbgl duration object to a duration in seconds.
NSTimeInterval MGLSecondsFromDuration(mbgl::Duration duration);

@end
