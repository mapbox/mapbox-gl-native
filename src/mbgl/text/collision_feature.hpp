#pragma once

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/geometry/feature_index.hpp>

#include <vector>

namespace mbgl {

class CollisionBox {
public:
    CollisionBox(Point<float> _anchor, float _x1, float _y1, float _x2, float _y2, float _maxScale) :
        anchor(std::move(_anchor)), x1(_x1), y1(_y1), x2(_x2), y2(_y2), maxScale(_maxScale) {}

    // the box is centered around the anchor point
    Point<float> anchor;

    // distances to the edges from the anchor
    float x1;
    float y1;
    float x2;
    float y2;

    // the box is only valid for scales < maxScale.
    // The box does not block other boxes at scales >= maxScale;
    float maxScale;

    // the scale at which the label can first be shown
    float placementScale = 0.0f;
};

class CollisionFeature {
public:
    // for text
    CollisionFeature(const GeometryCoordinates& line,
                     const Anchor& anchor,
                     const Shaping& shapedText,
                     const float boxScale,
                     const float padding,
                     const style::SymbolPlacementType placement,
                     const IndexedSubfeature& indexedFeature_)
        : CollisionFeature(line, anchor, shapedText.top, shapedText.bottom, shapedText.left, shapedText.right, boxScale, padding, placement, indexedFeature_, false) {}

    // for icons
    CollisionFeature(const GeometryCoordinates& line,
                     const Anchor& anchor,
                     const PositionedIcon& shapedIcon,
                     const float boxScale,
                     const float padding,
                     const style::SymbolPlacementType placement,
                     const IndexedSubfeature& indexedFeature_)
        : CollisionFeature(line, anchor, shapedIcon.top, shapedIcon.bottom, shapedIcon.left, shapedIcon.right, boxScale, padding, placement, indexedFeature_, true) {}

    CollisionFeature(const GeometryCoordinates& line,
                     const Anchor& anchor,
                     const float top,
                     const float bottom,
                     const float left,
                     const float right,
                     const float boxScale,
                     const float padding,
                     const style::SymbolPlacementType placement,
                     IndexedSubfeature,
                     const bool straight);

    std::vector<CollisionBox> boxes;
    IndexedSubfeature indexedFeature;

private:
    void bboxifyLabel(const GeometryCoordinates& line, GeometryCoordinate& anchorPoint,
                      const int segment, const float length, const float height);
};

} // namespace mbgl
