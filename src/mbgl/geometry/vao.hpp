#pragma once

#include <mbgl/shader/shader.hpp>
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
              int8_t* offset,
              gl::Context& context) {
        bindVertexArrayObject(context);
        if (bound_shader == 0) {
            vertexBuffer.bind(context);
            shader.bind(offset);
            if (vertexArray) {
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
              int8_t* offset,
              gl::Context& context) {
        bindVertexArrayObject(context);
        if (bound_shader == 0) {
            vertexBuffer.bind(context);
            elementsBuffer.bind(context);
            shader.bind(offset);
            if (vertexArray) {
                storeBinding(shader, vertexBuffer.getID(), elementsBuffer.getID(), offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.getID(), elementsBuffer.getID(), offset);
        }
    }

    gl::VertexArrayID getID() const {
        return *vertexArray;
    }

private:
    void bindVertexArrayObject(gl::Context&);
    void storeBinding(Shader& shader,
                      gl::BufferID vertexBuffer,
                      gl::BufferID elementsBuffer,
                      int8_t* offset);
    void verifyBinding(Shader& shader,
                       gl::BufferID vertexBuffer,
                       gl::BufferID elementsBuffer,
                       int8_t* offset);

    mbgl::optional<gl::UniqueVertexArray> vertexArray;

    // For debug reasons, we're storing the bind information so that we can
    // detect errors and report
    gl::ProgramID bound_shader = 0;
    const char* bound_shader_name = "";
    gl::BufferID bound_vertex_buffer = 0;
    gl::BufferID bound_elements_buffer = 0;
    int8_t *bound_offset = nullptr;
};

} // namespace mbgl
