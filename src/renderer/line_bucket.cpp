#include <llmr/renderer/line_bucket.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/shader-line.hpp>
#include <llmr/style/style.hpp>
#include <llmr/map/vector_tile.hpp>

#include <llmr/util/math.hpp>
#include <llmr/platform/gl.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace llmr;

LineBucket::LineBucket(const std::shared_ptr<LineBuffer>& buffer, const BucketDescription& bucket_desc)
    : geometry(bucket_desc.geometry),
      buffer(buffer),
      start(buffer->length()),
      length(0) {
}

void LineBucket::addGeometry(pbf& geom) {
    std::vector<Coordinate> line;
    Geometry::command cmd;

    Coordinate coord;
    Geometry geometry(geom);
    int32_t x, y;
    while ((cmd = geometry.next(x, y)) != Geometry::end) {
        if (cmd == Geometry::move_to) {
            if (line.size()) {
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

void LineBucket::addGeometry(const std::vector<Coordinate>& vertices) {
    const JoinType join = geometry.join;
    const CapType cap = geometry.cap;
    const float miterLimit = geometry.miter_limit;
    const float roundLimit = geometry.round_limit;

    if (vertices.size() < 1) {
        // alert('a line must have at least one vertex');
        return;
    }

    // Point
    if (vertices.size() == 1) {
        Coordinate coord = vertices.front();
        buffer->add(coord.x, coord.y, 1, 0, 0, 0);
        return;
    }

    Coordinate firstVertex = vertices.front();
    Coordinate lastVertex = vertices.back();
    bool closed = firstVertex.x == lastVertex.x && firstVertex.y == lastVertex.y;

    if (vertices.size() == 2 && closed) {
        // alert('a line may not have coincident points');
        return;
    }

    CapType beginCap = cap;
    CapType endCap = closed ? CapType::Butt : cap;

    Coordinate currentVertex = Coordinate::null(),
               prevVertex = Coordinate::null(),
               nextVertex = Coordinate::null();
    vec2<double> prevNormal = vec2<double>::null(),
                 nextNormal = vec2<double>::null();

    int8_t flip = 1;
    double distance = 0;

    if (closed) {
        currentVertex = vertices[vertices.size() - 2];
        nextNormal = util::normal<double>(currentVertex, lastVertex);
    }

    // Start all lines with a degenerate vertex
    buffer->addDegenerate();

    for (size_t i = 0; i < vertices.size(); i++) {
        if (nextNormal) prevNormal = { -nextNormal.x, -nextNormal.y };
        if (currentVertex) prevVertex = currentVertex;

        currentVertex = vertices[i];

        if (prevVertex) distance += util::dist<double>(currentVertex, prevVertex);

        // Find the next vertex.
        if (i + 1 < vertices.size()) {
            nextVertex = vertices[i + 1];
        } else {
            nextVertex = Coordinate::null();
        }

        bool segment = false;

        if (closed && i >= 2 && (i + 1 < vertices.size())) {
            segment = true;
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
        double roundness = fmax(abs(joinNormal.x), abs(joinNormal.y));


        bool roundJoin = false;
        // Determine whether we should actually draw a round/bevelled/butt line join. It looks
        // better if we do, but we can get away with drawing a mitered join for
        // joins that have a very small angle. For this, we have the "roundLimit"
        // parameter. We do this to reduce the number of vertices we have to
        // write into the line vertex buffer. Note that joinAngularity may be 0,
        // so the roundness grows to infinity. This is intended.
        if ((join == JoinType::Round || join == JoinType::Bevel || join == JoinType::Butt) && roundness > roundLimit) {
            roundJoin = true;
        }

        // Close up the previous line for a round join
        if (roundJoin && prevVertex && nextVertex) {
            // Add first vertex
            buffer->add(currentVertex.x, currentVertex.y, // vertex pos
                        flip * prevNormal.y, -flip * prevNormal.x, // extrude normal
                        0, 0, distance); // texture normal

            // Add second vertex.
            buffer->add(currentVertex.x, currentVertex.y, // vertex pos
                        -flip * prevNormal.y, flip * prevNormal.x, // extrude normal
                        0, 1, distance); // texture normal

            // Degenerate triangle
            if (join == JoinType::Round || join == JoinType::Butt) {
                buffer->addDegenerate();
            }

            if (join == JoinType::Round) prevVertex = Coordinate::null();

            prevNormal = { -nextNormal.x, -nextNormal.y };
            flip = 1;
        }

        // Add a cap.
        if (!prevVertex && (beginCap == CapType::Round || beginCap == CapType::Square || (roundJoin && join == JoinType::Round))) {
            int8_t tex = beginCap == CapType::Round || roundJoin ? 1 : 0;

            // Add first vertex
            buffer->add(currentVertex.x, currentVertex.y, // vertex pos
                        flip * (prevNormal.x + prevNormal.y), flip * (-prevNormal.x + prevNormal.y), // extrude normal
                        tex, 0, distance); // texture normal

            // Add second vertex
            buffer->add(currentVertex.x, currentVertex.y, // vertex pos
                        flip * (prevNormal.x - prevNormal.y), flip * (prevNormal.x + prevNormal.y), // extrude normal
                        tex, 1, distance); // texture normal
        }

        if (roundJoin) {
            // ROUND JOIN
            // Add first vertex
            buffer->add(currentVertex.x, currentVertex.y, // vertex pos
                        -flip * nextNormal.y, flip * nextNormal.x, // extrude normal
                        0, 0, distance); // texture normal

            // Add second vertex
            buffer->add(currentVertex.x, currentVertex.y, // vertex pos
                        flip * nextNormal.y, -flip * nextNormal.x, // extrude normal
                        0, 1, distance); // texture normal
        } else if ((nextVertex || endCap != CapType::Square) && (prevVertex || beginCap != CapType::Square)) {
            // MITER JOIN
            if (fabs(joinAngularity) < 0.01) {
                // The two normals are almost parallel.
                joinNormal.x = -nextNormal.y;
                joinNormal.y = nextNormal.x;
            } else if (roundness > miterLimit) {
                // If the miter grows too large, flip the direction to make a
                // bevel join.
                joinNormal.x = (prevNormal.x - nextNormal.x) / joinAngularity;
                joinNormal.y = (prevNormal.y - nextNormal.y) / joinAngularity;
                flip = -flip;
            }

            // Add first vertex
            buffer->add(currentVertex.x, currentVertex.y, // vertex pos
                        flip * joinNormal.x, flip * joinNormal.y, // extrude normal
                        0, 0, distance); // texture normal

            // Add second vertex
            buffer->add(currentVertex.x, currentVertex.y, // vertex pos
                        -flip * joinNormal.x, -flip * joinNormal.y, // extrude normal
                        0, 1, distance); // texture normal
        }

        // Add the end cap, but only if this vertex is distinct from the begin
        // vertex.
        if (!nextVertex && (endCap == CapType::Round || endCap == CapType::Square)) {
            int8_t capTex = endCap == CapType::Round ? 1 : 0;

            // Add first vertex
            buffer->add(currentVertex.x, currentVertex.y, // vertex pos
                        nextNormal.x - flip * nextNormal.y, flip * nextNormal.x + nextNormal.y, // extrude normal
                        capTex, 0, distance); // texture normal

            // Add second vertex
            buffer->add(currentVertex.x, currentVertex.y, // vertex pos
                        nextNormal.x + flip * nextNormal.y, -flip * nextNormal.x + nextNormal.y, // extrude normal
                        capTex, 1, distance); // texture normal
        }
    }

    // Update the length.
    length = buffer->length() - start;
}

void LineBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    painter.renderLine(*this, layer_name, id);
}

size_t LineBucket::size() const {
    return length;
}

void LineBucket::bind(LineShader& shader) {
    if (!array) {
        char *vertex_index = BUFFER_OFFSET(start * 4 * sizeof(int16_t));
        array.setup(shader, *buffer, vertex_index);
    } else {
        array.bind();
    }
}

void LineBucket::drawLines() {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, length);
}

void LineBucket::drawPoints() {
    glDrawArrays(GL_POINTS, 0, length);
}
