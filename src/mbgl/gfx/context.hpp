#pragma once

#include <mbgl/gfx/vertex_vector.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_vector.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/gfx/types.hpp>

namespace mbgl {
namespace gfx {

class Context {
protected:
    Context() = default;

public:
    Context(Context&&) = delete;
    Context(const Context&) = delete;
    Context& operator=(Context&& other) = delete;
    Context& operator=(const Context& other) = delete;
    virtual ~Context() = default;

public:
    template <class Vertex>
    VertexBuffer<Vertex>
    createVertexBuffer(VertexVector<Vertex>&& v,
                       const BufferUsageType usage = BufferUsageType::StaticDraw) {
        return { v.elements(), createVertexBufferResource(v.data(), v.bytes(), usage) };
    }

    template <class Vertex>
    void updateVertexBuffer(VertexBuffer<Vertex>& buffer, VertexVector<Vertex>&& v) {
        assert(v.elements() == buffer.elements);
        updateVertexBufferResource(*buffer.resource, v.data(), v.bytes());
    }

    template <class DrawMode>
    IndexBuffer createIndexBuffer(IndexVector<DrawMode>&& v,
                                  const BufferUsageType usage = BufferUsageType::StaticDraw) {
        return { v.elements(), createIndexBufferResource(v.data(), v.bytes(), usage) };
    }

    template <class DrawMode>
    void updateIndexBuffer(IndexBuffer& buffer, IndexVector<DrawMode>&& v) {
        assert(v.elements() == buffer.elements);
        updateIndexBufferResource(*buffer.resource, v.data(), v.bytes());
    }

protected:
    virtual std::unique_ptr<const VertexBufferResource>
    createVertexBufferResource(const void* data, std::size_t size, const BufferUsageType) = 0;
    virtual void
    updateVertexBufferResource(const VertexBufferResource&, const void* data, std::size_t size) = 0;

    virtual std::unique_ptr<const IndexBufferResource>
    createIndexBufferResource(const void* data, std::size_t size, const BufferUsageType) = 0;
    virtual void
    updateIndexBufferResource(const IndexBufferResource&, const void* data, std::size_t size) = 0;
};

} // namespace gfx
} // namespace mbgl
