#import "MGLFoundation.h"

#include <mbgl/util/run_loop.hpp>

void MGLInitializeRunLoop();

/* Using a compound statement (GNU Extension, supported by clang) */
#define MGL_OBJC_DYNAMIC_CAST(object, type) \
    ({ \
        __typeof__( object ) temp##__LINE__ = (object); \
        (type *)([temp##__LINE__ isKindOfClass:[type class]] ? temp##__LINE__ : nil); \
    })

