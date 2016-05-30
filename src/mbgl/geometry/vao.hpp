#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <stdexcept>

namespace mbgl {

class Shader;

class VertexArrayObject : public util::noncopyable {
public:
    static void Unbind();

    VertexArrayObject();
    ~VertexArrayObject();

    template <typename Shader, typename VertexBuffer>
    inline void bind(Shader& shader, VertexBuffer &vertexBuffer, GLbyte *offset, gl::ObjectStore& store) {
        bindVertexArrayObject(store);
        if (bound_shader == 0) {
            vertexBuffer.bind(store);
            shader.bind(offset);
            if (vao.created()) {
                storeBinding(shader, vertexBuffer.getID(), 0, offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.getID(), 0, offset);
        }
    }

    template <typename Shader, typename VertexBuffer, typename ElementsBuffer>
    inline void bind(Shader& shader, VertexBuffer &vertexBuffer, ElementsBuffer &elementsBuffer, GLbyte *offset, gl::ObjectStore& store) {
        bindVertexArrayObject(store);
        if (bound_shader == 0) {
            vertexBuffer.bind(store);
            elementsBuffer.bind(store);
            shader.bind(offset);
            if (vao.created()) {
                storeBinding(shader, vertexBuffer.getID(), elementsBuffer.getID(), offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.getID(), elementsBuffer.getID(), offset);
        }
    }

    GLuint getID() const {
        return vao.getID();
    }

private:
    void bindVertexArrayObject(gl::ObjectStore&);
    void storeBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer, GLbyte *offset);
    void verifyBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer, GLbyte *offset);

    gl::VAOHolder vao;

    // For debug reasons, we're storing the bind information so that we can
    // detect errors and report
    GLuint bound_shader = 0;
    const char *bound_shader_name = "";
    GLuint bound_vertex_buffer = 0;
    GLuint bound_elements_buffer = 0;
    GLbyte *bound_offset = 0;
};

} // namespace mbgl
