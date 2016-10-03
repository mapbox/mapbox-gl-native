#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/util/optional.hpp>

#include <stdexcept>

namespace mbgl {
namespace gl {

class VertexArrayObject {
public:
    template <typename Shader, typename T>
    void bind(Shader& shader,
              const VertexBuffer<T>& vertexBuffer,
              int8_t* offset,
              Context& context) {
        bindVertexArrayObject(context);
        if (bound_shader == 0) {
            context.vertexBuffer = vertexBuffer.buffer;
            context.bindAttributes(shader, vertexBuffer, offset);
            if (vertexArray) {
                storeBinding(shader, vertexBuffer.buffer, 0, offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.buffer, 0, offset);
        }
    }

    template <typename Shader, typename T, typename P>
    void bind(Shader& shader,
              const VertexBuffer<T>& vertexBuffer,
              const IndexBuffer<P>& indexBuffer,
              int8_t* offset,
              Context& context) {
        bindVertexArrayObject(context);
        if (bound_shader == 0) {
            context.vertexBuffer = vertexBuffer.buffer;
            context.elementBuffer = indexBuffer.buffer;
            context.bindAttributes(shader, vertexBuffer, offset);
            if (vertexArray) {
                storeBinding(shader, vertexBuffer.buffer, indexBuffer.buffer, offset);
            }
        } else {
            verifyBinding(shader, vertexBuffer.buffer, indexBuffer.buffer, offset);
        }
    }

    VertexArrayID getID() const {
        return *vertexArray;
    }

private:
    void bindVertexArrayObject(Context&);
    void storeBinding(Shader& shader,
                      BufferID vertexBuffer,
                      BufferID elementsBuffer,
                      int8_t* offset);
    void verifyBinding(Shader& shader,
                       BufferID vertexBuffer,
                       BufferID elementsBuffer,
                       int8_t* offset);

    optional<UniqueVertexArray> vertexArray;

    // For debug reasons, we're storing the bind information so that we can
    // detect errors and report
    ProgramID bound_shader = 0;
    const char* bound_shader_name = "";
    BufferID bound_vertex_buffer = 0;
    BufferID bound_elements_buffer = 0;
    int8_t *bound_offset = nullptr;
};

} // namespace gl
} // namespace mbgl
