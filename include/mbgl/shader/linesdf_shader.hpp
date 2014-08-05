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
    void setBlur(float blur);

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
};


}

#endif
