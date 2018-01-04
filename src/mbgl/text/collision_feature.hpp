#pragma once

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/geometry/feature_index.hpp>

#include <vector>

namespace mbgl {

class CollisionBox {
public:
    CollisionBox(Point<float> _anchor, Point<float> _offset, float _x1, float _y1, float _x2, float _y2, float _signedDistanceFromAnchor = 0, float _radius = 0) :
        anchor(std::move(_anchor)), offset(_offset), x1(_x1), y1(_y1), x2(_x2), y2(_y2), used(true), signedDistanceFromAnchor(_signedDistanceFromAnchor), radius(_radius) {}

    // the box is centered around the anchor point
    Point<float> anchor;
    
    // the offset of the box from the label's anchor point
    Point<float> offset;

    // distances to the edges from the anchor
    float x1;
    float y1;
    float x2;
    float y2;

    // Projected box geometry: generated/updated at placement time
    float px1;
    float py1;
    float px2;
    float py2;
    
    // Projected circle geometry: generated/updated at placement time
    float px;
    float py;
    bool used;

    float signedDistanceFromAnchor;
    float radius;
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
                     const IndexedSubfeature& indexedFeature_,
                     const float overscaling)
        : CollisionFeature(line, anchor, shapedText.top, shapedText.bottom, shapedText.left, shapedText.right, boxScale, padding, placement, indexedFeature_, overscaling) {}

    // for icons
    // Icons collision features are always SymbolPlacementType::Point, which means the collision feature
    // will be viewport-rotation-aligned even if the icon is map-rotation-aligned (e.g. `icon-rotation-alignment: map`
    // _or_ `symbol-placement: line`). We're relying on most icons being "close enough" to square that having
    // incorrect rotation alignment doesn't throw off collision detection too much.
    // See: https://github.com/mapbox/mapbox-gl-js/issues/4861
    CollisionFeature(const GeometryCoordinates& line,
                     const Anchor& anchor,
                     optional<PositionedIcon> shapedIcon,
                     const float boxScale,
                     const float padding,
                     const IndexedSubfeature& indexedFeature_)
        : CollisionFeature(line, anchor,
                           (shapedIcon ? shapedIcon->top() : 0),
                           (shapedIcon ? shapedIcon->bottom() : 0),
                           (shapedIcon ? shapedIcon->left() : 0),
                           (shapedIcon ? shapedIcon->right() : 0),
                           boxScale,
                           padding,
                           style::SymbolPlacementType::Point,
                           indexedFeature_, 1) {}

    CollisionFeature(const GeometryCoordinates& line,
                     const Anchor&,
                     const float top,
                     const float bottom,
                     const float left,
                     const float right,
                     const float boxScale,
                     const float padding,
                     const style::SymbolPlacementType,
                     IndexedSubfeature,
                     const float overscaling);

    std::vector<CollisionBox> boxes;
    IndexedSubfeature indexedFeature;
    bool alongLine;

private:
    void bboxifyLabel(const GeometryCoordinates& line, GeometryCoordinate& anchorPoint,
                      const int segment, const float length, const float height, const float overscaling);
};

} // namespace mbgl
