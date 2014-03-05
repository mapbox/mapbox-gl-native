#ifndef LLMR_RENDERER_SHADER_LINE
#define LLMR_RENDERER_SHADER_LINE

#include "shader.hpp"

namespace llmr {

class LineShader : public Shader {
public:
    LineShader();

    void bind(char *offset);

    void setExtrudeMatrix(const std::array<float, 16>& exmatrix);
    void setColor(const std::array<float, 4>& color);
    void setLineWidth(const std::array<float, 2>& linewidth);
    void setRatio(float ratio);
    void setDashArray(const std::array<float, 2>& dasharray);
    void setDebug(float debug);

private:
    int32_t a_pos = -1;
    int32_t a_extrude = -1;
    // int32_t a_linesofar = -1;

    std::array<float, 16> exmatrix = {{}};
    int32_t u_exmatrix = -1;

    std::array<float, 4> color = {{}};
    int32_t u_color = -1;

    std::array<float, 2> linewidth = {{}};
    int32_t u_linewidth = -1;

    float ratio = 0;
    int32_t u_ratio = -1;

    std::array<float, 2> dasharray = {{}};
    int32_t u_dasharray = -1;

    float debug = 0;
    int32_t u_debug = -1;
};


}

#endif
