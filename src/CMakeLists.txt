SET(llmr_SOURCES
    geometry/debug_font_buffer.cpp
    geometry/linevertexbuffer.cpp
    map/map.cpp
    map/tile.cpp
    map/transform.cpp
    renderer/painter.cpp
    renderer/shader-fill.cpp
    renderer/shader-line.cpp
    renderer/shader-outline.cpp
    renderer/shader.cpp
    shader/shaders.cpp
    util/animation.cpp
    util/mat4.c
)

SET(llmr_HEADERS
    ../include/llmr/geometry/debug_font_buffer.hpp
    ../include/llmr/geometry/linevertexbuffer.hpp
    ../include/llmr/llmr.hpp
    ../include/llmr/map/map.hpp
    ../include/llmr/map/tile.hpp
    ../include/llmr/map/transform.hpp
    ../include/llmr/platform/gl.hpp
    ../include/llmr/platform/platform.hpp
    ../include/llmr/renderer/painter.hpp
    ../include/llmr/renderer/shader-fill.hpp
    ../include/llmr/renderer/shader-line.hpp
    ../include/llmr/renderer/shader-outline.hpp
    ../include/llmr/renderer/shader.hpp
    ../include/llmr/shader/shaders.hpp
    ../include/llmr/util/animation.hpp
    ../include/llmr/util/math.hpp
    ../include/llmr/util/vec2.hpp
)

INCLUDE_DIRECTORIES(
    ../include
)

ADD_LIBRARY(llmr STATIC
    ${llmr_SOURCES}
    ${llmr_HEADERS}
)

LINK_DIRECTORIES(
)

TARGET_LINK_LIBRARIES(llmr
)
