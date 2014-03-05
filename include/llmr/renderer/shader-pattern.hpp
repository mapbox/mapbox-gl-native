#ifndef LLMR_RENDERER_SHADER_PATTERN
#define LLMR_RENDERER_SHADER_PATTERN

#include "shader.hpp"

namespace llmr {

class PatternShader : public Shader {
public:
    PatternShader();

    void bind(char *offset);

    void setColor(const std::array<float, 4>& color);
    void setOffset(const std::array<float, 2>& offset);
    void setPatternSize(const std::array<float, 2>& pattern_size);
    void setPatternTopLeft(const std::array<float, 2>& pattern_tl);
    void setPatternBottomRight(const std::array<float, 2>& pattern_br);
    void setMix(float mix);

private:
    int32_t a_pos = -1;

    std::array<float, 4> color = {{}};
    int32_t u_color = -1;

    std::array<float, 2> offset = {{}};
    int32_t u_offset = -1;

    std::array<float, 2> pattern_size = {{}};
    int32_t u_pattern_size = -1;

    std::array<float, 2> pattern_tl = {{}};
    int32_t u_pattern_tl = -1;

    std::array<float, 2> pattern_br = {{}};
    int32_t u_pattern_br = -1;

    float mix = 0;
    int32_t u_mix = -1;
};

}

#endif
