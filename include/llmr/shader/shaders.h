// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#ifndef LLMR_SHADER_SHADERS
#define LLMR_SHADER_SHADERS

#ifdef __cplusplus
extern "C" {
#endif

struct llmr_shader {
    const char *vertex;
    const char *fragment;
};
typedef struct llmr_shader llmr_shader_t;

enum {
    FILL_SHADER,
    LINE_SHADER,
    OUTLINE_SHADER,
    SHADER_COUNT
};

extern const llmr_shader_t llmr_shaders[SHADER_COUNT];

#ifdef __cplusplus
}
#endif

#endif
