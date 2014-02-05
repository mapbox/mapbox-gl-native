#ifndef LLMR_GEOMETRY_VAO
#define LLMR_GEOMETRY_VAO

#include <llmr/platform/gl.hpp>

#include <cassert>

namespace llmr {

class VertexArrayObject {
public:
    template <typename Shader, typename VertexBuffer>
    inline void setup(Shader& shader, VertexBuffer& vertex_buffer, char *vertex_offset) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        vertex_buffer.bind();
        shader.bind_vertex(vertex_offset);
    }

    inline ~VertexArrayObject() {
        if (vao) {
            glDeleteVertexArrays(1, &vao);
        }
    }

    inline operator bool() const {
        return vao != 0;
    }

    inline void bind() {
        assert(vao);
        glBindVertexArray(vao);
    }

private:
    GLuint vao = 0;
};

}

#endif
