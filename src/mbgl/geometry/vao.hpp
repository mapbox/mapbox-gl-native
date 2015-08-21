#ifndef MBGL_GEOMETRY_VAO
#define MBGL_GEOMETRY_VAO

#include <mbgl/shader/shader.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <stdexcept>

namespace mbgl {

class VertexArrayObject : public util::noncopyable {
public:
    static void Unbind();
    static void Delete(GLsizei n, const GLuint* arrays);

    VertexArrayObject();
    ~VertexArrayObject();

    template <typename Shader, typename VertexBuffer>
    inline void bind(Shader& shader, VertexBuffer &vertexBuffer, char *offset) {
        bindVertexArrayObject();
        if (bound_shader == 0) {
            vertexBuffer.bind();
            shader.bind(offset);
            if (vao) {
                storeBinding(shader, vertexBuffer.getID(), 0, offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.getID(), 0, offset);
        }
    }

    template <typename Shader, typename VertexBuffer, typename ElementsBuffer>
    inline void bind(Shader& shader, VertexBuffer &vertexBuffer, ElementsBuffer &elementsBuffer, char *offset) {
        bindVertexArrayObject();
        if (bound_shader == 0) {
            vertexBuffer.bind();
            elementsBuffer.bind();
            shader.bind(offset);
            if (vao) {
                storeBinding(shader, vertexBuffer.getID(), elementsBuffer.getID(), offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.getID(), elementsBuffer.getID(), offset);
        }
    }

    inline GLuint getID() const {
        return vao;
    }

private:
    void bindVertexArrayObject();
    void storeBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer, char *offset);
    void verifyBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer, char *offset);

    GLuint vao = 0;

    // For debug reasons, we're storing the bind information so that we can
    // detect errors and report
    GLuint bound_shader = 0;
    const char *bound_shader_name = "";
    GLuint bound_vertex_buffer = 0;
    GLuint bound_elements_buffer = 0;
    char *bound_offset = 0;
};

}

#endif
