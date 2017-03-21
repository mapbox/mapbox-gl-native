#import <Foundation/Foundation.h>

#import "MGLFoundation.h"
#include <mbgl/util/chrono.hpp>

NS_ASSUME_NONNULL_BEGIN


/// Converts from a duration in seconds to a duration object usable in mbgl.
MGL_EXPORT
mbgl::Duration MGLDurationFromTimeInterval(NSTimeInterval duration);

/// Converts from an mbgl duration object to a duration in seconds.
MGL_EXPORT
NSTimeInterval MGLTimeIntervalFromDuration(mbgl::Duration duration);

NS_ASSUME_NONNULL_END
