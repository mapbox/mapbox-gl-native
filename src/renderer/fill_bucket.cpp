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
                tessAddContour(tesselator, vertexSize, line.data(), stride, line.size() / vertexSize);
                line.clear();
                hasVertices = true;
            }
        }
        line.push_back(x);
        line.push_back(y);
    }

    if (line.size()) {
        tessAddContour(tesselator, vertexSize, line.data(), stride, line.size() / vertexSize);
        line.clear();
        hasVertices = true;
    }
}

void FillBucket::tessellate() {
    if (!hasVertices) {
        return;
    }

    hasVertices = false;

    // First combine contours and then triangulate, this removes unnecessary inner vertices.
    if (tessTesselate(tesselator, TESS_WINDING_POSITIVE, TESS_BOUNDARY_CONTOURS, 0, 0, 0)) {
        const TESSreal *vertices = tessGetVertices(tesselator);
        const int vertex_count = tessGetVertexCount(tesselator);
        const int element_count = tessGetElementCount(tesselator);
        const TESSindex *elements = tessGetElements(tesselator);

        if (vertex_count > 65536) {
            throw geometry_too_long_exception();
        }

        for (int i = 0; i < vertex_count; ++i) {
            vertexBuffer->add(vertices[i * 2], vertices[i * 2 + 1]);
        }


        if (!lineGroups.size() || (lineGroups.back().vertex_length + vertex_count > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            lineGroups.emplace_back();
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        line_group_type& group = lineGroups.back();
        uint32_t index = group.vertex_length;


        for (int i = 0; i < element_count; ++i) {
            const TESSindex group_start = elements[i * 2];
            const TESSindex group_end = group_start + elements[i * 2 + 1];

            for (int i = group_start; i < group_end; ++i) {
                const TESSindex prevI = (i == group_start ? group_end : i) - 1;
                lineElementsBuffer->add(index + prevI, index + i);
            }
        }

        // Add a line from the last vertex to the first vertex.
        // lineElementsBuffer->add(index + vertex_count - 1, index);

        group.vertex_length += vertex_count;
        group.elements_length += vertex_count;

        for (int i = 0; i < element_count; ++i) {
            const TESSindex group_start = elements[i * 2];
            const TESSindex group_count = elements[i * 2 + 1];
            const TESSreal *group_vertices = &vertices[group_start * 2];
            tessAddContour(tesselator, 2, group_vertices, stride, group_count);
        }

        if (tessTesselate(tesselator, TESS_WINDING_POSITIVE, TESS_POLYGONS, vertices_per_group, 2, 0)) {
            const TESSindex *vertex_indices = tessGetVertexIndices(tesselator);
            const TESSindex *elements = tessGetElements(tesselator);
            const int element_count = tessGetElementCount(tesselator);

            if (!triangleGroups.size() || (triangleGroups.back().vertex_length + vertex_count > 65535)) {
                // Move to a new group because the old one can't hold the geometry.
                triangleGroups.emplace_back();
            }

            // We're generating triangle fans, so we always start with the first
            // coordinate in this polygon.
            triangle_group_type& group = triangleGroups.back();
            uint32_t index = group.vertex_length;

            for (int i = 0; i < element_count; ++i) {
                const TESSindex *element_group = &elements[i * vertices_per_group];
                const TESSindex a = vertex_indices[element_group[0]];
                const TESSindex b = vertex_indices[element_group[1]];
                const TESSindex c = vertex_indices[element_group[2]];
                triangleElementsBuffer->add(index + a, index + b, index + c);
            }

            group.vertex_length += vertex_count;
            group.elements_length += element_count;

        } else {
            assert(false && "tesselation failed");
        }
    } else {
        assert(false && "tesselation failed");
    }
}

void FillBucket::addGeometry(const std::vector<Coordinate>& line) {
    uint32_t vertex_start = vertexBuffer->index();

    size_t length = line.size();

    // Don't add the first vertex twice.
    if (line.front() == line.back()) --length;

    for (size_t i = 0; i < length; i++) {
        const Coordinate& coord = line[i];
        vertexBuffer->add(coord.x, coord.y);
    }

    size_t vertex_end = vertexBuffer->index();

    size_t vertex_count = vertex_end - vertex_start;
    if (vertex_count > 65536) {
        throw geometry_too_long_exception();
    }

    {
        if (!triangleGroups.size() || (triangleGroups.back().vertex_length + vertex_count > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            triangleGroups.emplace_back();
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        triangle_group_type& group = triangleGroups.back();
        uint32_t index = group.vertex_length;
        for (size_t i = 2; i < vertex_count; ++i) {
            triangleElementsBuffer->add(index, index + i - 1, index + i);
        }

        group.vertex_length += vertex_count;
        group.elements_length += (vertex_count - 2);
    }

    {
        if (!lineGroups.size() || (lineGroups.back().vertex_length + vertex_count > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            lineGroups.emplace_back();
        }

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        line_group_type& group = lineGroups.back();
        uint32_t index = group.vertex_length;
        for (size_t i = 1; i < vertex_count; ++i) {
            lineElementsBuffer->add(index + i - 1, index + i);
        }

        // Add a line from the last vertex to the first vertex.
        lineElementsBuffer->add(index + vertex_count - 1, index);

        group.vertex_length += vertex_count;
        group.elements_length += vertex_count;
    }
}

void FillBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderFill(*this, layer_name, id);
}

bool FillBucket::empty() const {
    return triangleGroups.empty();
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
