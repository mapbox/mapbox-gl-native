#ifndef LLMR_SHADER_SHADER_DOT
#define LLMR_SHADER_SHADER_DOT

#include <llmr/shader/shader.hpp>

namespace llmr {

    class DotShader : public Shader {
    public:
        DotShader();

        void bind(char *offset);

        void setColor(const std::array<float, 4>& color);
        void setSize(float size);
        void setBlur(float blur);

    private:
        int32_t a_pos = -1;

        std::array<float, 4> color = {{}};
        int32_t u_color = -1;

        float size = 0;
        int32_t u_size = -1;

        float blur = 0;
        int32_t u_blur = -1;
};

}

#endif
