#ifndef MBGL_SHADER_SHADER_ICON
#define MBGL_SHADER_SHADER_ICON

#include <mbgl/shader/shader.hpp>

namespace mbgl {

class IconShader : public Shader {
public:
    IconShader();

    void bind(char *offset);

    void setExtrudeMatrix(const std::array<float, 16>& exmatrix);
    void setAngle(float angle);
    void setZoom(float zoom);
    void setFlip(float flip);
    void setFadeDist(float fadedist);
    void setMinFadeZoom(float minfadezoom);
    void setMaxFadeZoom(float maxfadezoom);
    void setFadeZoom(float fadezoom);
    void setOpacity(float opacity);
    void setTextureSize(const std::array<float, 2> &texsize);

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

    std::array<float, 16> exmatrix = {{}};
    int32_t u_exmatrix = -1;

    float angle = 0;
    int32_t u_angle = -1;

    float zoom = 0;
    int32_t u_zoom = -1;

    float flip = 0;
    int32_t u_flip = -1;

    float fadedist = 0;
    int32_t u_fadedist = -1;

    float minfadezoom = 0;
    int32_t u_minfadezoom = -1;

    float maxfadezoom = 0;
    int32_t u_maxfadezoom = -1;

    float fadezoom = 0;
    int32_t u_fadezoom = -1;

    float opacity = 0;
    int32_t u_opacity = -1;

    std::array<float, 2> texsize = {{}};
    int32_t u_texsize = -1;
};

}

#endif
