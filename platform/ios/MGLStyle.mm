#import "MGLStyle.h"

#import <mbgl/util/default_styles.hpp>

@implementation MGLStyle

// name is lowercase
#define MGL_DEFINE_STYLE(name) \
    static NSURL *MGLStyleURL_##name; \
    + (NSURL *)name##StyleURL { \
        static dispatch_once_t onceToken; \
        dispatch_once(&onceToken, ^{ \
            MGLStyleURL_##name = [NSURL URLWithString:@(mbgl::util::default_styles::name.url)]; \
        }); \
        return MGLStyleURL_##name; \
    }

MGL_DEFINE_STYLE(streets)
MGL_DEFINE_STYLE(emerald)
MGL_DEFINE_STYLE(light)
MGL_DEFINE_STYLE(dark)
MGL_DEFINE_STYLE(satellite)
MGL_DEFINE_STYLE(hybrid)

// Make sure all the styles listed in mbgl::util::default_styles::orderedStyles
// are defined above and also declared in MGLStyle.h.
static_assert(6 == mbgl::util::default_styles::numOrderedStyles,
              "mbgl::util::default_styles::orderedStyles and MGLStyle have different numbers of styles.");

@end
