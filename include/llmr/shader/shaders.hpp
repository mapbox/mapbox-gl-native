// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#ifndef LLMR_SHADER_SHADERS
#define LLMR_SHADER_SHADERS

namespace llmr {

struct shader_source {
    const char *vertex;
    const char *fragment;
};

enum {
    LINE_SHADER,
    LINEJOIN_SHADER,
    OUTLINE_SHADER,
    PATTERN_SHADER,
    PLAIN_SHADER,
    POINT_SHADER,
    RASTER_SHADER,
    TEXT_SHADER,
    SHADER_COUNT
};

extern const shader_source shaders[SHADER_COUNT];

}

#endif
