#include <mbgl/layout/symbol_projection.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/frame_history.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

	/*
	 * # Overview of coordinate spaces
	 *
	 * ## Tile coordinate spaces
	 * Each label has an anchor. Some labels have corresponding line geometries.
	 * The points for both anchors and lines are stored in tile units. Each tile has it's own
	 * coordinate space going from (0, 0) at the top left to (EXTENT, EXTENT) at the bottom right.
	 *
	 * ## GL coordinate space
	 * At the end of everything, the vertex shader needs to produce a position in GL coordinate space,
	 * which is (-1, 1) at the top left and (1, -1) in the bottom right.
	 *
	 * ## Map pixel coordinate spaces
	 * Each tile has a pixel coordinate space. It's just the tile units scaled so that one unit is
	 * whatever counts as 1 pixel at the current zoom.
	 * This space is used for pitch-alignment=map, rotation-alignment=map
	 *
	 * ## Rotated map pixel coordinate spaces
	 * Like the above, but rotated so axis of the space are aligned with the viewport instead of the tile.
	 * This space is used for pitch-alignment=map, rotation-alignment=viewport
	 *
	 * ## Viewport pixel coordinate space
	 * (0, 0) is at the top left of the canvas and (pixelWidth, pixelHeight) is at the bottom right corner
	 * of the canvas. This space is used for pitch-alignment=viewport
	 *
	 *
	 * # Vertex projection
	 * It goes roughly like this:
	 * 1. project the anchor and line from tile units into the correct label coordinate space
	 *      - map pixel space           pitch-alignment=map         rotation-alignment=map
	 *      - rotated map pixel space   pitch-alignment=map         rotation-alignment=viewport
	 *      - viewport pixel space      pitch-alignment=viewport    rotation-alignment=*
	 * 2. if the label follows a line, find the point along the line that is the correct distance from the anchor.
	 * 3. add the glyph's corner offset to the point from step 3
	 * 4. convert from the label coordinate space to gl coordinates
	 *
	 * For horizontal labels we want to do step 1 in the shader for performance reasons (no cpu work).
	 *      This is what `u_label_plane_matrix` is used for.
	 * For labels aligned with lines we have to steps 1 and 2 on the cpu since we need access to the line geometry.
	 *      This is what `updateLineLabels(...)` does.
	 *      Since the conversion is handled on the cpu we just set `u_label_plane_matrix` to an identity matrix.
	 *
	 * Steps 3 and 4 are done in the shaders for all labels.
	 */

	/*
	 * Returns a matrix for converting from tile units to the correct label coordinate space.
	 */
    mat4 getLabelPlaneMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits) {
        mat4 m;
        matrix::identity(m);
        if (pitchWithMap) {
            matrix::scale(m, m, 1 / pixelsToTileUnits, 1 / pixelsToTileUnits, 1);
            if (!rotateWithMap) {
                matrix::rotate_z(m, m, state.getAngle());
            }
        } else {
            matrix::scale(m, m, state.getSize().width / 2.0, -(state.getSize().height / 2.0), 1.0);
            matrix::translate(m, m, 1, -1, 0);
            matrix::multiply(m, m, posMatrix);
        }
        return m;
    }

	/*
	 * Returns a matrix for converting from the correct label coordinate space to gl coords.
	 */
    mat4 getGlCoordMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits) {
        mat4 m;
        matrix::identity(m);
        if (pitchWithMap) {
            matrix::multiply(m, m, posMatrix);
            matrix::scale(m, m, pixelsToTileUnits, pixelsToTileUnits, 1);
            if (!rotateWithMap) {
                matrix::rotate_z(m, m, -state.getAngle());
            }
        } else {
            matrix::scale(m, m, 1, -1, 1);
            matrix::translate(m, m, -1, -1, 0);
            matrix::scale(m, m, 2.0 / state.getSize().width, 2.0 / state.getSize().height, 1.0);
        }
        return m;
    }


    typedef std::pair<Point<float>,float> PointAndCameraDistance;

    PointAndCameraDistance project(const Point<float>& point, const mat4& matrix) {
        vec4 pos = {{ point.x, point.y, 0, 1 }};
        matrix::transformMat4(pos, pos, matrix);
        return {{ static_cast<float>(pos[0] / pos[3]), static_cast<float>(pos[1] / pos[3]) }, pos[3] };
    }

    float evaluateSizeForFeature(const ZoomEvaluatedSize& zoomEvaluatedSize, const PlacedSymbol& placedSymbol) {
        if (zoomEvaluatedSize.isFeatureConstant) {
            return zoomEvaluatedSize.size;
        } else {
            if (zoomEvaluatedSize.isZoomConstant) {
                return placedSymbol.lowerSize;
            } else {
                return placedSymbol.lowerSize + zoomEvaluatedSize.sizeT * (placedSymbol.upperSize - placedSymbol.lowerSize);
            }
        }
    }

    bool isVisible(const vec4& anchorPos, const float placementZoom, const std::array<double, 2>& clippingBuffer, const FrameHistory& frameHistory) {
        const float x = anchorPos[0] / anchorPos[3];
        const float y = anchorPos[1] / anchorPos[3];
        const bool inPaddedViewport = (
                x >= -clippingBuffer[0] &&
                x <= clippingBuffer[0] &&
                y >= -clippingBuffer[1] &&
                y <= clippingBuffer[1]);
        return inPaddedViewport && frameHistory.isVisible(placementZoom);
    }

    void addDynamicAttributes(const Point<float>& anchorPoint, const float angle, const float placementZoom,
            gl::VertexVector<SymbolDynamicLayoutAttributes::Vertex>& dynamicVertexArray) {
        auto dynamicVertex = SymbolDynamicLayoutAttributes::vertex(anchorPoint, angle, placementZoom);
        dynamicVertexArray.emplace_back(dynamicVertex);
        dynamicVertexArray.emplace_back(dynamicVertex);
        dynamicVertexArray.emplace_back(dynamicVertex);
        dynamicVertexArray.emplace_back(dynamicVertex);
    }

    void hideGlyphs(size_t numGlyphs, gl::VertexVector<SymbolDynamicLayoutAttributes::Vertex>& dynamicVertexArray) {
        const Point<float> offscreenPoint = { -INFINITY, -INFINITY };
        for (size_t i = 0; i < numGlyphs; i++) {
            addDynamicAttributes(offscreenPoint, 0, 25, dynamicVertexArray);
        }
    }

    struct PlacedGlyph {
        PlacedGlyph(Point<float> point_, float angle_) : point(point_), angle(angle_) {}
        Point<float> point;
        float angle;
    };
    
    enum PlacementResult {
        OK,
        NotEnoughRoom,
        NeedsFlipping
    };
    
    Point<float> projectTruncatedLineSegment(const Point<float>& previousTilePoint, const Point<float>& currentTilePoint, const Point<float>& previousProjectedPoint, const float minimumLength, const mat4& projectionMatrix) {
        // We are assuming "previousTilePoint" won't project to a point within one unit of the camera plane
        // If it did, that would mean our label extended all the way out from within the viewport to a (very distant)
        // point near the plane of the camera. We wouldn't be able to render the label anyway once it crossed the
        // plane of the camera.
        const Point<float> projectedUnitVertex = project(previousTilePoint + util::unit<float>(previousTilePoint - currentTilePoint), projectionMatrix).first;
        const Point<float> projectedUnitSegment = previousProjectedPoint - projectedUnitVertex;

        return previousProjectedPoint + (projectedUnitSegment * (minimumLength / util::mag<float>(projectedUnitSegment)));
    }

	optional<PlacedGlyph> placeGlyphAlongLine(const float offsetX, const float lineOffsetX, const float lineOffsetY, const bool flip,
            const Point<float>& projectedAnchorPoint, const Point<float>& tileAnchorPoint, const uint16_t anchorSegment, const GeometryCoordinates& line, const mat4& labelPlaneMatrix) {

        const float combinedOffsetX = flip ?
            offsetX - lineOffsetX :
            offsetX + lineOffsetX;

        int16_t dir = combinedOffsetX > 0 ? 1 : -1;

        float angle = 0.0;
        if (flip) {
            // The label needs to be flipped to keep text upright.
            // Iterate in the reverse direction.
            dir *= -1;
            angle = M_PI;
        }

        if (dir < 0) angle += M_PI;

        int32_t currentIndex = dir > 0 ? anchorSegment : anchorSegment + 1;

        Point<float> current = projectedAnchorPoint;
        Point<float> prev = projectedAnchorPoint;
        float distanceToPrev = 0.0;
        float currentSegmentDistance = 0.0;
        const float absOffsetX = std::abs(combinedOffsetX);

        while (distanceToPrev + currentSegmentDistance <= absOffsetX) {
            currentIndex += dir;

            // offset does not fit on the projected line
            if (currentIndex < 0 || currentIndex >= static_cast<int32_t>(line.size())) return {};

            prev = current;
            PointAndCameraDistance projection = project(convertPoint<float>(line.at(currentIndex)), labelPlaneMatrix);
            if (projection.second > 0) {
                current = projection.first;
            } else {
                // The vertex is behind the plane of the camera, so we can't project it
                // Instead, we'll create a vertex along the line that's far enough to include the glyph
                const Point<float> previousTilePoint = distanceToPrev == 0 ?
                    tileAnchorPoint :
                    convertPoint<float>(line.at(currentIndex - dir));
                const Point<float> currentTilePoint = convertPoint<float>(line.at(currentIndex));
                current = projectTruncatedLineSegment(previousTilePoint, currentTilePoint, prev, absOffsetX - distanceToPrev + 1, labelPlaneMatrix);
            }

            distanceToPrev += currentSegmentDistance;
            currentSegmentDistance = util::dist<float>(prev, current);
        }

        // The point is on the current segment. Interpolate to find it.
        const float segmentInterpolationT = (absOffsetX - distanceToPrev) / currentSegmentDistance;
        const Point<float> prevToCurrent = current - prev;
        Point<float> p = (prevToCurrent * segmentInterpolationT) + prev;

        // offset the point from the line to text-offset and icon-offset
        p += util::perp(prevToCurrent) * static_cast<float>(lineOffsetY * dir / util::mag(prevToCurrent));

        const float segmentAngle = angle + std::atan2(current.y - prev.y, current.x - prev.x);

        return {{ p, segmentAngle }};
    }

    PlacementResult placeGlyphsAlongLine(const PlacedSymbol& symbol,
                              const float fontSize,
                              const bool flip,
                              const bool keepUpright,
                              const mat4& posMatrix,
                              const mat4& labelPlaneMatrix,
                              const mat4& glCoordMatrix,
                              gl::VertexVector<SymbolDynamicLayoutAttributes::Vertex>& dynamicVertexArray,
                              const Point<float>& projectedAnchorPoint) {
        const float fontScale = fontSize / 24.0;
        const float lineOffsetX = symbol.lineOffset[0] * fontSize;
        const float lineOffsetY = symbol.lineOffset[1] * fontSize;

        std::vector<PlacedGlyph> placedGlyphs;
        if (symbol.glyphOffsets.size() > 1) {

            const float firstGlyphOffset = symbol.glyphOffsets.front();
            const float lastGlyphOffset = symbol.glyphOffsets.back();
            
            optional<PlacedGlyph> firstPlacedGlyph = placeGlyphAlongLine(fontScale * firstGlyphOffset, lineOffsetX, lineOffsetY, flip, projectedAnchorPoint, symbol.anchorPoint, symbol.segment, symbol.line, labelPlaneMatrix);
            if (!firstPlacedGlyph)
                return PlacementResult::NotEnoughRoom;

            optional<PlacedGlyph> lastPlacedGlyph = placeGlyphAlongLine(fontScale * lastGlyphOffset, lineOffsetX, lineOffsetY, flip, projectedAnchorPoint, symbol.anchorPoint, symbol.segment, symbol.line, labelPlaneMatrix);
            if (!lastPlacedGlyph)
                return PlacementResult::NotEnoughRoom;

            const Point<float> firstPoint = project(firstPlacedGlyph->point, glCoordMatrix).first;
            const Point<float> lastPoint = project(lastPlacedGlyph->point, glCoordMatrix).first;

            if (keepUpright && !flip &&
                (symbol.useVerticalMode ? firstPoint.y < lastPoint.y : firstPoint.x > lastPoint.x)) {
                return PlacementResult::NeedsFlipping;
            }

            placedGlyphs.push_back(*firstPlacedGlyph);
            for (size_t glyphIndex = 1; glyphIndex < symbol.glyphOffsets.size() - 1; glyphIndex++) {
                const float glyphOffsetX = symbol.glyphOffsets[glyphIndex];
                // Since first and last glyph fit on the line, we're sure that the rest of the glyphs can be placed
                auto placedGlyph = placeGlyphAlongLine(glyphOffsetX * fontScale, lineOffsetX, lineOffsetY, flip, projectedAnchorPoint, symbol.anchorPoint, symbol.segment, symbol.line, labelPlaneMatrix);
                placedGlyphs.push_back(*placedGlyph);
            }
            placedGlyphs.push_back(*lastPlacedGlyph);
        } else {
            // Only a single glyph to place
            // So, determine whether to flip based on projected angle of the line segment it's on
            if (keepUpright && !flip) {
                const Point<float> a = project(symbol.anchorPoint, posMatrix).first;
                const Point<float> tileSegmentEnd = convertPoint<float>(symbol.line.at(symbol.segment + 1));
                const PointAndCameraDistance projectedVertex = project(tileSegmentEnd, posMatrix);
                // We know the anchor will be in the viewport, but the end of the line segment may be
                // behind the plane of the camera, in which case we can use a point at any arbitrary (closer)
                // point on the segment.
                const Point<float> b = (projectedVertex.second > 0) ?
                    projectedVertex.first :
                    projectTruncatedLineSegment(symbol.anchorPoint,tileSegmentEnd, a, 1, posMatrix);
                
                if (symbol.useVerticalMode ? b.y > a.y : b.x < a.x) {
                    return PlacementResult::NeedsFlipping;
                }
            }
            assert(symbol.glyphOffsets.size() == 1); // We are relying on SymbolInstance.hasText filtering out symbols without any glyphs at all
            const float glyphOffsetX = symbol.glyphOffsets.front();
            optional<PlacedGlyph> singleGlyph = placeGlyphAlongLine(fontScale * glyphOffsetX, lineOffsetX, lineOffsetY, flip, projectedAnchorPoint, symbol.anchorPoint, symbol.segment,
                symbol.line, labelPlaneMatrix);
            if (!singleGlyph)
                return PlacementResult::NotEnoughRoom;

            placedGlyphs.push_back(*singleGlyph);
        }

        for (auto& placedGlyph : placedGlyphs) {
            addDynamicAttributes(placedGlyph.point, placedGlyph.angle, symbol.placementZoom, dynamicVertexArray);
        }
        
        return PlacementResult::OK;
    }


    void reprojectLineLabels(gl::VertexVector<SymbolDynamicLayoutAttributes::Vertex>& dynamicVertexArray, const std::vector<PlacedSymbol>& placedSymbols,
			const mat4& posMatrix, const style::SymbolPropertyValues& values,
            const RenderTile& tile, const SymbolSizeBinder& sizeBinder, const TransformState& state, const FrameHistory& frameHistory) {

        const ZoomEvaluatedSize partiallyEvaluatedSize = sizeBinder.evaluateForZoom(state.getZoom());

        const std::array<double, 2> clippingBuffer = {{ 256.0 / state.getSize().width * 2.0 + 1.0, 256.0 / state.getSize().height * 2.0 + 1.0 }};
        
        const bool pitchWithMap = values.pitchAlignment == style::AlignmentType::Map;
        const bool rotateWithMap = values.rotationAlignment == style::AlignmentType::Map;
        const float pixelsToTileUnits = tile.id.pixelsToTileUnits(1, state.getZoom());

        const mat4 labelPlaneMatrix = getLabelPlaneMatrix(posMatrix, pitchWithMap,
                rotateWithMap, state, pixelsToTileUnits);
        
        const mat4 glCoordMatrix = getGlCoordMatrix(posMatrix, pitchWithMap, rotateWithMap, state, pixelsToTileUnits);
        
        dynamicVertexArray.clear();

        for (auto& placedSymbol : placedSymbols) {
			vec4 anchorPos = {{ placedSymbol.anchorPoint.x, placedSymbol.anchorPoint.y, 0, 1 }};
            matrix::transformMat4(anchorPos, anchorPos, posMatrix);

            // Don't bother calculating the correct point for invisible labels.
            if (!isVisible(anchorPos, placedSymbol.placementZoom, clippingBuffer, frameHistory)) {
                hideGlyphs(placedSymbol.glyphOffsets.size(), dynamicVertexArray);
                continue;
            }

            const float cameraToAnchorDistance = anchorPos[3];
            const float perspectiveRatio = 1 + 0.5 * ((cameraToAnchorDistance / state.getCameraToCenterDistance()) - 1.0);

            const float fontSize = evaluateSizeForFeature(partiallyEvaluatedSize, placedSymbol);
            const float pitchScaledFontSize = values.pitchAlignment == style::AlignmentType::Map ?
                fontSize * perspectiveRatio :
                fontSize / perspectiveRatio;
            
            const Point<float> anchorPoint = project(placedSymbol.anchorPoint, labelPlaneMatrix).first;

            PlacementResult placeUnflipped = placeGlyphsAlongLine(placedSymbol, pitchScaledFontSize, false /*unflipped*/, values.keepUpright, posMatrix, labelPlaneMatrix, glCoordMatrix, dynamicVertexArray, anchorPoint);

            if (placeUnflipped == PlacementResult::NotEnoughRoom ||
                (placeUnflipped == PlacementResult::NeedsFlipping &&
                 placeGlyphsAlongLine(placedSymbol, pitchScaledFontSize, true /*flipped*/, values.keepUpright, posMatrix, labelPlaneMatrix, glCoordMatrix, dynamicVertexArray, anchorPoint) == PlacementResult::NotEnoughRoom)) {
                hideGlyphs(placedSymbol.glyphOffsets.size(), dynamicVertexArray);
            }
        }
    }
} // end namespace mbgl
