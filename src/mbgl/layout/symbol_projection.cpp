#include <mbgl/layout/symbol_projection.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

    mat4 getLabelPlaneMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits) {
        mat4 m;
        matrix::identity(m);
        if (pitchWithMap) {
            matrix::scale(m, m, 1 / pixelsToTileUnits, 1 / pixelsToTileUnits, 1);
            if (!rotateWithMap) {
                matrix::rotate_z(m, m, state.getAngle());
            }
        } else {
            matrix::scale(m, m, state.getSize().width / 2.0, -state.getSize().height / 2.0, 1.0);
            matrix::translate(m, m, 1, -1, 0);
            matrix::multiply(m, m, posMatrix);
        }
        return m;
    }

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


    Point<float> project(const Point<float>& point, const mat4& matrix) {
        vec4 pos = {{ point.x, point.y, 0, 1 }};
        matrix::transformMat4(pos, pos, matrix);
        return { static_cast<float>(pos[0] / pos[3]), static_cast<float>(pos[1] / pos[3]) };
    }

    bool isVisible(const vec4& anchorPos, const float, const std::array<double, 2>& clippingBuffer) {
        const float x = anchorPos[0] / anchorPos[3];
        const float y = anchorPos[1] / anchorPos[3];
        const bool inPaddedViewport = (
                x >= -clippingBuffer[0] &&
                x <= clippingBuffer[0] &&
                y >= -clippingBuffer[1] &&
                y <= clippingBuffer[1]);
        // TODO do framehistory check here as well
        return inPaddedViewport;
    }

    void addDynamicAttributes(const Point<float>&, const float, const float) {
    }

    void hideGlyphs(size_t) {
    }

    struct PlacedGlyph {
        PlacedGlyph(Point<float> point_, float angle_) : point(point_), angle(angle_) {}
        Point<float> point;
        float angle;
    };

	optional<PlacedGlyph> placeGlyphAlongLine(const float offsetX, const float lineOffsetX, const float lineOffsetY, const bool flip,
            Point<float> anchorPoint, const uint16_t anchorSegment, const std::vector<Point<float>>& line, const mat4& labelPlaneMatrix) {

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

        Point<float>& current = anchorPoint;
        Point<float>& prev = anchorPoint;
        float distanceToPrev = 0.0;
        float currentSegmentDistance = 0.0;
        const float absOffsetX = std::abs(combinedOffsetX);

        while (distanceToPrev + currentSegmentDistance <= absOffsetX) {
            currentIndex += dir;

            // offset does not fit on the projected line
            if (currentIndex < 0 || currentIndex >= static_cast<int32_t>(line.size())) return {};

            prev = current;
            current = project(line.at(currentIndex), labelPlaneMatrix);

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

    void placeGlyphsAlongLine(const PlacedSymbol& symbol, const float fontSize, const bool flip, const mat4& labelPlaneMatrix) {
        const float fontScale = fontSize / 24.0;
        const float lineOffsetX = symbol.lineOffsetX * fontSize;
        const float lineOffsetY = symbol.lineOffsetY * fontSize;

        const Point<float> anchorPoint = project(symbol.anchorPoint, labelPlaneMatrix);

        std::vector<PlacedGlyph> placedGlyphs;
        for (auto glyphOffsetX : symbol.glyphOffsets) {
            auto placedGlyph = placeGlyphAlongLine(glyphOffsetX * fontScale, lineOffsetX, lineOffsetY, flip, anchorPoint, symbol.segment, symbol.line, labelPlaneMatrix);
            if (placedGlyph) {
                placedGlyphs.push_back(*placedGlyph);
            } else {
                hideGlyphs(symbol.glyphOffsets.size());
                return;
            }
        }

        for (auto& placedGlyph : placedGlyphs) {
            addDynamicAttributes(placedGlyph.point, placedGlyph.angle, symbol.placementZoom);
        }
    }

    void reprojectLineLabels(SymbolBucket& bucket, const mat4& posMatrix, const bool isText, const style::SymbolPropertyValues& values,
            //const RenderTile& tile, const TransformState& state) {
            const RenderTile&, const TransformState& state) {
        // const sizeData = isText ? bucket.textSizeData : bucket.iconSizeData;
        // const partiallyEvaluatedSize = symbolSize.evaluateSizeForZoom(sizeData, painter.transform, layer, isText);

        const std::array<double, 2> clippingBuffer = {{ 256.0 / state.getSize().width * 2.0 + 1.0, 256.0 / state.getSize().height }};

        //TODO
        const float pixelsToTileUnits = 1.0;
        const mat4 labelPlaneMatrix = getLabelPlaneMatrix(posMatrix, values.pitchAlignment == style::AlignmentType::Map,
                values.rotationAlignment == style::AlignmentType::Map, state, pixelsToTileUnits);
        
        // 
        //     const dynamicLayoutVertexArray = isText ?
        //             bucket.buffers.glyph.dynamicLayoutVertexArray :
        //                     bucket.buffers.icon.dynamicLayoutVertexArray;
        //                         dynamicLayoutVertexArray.clear();

        const std::vector<PlacedSymbol>& placedSymbols = isText ? bucket.text.placedSymbols : bucket.icon.placedSymbols;
        for (auto& placedSymbol : placedSymbols) {
			vec4 anchorPos = {{ placedSymbol.anchorPoint.x, placedSymbol.anchorPoint.y, 0, 1 }};
            matrix::transformMat4(anchorPos, anchorPos, posMatrix);

            // Don't bother calculating the correct point for invisible labels.
            if (!isVisible(anchorPos, placedSymbol.placementZoom, clippingBuffer)) {
                hideGlyphs(placedSymbol.glyphOffsets.size());
                continue;
            }

            bool flip = false;
            if (values.keepUpright) {
                if (false) {
                    const Point<float> a = project(placedSymbol.line.at(placedSymbol.segment), posMatrix);
                    const Point<float> b = project(placedSymbol.line.at(placedSymbol.segment + 1), posMatrix);
                    flip = placedSymbol.vertical ? b.y > a.y : b.x < a.x;
                } 
            }

            const float cameraToAnchorDistance = anchorPos[3];
            const float perspectiveRatio = 1 + 0.5 * ((cameraToAnchorDistance / state.getCameraToCenterDistance()) - 1.0);


            //const fontSize = symbolSize.evaluateSizeForFeature(sizeData, partiallyEvaluatedSize, symbol);
            const float fontSize = 16;
            const float pitchScaledFontSize = values.pitchAlignment == style::AlignmentType::Map ?
                fontSize * perspectiveRatio :
                fontSize / perspectiveRatio;

            //placeGlyphsAlongLine(symbol, pitchScaledFontSize, flip, labelPlaneMatrix, bucket.glyphOffsetArray, lineVertexArray, dynamicLayoutVertexArray);
            placeGlyphsAlongLine(placedSymbol, pitchScaledFontSize, flip, labelPlaneMatrix);
        }
    }
} // end namespace mbgl
