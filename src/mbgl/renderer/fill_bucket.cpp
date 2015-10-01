#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/shader/outlinepattern_shader.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/platform/log.hpp>

#include <mapbox/earcut.hpp>

#include <cassert>

struct GeometryTooLongException : std::exception {};

using namespace mbgl;

namespace mapbox {
namespace util {
template <> struct nth<0, GeometryCoordinate> {
    inline static int64_t get(const GeometryCoordinate& t) { return t.x; };
};

template <> struct nth<1, GeometryCoordinate> {
    inline static int64_t get(const GeometryCoordinate& t) { return t.y; };
};
}
}

FillBucket::FillBucket() {
}

FillBucket::~FillBucket() {
}

void FillBucket::addGeometry(const GeometryCollection& geometry) {
    for (const auto& polygon : classifyRings(geometry)) {
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
                lineGroups.emplace_back(std::make_unique<LineGroup>());

            LineGroup& lineGroup = *lineGroups.back();
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
            triangleGroups.emplace_back(std::make_unique<TriangleGroup>());
        }

        TriangleGroup& triangleGroup = *triangleGroups.back();
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

void FillBucket::upload(gl::GLObjectStore& glObjectStore) {
    vertexBuffer.upload(glObjectStore);
    triangleElementsBuffer.upload(glObjectStore);
    lineElementsBuffer.upload(glObjectStore);

    // From now on, we're going to render during the opaque and translucent pass.
    uploaded = true;
}

void FillBucket::render(Painter& painter,
                        const StyleLayer& layer,
                        const UnwrappedTileID& tileID,
                        const mat4& matrix) {
    painter.renderFill(*this, *layer.as<FillLayer>(), tileID, matrix);
}

bool FillBucket::hasData() const {
    return !triangleGroups.empty() || !lineGroups.empty();
}

bool FillBucket::needsClipping() const {
    return true;
}

void FillBucket::drawElements(PlainShader& shader, gl::GLObjectStore& glObjectStore) {
    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : triangleGroups) {
        assert(group);
        group->array[0].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index, glObjectStore);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void FillBucket::drawElements(PatternShader& shader, gl::GLObjectStore& glObjectStore) {
    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : triangleGroups) {
        assert(group);
        group->array[1].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index, glObjectStore);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void FillBucket::drawVertices(OutlineShader& shader, gl::GLObjectStore& glObjectStore) {
    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : lineGroups) {
        assert(group);
        group->array[0].bind(shader, vertexBuffer, lineElementsBuffer, vertex_index, glObjectStore);
        MBGL_CHECK_ERROR(glDrawElements(GL_LINES, group->elements_length * 2, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * lineElementsBuffer.itemSize;
    }
}

void FillBucket::drawVertices(OutlinePatternShader& shader, gl::GLObjectStore& glObjectStore) {
    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : lineGroups) {
        assert(group);
        group->array[1].bind(shader, vertexBuffer, lineElementsBuffer, vertex_index, glObjectStore);
        MBGL_CHECK_ERROR(glDrawElements(GL_LINES, group->elements_length * 2, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * lineElementsBuffer.itemSize;
    }
}
