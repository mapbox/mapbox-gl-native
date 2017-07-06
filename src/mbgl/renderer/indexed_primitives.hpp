#pragma once

#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/segment.hpp>

#include <stdexcept>

namespace mbgl {

template <class DrawMode, class LayoutVertex, class AttributeLayout>
class Drawable;

template <class DrawMode, class LayoutVertex, class AttributeLayout>
class IndexedPrimitives {
public:
    void add(std::initializer_list<LayoutVertex> v,
             std::initializer_list<std::array<uint16_t, DrawMode::bufferGroupSize>> i) {
        // Check that all vertices fit into the current segment, if not, create a new one.
        if (segmentInfo.empty() || segmentInfo.back().vertexLength + v.size() > std::numeric_limits<uint16_t>::max()) {
            segmentInfo.emplace_back(vertices.vertexSize(), indices.indexSize());
        }

        auto& segment = segmentInfo.back();
        const uint16_t offset = segment.vertexLength;

        for (const auto& primitive : i) {
            for (uint16_t index : primitive) {
                // Check that the index references a vertex supplied in this list and that it is not
                // out of bounds.
                if (index >= v.size()) {
                    throw std::out_of_range("primitive contains indices outside its vertex group");
                }
            }

            // Insert all indices into the list.
            addIndices(primitive, offset, std::make_index_sequence<DrawMode::bufferGroupSize>());
        }

        // Insert all vertices into the list.
        for (const auto& vertex : v) {
            vertices.emplace_back(vertex);
        }

        // Update the current segment statistics.
        segment.vertexLength += v.size();
        segment.indexLength += i.size() * DrawMode::bufferGroupSize;
    }

    // Accessors for test suite.
    const gl::VertexVector<LayoutVertex>& getVertices() const {
        return vertices;
    }

    const gl::IndexVector<DrawMode>& getIndices() const {
        return indices;
    }

    const gl::SegmentInfoVector& getSegmentInfo() const {
        return segmentInfo;
    }

private:
    // Helper function for expanding primitives.
    template <std::size_t N, std::size_t... I>
    void addIndices(std::array<uint16_t, N> primitive, const uint16_t offset, std::index_sequence<I...>) {
        // Adds the current offset to the indices.
        indices.emplace_back((std::get<I>(primitive) + offset)...);
    }

private:
    friend class Drawable<DrawMode, LayoutVertex, AttributeLayout>;
    gl::VertexVector<LayoutVertex> vertices;
    gl::IndexVector<DrawMode> indices;
    gl::SegmentInfoVector segmentInfo;
};

} // namespace mbgl
