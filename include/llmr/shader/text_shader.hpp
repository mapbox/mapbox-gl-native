#ifndef LLMR_SHADER_TEXT_SHADER
#define LLMR_SHADER_TEXT_SHADER

#include "shader.hpp"

namespace llmr {

class TextShader : public Shader {
public:
    TextShader();

    void bind(char *offset);

    void setColor(float r, float g, float b, float a);
    void setColor(const std::array<float, 4> &color);
    void setBuffer(float buffer);
    void setGamma(float gamma);
    void setExmatrix(const std::array<float, 16> &exmatrix);
    void setAngle(float angle);
    void setZoom(float zoom);
    void setFlip(float flip);
    void setFadedist(float fadedist);
    void setMinfadezoom(float minfadezoom);
    void setMaxfadezoom(float maxfadezoom);
    void setFadezoom(float fadezoom);
    void setTexsize(const std::array<float, 2> &texsize);

private:
    int32_t a_pos = -1;
    int32_t a_offset = -1;
    int32_t a_tex = -1;
    int32_t a_angle = -1;
    int32_t a_minzoom = -1;
    int32_t a_maxzoom = -1;
    int32_t a_rangeend = -1;
    int32_t a_rangestart = -1;
    int32_t a_labelminzoom = -1;

    std::array<float, 4> color = {{}};
    int32_t u_color = -1;

    float buffer = 0.0f;
    int32_t u_buffer = -1;

    float gamma = 0.0f;
    int32_t u_gamma = -1;

    std::array<float, 16> exmatrix = {{}};
    int32_t u_exmatrix = -1;

    float angle = 0.0f;
    int32_t u_angle = -1;

    float zoom = 0.0f;
    int32_t u_zoom = -1;

    float flip = 0.0f;
    int32_t u_flip = -1;

    float fadedist = 0.0f;
    int32_t u_fadedist = -1;

    float minfadezoom = 0.0f;
    int32_t u_minfadezoom = -1;

    float maxfadezoom = 0.0f;
    int32_t u_maxfadezoom = -1;

    float fadezoom = 0.0f;
    int32_t u_fadezoom = -1;

    std::array<float, 2> texsize = {{}};
    int32_t u_texsize = -1;
};
}

#endif
