#ifndef MBGL_SHADER_SHADER_LINE
#define MBGL_SHADER_SHADER_LINE

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class LineShader : public Shader {
public:
    LineShader();

    void bind(gl::Config&, char *offset) override;

    UniformMatrix<4>               u_matrix    = {"u_matrix",    *this};
    UniformMatrix<4>               u_exmatrix  = {"u_exmatrix",  *this};
    Uniform<std::array<float, 4>>  u_color     = {"u_color",     *this};
    Uniform<std::array<float, 2>>  u_linewidth = {"u_linewidth", *this};
    Uniform<float>                 u_ratio     = {"u_ratio",     *this};
    Uniform<float>                 u_blur      = {"u_blur",      *this};

private:
    int32_t a_pos = -1;
    int32_t a_data = -1;
};


}

#endif
