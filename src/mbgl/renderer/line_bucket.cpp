#include <mbgl/renderer/line_bucket.hpp>

#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layout.hpp>
#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/platform/gl.hpp>

#include <cassert>

using namespace mbgl;

LineBucket::LineBucket(LineVertexBuffer& vertexBuffer_,
                       TriangleElementsBuffer& triangleElementsBuffer_)
    : vertexBuffer(vertexBuffer_),
      triangleElementsBuffer(triangleElementsBuffer_),
      vertex_start(vertexBuffer_.index()),
      triangle_elements_start(triangleElementsBuffer_.index()){};

LineBucket::~LineBucket() {
    // Do not remove. header file only contains forward definitions to unique pointers.
}

void LineBucket::addGeometry(const GeometryCollection& geometryCollection) {
    for (auto& line : geometryCollection) {
        addGeometry(line);
    }
}

void LineBucket::addGeometry(const std::vector<Coordinate>& vertices) {
    const auto len = [&vertices] {
        auto l = vertices.size();
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

    const float miterLimit = layout.join == JoinType::Bevel ? 1.05f : layout.miter_limit;

    const Coordinate firstVertex = vertices.front();
    const Coordinate lastVertex = vertices[len - 1];
    const bool closed = firstVertex == lastVertex;

    if (len == 2 && closed) {
        // fprintf(stderr, "a line may not have coincident points\n");
        return;
    }

    const CapType beginCap = layout.cap;
    const CapType endCap = closed ? CapType::Butt : layout.cap;

    int8_t flip = 1;
    double distance = 0;
    bool startOfLine = true;
    Coordinate currentVertex = Coordinate::null(), prevVertex = Coordinate::null(),
               nextVertex = Coordinate::null();
    vec2<double> prevNormal = vec2<double>::null(), nextNormal = vec2<double>::null();

    // the last three vertices added
    e1 = e2 = e3 = -1;

    if (closed) {
        currentVertex = vertices[len - 2];
        nextNormal = util::perp(util::unit(vec2<double>(firstVertex - currentVertex)));
    }

    const int32_t startVertex = (int32_t)vertexBuffer.index();
    std::vector<TriangleElement> triangleStore;

    for (size_t i = 0; i < len; ++i) {
        if (closed && i == len - 1) {
            // if the line is closed, we treat the last vertex like the first
            nextVertex = vertices[1];
        } else if (i + 1 < len) {
            // just the next vertex
            nextVertex = vertices[i + 1];
        } else {
            // there is no next vertex
            nextVertex = Coordinate::null();
        }

        // if two consecutive vertices exist, skip the current one
        if (nextVertex && vertices[i] == nextVertex) {
            continue;
        }

        if (nextNormal) {
            prevNormal = nextNormal;
        }
        if (currentVertex) {
            prevVertex = currentVertex;
        }

        currentVertex = vertices[i];

        // Calculate how far along the line the currentVertex is
        if (prevVertex)
            distance += util::dist<double>(currentVertex, prevVertex);

        // Calculate the normal towards the next vertex in this line. In case
        // there is no next vertex, pretend that the line is continuing straight,
        // meaning that we are just using the previous normal.
        nextNormal = nextVertex ? util::perp(util::unit(vec2<double>(nextVertex - currentVertex)))
                                : prevNormal;

        // If we still don't have a previous normal, this is the beginning of a
        // non-closed line, so we're doing a straight "join".
        if (!prevNormal) {
            prevNormal = nextNormal;
        }

        // Determine the normal of the join extrusion. It is the angle bisector
        // of the segments between the previous line and the next line.
        vec2<double> joinNormal = util::unit(prevNormal + nextNormal);

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
        const float cosHalfAngle = joinNormal.x * nextNormal.x + joinNormal.y * nextNormal.y;
        const float miterLength = cosHalfAngle != 0 ? 1 / cosHalfAngle: 1;

        // The join if a middle vertex, otherwise the cap
        const bool middleVertex = prevVertex && nextVertex;
        JoinType currentJoin = layout.join;
        const CapType currentCap = nextVertex ? beginCap : endCap;

        if (middleVertex) {
            if (currentJoin == JoinType::Round) {
                if (miterLength < layout.round_limit) {
                    currentJoin = JoinType::Miter;
                } else if (miterLength <= 2) {
                    currentJoin = JoinType::FakeRound;
                }
            }

            if (currentJoin == JoinType::Miter && miterLength > miterLimit) {
                currentJoin = JoinType::Bevel;
            }

            if (currentJoin == JoinType::Bevel) {
                // The maximum extrude length is 128 / 63 = 2 times the width of the line
                // so if miterLength >= 2 we need to draw a different type of bevel where.
                if (miterLength > 2) {
                    currentJoin = JoinType::FlipBevel;
                }

                // If the miterLength is really small and the line bevel wouldn't be visible,
                // just draw a miter join to save a triangle.
                if (miterLength < miterLimit) {
                    currentJoin = JoinType::Miter;
                }
            }
        }

        if (middleVertex && currentJoin == JoinType::Miter) {
            joinNormal = joinNormal * miterLength;
            addCurrentVertex(currentVertex, flip, distance, joinNormal, 0, 0, false, startVertex,
                             triangleStore);

        } else if (middleVertex && currentJoin == JoinType::FlipBevel) {
            // miter is too big, flip the direction to make a beveled join

            if (miterLength > 100) {
                // Almost parallel lines
                joinNormal = nextNormal;
            } else {
                const float direction = prevNormal.x * nextNormal.y - prevNormal.y * nextNormal.x > 0 ? -1 : 1;
                const float bevelLength = miterLength * util::mag(prevNormal + nextNormal) /
                                          util::mag(prevNormal - nextNormal);
                joinNormal = util::perp(joinNormal) * bevelLength * direction;
            }

            addCurrentVertex(currentVertex, flip, distance, joinNormal, 0, 0, false, startVertex,
                             triangleStore);
            flip = -flip;

        } else if (middleVertex && (currentJoin == JoinType::Bevel || currentJoin == JoinType::FakeRound)) {
            const bool lineTurnsLeft = flip * (prevNormal.x * nextNormal.y - prevNormal.y * nextNormal.x) > 0;
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
                addCurrentVertex(currentVertex, flip, distance, prevNormal, offsetA, offsetB, false,
                                 startVertex, triangleStore);
            }

            if (currentJoin == JoinType::FakeRound) {
                // The join angle is sharp enough that a round join would be visible.
                // Bevel joins fill the gap between segments with a single pie slice triangle.
                // Create a round join by adding multiple pie slices. The join isn't actually round, but
                // it looks like it is at the sizes we render lines at.

                // Add more triangles for sharper angles.
                // This math is just a good enough approximation. It isn't "correct".
                const int n = std::floor((0.5 - (cosHalfAngle - 0.5)) * 8);

                for (int m = 0; m < n; m++) {
                    auto approxFractionalJoinNormal = util::unit(nextNormal * ((m + 1.0f) / (n + 1.0f)) + prevNormal);
                    addPieSliceVertex(currentVertex, flip, distance, approxFractionalJoinNormal, lineTurnsLeft, startVertex, triangleStore);
                }

                addPieSliceVertex(currentVertex, flip, distance, joinNormal, lineTurnsLeft, startVertex, triangleStore);

                for (int k = n - 1; k >= 0; k--) {
                    auto approxFractionalJoinNormal = util::unit(prevNormal * ((k + 1.0f) / (n + 1.0f)) + nextNormal);
                    addPieSliceVertex(currentVertex, flip, distance, approxFractionalJoinNormal, lineTurnsLeft, startVertex, triangleStore);
                }
            }

            // Start next segment
            if (nextVertex) {
                addCurrentVertex(currentVertex, flip, distance, nextNormal, -offsetA, -offsetB,
                                 false, startVertex, triangleStore);
            }

        } else if (!middleVertex && currentCap == CapType::Butt) {
            if (!startOfLine) {
                // Close previous segment with a butt
                addCurrentVertex(currentVertex, flip, distance, prevNormal, 0, 0, false,
                                 startVertex, triangleStore);
            }

            // Start next segment with a butt
            if (nextVertex) {
                addCurrentVertex(currentVertex, flip, distance, nextNormal, 0, 0, false,
                                 startVertex, triangleStore);
            }

        } else if (!middleVertex && currentCap == CapType::Square) {
            if (!startOfLine) {
                // Close previous segment with a square cap
                addCurrentVertex(currentVertex, flip, distance, prevNormal, 1, 1, false,
                                 startVertex, triangleStore);

                // The segment is done. Unset vertices to disconnect segments.
                e1 = e2 = -1;
                flip = 1;
            }

            // Start next segment
            if (nextVertex) {
                addCurrentVertex(currentVertex, flip, distance, nextNormal, -1, -1, false,
                                 startVertex, triangleStore);
            }

        } else if (middleVertex ? currentJoin == JoinType::Round : currentCap == CapType::Round) {
            if (!startOfLine) {
                // Close previous segment with a butt
                addCurrentVertex(currentVertex, flip, distance, prevNormal, 0, 0, false,
                                 startVertex, triangleStore);

                // Add round cap or linejoin at end of segment
                addCurrentVertex(currentVertex, flip, distance, prevNormal, 1, 1, true, startVertex,
                                 triangleStore);

                // The segment is done. Unset vertices to disconnect segments.
                e1 = e2 = -1;
                flip = 1;
            }

            // Start next segment with a butt
            if (nextVertex) {
                // Add round cap before first segment
                addCurrentVertex(currentVertex, flip, distance, nextNormal, -1, -1, true,
                                 startVertex, triangleStore);

                addCurrentVertex(currentVertex, flip, distance, nextNormal, 0, 0, false,
                                 startVertex, triangleStore);
            }
        }

        startOfLine = false;
    }

    const size_t endVertex = vertexBuffer.index();
    const size_t vertexCount = endVertex - startVertex;

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

void LineBucket::addCurrentVertex(const Coordinate& currentVertex,
                                  float flip,
                                  double distance,
                                  const vec2<double>& normal,
                                  float endLeft,
                                  float endRight,
                                  bool round,
                                  int32_t startVertex,
                                  std::vector<TriangleElement>& triangleStore) {
    int8_t tx = round ? 1 : 0;

    vec2<double> extrude = normal * flip;
    if (endLeft)
        extrude = extrude - (util::perp(normal) * endLeft);
    e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, extrude.x, extrude.y, tx, 0,
                                   distance) -
         startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }
    e1 = e2;
    e2 = e3;

    extrude = normal * (-flip);
    if (endRight)
        extrude = extrude - (util::perp(normal) * endRight);
    e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, extrude.x, extrude.y, tx, 1,
                                   distance) -
         startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }
    e1 = e2;
    e2 = e3;
}

void LineBucket::addPieSliceVertex(const Coordinate& currentVertex,
                                   float flip,
                                   double distance,
                                   const vec2<double>& extrude,
                                   bool lineTurnsLeft,
                                   int32_t startVertex,
                                  std::vector<TriangleElement>& triangleStore) {
    int8_t ty = lineTurnsLeft;

    auto flippedExtrude = extrude * (flip * (lineTurnsLeft ? -1 : 1));
    e3 = (int32_t)vertexBuffer.add(currentVertex.x, currentVertex.y, flippedExtrude.x, flippedExtrude.y, 0, ty,
                                   distance) - startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }

    if (lineTurnsLeft) {
        e2 = e3;
    } else {
        e1 = e3;
    }
}

void LineBucket::upload() {
    vertexBuffer.upload();
    triangleElementsBuffer.upload();

    // From now on, we're only going to render during the translucent pass.
    uploaded = true;
}

void LineBucket::render(Painter& painter,
                        const StyleLayer& layer_desc,
                        const TileID& id,
                        const mat4& matrix) {
    painter.renderLine(*this, layer_desc, id, matrix);
}

bool LineBucket::hasData() const {
    return !triangleGroups.empty();
}

void LineBucket::drawLines(LineShader& shader) {
    char* vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    char* elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer.itemSize);
    for (auto& group : triangleGroups) {
        assert(group);
        if (!group->elements_length) {
            continue;
        }
        group->array[0].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void LineBucket::drawLineSDF(LineSDFShader& shader) {
    char* vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    char* elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer.itemSize);
    for (auto& group : triangleGroups) {
        assert(group);
        if (!group->elements_length) {
            continue;
        }
        group->array[2].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}

void LineBucket::drawLinePatterns(LinepatternShader& shader) {
    char* vertex_index = BUFFER_OFFSET(vertex_start * vertexBuffer.itemSize);
    char* elements_index = BUFFER_OFFSET(triangle_elements_start * triangleElementsBuffer.itemSize);
    for (auto& group : triangleGroups) {
        assert(group);
        if (!group->elements_length) {
            continue;
        }
        group->array[1].bind(shader, vertexBuffer, triangleElementsBuffer, vertex_index);
        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elements_index));
        vertex_index += group->vertex_length * vertexBuffer.itemSize;
        elements_index += group->elements_length * triangleElementsBuffer.itemSize;
    }
}
