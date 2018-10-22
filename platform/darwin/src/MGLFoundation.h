#pragma once

#import <Foundation/Foundation.h>

#define MGL_EXPORT __attribute__((visibility ("default")))

/* Using a compound statement (GNU Extension, supported by clang) */
#define MGL_OBJC_DYNAMIC_CAST(object, type) \
    ({ \
        __typeof__( object ) temp##__LINE__ = (object); \
        (type *)([temp##__LINE__ isKindOfClass:[type class]] ? temp##__LINE__ : nil); \
    })
