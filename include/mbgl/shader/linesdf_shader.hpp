#ifndef MBGL_SHADER_SHADER_LINESDF
#define MBGL_SHADER_SHADER_LINESDF

#include <mbgl/shader/shader.hpp>

namespace mbgl {

class LineSDFShader : public Shader {
public:
    LineSDFShader();

    void bind(char *offset);

    void setExtrudeMatrix(const std::array<float, 16>& exmatrix);
    void setColor(const std::array<float, 4>& color);
    void setLineWidth(const std::array<float, 2>& linewidth);
    void setPatternScaleA(const std::array<float, 2>& patternscale_a);
    void setPatternScaleB(const std::array<float, 2>& patternscale_b);
    void setTexYA(float tex_y_a);
    void setTexYB(float tex_y_b);
    void setBlur(float blur);
    void setFade(float fade);
    void setGamma(float sdfgamma);

private:
    int32_t a_pos = -1;
    int32_t a_extrude = -1;
    int32_t a_linesofar = -1;

    std::array<float, 16> exmatrix = {{}};
    int32_t u_exmatrix = -1;

    std::array<float, 4> color = {{}};
    int32_t u_color = -1;

    std::array<float, 2> linewidth = {{}};
    int32_t u_linewidth = -1;

    float blur = 0.0f;
    int32_t u_blur = -1;

    std::array<float, 2> patternscale_a;
    int32_t u_patternscale_a = -1;

    std::array<float, 2> patternscale_b;
    int32_t u_patternscale_b = -1;

    float tex_y_a;
    int32_t u_tex_y_a = -1;
    
    float tex_y_b;
    int32_t u_tex_y_b = -1;

    float fade = 0.0f;
    int32_t u_fade = -1;

    float sdfgamma = 0.0f;
    int32_t u_sdfgamma = -1;
};


}

#endif
