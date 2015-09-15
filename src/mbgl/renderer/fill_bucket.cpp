#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/geometry/fill_buffer.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layout.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/outline_shader.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/log.hpp>

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace mbgl;

void *FillBucket::alloc(void *, unsigned int size) {
    return ::malloc(size);
}

void *FillBucket::realloc(void *, void *ptr, unsigned int size) {
    return ::realloc(ptr, size);
}

void FillBucket::free(void *, void *ptr) {
    ::free(ptr);
}

FillBucket::FillBucket(FillVertexBuffer &vertexBuffer_,
                       TriangleElementsBuffer &triangleElementsBuffer_,
                       LineElementsBuffer &lineElementsBuffer_)
    : allocator(new TESSalloc{
          &alloc,
          &realloc,
          &free,
          nullptr, // userData
          64,      // meshEdgeBucketSize
          64,      // meshVertexBucketSize
          32,      // meshFaceBucketSize
          64,      // dictNodeBucketSize
          8,       // regionBucketSize
          128,     // extraVertices allocated for the priority queue.
      }),
      tesselator(tessNewTess(allocator)),
      vertexBuffer(vertexBuffer_),
      triangleElementsBuffer(triangleElementsBuffer_),
      lineElementsBuffer(lineElementsBuffer_),
      vertex_start(vertexBuffer_.index()),
      triangle_elements_start(triangleElementsBuffer_.index()),
      line_elements_start(lineElementsBuffer.index()) {
    assert(tesselator);
}

FillBucket::~FillBucket() {
    if (tesselator) {
        tessDeleteTess(tesselator);
    }
    if (allocator) {
        delete allocator;
    }
}

void FillBucket::addGeometry(const GeometryCollection& geometryCollection) {
    for (auto& line_ : geometryCollection) {
        for (auto& v : line_) {
            line.emplace_back(v.x, v.y);
        }
        if (!line.empty()) {
            clipper.AddPath(line, ClipperLib::ptSubject, true);
            line.clear();
            hasVertices = true;
        }
    }

    tessellate();
}

void FillBucket::tessellate() {
    if (!hasVertices) {
        return;
    }
    hasVertices = false;

    std::vector<std::vector<ClipperLib::IntPoint>> polygons;
    clipper.Execute(ClipperLib::ctUnion, polygons, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
    clipper.Clear();

    if (polygons.empty()) {
        return;
    }

    GLsizei total_vertex_count = 0;
    for (const auto& polygon : polygons) {
        total_vertex_count += polygon.size();
    }

    if (total_vertex_count > 65536) {
        throw geometry_too_long_exception();
    }

    if (lineGroups.empty() || (lineGroups.back()->vertex_length + total_vertex_count > 65535)) {
        // Move to a new group because the old one can't hold the geometry.
        lineGroups.emplace_back(std::make_unique<LineGroup>());
    }

    assert(lineGroups.back());
    LineGroup& lineGroup = *lineGroups.back();
    GLsizei lineIndex = lineGroup.vertex_length;

    for (const auto& polygon : polygons) {
        const GLsizei group_count = static_cast<GLsizei>(polygon.size());
        assert(group_count >= 3);

        std::vector<TESSreal> clipped_line;
        for (const auto& pt : polygon) {
            clipped_line.push_back(pt.X);
            clipped_line.push_back(pt.Y);
            vertexBuffer.add(pt.X, pt.Y);
        }

        for (GLsizei i = 0; i < group_count; i++) {
            const GLsizei prev_i = (i == 0 ? group_count : i) - 1;
            lineElementsBuffer.add(lineIndex + prev_i, lineIndex + i);
        }

        lineIndex += group_count;

        tessAddContour(tesselator, vertexSize, clipped_line.data(), stride, (int)clipped_line.size() / vertexSize);
    }

    lineGroup.elements_length += total_vertex_count;

    if (tessTesselate(tesselator, TESS_WINDING_ODD, TESS_POLYGONS, vertices_per_group, vertexSize, 0)) {
        const TESSreal *vertices = tessGetVertices(tesselator);
        const GLsizei vertex_count = tessGetVertexCount(tesselator);
        TESSindex *vertex_indices = const_cast<TESSindex *>(tessGetVertexIndices(tesselator));
        const TESSindex *elements = tessGetElements(tesselator);
        const int triangle_count = tessGetElementCount(tesselator);

        for (GLsizei i = 0; i < vertex_count; ++i) {
            if (vertex_indices[i] == TESS_UNDEF) {
                vertexBuffer.add(::round(vertices[i * 2]), ::round(vertices[i * 2 + 1]));
                vertex_indices[i] = (TESSindex)total_vertex_count;
                total_vertex_count++;
            }
        }

        if (triangleGroups.empty() || (triangleGroups.back()->vertex_length + total_vertex_count > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            triangleGroups.emplace_back(std::make_unique<TriangleGroup>());
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        assert(triangleGroups.back());
        TriangleGroup& triangleGroup = *triangleGroups.back();
        GLsizei triangleIndex = triangleGroup.vertex_length;

        for (int i = 0; i < triangle_count; ++i) {
            const TESSindex *element_group = &elements[i * vertices_per_group];

            if (element_group[0] != TESS_UNDEF && element_group[1] != TESS_UNDEF && element_group[2] != TESS_UNDEF) {
                const TESSindex a = vertex_indices[element_group[0]];
                const TESSindex b = vertex_indices[element_group[1]];
                const TESSindex c = vertex_indices[element_group[2]];

                if (a != TESS_UNDEF && b != TESS_UNDEF && c != TESS_UNDEF) {
                    triangleElementsBuffer.add(triangleIndex + a, triangleIndex + b, triangleIndex + c);
                } else {
#if defined(DEBUG)
                    // TODO: We're missing a vertex that was not part of the line.
                    Log::Error(Event::OpenGL, "undefined element buffer");
#endif
                }
            } else {
#if defined(DEBUG)
                Log::Error(Event::OpenGL, "undefined element buffer");
#endif
            }
        }

        triangleGroup.vertex_length += total_vertex_count;
        triangleGroup.elements_length += triangle_count;
    } else {
#if defined(DEBUG)
        Log::Error(Event::OpenGL, "tessellation failed");
#endif
    }

    // We're adding the total vertex count *after* we added additional vertices
    // in the tessellation step. They won't be part of the actual lines, but
    // we need to skip over them anyway if we draw the next group.
    lineGroup.vertex_length += total_vertex_count;
}

void FillBucket::upload() {
    vertexBuffer.upload();
    triangleElementsBuffer.upload();
    lineElementsBuffer.upload();

    // From now on, we're going to render during the opaque and translucent pass.
    uploaded = true;
}

void FillBucket::render(Painter& painter,
                        const StyleLayer& layer_desc,
                        const TileID& id,
                        const mat4& matrix) {
    painter.renderFill(*this, layer_desc, id, matrix);
}

bool FillBucket::hasData() const {
    return !triangleGroups.empty() || !lineGroups.empty();
}

void FillBucket::drawElements(PlainShader& shader) {
    GLbyte *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    GLbyte *elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer.itemSize);
    for (auto& group : triangleGroups) {
        assert(group);
        group->array[0].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void FillBucket::drawElements(PatternShader& shader) {
    GLbyte *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    GLbyte *elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer.itemSize);
    for (auto& group : triangleGroups) {
        assert(group);
        group->array[1].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void FillBucket::drawVertices(OutlineShader& shader) {
    GLbyte *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    GLbyte *elements_index = BUFFER_OFFSET(line_elements_start * lineElementsBuffer.itemSize);
    for (auto& group : lineGroups) {
        assert(group);
        group->array[0].bind(shader, vertexBuffer, lineElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_LINES, group->elements_length * 2, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * lineElementsBuffer.itemSize;
    }
}
