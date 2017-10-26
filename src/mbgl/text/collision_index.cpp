#include <mbgl/text/collision_index.hpp>
#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/util/intersection_tests.hpp>
#include <mbgl/layout/symbol_projection.hpp>

#include <mbgl/renderer/buckets/symbol_bucket.hpp> // For PlacedSymbol: pull out to another location

#include <mapbox/geometry/envelope.hpp>
#include <mapbox/geometry/multi_point.hpp>

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
    , collisionGrid(transformState.getSize().width, transformState.getSize().height, 30)
    , ignoredGrid(transformState.getSize().width, transformState.getSize().height, 30) {
    pitchFactor = std::cos(transformState.getPitch()) * transformState.getCameraToCenterDistance();
}

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


bool CollisionIndex::placeFeature(CollisionFeature& feature,
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
        GridUnit px1 = box.px1 = box.x1 / tileToViewport + projectedPoint.first.x;
        GridUnit py1 = box.py1 = box.y1 / tileToViewport + projectedPoint.first.y;
        GridUnit px2 = box.px2 = box.x2 / tileToViewport + projectedPoint.first.x;
        GridUnit py2 = box.py2 = box.y2 / tileToViewport + projectedPoint.first.y;
        
        if (!allowOverlap) {
            if (collisionGrid.hitTest({{ px1, py1 }, { px2, py2 }})) {
                return false;
            }
        }

        return true;
    } else {
        return placeLineFeature(feature, posMatrix, labelPlaneMatrix, textPixelRatio, symbol, scale, fontSize, allowOverlap, pitchWithMap, collisionDebug);
    }
}

bool CollisionIndex::placeLineFeature(CollisionFeature& feature,
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

    const auto tileToViewport = projectedAnchor.first * textPixelRatio;
    // equivalent to pixel_to_tile_units
    const auto pixelsToTileUnits = tileToViewport / scale;

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
        const float radius = tileUnitRadius / tileToViewport;

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
        
        GridUnit px = circle.px = projectedPoint.x;
        GridUnit py = circle.py = projectedPoint.y;
        GridUnit r = circle.radius = radius;

        if (!allowOverlap) {
            if (collisionGrid.hitTest({{px, py}, r})) {
                if (!collisionDebug) {
                    return false;
                } else {
                    // Don't early exit if we're showing the debug circles because we still want to calculate
                    // which circles are in use
                    collisionDetected = true;
                }
            }
        }
    }

    return !collisionDetected;
}


void CollisionIndex::insertFeature(CollisionFeature& feature, bool ignorePlacement) {
    if (feature.alongLine) {
        for (auto& box : feature.boxes) {
            GridUnit px = box.px;
            GridUnit py = box.py;
            GridUnit radius = box.radius;
            if (ignorePlacement) {
                // TODO: revisit whether GridIndex should move vs. copy on insert
                ignoredGrid.insert(IndexedSubfeature(feature.indexedFeature), {{ px, py }, radius});
            } else {
                collisionGrid.insert(IndexedSubfeature(feature.indexedFeature), {{ px, py }, radius});
            }
        }
    } else {
        for (auto& box : feature.boxes) {
            // TODO: Iteration is vestigial, there should be only one box
            GridUnit px1 = box.px1;
            GridUnit py1 = box.py1;
            GridUnit px2 = box.px2;
            GridUnit py2 = box.py2;
            if (ignorePlacement) {
                // TODO: revisit whether GridIndex should move vs. copy on insert
                ignoredGrid.insert(IndexedSubfeature(feature.indexedFeature), {{ px1, py1 }, { px2, py2 }});
            } else {
                collisionGrid.insert(IndexedSubfeature(feature.indexedFeature), {{ px1, py1 }, { px2, py2 }});
            }
        }
    }
}

std::vector<IndexedSubfeature> CollisionIndex::queryRenderedSymbols(const GeometryCoordinates& queryGeometry, const UnwrappedTileID&, const float) const {
    std::vector<IndexedSubfeature> result;
    if (true || queryGeometry.empty() || (collisionGrid.empty() && ignoredGrid.empty())) {
        return result;
    }
    
    // TODO: reimplement queryRenderedSymbols
/*
    mat4 posMatrix;
    transformState.matrixFor(posMatrix, tileID);

    GeometryCoordinates polygon;
    auto minX = std::numeric_limits<GridUnit>::max();
    auto minY = std::numeric_limits<GridUnit>::max();
    auto maxX = std::numeric_limits<GridUnit>::min();
    auto maxY = std::numeric_limits<GridUnit>::min();
    
    for (const auto& point : queryGeometry) {
        auto projected = projectPoint(posMatrix, convertPoint<float>(point));
        GridUnit px = projected.x;
        GridUnit py = projected.y;
        minX = std::min(minX, px);
        minY = std::min(minY, py);
        maxX = std::max(maxX, px);
        maxY = std::max(maxY, py);
        polygon.push_back(convertPoint<int16_t>(projected));
    }
    
    std::vector<IndexedSubfeature> thisTileFeatures;
    std::vector<IndexedSubfeature> features = collisionGrid.query({{minX, minY}, {maxX, maxY}});
    // TODO: reimplement "thisTileFeatures" filter
    // onst tileID = tileCoord.id;
    for (auto& feature : features) {
        //if (features[i].tileID === tileID) {
            thisTileFeatures.push_back(feature);
        //}
    }
    
    std::vector<IndexedSubfeature> ignoredFeatures = ignoredGrid.query({{minX, minY}, {maxX, maxY}});
    // TODO: reimplement "thisTileFeatures" filter
    // onst tileID = tileCoord.id;
    for (auto& feature : ignoredFeatures) {
        //if (features[i].tileID === tileID) {
            thisTileFeatures.push_back(feature);
        //}
    }

    for (auto& feature : thisTileFeatures) {
        for (let i = 0; i < thisTileFeatures.length; i++) {
            const blocking = collisionBoxArray.get(thisTileFeatures[i]);
            const sourceLayer = blocking.sourceLayerIndex;
            const featureIndex = blocking.featureIndex;

            // Skip already seen features.
            if (sourceLayerFeatures[sourceLayer] === undefined) {
                sourceLayerFeatures[sourceLayer] = {};
            }
            if (sourceLayerFeatures[sourceLayer][featureIndex]) continue;


            // Check if query intersects with the feature box
            // "Collision Circles" for line labels are treated as boxes here
            // Since there's no actual collision taking place, the circle vs. square
            // distinction doesn't matter as much, and box geometry is easier
            // to work with.
            const projectedPoint = this.projectAndGetPerspectiveRatio(posMatrix, blocking.anchorPointX, blocking.anchorPointY);
            const tileToViewport = textPixelRatio * projectedPoint.perspectiveRatio;
            const x1 = blocking.x1 / tileToViewport + projectedPoint.point.x;
            const y1 = blocking.y1 / tileToViewport + projectedPoint.point.y;
            const x2 = blocking.x2 / tileToViewport + projectedPoint.point.x;
            const y2 = blocking.y2 / tileToViewport + projectedPoint.point.y;
            const bbox = [
                new Point(x1, y1),
                new Point(x2, y1),
                new Point(x2, y2),
                new Point(x1, y2)
            ];
            if (!intersectionTests.polygonIntersectsPolygon(query, bbox)) continue;

            sourceLayerFeatures[sourceLayer][featureIndex] = true;
            result.push(thisTileFeatures[i]);
        }

        return result;

    // Predicate for ruling out already seen features.
    std::unordered_map<std::string, std::unordered_set<std::size_t>> sourceLayerFeatures;
    auto seenFeature = [&] (const CollisionTreeBox& treeBox) -> bool {
        const IndexedSubfeature& feature = std::get<2>(treeBox);
        const auto& seenFeatures = sourceLayerFeatures[feature.sourceLayerName];
        return seenFeatures.find(feature.index) == seenFeatures.end();
    };


    // Check if query polygon intersects with the feature box at current scale.
    auto intersectsAtScale = [&] (const CollisionTreeBox& treeBox) -> bool {
        const CollisionBox& collisionBox = std::get<1>(treeBox);
        const auto projectedAnchor = projectAndGetPerspectiveRatio(posMatrix, collisionBox.anchor);
        const float tileToViewport = textPixelRatio * projectedAnchor.second;

        const int16_t x1 = projectedAnchor.first.x + (collisionBox.x1 / tileToViewport);
        const int16_t y1 = projectedAnchor.first.y + (collisionBox.y1 / tileToViewport);
        const int16_t x2 = projectedAnchor.first.x + (collisionBox.x2 / tileToViewport);
        const int16_t y2 = projectedAnchor.first.y + (collisionBox.y2 / tileToViewport);
        auto bbox = GeometryCoordinates {
            { x1, y1 }, { x2, y1 }, { x2, y2 }, { x1, y2 }
        };
        return util::polygonIntersectsPolygon(polygon, bbox);
    };

    auto predicates = bgi::satisfies(seenFeature) && bgi::satisfies(intersectsAtScale);

    return result;
    */
}

std::pair<float,float> CollisionIndex::projectAnchor(const mat4& posMatrix, const Point<float>& point) const {
    vec4 p = {{ point.x, point.y, 0, 1 }};
    matrix::transformMat4(p, p, posMatrix);
    return std::make_pair(
        0.5 + 0.5 * (p[3] / transformState.getCameraToCenterDistance()),
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
        0.5 + 0.5 * (p[3] / transformState.getCameraToCenterDistance())
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
