#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/gl/gl.hpp>

#include <cassert>

namespace mbgl {

using namespace style;

LineBucket::LineBucket(uint32_t overscaling_) : overscaling(overscaling_) {
}

LineBucket::~LineBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}

void LineBucket::addGeometry(const GeometryCollection& geometryCollection) {
    for (auto& line : geometryCollection) {
        addGeometry(line);
    }
}


/*
 * Sharp corners cause dashed lines to tilt because the distance along the line
 * is the same at both the inner and outer corners. To improve the appearance of
 * dashed lines we add extra points near sharp corners so that a smaller part
 * of the line is tilted.
 *
 * COS_HALF_SHARP_CORNER controls how sharp a corner has to be for us to add an
 * extra vertex. The default is 75 degrees.
 *
 * The newly created vertices are placed SHARP_CORNER_OFFSET pixels from the corner.
 */
const float COS_HALF_SHARP_CORNER = std::cos(75.0 / 2.0 * (M_PI / 180.0));
const float SHARP_CORNER_OFFSET = 15.0f;

// The number of bits that is used to store the line distance in the buffer.
const int LINE_DISTANCE_BUFFER_BITS = 14;

// We don't have enough bits for the line distance as we'd like to have, so
// use this value to scale the line distance (in tile units) down to a smaller
// value. This lets us store longer distances while sacrificing precision.
const float LINE_DISTANCE_SCALE = 1.0 / 2.0;

// The maximum line distance, in tile units, that fits in the buffer.
const float MAX_LINE_DISTANCE = std::pow(2, LINE_DISTANCE_BUFFER_BITS) / LINE_DISTANCE_SCALE;

void LineBucket::addGeometry(const GeometryCoordinates& vertices) {
    const GLsizei len = [&vertices] {
        GLsizei l = static_cast<GLsizei>(vertices.size());
        // If the line has duplicate vertices at the end, adjust length to remove them.
        while (l > 2 && vertices[l - 1] == vertices[l - 2]) {
            l--;
        }
        return l;
    }();

    if (len < 2) {
        // fprintf(stderr, "a line must have at least two vertices\n");
        return;
    }

    const float miterLimit = layout.lineJoin == LineJoinType::Bevel ? 1.05f : float(layout.lineMiterLimit);

    const double sharpCornerOffset = SHARP_CORNER_OFFSET * (float(util::EXTENT) / (util::tileSize * overscaling));

    const GeometryCoordinate firstVertex = vertices.front();
    const GeometryCoordinate lastVertex = vertices[len - 1];
    const bool closed = firstVertex == lastVertex;

    if (len == 2 && closed) {
        // fprintf(stderr, "a line may not have coincident points\n");
        return;
    }

    const LineCapType beginCap = layout.lineCap;
    const LineCapType endCap = closed ? LineCapType::Butt : LineCapType(layout.lineCap);

    double distance = 0;
    bool startOfLine = true;
    optional<GeometryCoordinate> currentVertex;
    optional<GeometryCoordinate> prevVertex;
    optional<GeometryCoordinate> nextVertex;
    optional<Point<double>> prevNormal;
    optional<Point<double>> nextNormal;

    // the last three vertices added
    e1 = e2 = e3 = -1;

    if (closed) {
        currentVertex = vertices[len - 2];
        nextNormal = util::perp(util::unit(convertPoint<double>(firstVertex - *currentVertex)));
    }

    const GLint startVertex = vertexBuffer.index();
    std::vector<TriangleElement> triangleStore;

    for (GLsizei i = 0; i < len; ++i) {
        if (closed && i == len - 1) {
            // if the line is closed, we treat the last vertex like the first
            nextVertex = vertices[1];
        } else if (i + 1 < len) {
            // just the next vertex
            nextVertex = vertices[i + 1];
        } else {
            // there is no next vertex
            nextVertex = {};
        }

        // if two consecutive vertices exist, skip the current one
        if (nextVertex && vertices[i] == *nextVertex) {
            continue;
        }

        if (nextNormal) {
            prevNormal = *nextNormal;
        }
        if (currentVertex) {
            prevVertex = *currentVertex;
        }

        currentVertex = vertices[i];

        // Calculate the normal towards the next vertex in this line. In case
        // there is no next vertex, pretend that the line is continuing straight,
        // meaning that we are just using the previous normal.
        nextNormal = nextVertex ? util::perp(util::unit(convertPoint<double>(*nextVertex - *currentVertex)))
                                : prevNormal;

        // If we still don't have a previous normal, this is the beginning of a
        // non-closed line, so we're doing a straight "join".
        if (!prevNormal) {
            prevNormal = *nextNormal;
        }

        // Determine the normal of the join extrusion. It is the angle bisector
        // of the segments between the previous line and the next line.
        Point<double> joinNormal = util::unit(*prevNormal + *nextNormal);

        /*  joinNormal     prevNormal
         *             ↖      ↑
         *                .________. prevVertex
         *                |
         * nextNormal  ←  |  currentVertex
         *                |
         *     nextVertex !
         *
         */

        // Calculate the length of the miter (the ratio of the miter to the width).
        // Find the cosine of the angle between the next and join normals
        // using dot product. The inverse of that is the miter length.
        const double cosHalfAngle = joinNormal.x * nextNormal->x + joinNormal.y * nextNormal->y;
        const double miterLength = cosHalfAngle != 0 ? 1 / cosHalfAngle: 1;

        const bool isSharpCorner = cosHalfAngle < COS_HALF_SHARP_CORNER && prevVertex && nextVertex;

        if (isSharpCorner && i > 0) {
            const double prevSegmentLength = util::dist<double>(*currentVertex, *prevVertex);
            if (prevSegmentLength > 2.0 * sharpCornerOffset) {
                GeometryCoordinate newPrevVertex = *currentVertex - convertPoint<int16_t>(util::round(convertPoint<double>(*currentVertex - *prevVertex) * (sharpCornerOffset / prevSegmentLength)));
                distance += util::dist<double>(newPrevVertex, *prevVertex);
                addCurrentVertex(newPrevVertex, distance, *prevNormal, 0, 0, false, startVertex, triangleStore);
                prevVertex = newPrevVertex;
            }
        }

        // The join if a middle vertex, otherwise the cap
        const bool middleVertex = prevVertex && nextVertex;
        LineJoinType currentJoin = layout.lineJoin;
        const LineCapType currentCap = nextVertex ? beginCap : endCap;

        if (middleVertex) {
            if (currentJoin == LineJoinType::Round) {
                if (miterLength < layout.lineRoundLimit) {
                    currentJoin = LineJoinType::Miter;
                } else if (miterLength <= 2) {
                    currentJoin = LineJoinType::FakeRound;
                }
            }

            if (currentJoin == LineJoinType::Miter && miterLength > miterLimit) {
                currentJoin = LineJoinType::Bevel;
            }

            if (currentJoin == LineJoinType::Bevel) {
                // The maximum extrude length is 128 / 63 = 2 times the width of the line
                // so if miterLength >= 2 we need to draw a different type of bevel where.
                if (miterLength > 2) {
                    currentJoin = LineJoinType::FlipBevel;
                }

                // If the miterLength is really small and the line bevel wouldn't be visible,
                // just draw a miter join to save a triangle.
                if (miterLength < miterLimit) {
                    currentJoin = LineJoinType::Miter;
                }
            }
        }

        // Calculate how far along the line the currentVertex is
        if (prevVertex)
            distance += util::dist<double>(*currentVertex, *prevVertex);

        if (middleVertex && currentJoin == LineJoinType::Miter) {
            joinNormal = joinNormal * miterLength;
            addCurrentVertex(*currentVertex, distance, joinNormal, 0, 0, false, startVertex,
                             triangleStore);

        } else if (middleVertex && currentJoin == LineJoinType::FlipBevel) {
            // miter is too big, flip the direction to make a beveled join

            if (miterLength > 100) {
                // Almost parallel lines
                joinNormal = *nextNormal;
            } else {
                const double direction = prevNormal->x * nextNormal->y - prevNormal->y * nextNormal->x > 0 ? -1 : 1;
                const double bevelLength = miterLength * util::mag(*prevNormal + *nextNormal) /
                                          util::mag(*prevNormal - *nextNormal);
                joinNormal = util::perp(joinNormal) * bevelLength * direction;
            }

            addCurrentVertex(*currentVertex, distance, joinNormal, 0, 0, false, startVertex,
                             triangleStore);

            addCurrentVertex(*currentVertex, distance, joinNormal * -1.0, 0, 0, false, startVertex,
                             triangleStore);
        } else if (middleVertex && (currentJoin == LineJoinType::Bevel || currentJoin == LineJoinType::FakeRound)) {
            const bool lineTurnsLeft = (prevNormal->x * nextNormal->y - prevNormal->y * nextNormal->x) > 0;
            const float offset = -std::sqrt(miterLength * miterLength - 1);
            float offsetA;
            float offsetB;

            if (lineTurnsLeft) {
                offsetB = 0;
                offsetA = offset;
            } else {
                offsetA = 0;
                offsetB = offset;
            }

            // Close previous segement with bevel
            if (!startOfLine) {
                addCurrentVertex(*currentVertex, distance, *prevNormal, offsetA, offsetB, false,
                                 startVertex, triangleStore);
            }

            if (currentJoin == LineJoinType::FakeRound) {
                // The join angle is sharp enough that a round join would be visible.
                // Bevel joins fill the gap between segments with a single pie slice triangle.
                // Create a round join by adding multiple pie slices. The join isn't actually round, but
                // it looks like it is at the sizes we render lines at.

                // Add more triangles for sharper angles.
                // This math is just a good enough approximation. It isn't "correct".
                const int n = std::floor((0.5 - (cosHalfAngle - 0.5)) * 8);

                for (int m = 0; m < n; m++) {
                    auto approxFractionalJoinNormal = util::unit(*nextNormal * ((m + 1.0) / (n + 1.0)) + *prevNormal);
                    addPieSliceVertex(*currentVertex, distance, approxFractionalJoinNormal, lineTurnsLeft, startVertex, triangleStore);
                }

                addPieSliceVertex(*currentVertex, distance, joinNormal, lineTurnsLeft, startVertex, triangleStore);

                for (int k = n - 1; k >= 0; k--) {
                    auto approxFractionalJoinNormal = util::unit(*prevNormal * ((k + 1.0) / (n + 1.0)) + *nextNormal);
                    addPieSliceVertex(*currentVertex, distance, approxFractionalJoinNormal, lineTurnsLeft, startVertex, triangleStore);
                }
            }

            // Start next segment
            if (nextVertex) {
                addCurrentVertex(*currentVertex, distance, *nextNormal, -offsetA, -offsetB,
                                 false, startVertex, triangleStore);
            }

        } else if (!middleVertex && currentCap == LineCapType::Butt) {
            if (!startOfLine) {
                // Close previous segment with a butt
                addCurrentVertex(*currentVertex, distance, *prevNormal, 0, 0, false,
                                 startVertex, triangleStore);
            }

            // Start next segment with a butt
            if (nextVertex) {
                addCurrentVertex(*currentVertex, distance, *nextNormal, 0, 0, false,
                                 startVertex, triangleStore);
            }

        } else if (!middleVertex && currentCap == LineCapType::Square) {
            if (!startOfLine) {
                // Close previous segment with a square cap
                addCurrentVertex(*currentVertex, distance, *prevNormal, 1, 1, false,
                                 startVertex, triangleStore);

                // The segment is done. Unset vertices to disconnect segments.
                e1 = e2 = -1;
            }

            // Start next segment
            if (nextVertex) {
                addCurrentVertex(*currentVertex, distance, *nextNormal, -1, -1, false,
                                 startVertex, triangleStore);
            }

        } else if (middleVertex ? currentJoin == LineJoinType::Round : currentCap == LineCapType::Round) {
            if (!startOfLine) {
                // Close previous segment with a butt
                addCurrentVertex(*currentVertex, distance, *prevNormal, 0, 0, false,
                                 startVertex, triangleStore);

                // Add round cap or linejoin at end of segment
                addCurrentVertex(*currentVertex, distance, *prevNormal, 1, 1, true, startVertex,
                                 triangleStore);

                // The segment is done. Unset vertices to disconnect segments.
                e1 = e2 = -1;
            }

            // Start next segment with a butt
            if (nextVertex) {
                // Add round cap before first segment
                addCurrentVertex(*currentVertex, distance, *nextNormal, -1, -1, true,
                                 startVertex, triangleStore);

                addCurrentVertex(*currentVertex, distance, *nextNormal, 0, 0, false,
                                 startVertex, triangleStore);
            }
        }

        if (isSharpCorner && i < len - 1) {
            const double nextSegmentLength = util::dist<double>(*currentVertex, *nextVertex);
            if (nextSegmentLength > 2 * sharpCornerOffset) {
                GeometryCoordinate newCurrentVertex = *currentVertex + convertPoint<int16_t>(util::round(convertPoint<double>(*nextVertex - *currentVertex) * (sharpCornerOffset / nextSegmentLength)));
                distance += util::dist<double>(newCurrentVertex, *currentVertex);
                addCurrentVertex(newCurrentVertex, distance, *nextNormal, 0, 0, false, startVertex, triangleStore);
                currentVertex = newCurrentVertex;
            }
        }

        startOfLine = false;
    }

    const GLsizei endVertex = vertexBuffer.index();
    const GLsizei vertexCount = endVertex - startVertex;

    // Store the triangle/line groups.
    {
        if (triangleGroups.empty() || (triangleGroups.back()->vertex_length + vertexCount > 65535)) {
            // Move to a new group because the old one can't hold the geometry.
            triangleGroups.emplace_back(std::make_unique<TriangleGroup>());
        }

        assert(triangleGroups.back());
        auto& group = *triangleGroups.back();
        for (const auto& triangle : triangleStore) {
            triangleElementsBuffer.add(group.vertex_length + triangle.a,
                                       group.vertex_length + triangle.b,
                                       group.vertex_length + triangle.c);
        }

        group.vertex_length += vertexCount;
        group.elements_length += triangleStore.size();
    }
}

void LineBucket::addCurrentVertex(const GeometryCoordinate& currentVertex,
                                  double &distance,
                                  const Point<double>& normal,
                                  double endLeft,
                                  double endRight,
                                  bool round,
                                  GLint startVertex,
                                  std::vector<TriangleElement>& triangleStore) {
    int8_t tx = round ? 1 : 0;

    Point<double> extrude = normal;
    if (endLeft)
        extrude = extrude - (util::perp(normal) * endLeft);
    e3 = vertexBuffer.add(currentVertex.x, currentVertex.y, extrude.x, extrude.y, tx, 0, endLeft, distance * LINE_DISTANCE_SCALE)
         - startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }
    e1 = e2;
    e2 = e3;

    extrude = normal * -1.0;
    if (endRight)
        extrude = extrude - (util::perp(normal) * endRight);
    e3 = vertexBuffer.add(currentVertex.x, currentVertex.y, extrude.x, extrude.y, tx, 1, -endRight, distance * LINE_DISTANCE_SCALE)
         - startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }
    e1 = e2;
    e2 = e3;

    // There is a maximum "distance along the line" that we can store in the buffers.
    // When we get close to the distance, reset it to zero and add the vertex again with
    // a distance of zero. The max distance is determined by the number of bits we allocate
    // to `linesofar`.
    if (distance > MAX_LINE_DISTANCE / 2.0f) {
        distance = 0;
        addCurrentVertex(currentVertex, distance, normal, endLeft, endRight, round, startVertex, triangleStore);
    }
}

void LineBucket::addPieSliceVertex(const GeometryCoordinate& currentVertex,
                                   double distance,
                                   const Point<double>& extrude,
                                   bool lineTurnsLeft,
                                   GLint startVertex,
                                  std::vector<TriangleElement>& triangleStore) {
    int8_t ty = lineTurnsLeft;

    Point<double> flippedExtrude = extrude * (lineTurnsLeft ? -1.0 : 1.0);
    e3 = vertexBuffer.add(currentVertex.x, currentVertex.y, flippedExtrude.x, flippedExtrude.y, 0, ty, 0, distance * LINE_DISTANCE_SCALE)
         - startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }

    if (lineTurnsLeft) {
        e2 = e3;
    } else {
        e1 = e3;
    }
}

void LineBucket::upload(gl::ObjectStore& store, gl::Config&) {
    vertexBuffer.upload(store);
    triangleElementsBuffer.upload(store);

    // From now on, we're only going to render during the translucent pass.
    uploaded = true;
}

void LineBucket::render(Painter& painter,
                        PaintParameters& parameters,
                        const Layer& layer,
                        const RenderTile& tile) {
    painter.renderLine(parameters, *this, *layer.as<LineLayer>(), tile);
}

bool LineBucket::hasData() const {
    return !triangleGroups.empty();
}

bool LineBucket::needsClipping() const {
    return true;
}

void LineBucket::drawLines(LineShader& shader, gl::ObjectStore& store, bool overdraw) {
    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : triangleGroups) {
        assert(group);
        if (!group->elements_length) {
            continue;
        }
        group->array[overdraw ? 1 : 0].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void LineBucket::drawLineSDF(LineSDFShader& shader, gl::ObjectStore& store, bool overdraw) {
    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : triangleGroups) {
        assert(group);
        if (!group->elements_length) {
            continue;
        }
        group->array[overdraw ? 3 : 2].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void LineBucket::drawLinePatterns(LinepatternShader& shader, gl::ObjectStore& store, bool overdraw) {
    GLbyte* vertex_index = BUFFER_OFFSET(0);
    GLbyte* elements_index = BUFFER_OFFSET(0);
    for (auto& group : triangleGroups) {
        assert(group);
        if (!group->elements_length) {
            continue;
        }
        group->array[overdraw ? 5 : 4].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index, store);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

} // namespace mbgl
