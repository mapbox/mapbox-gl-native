#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>

#include <llmr/platform/gl.hpp>


#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace llmr;



void *FillBucket::alloc(void *, unsigned int size) {
    return ::malloc(size);
}

void *FillBucket::realloc(void *, void *ptr, unsigned int size) {
    return ::realloc(ptr, size);
}

void FillBucket::free(void *, void *ptr) {
    ::free(ptr);
}

FillBucket::FillBucket(const std::shared_ptr<FillVertexBuffer>& vertexBuffer,
                       const std::shared_ptr<TriangleElementsBuffer>& triangleElementsBuffer,
                       const std::shared_ptr<LineElementsBuffer>& lineElementsBuffer,
                       const BucketDescription& bucket_desc)
    : geom_desc(bucket_desc.geometry),
    allocator(new TESSalloc {
    &alloc,
    &realloc,
    &free,
    nullptr, // userData
    64, // meshEdgeBucketSize
    64, // meshVertexBucketSize
    32, // meshFaceBucketSize
    64, // dictNodeBucketSize
    8, // regionBucketSize
    128, // extraVertices allocated for the priority queue.
}),
tesselator(tessNewTess(allocator)),
vertexBuffer(vertexBuffer),
triangleElementsBuffer(triangleElementsBuffer),
lineElementsBuffer(lineElementsBuffer),
vertex_start(vertexBuffer->index()),
triangle_elements_start(triangleElementsBuffer->index()),
line_elements_start(lineElementsBuffer->index()) {
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

void FillBucket::addGeometry(pbf& geom) {
    Geometry::command cmd;

    Coordinate coord;
    Geometry geometry(geom);
    int32_t x, y;
    while ((cmd = geometry.next(x, y)) != Geometry::end) {
        if (cmd == Geometry::move_to) {
            if (line.size()) {
                clipper.AddPath(line, ClipperLib::ptSubject, true);
                line.clear();
                hasVertices = true;
            }
        }
        line.emplace_back(x, y);
    }

    if (line.size()) {
        clipper.AddPath(line, ClipperLib::ptSubject, true);
        line.clear();
        hasVertices = true;
    }
}

void FillBucket::tessellate() {
    if (!hasVertices) {
        return;
    }
    hasVertices = false;


    std::vector<std::vector<ClipperLib::IntPoint>> polygons;
    clipper.Execute(ClipperLib::ctUnion, polygons, ClipperLib::pftPositive);
    clipper.Clear();

    if (polygons.size() == 0) {
        return;
    }

    size_t total_vertex_count = 0;
    for (const std::vector<ClipperLib::IntPoint>& polygon : polygons) {
        total_vertex_count += polygon.size();
    }

    if (total_vertex_count > 65536) {
        throw geometry_too_long_exception();
    }

    if (!lineGroups.size() || (lineGroups.back().vertex_length + total_vertex_count > 65535)) {
        // Move to a new group because the old one can't hold the geometry.
        lineGroups.emplace_back();
    }

    line_group_type& lineGroup = lineGroups.back();
    uint32_t lineIndex = lineGroup.vertex_length;

    for (const std::vector<ClipperLib::IntPoint>& polygon : polygons) {
        const size_t group_count = polygon.size();
        assert(group_count >= 3);

        std::vector<TESSreal> line;
        for (const ClipperLib::IntPoint& pt : polygon) {
            line.push_back(pt.X);
            line.push_back(pt.Y);
            vertexBuffer->add(pt.X, pt.Y);
        }

        for (size_t i = 0; i < group_count; i++) {
            const size_t prev_i = (i == 0 ? group_count : i) - 1;
            lineElementsBuffer->add(lineIndex + prev_i, lineIndex + i);
        }

        lineIndex += group_count;

        tessAddContour(tesselator, vertexSize, line.data(), stride, line.size() / vertexSize);
    }

    if (tessTesselate(tesselator, TESS_WINDING_POSITIVE, TESS_POLYGONS, vertices_per_group, vertexSize, 0)) {
        const TESSreal *vertices = tessGetVertices(tesselator);
        const size_t vertex_count = tessGetVertexCount(tesselator);
        TESSindex *vertex_indices = const_cast<TESSindex *>(tessGetVertexIndices(tesselator));
        const TESSindex *elements = tessGetElements(tesselator);
        const int triangle_count = tessGetElementCount(tesselator);

        for (size_t i = 0; i < vertex_count; ++i) {
            if (vertex_indices[i] == TESS_UNDEF) {
                vertexBuffer->add(round(vertices[i * 2]), round(vertices[i * 2 + 1]));
                vertex_indices[i] = total_vertex_count;
                total_vertex_count++;
            }
        }

        if (!triangleGroups.size() || (triangleGroups.back().vertex_length + total_vertex_count > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            triangleGroups.emplace_back();
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        triangle_group_type& triangleGroup = triangleGroups.back();
        uint32_t triangleIndex = triangleGroup.vertex_length;

        for (int i = 0; i < triangle_count; ++i) {
            const TESSindex *element_group = &elements[i * vertices_per_group];

            if (element_group[0] != TESS_UNDEF && element_group[1] != TESS_UNDEF && element_group[2] != TESS_UNDEF) {
                const TESSindex a = vertex_indices[element_group[0]];
                const TESSindex b = vertex_indices[element_group[1]];
                const TESSindex c = vertex_indices[element_group[2]];

                if (a != TESS_UNDEF && b != TESS_UNDEF && c != TESS_UNDEF) {
                    triangleElementsBuffer->add(triangleIndex + a, triangleIndex + b, triangleIndex + c);
                } else {
                    // TODO: We're missing a vertex that was not part of the line.
                    fprintf(stderr, "undefined element buffer\n");
                }
            } else {
                fprintf(stderr, "undefined element buffer\n");
            }
        }

        triangleGroup.vertex_length += total_vertex_count;
        triangleGroup.elements_length += triangle_count;
    } else {
        fprintf(stderr, "tessellation failed\n");
    }

    lineGroup.vertex_length += total_vertex_count;
    lineGroup.elements_length += total_vertex_count;
}

void FillBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderFill(*this, layer_name, id);
}

bool FillBucket::empty() const {
    return triangleGroups.empty() && lineGroups.empty();
}

void FillBucket::drawElements(PlainShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer->itemSize);
    char *elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer->itemSize);
    for (triangle_group_type& group : triangleGroups) {
        group.array.bind(shader, *vertexBuffer, *triangleElementsBuffer, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * vertexBuffer->itemSize;
        elements_index += group.elements_length * triangleElementsBuffer->itemSize;
    }
}

void FillBucket::drawElements(PatternShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer->itemSize);
    char *elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer->itemSize);
    for (triangle_group_type& group : triangleGroups) {
        group.array.bind(shader, *vertexBuffer, *triangleElementsBuffer, vertex_index);
        glDrawElements(GL_TRIANGLES, group.elements_length * 3, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * vertexBuffer->itemSize;
        elements_index += group.elements_length * triangleElementsBuffer->itemSize;
    }
}

void FillBucket::drawVertices(OutlineShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer->itemSize);
    char *elements_index = BUFFER_OFFSET(line_elements_start * lineElementsBuffer->itemSize);
    for (line_group_type& group : lineGroups) {
        group.array.bind(shader, *vertexBuffer, *lineElementsBuffer, vertex_index);
        glDrawElements(GL_LINES, group.elements_length * 2, GL_UNSIGNED_SHORT, elements_index);
        vertex_index += group.vertex_length * vertexBuffer->itemSize;
        elements_index += group.elements_length * lineElementsBuffer->itemSize;
    }
}
