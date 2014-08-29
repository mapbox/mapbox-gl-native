#ifndef MBGL_SHADER_SHADER_LINEPATTERN
#define MBGL_SHADER_SHADER_LINEPATTERN

#include <mbgl/shader/shader.hpp>

namespace mbgl {

class LinepatternShader : public Shader {
public:
    LinepatternShader();

    void bind(char *offset);

    void setExtrudeMatrix(const std::array<float, 16>& exmatrix);
    void setLineWidth(const std::array<float, 2>& linewidth);
    void setRatio(float ratio);
    void setPatternSize(const std::array<float, 2>& new_pattern_size);
    void setPatternTopLeft(const std::array<float, 2>& new_pattern_tl);
    void setPatternBottomRight(const std::array<float, 2>& new_pattern_br);
    void setPoint(float new_point);
    void setBlur(float new_blur);
    void setFade(float new_fade);

private:
    int32_t a_pos = -1;
    int32_t a_extrude = -1;
    int32_t a_linesofar = -1;

    std::array<float, 16> exmatrix = {{}};
    int32_t u_exmatrix = -1;

    std::array<float, 2> linewidth = {{}};
    int32_t u_linewidth = -1;

    float ratio = 0;
    int32_t u_ratio = -1;

    std::array<float, 2> pattern_size = {{}};
    int32_t u_pattern_size = -1;

    std::array<float, 2> pattern_tl = {{}};
    int32_t u_pattern_tl = -1;

    std::array<float, 2> pattern_br = {{}};
    int32_t u_pattern_br = -1;

    float point = 0.0f;
    int32_t u_point = -1;

    float blur = 0.0f;
    int32_t u_blur = -1;

    float fade = 0;
    int32_t u_fade = -1;
};
}

#endif
