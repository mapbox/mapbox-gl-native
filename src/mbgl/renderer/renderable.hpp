#ifndef MBGL_RENDERER_RENDERABLE
#define MBGL_RENDERER_RENDERABLE

#include <mbgl/util/clip_id.hpp>

namespace mbgl {

struct Renderable {
    ClipID& clip;
};

} // namespace mbgl

#endif
