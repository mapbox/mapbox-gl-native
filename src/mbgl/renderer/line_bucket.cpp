#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/geometry.hpp>

#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layout.hpp>
#include <mbgl/map/vector_tile.hpp>

#include <mbgl/util/math.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/platform/gl.hpp>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace mbgl;

LineBucket::LineBucket(std::unique_ptr<const StyleLayoutLine> styleLayout_,
                       LineVertexBuffer &vertexBuffer_,
                       TriangleElementsBuffer &triangleElementsBuffer_,
                       PointElementsBuffer &pointElementsBuffer_)
    : styleLayout(std::move(styleLayout_)),
      vertexBuffer(vertexBuffer_),
      triangleElementsBuffer(triangleElementsBuffer_),
      pointElementsBuffer(pointElementsBuffer_),
      vertex_start(vertexBuffer_.index()),
      triangle_elements_start(triangleElementsBuffer_.index()),
      point_elements_start(pointElementsBuffer_.index()) {
    assert(styleLayout);
}

LineBucket::~LineBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}


void LineBucket::addGeometry(pbf& geom) {
    std::vector<Coordinate> line;
    Geometry::command cmd;

    Coordinate coord;
    Geometry geometry(geom);
    int32_t x, y;
    while ((cmd = geometry.next(x, y)) != Geometry::end) {
        if (cmd == Geometry::move_to) {
            if (!line.empty()) {
                addGeometry(line);
                line.clear();
            }
        }
        line.emplace_back(x, y);
    }
    if (line.size()) {
        addGeometry(line);
    }
}

struct TriangleElement {
    TriangleElement(uint16_t a_, uint16_t b_, uint16_t c_) : a(a_), b(b_), c(c_) {}
    uint16_t a, b, c;
};

typedef uint16_t PointElement;

void LineBucket::addGeometry(const std::vector<Coordinate>& vertices) {
    auto &layout = *styleLayout;
    // TODO: use roundLimit
    // const float roundLimit = geometry.round_limit;

    if (vertices.size() < 2) {
        // fprintf(stderr, "a line must have at least two vertices\n");
        return;
    }

    Coordinate firstVertex = vertices.front();
    Coordinate lastVertex = vertices.back();
    bool closed = firstVertex.x == lastVertex.x && firstVertex.y == lastVertex.y;

    if (vertices.size() == 2 && closed) {
        // fprintf(stderr, "a line may not have coincident points\n");
        return;
    }

    CapType beginCap = layout.cap;
    CapType endCap = closed ? CapType::Butt : layout.cap;

    JoinType currentJoin = JoinType::Miter;

    Coordinate currentVertex = Coordinate::null(),
               prevVertex = Coordinate::null(),
               nextVertex = Coordinate::null();
    vec2<double> prevNormal = vec2<double>::null(),
                 nextNormal = vec2<double>::null();

    int32_t e1 = -1, e2 = -1, e3 = -1;

    int8_t flip = 1;
    double distance = 0;

    if (closed) {
        currentVertex = vertices[vertices.size() - 2];
        nextNormal = util::normal<double>(currentVertex, lastVertex);
    }

    int32_t start_vertex = (int32_t)vertexBuffer.index();

    std::vector<TriangleElement> triangle_store;
    std::vector<PointElement> point_store;

    for (size_t i = 0; i < vertices.size(); ++i) {
        if (nextNormal) prevNormal = { -nextNormal.x, -nextNormal.y };
        if (currentVertex) prevVertex = currentVertex;

        currentVertex = vertices[i];
        currentJoin = layout.join;

        if (prevVertex) distance += util::dist<double>(currentVertex, prevVertex);

        // Find the next vertex.
        if (i + 1 < vertices.size()) {
            nextVertex = vertices[i + 1];
        } else {
            nextVertex = Coordinate::null();
        }

        // If the line is closed, we treat the last vertex like the first vertex.
        if (!nextVertex && closed) {
            nextVertex = vertices[1];
        }

        if (nextVertex) {
            // if two consecutive vertices exist, skip one
            if (currentVertex.x == nextVertex.x && currentVertex.y == nextVertex.y) continue;
        }

        // Calculate the normal towards the next vertex in this line. In case
        // there is no next vertex, pretend that the line is continuing straight,
        // meaning that we are just reversing the previous normal
        if (nextVertex) {
            nextNormal = util::normal<double>(currentVertex, nextVertex);
        } else {
            nextNormal = { -prevNormal.x, -prevNormal.y };
        }

        // If we still don't have a previous normal, this is the beginning of a
        // non-closed line, so we're doing a straight "join".
        if (!prevNormal) {
            prevNormal = { -nextNormal.x, -nextNormal.y };
        }

        // Determine the normal of the join extrusion. It is the angle bisector
        // of the segments between the previous line and the next line.
        vec2<double> joinNormal = {
            prevNormal.x + nextNormal.x,
            prevNormal.y + nextNormal.y
        };

        // Cross product yields 0..1 depending on whether they are parallel
        // or perpendicular.
        double joinAngularity = nextNormal.x * joinNormal.y - nextNormal.y * joinNormal.x;
        joinNormal.x /= joinAngularity;
        joinNormal.y /= joinAngularity;
        double roundness = std::fmax(std::abs(joinNormal.x), std::abs(joinNormal.y));


        // Switch to miter joins if the angle is very low.
        if (currentJoin != JoinType::Miter) {
            if (std::fabs(joinAngularity) < 0.5 && roundness < layout.miter_limit) {
                currentJoin = JoinType::Miter;
            }
        }

        // Add offset square begin cap.
        if (!prevVertex && beginCap == CapType::Square) {
            // Add first vertex
            e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, // vertex pos
                                   flip * (prevNormal.x + prevNormal.y), flip * (-prevNormal.x + prevNormal.y), // extrude normal
                                   0, 0, distance) - start_vertex; // texture normal

            if (e1 >= 0 && e2 >= 0 && e3 >= 0) triangle_store.emplace_back(e1, e2, e3);
            e1 = e2; e2 = e3;

            // Add second vertex
            e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, // vertex pos
                                   flip * (prevNormal.x - prevNormal.y), flip * (prevNormal.x + prevNormal.y), // extrude normal
                                   0, 1, distance) - start_vertex; // texture normal

            if (e1 >= 0 && e2 >= 0 && e3 >= 0) triangle_store.emplace_back(e1, e2, e3);
            e1 = e2; e2 = e3;
        }

        // Add offset square end cap.
        else if (!nextVertex && endCap == CapType::Square) {
            // Add first vertex
            e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, // vertex pos
                                   nextNormal.x - flip * nextNormal.y, flip * nextNormal.x + nextNormal.y, // extrude normal
                                   0, 0, distance) - start_vertex; // texture normal

            if (e1 >= 0 && e2 >= 0 && e3 >= 0) triangle_store.emplace_back(e1, e2, e3);
            e1 = e2; e2 = e3;

            // Add second vertex
            e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, // vertex pos
                                   nextNormal.x + flip * nextNormal.y, -flip * nextNormal.x + nextNormal.y, // extrude normal
                                   0, 1, distance) - start_vertex; // texture normal

            if (e1 >= 0 && e2 >= 0 && e3 >= 0) triangle_store.emplace_back(e1, e2, e3);
            e1 = e2; e2 = e3;
        }

        else if (currentJoin == JoinType::Miter) {
            // MITER JOIN
            if (std::fabs(joinAngularity) < 0.01) {
                // The two normals are almost parallel.
                joinNormal.x = -nextNormal.y;
                joinNormal.y = nextNormal.x;
            } else if (roundness > layout.miter_limit) {
                // If the miter grows too large, flip the direction to make a
                // bevel join.
                joinNormal.x = (prevNormal.x - nextNormal.x) / joinAngularity;
                joinNormal.y = (prevNormal.y - nextNormal.y) / joinAngularity;
            }

            if (roundness > layout.miter_limit) {
                flip = -flip;
            }

            // Add first vertex
            e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, // vertex pos
                                   flip * joinNormal.x, flip * joinNormal.y, // extrude normal
                                   0, 0, distance) - start_vertex; // texture normal

            if (e1 >= 0 && e2 >= 0 && e3 >= 0) triangle_store.emplace_back(e1, e2, e3);
            e1 = e2; e2 = e3;

            // Add second vertex
            e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, // vertex pos
                                   -flip * joinNormal.x, -flip * joinNormal.y, // extrude normal
                                   0, 1, distance) - start_vertex; // texture normal

            if (e1 >= 0 && e2 >= 0 && e3 >= 0) triangle_store.emplace_back(e1, e2, e3);
            e1 = e2; e2 = e3;

            if ((!prevVertex && beginCap == CapType::Round) ||
                    (!nextVertex && endCap == CapType::Round)) {
                point_store.emplace_back(e1);
            }
        }

        else {
            // Close up the previous line
            // Add first vertex
            e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, // vertex pos
                                   flip * prevNormal.y, -flip * prevNormal.x, // extrude normal
                                   0, 0, distance) - start_vertex; // texture normal

            if (e1 >= 0 && e2 >= 0 && e3 >= 0) triangle_store.emplace_back(e1, e2, e3);
            e1 = e2; e2 = e3;

            // Add second vertex.
            e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, // vertex pos
                                   -flip * prevNormal.y, flip * prevNormal.x, // extrude normal
                                   0, 1, distance) - start_vertex; // texture normal

            if (e1 >= 0 && e2 >= 0 && e3 >= 0) triangle_store.emplace_back(e1, e2, e3);
            e1 = e2; e2 = e3;

            prevNormal = { -nextNormal.x, -nextNormal.y };
            flip = 1;


            // begin/end caps
            if ((!prevVertex && beginCap == CapType::Round) ||
                    (!nextVertex && endCap == CapType::Round)) {
                point_store.emplace_back(e1);
            }


            if (currentJoin == JoinType::Round) {
                if (prevVertex && nextVertex && (!closed || i > 0)) {
                    point_store.emplace_back(e1);
                }

                // Reset the previous vertices so that we don't accidentally create
                // any triangles.
                e1 = -1; e2 = -1; e3 = -1;
            }

            // Start the new quad.
            // Add first vertex
            e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, // vertex pos
                                   -flip * nextNormal.y, flip * nextNormal.x, // extrude normal
                                   0, 0, distance) - start_vertex; // texture normal

            if (e1 >= 0 && e2 >= 0 && e3 >= 0) triangle_store.emplace_back(e1, e2, e3);
            e1 = e2; e2 = e3;

            // Add second vertex
            e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, // vertex pos
                                   flip * nextNormal.y, -flip * nextNormal.x, // extrude normal
                                   0, 1, distance) - start_vertex; // texture normal

            if (e1 >= 0 && e2 >= 0 && e3 >= 0) triangle_store.emplace_back(e1, e2, e3);
            e1 = e2; e2 = e3;
        }
    }

    size_t end_vertex = vertexBuffer.index();
    size_t vertex_count = end_vertex - start_vertex;

    // Store the triangle/line groups.
    {
        if (!triangleGroups.size() || (triangleGroups.back()->vertex_length + vertex_count > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            triangleGroups.emplace_back(util::make_unique<triangle_group_type>());
        }

        assert(triangleGroups.back());
        triangle_group_type& group = *triangleGroups.back();
        for (const TriangleElement& triangle : triangle_store) {
            triangleElementsBuffer.add(
                group.vertex_length + triangle.a,
                group.vertex_length + triangle.b,
                group.vertex_length + triangle.c
            );
        }

        group.vertex_length += vertex_count;
        group.elements_length += triangle_store.size();
    }

    // Store the line join/cap groups.
    {
        if (!pointGroups.size() || (pointGroups.back()->vertex_length + vertex_count > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            pointGroups.emplace_back(util::make_unique<point_group_type>());
        }

        assert(pointGroups.back());
        point_group_type& group = *pointGroups.back();
        for (PointElement point : point_store) {
            pointElementsBuffer.add(group.vertex_length + point);
        }

        group.vertex_length += vertex_count;
        group.elements_length += point_store.size();
    }
}

void LineBucket::render(Painter& painter, util::ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix) {
    painter.renderLine(*this, layer_desc, id, matrix);
}

bool LineBucket::hasData() const {
    return !triangleGroups.empty() || !pointGroups.empty();
}

bool LineBucket::hasPoints() const {
    if (!pointGroups.empty()) {
        for (const auto& group : pointGroups) {
            assert(group);
            if (group->elements_length) {
                return true;
            }
        }
    }
    return false;
}

void LineBucket::drawLines(LineShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    char *elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer.itemSize);
    for (auto& group : triangleGroups) {
        assert(group);
        if (!group->elements_length) {
            continue;
        }
        group->array[0].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void LineBucket::drawLineSDF(LineSDFShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    char *elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer.itemSize);
    for (auto& group : triangleGroups) {
        assert(group);
        if (!group->elements_length) {
            continue;
        }
        group->array[2].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void LineBucket::drawLinePatterns(LinepatternShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    char *elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer.itemSize);
    for (auto& group : triangleGroups) {
        assert(group);
        if (!group->elements_length) {
            continue;
        }
        group->array[1].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void LineBucket::drawPoints(LinejoinShader& shader) {
    char *vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    char *elements_index = BUFFER_OFFSET(point_elements_start * pointElementsBuffer.itemSize);
    for (auto& group : pointGroups) {
        assert(group);
        if (!group->elements_length) {
            continue;
        }
        group->array[0].bind(shader, vertexBuffer, pointElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_POINTS, group->elements_length, GL_UNSIGNED_SHORT, elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * pointElementsBuffer.itemSize;
    }
}
