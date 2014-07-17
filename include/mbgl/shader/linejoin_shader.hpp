#ifndef MBGL_SHADER_SHADER_LINEJOIN
#define MBGL_SHADER_SHADER_LINEJOIN

#include <mbgl/shader/shader.hpp>

namespace mbgl {

class LinejoinShader : public Shader {
public:
    LinejoinShader();

    void bind(char *offset);

    void setColor(const std::array<float, 4>& color);
    void setWorld(const std::array<float, 2>& world);
    void setLineWidth(const std::array<float, 2>& linewidth);
    void setSize(float size);

private:
    int32_t a_pos = -1;

    std::array<float, 4> color = {{}};
    int32_t u_color = -1;

    std::array<float, 2> world = {{}};
    int32_t u_world = -1;

    std::array<float, 2> linewidth = {{}};
    int32_t u_linewidth = -1;

    float size = 0;
    int32_t u_size = -1;
};

}

#endif
