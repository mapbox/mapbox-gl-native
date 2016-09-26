#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>

#include <stdexcept>

namespace mbgl {

class VertexArrayObject : public util::noncopyable {
public:
    VertexArrayObject();
    ~VertexArrayObject();

    template <typename Shader, typename T>
    void bind(Shader& shader,
              const gl::VertexBuffer<T>& vertexBuffer,
              int8_t* offset,
              gl::Context& context) {
        bindVertexArrayObject(context);
        if (bound_shader == 0) {
            context.vertexBuffer = vertexBuffer.buffer;
            shader.bind(vertexBuffer, offset);
            if (vertexArray) {
                storeBinding(shader, vertexBuffer.buffer, 0, offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.buffer, 0, offset);
        }
    }

    template <typename Shader, typename T, typename P>
    void bind(Shader& shader,
              const gl::VertexBuffer<T>& vertexBuffer,
              const gl::IndexBuffer<P>& indexBuffer,
              int8_t* offset,
              gl::Context& context) {
        bindVertexArrayObject(context);
        if (bound_shader == 0) {
            context.vertexBuffer = vertexBuffer.buffer;
            context.elementBuffer = indexBuffer.buffer;
            shader.bind(vertexBuffer, offset);
            if (vertexArray) {
                storeBinding(shader, vertexBuffer.buffer, indexBuffer.buffer, offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.buffer, indexBuffer.buffer, offset);
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
