#include <mbgl/renderer/buckets/line_bucket.hpp>
#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>

#include <cassert>

namespace mbgl {

using namespace style;

LineBucket::LineBucket(const BucketParameters& parameters,
                       const std::vector<const RenderLayer*>& layers,
                       const style::LineLayoutProperties::Unevaluated& layout_)
    : Bucket(LayerType::Line),
      layout(layout_.evaluate(PropertyEvaluationParameters(parameters.tileID.overscaledZ))),
      overscaling(parameters.tileID.overscaleFactor()),
      zoom(parameters.tileID.overscaledZ) {
    for (const auto& layer : layers) {
        paintPropertyBinders.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(layer->getID()),
            std::forward_as_tuple(
                layer->as<RenderLineLayer>()->evaluated,
                parameters.tileID.overscaledZ));
    }
}

void LineBucket::addFeature(const GeometryTileFeature& feature,
                            const GeometryCollection& geometryCollection) {
    for (auto& line : geometryCollection) {
        addGeometry(line, feature);
    }

    for (auto& pair : paintPropertyBinders) {
        pair.second.populateVertexVectors(feature, vertices.vertexSize());
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

void LineBucket::addGeometry(const GeometryCoordinates& coordinates, const GeometryTileFeature& feature) {
    const FeatureType type = feature.getType();
    const std::size_t len = [&coordinates] {
        std::size_t l = coordinates.size();
        // If the line has duplicate vertices at the end, adjust length to remove them.
        while (l >= 2 && coordinates[l - 1] == coordinates[l - 2]) {
            l--;
        }
        return l;
    }();

    const std::size_t first = [&coordinates, &len] {
        std::size_t i = 0;
        // If the line has duplicate vertices at the start, adjust index to remove them.
        while (i < len - 1 && coordinates[i] == coordinates[i + 1]) {
            i++;
        }
        return i;
    }();

    // Ignore invalid geometry.
    if (len < (type == FeatureType::Polygon ? 3 : 2)) {
        return;
    }

    const LineJoinType joinType = layout.evaluate<LineJoin>(zoom, feature);

    const float miterLimit = joinType == LineJoinType::Bevel ? 1.05f : float(layout.get<LineMiterLimit>());

    const double sharpCornerOffset = SHARP_CORNER_OFFSET * (float(util::EXTENT) / (util::tileSize * overscaling));

    const GeometryCoordinate firstCoordinate = coordinates[first];
    const LineCapType beginCap = layout.get<LineCap>();
    const LineCapType endCap = type == FeatureType::Polygon ? LineCapType::Butt : LineCapType(layout.get<LineCap>());

    double distance = 0;
    bool startOfLine = true;
    optional<GeometryCoordinate> currentCoordinate;
    optional<GeometryCoordinate> prevCoordinate;
    optional<GeometryCoordinate> nextCoordinate;
    optional<Point<double>> prevNormal;
    optional<Point<double>> nextNormal;

    // the last three vertices added
    e1 = e2 = e3 = -1;

    if (type == FeatureType::Polygon) {
        currentCoordinate = coordinates[len - 2];
        nextNormal = util::perp(util::unit(convertPoint<double>(firstCoordinate - *currentCoordinate)));
    }

    const std::size_t startVertex = vertices.vertexSize();
    std::vector<TriangleElement> triangleStore;

    for (std::size_t i = first; i < len; ++i) {
        if (type == FeatureType::Polygon && i == len - 1) {
            // if the line is closed, we treat the last vertex like the first
            nextCoordinate = coordinates[first + 1];
        } else if (i + 1 < len) {
            // just the next vertex
            nextCoordinate = coordinates[i + 1];
        } else {
            // there is no next vertex
            nextCoordinate = {};
        }

        // if two consecutive vertices exist, skip the current one
        if (nextCoordinate && coordinates[i] == *nextCoordinate) {
            continue;
        }

        if (nextNormal) {
            prevNormal = *nextNormal;
        }
        if (currentCoordinate) {
            prevCoordinate = *currentCoordinate;
        }

        currentCoordinate = coordinates[i];

        // Calculate the normal towards the next vertex in this line. In case
        // there is no next vertex, pretend that the line is continuing straight,
        // meaning that we are just using the previous normal.
        nextNormal = nextCoordinate ? util::perp(util::unit(convertPoint<double>(*nextCoordinate - *currentCoordinate)))
                                : prevNormal;

        // If we still don't have a previous normal, this is the beginning of a
        // non-closed line, so we're doing a straight "join".
        if (!prevNormal) {
            prevNormal = *nextNormal;
        }

        // Determine the normal of the join extrusion. It is the angle bisector
        // of the segments between the previous line and the next line.
        // In the case of 180° angles, the prev and next normals cancel each other out:
        // prevNormal + nextNormal = (0, 0), its magnitude is 0, so the unit vector would be
        // undefined. In that case, we're keeping the joinNormal at (0, 0), so that the cosHalfAngle
        // below will also become 0 and miterLength will become Infinity.
        Point<double> joinNormal = *prevNormal + *nextNormal;
        if (joinNormal.x != 0 || joinNormal.y != 0) {
            joinNormal = util::unit(joinNormal);
        }

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
        const double miterLength =
            cosHalfAngle != 0 ? 1 / cosHalfAngle : std::numeric_limits<double>::infinity();

        const bool isSharpCorner = cosHalfAngle < COS_HALF_SHARP_CORNER && prevCoordinate && nextCoordinate;

        if (isSharpCorner && i > first) {
            const auto prevSegmentLength = util::dist<double>(*currentCoordinate, *prevCoordinate);
            if (prevSegmentLength > 2.0 * sharpCornerOffset) {
                GeometryCoordinate newPrevVertex = *currentCoordinate - convertPoint<int16_t>(util::round(convertPoint<double>(*currentCoordinate - *prevCoordinate) * (sharpCornerOffset / prevSegmentLength)));
                distance += util::dist<double>(newPrevVertex, *prevCoordinate);
                addCurrentVertex(newPrevVertex, distance, *prevNormal, 0, 0, false, startVertex, triangleStore);
                prevCoordinate = newPrevVertex;
            }
        }

        // The join if a middle vertex, otherwise the cap
        const bool middleVertex = prevCoordinate && nextCoordinate;
        LineJoinType currentJoin = joinType;
        const LineCapType currentCap = nextCoordinate ? beginCap : endCap;

        if (middleVertex) {
            if (currentJoin == LineJoinType::Round) {
                if (miterLength < layout.get<LineRoundLimit>()) {
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
                // so if miterLength >= 2 we need to draw a different type of bevel here.
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
        if (prevCoordinate)
            distance += util::dist<double>(*currentCoordinate, *prevCoordinate);

        if (middleVertex && currentJoin == LineJoinType::Miter) {
            joinNormal = joinNormal * miterLength;
            addCurrentVertex(*currentCoordinate, distance, joinNormal, 0, 0, false, startVertex,
                             triangleStore);

        } else if (middleVertex && currentJoin == LineJoinType::FlipBevel) {
            // miter is too big, flip the direction to make a beveled join

            if (miterLength > 100) {
                // Almost parallel lines
                joinNormal = *nextNormal * -1.0;
            } else {
                const double direction = prevNormal->x * nextNormal->y - prevNormal->y * nextNormal->x > 0 ? -1 : 1;
                const double bevelLength = miterLength * util::mag(*prevNormal + *nextNormal) /
                                          util::mag(*prevNormal - *nextNormal);
                joinNormal = util::perp(joinNormal) * bevelLength * direction;
            }

            addCurrentVertex(*currentCoordinate, distance, joinNormal, 0, 0, false, startVertex,
                             triangleStore);

            addCurrentVertex(*currentCoordinate, distance, joinNormal * -1.0, 0, 0, false, startVertex,
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
                addCurrentVertex(*currentCoordinate, distance, *prevNormal, offsetA, offsetB, false,
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
                    addPieSliceVertex(*currentCoordinate, distance, approxFractionalJoinNormal, lineTurnsLeft, startVertex, triangleStore);
                }

                addPieSliceVertex(*currentCoordinate, distance, joinNormal, lineTurnsLeft, startVertex, triangleStore);

                for (int k = n - 1; k >= 0; k--) {
                    auto approxFractionalJoinNormal = util::unit(*prevNormal * ((k + 1.0) / (n + 1.0)) + *nextNormal);
                    addPieSliceVertex(*currentCoordinate, distance, approxFractionalJoinNormal, lineTurnsLeft, startVertex, triangleStore);
                }
            }

            // Start next segment
            if (nextCoordinate) {
                addCurrentVertex(*currentCoordinate, distance, *nextNormal, -offsetA, -offsetB,
                                 false, startVertex, triangleStore);
            }

        } else if (!middleVertex && currentCap == LineCapType::Butt) {
            if (!startOfLine) {
                // Close previous segment with a butt
                addCurrentVertex(*currentCoordinate, distance, *prevNormal, 0, 0, false,
                                 startVertex, triangleStore);
            }

            // Start next segment with a butt
            if (nextCoordinate) {
                addCurrentVertex(*currentCoordinate, distance, *nextNormal, 0, 0, false,
                                 startVertex, triangleStore);
            }

        } else if (!middleVertex && currentCap == LineCapType::Square) {
            if (!startOfLine) {
                // Close previous segment with a square cap
                addCurrentVertex(*currentCoordinate, distance, *prevNormal, 1, 1, false,
                                 startVertex, triangleStore);

                // The segment is done. Unset vertices to disconnect segments.
                e1 = e2 = -1;
            }

            // Start next segment
            if (nextCoordinate) {
                addCurrentVertex(*currentCoordinate, distance, *nextNormal, -1, -1, false,
                                 startVertex, triangleStore);
            }

        } else if (middleVertex ? currentJoin == LineJoinType::Round : currentCap == LineCapType::Round) {
            if (!startOfLine) {
                // Close previous segment with a butt
                addCurrentVertex(*currentCoordinate, distance, *prevNormal, 0, 0, false,
                                 startVertex, triangleStore);

                // Add round cap or linejoin at end of segment
                addCurrentVertex(*currentCoordinate, distance, *prevNormal, 1, 1, true, startVertex,
                                 triangleStore);

                // The segment is done. Unset vertices to disconnect segments.
                e1 = e2 = -1;
            }

            // Start next segment with a butt
            if (nextCoordinate) {
                // Add round cap before first segment
                addCurrentVertex(*currentCoordinate, distance, *nextNormal, -1, -1, true,
                                 startVertex, triangleStore);

                addCurrentVertex(*currentCoordinate, distance, *nextNormal, 0, 0, false,
                                 startVertex, triangleStore);
            }
        }

        if (isSharpCorner && i < len - 1) {
            const auto nextSegmentLength = util::dist<double>(*currentCoordinate, *nextCoordinate);
            if (nextSegmentLength > 2 * sharpCornerOffset) {
                GeometryCoordinate newCurrentVertex = *currentCoordinate + convertPoint<int16_t>(util::round(convertPoint<double>(*nextCoordinate - *currentCoordinate) * (sharpCornerOffset / nextSegmentLength)));
                distance += util::dist<double>(newCurrentVertex, *currentCoordinate);
                addCurrentVertex(newCurrentVertex, distance, *nextNormal, 0, 0, false, startVertex, triangleStore);
                currentCoordinate = newCurrentVertex;
            }
        }

        startOfLine = false;
    }

    const std::size_t endVertex = vertices.vertexSize();
    const std::size_t vertexCount = endVertex - startVertex;

    if (segments.empty() || segments.back().vertexLength + vertexCount > std::numeric_limits<uint16_t>::max()) {
        segments.emplace_back(startVertex, triangles.indexSize());
    }

    auto& segment = segments.back();
    assert(segment.vertexLength <= std::numeric_limits<uint16_t>::max());
    uint16_t index = segment.vertexLength;

    for (const auto& triangle : triangleStore) {
        triangles.emplace_back(index + triangle.a, index + triangle.b, index + triangle.c);
    }

    segment.vertexLength += vertexCount;
    segment.indexLength += triangleStore.size() * 3;
}

void LineBucket::addCurrentVertex(const GeometryCoordinate& currentCoordinate,
                                  double &distance,
                                  const Point<double>& normal,
                                  double endLeft,
                                  double endRight,
                                  bool round,
                                  std::size_t startVertex,
                                  std::vector<TriangleElement>& triangleStore) {
    Point<double> extrude = normal;
    if (endLeft)
        extrude = extrude - (util::perp(normal) * endLeft);
    vertices.emplace_back(LineProgram::layoutVertex(currentCoordinate, extrude, round, false, endLeft, distance * LINE_DISTANCE_SCALE));
    e3 = vertices.vertexSize() - 1 - startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }
    e1 = e2;
    e2 = e3;

    extrude = normal * -1.0;
    if (endRight)
        extrude = extrude - (util::perp(normal) * endRight);
    vertices.emplace_back(LineProgram::layoutVertex(currentCoordinate, extrude, round, true, -endRight, distance * LINE_DISTANCE_SCALE));
    e3 = vertices.vertexSize() - 1 - startVertex;
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
        addCurrentVertex(currentCoordinate, distance, normal, endLeft, endRight, round, startVertex, triangleStore);
    }
}

void LineBucket::addPieSliceVertex(const GeometryCoordinate& currentVertex,
                                   double distance,
                                   const Point<double>& extrude,
                                   bool lineTurnsLeft,
                                   std::size_t startVertex,
                                   std::vector<TriangleElement>& triangleStore) {
    Point<double> flippedExtrude = extrude * (lineTurnsLeft ? -1.0 : 1.0);
    vertices.emplace_back(LineProgram::layoutVertex(currentVertex, flippedExtrude, false, lineTurnsLeft, 0, distance * LINE_DISTANCE_SCALE));
    e3 = vertices.vertexSize() - 1 - startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }

    if (lineTurnsLeft) {
        e2 = e3;
    } else {
        e1 = e3;
    }
}

void LineBucket::upload(gl::Context& context) {
    vertexBuffer = context.createVertexBuffer(std::move(vertices));
    indexBuffer = context.createIndexBuffer(std::move(triangles));

    for (auto& pair : paintPropertyBinders) {
        pair.second.upload(context);
    }

    uploaded = true;
}

bool LineBucket::hasData() const {
    return !segments.empty();
}

template <class Property>
static float get(const RenderLineLayer& layer, const std::map<std::string, LineProgram::PaintPropertyBinders>& paintPropertyBinders) {
    auto it = paintPropertyBinders.find(layer.getID());
    if (it == paintPropertyBinders.end() || !it->second.statistics<Property>().max()) {
        return layer.evaluated.get<Property>().constantOr(Property::defaultValue());
    } else {
        return *it->second.statistics<Property>().max();
    }
}

float LineBucket::getLineWidth(const RenderLineLayer& layer) const {
    float lineWidth = get<LineWidth>(layer, paintPropertyBinders);
    float gapWidth = get<LineGapWidth>(layer, paintPropertyBinders);

    if (gapWidth) {
        return gapWidth + 2 * lineWidth;
    } else {
        return lineWidth;
    }
}

float LineBucket::getQueryRadius(const RenderLayer& layer) const {
    if (!layer.is<RenderLineLayer>()) {
        return 0;
    }

    auto lineLayer = layer.as<RenderLineLayer>();

    const std::array<float, 2>& translate = lineLayer->evaluated.get<LineTranslate>();
    float offset = get<LineOffset>(*lineLayer, paintPropertyBinders);
    return getLineWidth(*lineLayer) / 2.0 + std::abs(offset) + util::length(translate[0], translate[1]);
}


} // namespace mbgl
