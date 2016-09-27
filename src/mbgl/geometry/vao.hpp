#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>

#include <stdexcept>

namespace mbgl {

class VertexArrayObject : public util::noncopyable {
public:
    VertexArrayObject();
    ~VertexArrayObject();

    template <typename VertexBuffer>
    void bind(Shader& shader,
              VertexBuffer& vertexBuffer,
              GLbyte* offset,
              gl::ObjectStore& store,
              gl::Context& context) {
        bindVertexArrayObject(store, context);
        if (bound_shader == 0) {
            vertexBuffer.bind(store, context);
            shader.bind(offset);
            if (vao) {
                storeBinding(shader, vertexBuffer.getID(), 0, offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.getID(), 0, offset);
        }
    }

    template <typename VertexBuffer, typename ElementsBuffer>
    void bind(Shader& shader,
              VertexBuffer& vertexBuffer,
              ElementsBuffer& elementsBuffer,
              GLbyte* offset,
              gl::ObjectStore& store,
              gl::Context& context) {
        bindVertexArrayObject(store, context);
        if (bound_shader == 0) {
            vertexBuffer.bind(store, context);
            elementsBuffer.bind(store, context);
            shader.bind(offset);
            if (vao) {
                storeBinding(shader, vertexBuffer.getID(), elementsBuffer.getID(), offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.getID(), elementsBuffer.getID(), offset);
        }
    }

    GLuint getID() const {
        return *vao;
    }

private:
    void bindVertexArrayObject(gl::ObjectStore&, gl::Context&);
    void storeBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer, GLbyte *offset);
    void verifyBinding(Shader &shader, GLuint vertexBuffer, GLuint elementsBuffer, GLbyte *offset);

    mbgl::optional<gl::UniqueVAO> vao;

    // For debug reasons, we're storing the bind information so that we can
    // detect errors and report
    GLuint bound_shader = 0;
    const char* bound_shader_name = "";
    GLuint bound_vertex_buffer = 0;
    GLuint bound_elements_buffer = 0;
    GLbyte *bound_offset = nullptr;
};

} // namespace mbgl
