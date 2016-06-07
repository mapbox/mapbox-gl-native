#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/renderer/fill_renderable.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/outlinepattern_shader.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/platform/log.hpp>

#include <mapbox/earcut.hpp>

#include <cassert>

namespace mapbox {
namespace util {
template <> struct nth<0, mbgl::GeometryCoordinate> {
    inline static int64_t get(const mbgl::GeometryCoordinate& t) { return t.x; };
};

template <> struct nth<1, mbgl::GeometryCoordinate> {
    inline static int64_t get(const mbgl::GeometryCoordinate& t) { return t.y; };
};
}
}

namespace mbgl {

using namespace style;

struct GeometryTooLongException : std::exception {};

FillBucket::FillBucket() : renderable(std::make_unique<FillRenderable>()) {
}

FillBucket::~FillBucket() {
}

void FillBucket::addGeometry(const GeometryCollection& geometry) {
    auto& vertexBuffer = renderable->vertexBuffer;
    auto& triangleElementsBuffer = renderable->triangleElementsBuffer;
    auto& lineElementsBuffer = renderable->lineElementsBuffer;
    auto& triangleGroups = renderable->triangleGroups;
    auto& lineGroups = renderable->lineGroups;

    for (auto& polygon : classifyRings(geometry)) {
        // Optimize polygons with many interior rings for earcut tesselation.
        limitHoles(polygon, 500);

        std::size_t totalVertices = 0;

        for (const auto& ring : polygon) {
            totalVertices += ring.size();
            if (totalVertices > 65535)
                throw GeometryTooLongException();
        }

        for (const auto& ring : polygon) {
            std::size_t nVertices = ring.size();

            if (nVertices == 0)
                continue;

            if (lineGroups.empty() || lineGroups.back()->vertex_length + nVertices > 65535)
                lineGroups.emplace_back(std::make_unique<FillRenderable::LineGroup>());

            auto& lineGroup = *lineGroups.back();
            GLsizei lineIndex = lineGroup.vertex_length;

            vertexBuffer.add(ring[0].x, ring[0].y);
            lineElementsBuffer.add(lineIndex + nVertices - 1, lineIndex);

            for (uint32_t i = 1; i < nVertices; i++) {
                vertexBuffer.add(ring[i].x, ring[i].y);
                lineElementsBuffer.add(lineIndex + i - 1, lineIndex + i);
            }

            lineGroup.vertex_length += nVertices;
            lineGroup.elements_length += nVertices;
        }

        std::vector<uint32_t> indices = mapbox::earcut(polygon);

        std::size_t nIndicies = indices.size();
        assert(nIndicies % 3 == 0);

        if (triangleGroups.empty() || triangleGroups.back()->vertex_length + totalVertices > 65535) {
            triangleGroups.emplace_back(std::make_unique<FillRenderable::TriangleGroup>());
        }

        auto& triangleGroup = *triangleGroups.back();
        GLsizei triangleIndex = triangleGroup.vertex_length;

        for (uint32_t i = 0; i < nIndicies; i += 3) {
            triangleElementsBuffer.add(triangleIndex + indices[i],
                                       triangleIndex + indices[i + 1],
                                       triangleIndex + indices[i + 2]);
        }

        triangleGroup.vertex_length += totalVertices;
        triangleGroup.elements_length += nIndicies / 3;
    }
}

void FillBucket::upload(gl::ObjectStore& store) {
    if (renderable) {
        renderable->vertexBuffer.upload(store);
        renderable->triangleElementsBuffer.upload(store);
        renderable->lineElementsBuffer.upload(store);
    }

    // From now on, we're going to render during the opaque and translucent pass.
    uploaded = true;
}

void FillBucket::render(Painter& painter,
                        const Layer& layer,
                        const UnwrappedTileID& tileID,
                        const mat4& matrix) {
    painter.renderFill(*this, *layer.as<FillLayer>(), tileID, matrix);
}

bool FillBucket::hasData() const {
    return renderable && (!renderable->triangleGroups.empty() || !renderable->lineGroups.empty());
}

bool FillBucket::needsClipping() const {
    return true;
}

void FillBucket::drawFill(PlainShader& shader, gl::ObjectStore& store) {
    if (!renderable) {
        return;
    }
    auto& vertexBuffer = renderable->vertexBuffer;
    auto& triangleElementsBuffer = renderable->triangleElementsBuffer;
    auto& triangleGroups = renderable->triangleGroups;

    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : triangleGroups) {
        assert(group);
        group->array[0].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void FillBucket::drawFill(PatternShader& shader, gl::ObjectStore& store) {
    if (!renderable) {
        return;
    }
    auto& vertexBuffer = renderable->vertexBuffer;
    auto& triangleElementsBuffer = renderable->triangleElementsBuffer;
    auto& triangleGroups = renderable->triangleGroups;

    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : triangleGroups) {
        assert(group);
        group->array[1].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void FillBucket::drawFillOutline(OutlineShader& shader, gl::ObjectStore& store) {
    if (!renderable) {
        return;
    }
    auto& vertexBuffer = renderable->vertexBuffer;
    auto& lineElementsBuffer = renderable->lineElementsBuffer;
    auto& lineGroups = renderable->lineGroups;

    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : lineGroups) {
        assert(group);
        group->array[0].bind(shader, vertexBuffer, lineElementsBuffer, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_LINES, group->elements_length * 2, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * lineElementsBuffer.itemSize;
    }
}

void FillBucket::drawFillOutline(OutlinePatternShader& shader, gl::ObjectStore& store) {
    if (!renderable) {
        return;
    }
    auto& vertexBuffer = renderable->vertexBuffer;
    auto& lineElementsBuffer = renderable->lineElementsBuffer;
    auto& lineGroups = renderable->lineGroups;

    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : lineGroups) {
        assert(group);
        group->array[1].bind(shader, vertexBuffer, lineElementsBuffer, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_LINES, group->elements_length * 2, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * lineElementsBuffer.itemSize;
    }
}

} // namespace mbgl
