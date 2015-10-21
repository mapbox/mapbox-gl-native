#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/log.hpp>

#include <mapbox/earcut.hpp>

#include <cassert>

struct GeometryTooLongException : std::exception {};

using namespace mbgl;

namespace mapbox {
namespace util {
template <> struct nth<0, Coordinate> {
    inline static int64_t get(const Coordinate& t) { return t.x; };
};

template <> struct nth<1, Coordinate> {
    inline static int64_t get(const Coordinate& t) { return t.y; };
};
}
}

using Ring = std::vector<Coordinate>;

double signedArea(const Ring& ring) {
    double sum = 0;

    for (std::size_t i = 0, len = ring.size(), j = len - 1; i < len; j = i++) {
        const Coordinate& p1 = ring[i];
        const Coordinate& p2 = ring[j];
        sum += (p2.x - p1.x) * (p1.y + p2.y);
    }

    return sum;
}

// classifies an array of rings into polygons with outer rings and holes
std::vector<std::vector<Ring>> classifyRings(const std::vector<Ring>& rings) {
    std::vector<std::vector<Ring>> polygons;

    std::size_t len = rings.size();

    if (len <= 1) {
        polygons.push_back(rings);
        return polygons;
    }

    std::vector<Ring> polygon;
    bool ccw = false;

    for (std::size_t i = 0; i < len; i++) {
        double area = signedArea(rings[i]);

        if (area == 0)
            continue;

        if (!ccw)
            ccw = area < 0;

        if (ccw == (area < 0) && !polygon.empty()) {
            polygons.push_back(polygon);
            polygon.clear();
        }

        polygon.push_back(rings[i]);
    }

    if (!polygon.empty())
        polygons.push_back(polygon);

    return polygons;
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

        mapbox::Earcut<uint16_t> earcut;
        earcut(polygon);

        std::size_t nIndicies = earcut.indices.size();
        assert(nIndicies % 3 == 0);

        if (triangleGroups.empty() || triangleGroups.back()->vertex_length + totalVertices > 65535) {
            triangleGroups.emplace_back(std::make_unique<TriangleGroup>());
        }

        TriangleGroup& triangleGroup = *triangleGroups.back();
        GLsizei triangleIndex = triangleGroup.vertex_length;

        for (uint32_t i = 0; i < nIndicies; i += 3) {
            triangleElementsBuffer.add(triangleIndex + earcut.indices[i],
                                       triangleIndex + earcut.indices[i + 1],
                                       triangleIndex + earcut.indices[i + 2]);
        }

        triangleGroup.vertex_length += totalVertices;
        triangleGroup.elements_length += nIndicies / 3;
    }
}

void FillBucket::upload() {
    vertexBuffer.upload();
    triangleElementsBuffer.upload();
    lineElementsBuffer.upload();

    // From now on, we're going to render during the opaque and translucent pass.
    uploaded = true;
}

void FillBucket::render(Painter& painter,
                        const StyleLayer& layer,
                        const TileID& id,
                        const mat4& matrix) {
    painter.renderFill(*this, *layer.as<FillLayer>(), id, matrix);
}

bool FillBucket::hasData() const {
    return !triangleGroups.empty() || !lineGroups.empty();
}

void FillBucket::drawElements(PlainShader& shader) {
    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : triangleGroups) {
        assert(group);
        group->array[0].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void FillBucket::drawElements(PatternShader& shader) {
    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : triangleGroups) {
        assert(group);
        group->array[1].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void FillBucket::drawVertices(OutlineShader& shader) {
    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : lineGroups) {
        assert(group);
        group->array[0].bind(shader, vertexBuffer, lineElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_LINES, group->elements_length * 2, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * lineElementsBuffer.itemSize;
    }
}
