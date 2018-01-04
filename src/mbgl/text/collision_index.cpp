#include <mbgl/text/collision_index.hpp>
#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/util/intersection_tests.hpp>
#include <mbgl/layout/symbol_projection.hpp>

#include <mapbox/geometry/envelope.hpp>

#include <mbgl/renderer/buckets/symbol_bucket.hpp> // For PlacedSymbol: pull out to another location

#include <cmath>

namespace mbgl {

// When a symbol crosses the edge that causes it to be included in
// collision detection, it will cause changes in the symbols around
// it. This constant specifies how many pixels to pad the edge of
// the viewport for collision detection so that the bulk of the changes
// occur offscreen. Making this constant greater increases label
// stability, but it's expensive.
static const float viewportPadding = 100;

CollisionIndex::CollisionIndex(const TransformState& transformState_)
    : transformState(transformState_)
    , collisionGrid(transformState.getSize().width + 2 * viewportPadding, transformState.getSize().height + 2 * viewportPadding, 25)
    , ignoredGrid(transformState.getSize().width + 2 * viewportPadding, transformState.getSize().height + 2 * viewportPadding, 25)
    , screenRightBoundary(transformState.getSize().width + viewportPadding)
    , screenBottomBoundary(transformState.getSize().height + viewportPadding)
    , gridRightBoundary(transformState.getSize().width + 2 * viewportPadding)
    , gridBottomBoundary(transformState.getSize().height + 2 * viewportPadding)
    , pitchFactor(std::cos(transformState.getPitch()) * transformState.getCameraToCenterDistance())
{}

float CollisionIndex::approximateTileDistance(const TileDistance& tileDistance, const float lastSegmentAngle, const float pixelsToTileUnits, const float cameraToAnchorDistance, const bool pitchWithMap) {
    // This is a quick and dirty solution for chosing which collision circles to use (since collision circles are
    // laid out in tile units). Ideally, I think we should generate collision circles on the fly in viewport coordinates
    // at the time we do collision detection.

    // incidenceStretch is the ratio of how much y space a label takes up on a tile while drawn perpendicular to the viewport vs
    //  how much space it would take up if it were drawn flat on the tile
    // Using law of sines, camera_to_anchor/sin(ground_angle) = camera_to_center/sin(incidence_angle)
    // Incidence angle 90 -> head on, sin(incidence_angle) = 1, no stretch
    // Incidence angle 1 -> very oblique, sin(incidence_angle) =~ 0, lots of stretch
    // ground_angle = u_pitch + PI/2 -> sin(ground_angle) = cos(u_pitch)
    // incidenceStretch = 1 / sin(incidenceAngle)

    const float incidenceStretch = pitchWithMap ? 1 : cameraToAnchorDistance / pitchFactor;
    const float lastSegmentTile = tileDistance.lastSegmentViewportDistance * pixelsToTileUnits;
    return tileDistance.prevTileDistance +
        lastSegmentTile +
        (incidenceStretch - 1) * lastSegmentTile * std::abs(std::sin(lastSegmentAngle));
}

bool CollisionIndex::isOffscreen(const CollisionBox& box) const {
    return box.px2 < viewportPadding || box.px1 >= screenRightBoundary || box.py2 < viewportPadding || box.py1 >= screenBottomBoundary;
}

bool CollisionIndex::isInsideGrid(const CollisionBox& box) const {
    return box.px2 >= 0 && box.px1 < gridRightBoundary && box.py2 >= 0 && box.py1 < gridBottomBoundary;
}


std::pair<bool,bool> CollisionIndex::placeFeature(CollisionFeature& feature,
                                      const mat4& posMatrix,
                                      const mat4& labelPlaneMatrix,
                                      const float textPixelRatio,
                                      PlacedSymbol& symbol,
                                      const float scale,
                                      const float fontSize,
                                      const bool allowOverlap,
                                      const bool pitchWithMap,
                                      const bool collisionDebug) {
    if (!feature.alongLine) {
        CollisionBox& box = feature.boxes.front();
        const auto projectedPoint = projectAndGetPerspectiveRatio(posMatrix, box.anchor);
        const float tileToViewport = textPixelRatio * projectedPoint.second;
        box.px1 = box.x1 * tileToViewport + projectedPoint.first.x;
        box.py1 = box.y1 * tileToViewport + projectedPoint.first.y;
        box.px2 = box.x2 * tileToViewport + projectedPoint.first.x;
        box.py2 = box.y2 * tileToViewport + projectedPoint.first.y;

        if (!isInsideGrid(box) ||
            (!allowOverlap && collisionGrid.hitTest({{ box.px1, box.py1 }, { box.px2, box.py2 }}))) {
            return { false, false };
        }

        return {true, isOffscreen(box)};
    } else {
        return placeLineFeature(feature, posMatrix, labelPlaneMatrix, textPixelRatio, symbol, scale, fontSize, allowOverlap, pitchWithMap, collisionDebug);
    }
}

std::pair<bool,bool> CollisionIndex::placeLineFeature(CollisionFeature& feature,
                                      const mat4& posMatrix,
                                      const mat4& labelPlaneMatrix,
                                      const float textPixelRatio,
                                      PlacedSymbol& symbol,
                                      const float scale,
                                      const float fontSize,
                                      const bool allowOverlap,
                                      const bool pitchWithMap,
                                      const bool collisionDebug) {

    const auto tileUnitAnchorPoint = symbol.anchorPoint;
    const auto projectedAnchor = projectAnchor(posMatrix, tileUnitAnchorPoint);

    const float fontScale = fontSize / 24;
    const float lineOffsetX = symbol.lineOffset[0] * fontSize;
    const float lineOffsetY = symbol.lineOffset[1] * fontSize;

    const auto labelPlaneAnchorPoint = project(tileUnitAnchorPoint, labelPlaneMatrix).first;

    const auto firstAndLastGlyph = placeFirstAndLastGlyph(
        fontScale,
        lineOffsetX,
        lineOffsetY,
        /*flip*/ false,
        labelPlaneAnchorPoint,
        tileUnitAnchorPoint,
        symbol,
        labelPlaneMatrix,
        /*return tile distance*/ true);

    bool collisionDetected = false;
    bool inGrid = false;
    bool entirelyOffscreen = true;

    const auto tileToViewport = projectedAnchor.first * textPixelRatio;
    // pixelsToTileUnits is used for translating line geometry to tile units
    // ... so we care about 'scale' but not 'perspectiveRatio'
    // equivalent to pixel_to_tile_units
    const auto pixelsToTileUnits = 1 / (textPixelRatio * scale);

    float firstTileDistance = 0, lastTileDistance = 0;
    if (firstAndLastGlyph) {
        firstTileDistance = approximateTileDistance(*(firstAndLastGlyph->first.tileDistance), firstAndLastGlyph->first.angle, pixelsToTileUnits, projectedAnchor.second, pitchWithMap);
        lastTileDistance = approximateTileDistance(*(firstAndLastGlyph->second.tileDistance), firstAndLastGlyph->second.angle, pixelsToTileUnits, projectedAnchor.second, pitchWithMap);
    }

    bool atLeastOneCirclePlaced = false;
    for (size_t i = 0; i < feature.boxes.size(); i++) {
        CollisionBox& circle = feature.boxes[i];
        const float boxSignedDistanceFromAnchor = circle.signedDistanceFromAnchor;
        if (!firstAndLastGlyph ||
            (boxSignedDistanceFromAnchor < -firstTileDistance) ||
            (boxSignedDistanceFromAnchor > lastTileDistance)) {
            // The label either doesn't fit on its line or we
            // don't need to use this circle because the label
            // doesn't extend this far. Either way, mark the circle unused.
            circle.used = false;
            continue;
        }

        const auto projectedPoint = projectPoint(posMatrix, circle.anchor);
        const float tileUnitRadius = (circle.x2 - circle.x1) / 2;
        const float radius = tileUnitRadius * tileToViewport;

        if (atLeastOneCirclePlaced) {
            const CollisionBox& previousCircle = feature.boxes[i - 1];
            const float dx = projectedPoint.x - previousCircle.px;
            const float dy = projectedPoint.y - previousCircle.py;
            // The circle edges touch when the distance between their centers is 2x the radius
            // When the distance is 1x the radius, they're doubled up, and we could remove
            // every other circle while keeping them all in touch.
            // We actually start removing circles when the distance is √2x the radius:
            //  thinning the number of circles as much as possible is a major performance win,
            //  and the small gaps introduced don't make a very noticeable difference.
            const bool placedTooDensely = radius * radius * 2 > dx * dx + dy * dy;
            if (placedTooDensely) {
                const bool atLeastOneMoreCircle = (i + 1) < feature.boxes.size();
                if (atLeastOneMoreCircle) {
                    const CollisionBox& nextCircle = feature.boxes[i + 1];
                    const float nextBoxDistanceFromAnchor = nextCircle.signedDistanceFromAnchor;
                    if ((nextBoxDistanceFromAnchor > -firstTileDistance) &&
                    (nextBoxDistanceFromAnchor < lastTileDistance)) {
                        // Hide significantly overlapping circles, unless this is the last one we can
                        // use, in which case we want to keep it in place even if it's tightly packed
                        // with the one before it.
                        circle.used = false;
                        continue;
                    }
                }
            }
        }

        atLeastOneCirclePlaced = true;
        circle.px1 = projectedPoint.x - radius;
        circle.px2 = projectedPoint.x + radius;
        circle.py1 = projectedPoint.y - radius;
        circle.py2 = projectedPoint.y + radius;
        
        circle.used = true;
        
        circle.px = projectedPoint.x;
        circle.py = projectedPoint.y;
        circle.radius = radius;
        
        entirelyOffscreen &= isOffscreen(circle);
        inGrid |= isInsideGrid(circle);

        if (!allowOverlap) {
            if (collisionGrid.hitTest({{circle.px, circle.py}, circle.radius})) {
                if (!collisionDebug) {
                    return {false, false};
                } else {
                    // Don't early exit if we're showing the debug circles because we still want to calculate
                    // which circles are in use
                    collisionDetected = true;
                }
            }
        }
    }

    return {!collisionDetected && firstAndLastGlyph && inGrid, entirelyOffscreen};
}


void CollisionIndex::insertFeature(CollisionFeature& feature, bool ignorePlacement) {
    if (feature.alongLine) {
        for (auto& circle : feature.boxes) {
            if (!circle.used) {
                continue;
            }

            if (ignorePlacement) {
                ignoredGrid.insert(IndexedSubfeature(feature.indexedFeature), {{ circle.px, circle.py }, circle.radius});
            } else {
                collisionGrid.insert(IndexedSubfeature(feature.indexedFeature), {{ circle.px, circle.py }, circle.radius});
            }
        }
    } else {
        assert(feature.boxes.size() == 1);
        auto& box = feature.boxes[0];
        if (ignorePlacement) {
            ignoredGrid.insert(IndexedSubfeature(feature.indexedFeature), {{ box.px1, box.py1 }, { box.px2, box.py2 }});
        } else {
            collisionGrid.insert(IndexedSubfeature(feature.indexedFeature), {{ box.px1, box.py1 }, { box.px2, box.py2 }});
        }
    }
}

bool polygonIntersectsBox(const LineString<float>& polygon, const GridIndex<IndexedSubfeature>::BBox& bbox) {
    // This is just a wrapper that allows us to use the integer-based util::polygonIntersectsPolygon
    // Conversion limits our query accuracy to single-pixel resolution
    GeometryCoordinates integerPolygon;
    for (const auto& point : polygon) {
        integerPolygon.push_back(convertPoint<int16_t>(point));
    }
    int16_t minX1 = bbox.min.x;
    int16_t maxY1 = bbox.max.y;
    int16_t minY1 = bbox.min.y;
    int16_t maxX1 = bbox.max.x;

    auto bboxPoints = GeometryCoordinates {
        { minX1, minY1 }, { maxX1, minY1 }, { maxX1, maxY1 }, { minX1, maxY1 }
    };
    
    return util::polygonIntersectsPolygon(integerPolygon, bboxPoints);
}

std::vector<IndexedSubfeature> CollisionIndex::queryRenderedSymbols(const GeometryCoordinates& queryGeometry, const UnwrappedTileID& tileID, const std::string& sourceID) const {
    std::vector<IndexedSubfeature> result;
    if (queryGeometry.empty() || (collisionGrid.empty() && ignoredGrid.empty())) {
        return result;
    }
    
    mat4 posMatrix;
    mat4 projMatrix;
    transformState.getProjMatrix(projMatrix);
    transformState.matrixFor(posMatrix, tileID);
    matrix::multiply(posMatrix, projMatrix, posMatrix);

    // queryGeometry is specified in integer tile units, but in projecting we switch to float pixels
    LineString<float> projectedQuery;
    for (const auto& point : queryGeometry) {
        auto projected = projectPoint(posMatrix, convertPoint<float>(point));
        projectedQuery.push_back(projected);
    }
    
    auto envelope = mapbox::geometry::envelope(projectedQuery);
    
    using QueryResult = std::pair<IndexedSubfeature, GridIndex<IndexedSubfeature>::BBox>;
    
    std::vector<QueryResult> thisTileFeatures;
    std::vector<QueryResult> features = collisionGrid.queryWithBoxes(envelope);

    for (auto& queryResult : features) {
        auto& feature = queryResult.first;
        if (feature.sourceID == sourceID && feature.tileID == tileID.canonical) {
            // We only have to filter on the canonical ID because even if the feature is showing multiple times
            // we treat it as one feature.
            thisTileFeatures.push_back(queryResult);
        }
    }
    
    std::vector<QueryResult> ignoredFeatures = ignoredGrid.queryWithBoxes(envelope);
    for (auto& queryResult : ignoredFeatures) {
        auto& feature = queryResult.first;
        if (feature.sourceID == sourceID && feature.tileID == tileID.canonical) {
            thisTileFeatures.push_back(queryResult);
        }
    }

    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_set<std::size_t>>> sourceLayerFeatures;
    for (auto& queryResult : thisTileFeatures) {
        auto& feature = queryResult.first;
        auto& bbox = queryResult.second;

        // Skip already seen features.
        auto& seenFeatures = sourceLayerFeatures[feature.sourceLayerName][feature.bucketName];
        if (seenFeatures.find(feature.index) != seenFeatures.end())
            continue;

        seenFeatures.insert(feature.index);
        
        if (!polygonIntersectsBox(projectedQuery, bbox)) {
            continue;
        }

        result.push_back(feature);
    }

    return result;

}

std::pair<float,float> CollisionIndex::projectAnchor(const mat4& posMatrix, const Point<float>& point) const {
    vec4 p = {{ point.x, point.y, 0, 1 }};
    matrix::transformMat4(p, p, posMatrix);
    return std::make_pair(
        0.5 + 0.5 * (transformState.getCameraToCenterDistance() / p[3]),
        p[3]
    );
}

std::pair<Point<float>,float> CollisionIndex::projectAndGetPerspectiveRatio(const mat4& posMatrix, const Point<float>& point) const {
    vec4 p = {{ point.x, point.y, 0, 1 }};
    matrix::transformMat4(p, p, posMatrix);
    return std::make_pair(
        Point<float>(
            (((p[0]  / p[3] + 1) / 2) * transformState.getSize().width) + viewportPadding,
            (((-p[1] / p[3] + 1) / 2) * transformState.getSize().height) + viewportPadding
        ),
        // See perspective ratio comment in symbol_sdf.vertex
        // We're doing collision detection in viewport space so we need
        // to scale down boxes in the distance
        0.5 + 0.5 * (transformState.getCameraToCenterDistance() / p[3])
    );
}

Point<float> CollisionIndex::projectPoint(const mat4& posMatrix, const Point<float>& point) const {
    vec4 p = {{ point.x, point.y, 0, 1 }};
    matrix::transformMat4(p, p, posMatrix);
    return Point<float>(
        (((p[0]  / p[3] + 1) / 2) * transformState.getSize().width) + viewportPadding,
        (((-p[1] / p[3] + 1) / 2) * transformState.getSize().height) + viewportPadding
    );
}

} // namespace mbgl
