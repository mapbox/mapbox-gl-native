#ifndef MBGL_SHADER_SHADER_PATTERN
#define MBGL_SHADER_SHADER_PATTERN

#include <mbgl/shader/shader.hpp>

namespace mbgl {

class PatternShader : public Shader {
public:
    PatternShader();

    void bind(char *offset);

    void setPatternTopLeft(const std::array<float, 2>& pattern_tl);
    void setPatternBottomRight(const std::array<float, 2>& pattern_br);
    void setOpacity(float opacity);
    void setImage(int image);
    void setMix(float mix);
    void setPatternMatrix(const std::array<float, 9> &patternmatrix);

private:
    int32_t a_pos = -1;

    std::array<float, 2> pattern_tl = {{}};
    int32_t u_pattern_tl = -1;

    std::array<float, 2> pattern_br = {{}};
    int32_t u_pattern_br = -1;

    float opacity = 0;
    int32_t u_opacity = -1;

    int image = 0;
    int32_t u_image = -1;

    float mix = 0;
    int32_t u_mix = -1;

    std::array<float, 9> patternmatrix = {{}};
    int32_t u_patternmatrix = -1;
};

}

#endif
