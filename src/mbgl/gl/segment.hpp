#pragma once

#include <mbgl/gl/context.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/logging.hpp>

#include <cstddef>
#include <vector>

namespace mbgl {
namespace gl {

class SegmentInfo {
public:
    SegmentInfo(std::size_t vertexOffset_,
                std::size_t indexOffset_,
                std::size_t vertexLength_ = 0,
                std::size_t indexLength_ = 0)
        : vertexOffset(vertexOffset_),
          indexOffset(indexOffset_),
          vertexLength(vertexLength_),
          indexLength(indexLength_) {
    }

    const std::size_t vertexOffset;
    const std::size_t indexOffset;
    std::size_t vertexLength;
    std::size_t indexLength;
};

inline bool operator==(const SegmentInfo& lhs, const SegmentInfo& rhs) {
    return lhs.vertexOffset == rhs.vertexOffset && lhs.indexOffset == rhs.indexOffset &&
           lhs.vertexLength == rhs.vertexLength && lhs.indexLength == rhs.indexLength;
}

using SegmentInfoVector = std::vector<SegmentInfo>;

template <class Attributes>
class Segment {
public:
    Segment(std::size_t vertexOffset,
            std::size_t indexOffset,
            std::size_t vertexLength = 0,
            std::size_t indexLength = 0)
        : info(vertexOffset, indexOffset, vertexLength, indexLength) {
    }

    Segment(SegmentInfo info_)
        : info(std::move(info_)) {
    }

    SegmentInfo info;

    void bind(Context& context,
              BufferID indexBuffer_,
              const typename Attributes::Locations& attributeLocations,
              const typename Attributes::Bindings& attributeBindings_) const {
        if (context.supportsVertexArrays()) {
            if (!vao) {
                vao = context.createVertexArray();
                context.vertexBuffer.setDirty();
            }
            context.vertexArrayObject = *vao;
            if (indexBuffer != indexBuffer_) {
                indexBuffer = indexBuffer_;
                context.elementBuffer.setDirty();
                context.elementBuffer = indexBuffer_;
            }
        } else {
            // No VAO support. Force attributes to be rebound.
            context.elementBuffer = indexBuffer_;
            attributeBindings = {};
        }

        Attributes::bind(context,
                         attributeLocations,
                         attributeBindings,
                         attributeBindings_,
                         info.vertexOffset);
    }

private:
    mutable optional<UniqueVertexArray> vao;
    mutable optional<BufferID> indexBuffer;
    mutable typename Attributes::Bindings attributeBindings;
};

template <class Attributes>
using SegmentVector = std::vector<Segment<Attributes>>;

} // namespace gl
} // namespace mbgl
