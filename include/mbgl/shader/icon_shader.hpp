#ifndef MBGL_SHADER_SHADER_ICON
#define MBGL_SHADER_SHADER_ICON

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class IconShader : public Shader {
public:
    IconShader();

    void bind(char *offset);

    UniformMatrix<4>              u_matrix      = {"u_matrix",      *this};
    UniformMatrix<4>              u_exmatrix    = {"u_exmatrix",    *this};
    Uniform<float>                u_angle       = {"u_angle",       *this};
    Uniform<float>                u_zoom        = {"u_zoom",        *this};
    Uniform<float>                u_flip        = {"u_flip",        *this};
    Uniform<float>                u_fadedist    = {"u_fadedist",    *this};
    Uniform<float>                u_minfadezoom = {"u_minfadezoom", *this};
    Uniform<float>                u_maxfadezoom = {"u_maxfadezoom", *this};
    Uniform<float>                u_fadezoom    = {"u_fadezoom",    *this};
    Uniform<float>                u_opacity     = {"u_opacity",     *this};
    Uniform<std::array<float, 2>> u_texsize     = {"u_texsize",     *this};

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
};

}

#endif
