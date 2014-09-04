#ifndef MBGL_SHADER_TEXT_SHADER
#define MBGL_SHADER_TEXT_SHADER

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class TextShader : public Shader {
public:
    TextShader();

    void bind(char *offset);

    UniformMatrix<4>              u_matrix      = {"u_matrix",      *this};
    UniformMatrix<4>              u_exmatrix    = {"u_exmatrix",    *this};
    Uniform<std::array<float, 4>> u_color       = {"u_color",       *this};
    Uniform<std::array<float, 2>> u_texsize     = {"u_texsize",     *this};
    Uniform<float>                u_buffer      = {"u_buffer",      *this};
    Uniform<float>                u_gamma       = {"u_gamma",       *this};
    Uniform<float>                u_angle       = {"u_angle",       *this};
    Uniform<float>                u_zoom        = {"u_zoom",        *this};
    Uniform<float>                u_flip        = {"u_flip",        *this};
    Uniform<float>                u_fadedist    = {"u_fadedist",    *this};
    Uniform<float>                u_minfadezoom = {"u_minfadezoom", *this};
    Uniform<float>                u_maxfadezoom = {"u_maxfadezoom", *this};
    Uniform<float>                u_fadezoom    = {"u_fadezoom",    *this};

private:
    int32_t a_pos = -1;
    int32_t a_offset = -1;
    int32_t a_data1 = -1;
    int32_t a_data2 = -1;
};
}

#endif
