#pragma once

#include <string>

#ifndef NDEBUG
#define __MBGL_DEBUG_GROUP_NAME2(counter) __MBGL_DEBUG_GROUP_##counter
#define __MBGL_DEBUG_GROUP_NAME(counter) __MBGL_DEBUG_GROUP_NAME2(counter)
#define MBGL_DEBUG_GROUP(string)                                                                   \
    ::mbgl::gl::debugging::group __MBGL_DEBUG_GROUP_NAME(__LINE__)(string);
#else
#define MBGL_DEBUG_GROUP(string)
#endif

namespace mbgl {
namespace gl {
namespace debugging {

void enable();

struct group {
    group(const std::string&);
    ~group();
};

} // namespace debugging
} // namespace gl
} // namespace mbgl
