#pragma once

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/geometry/feature_index.hpp>

#include <vector>

namespace mbgl {

class CollisionBox {
public:
    CollisionBox(Point<float> _anchor, Point<float> _offset, float _x1, float _y1, float _x2, float _y2, float _tileUnitDistanceToAnchor = 0) :
        anchor(std::move(_anchor)), offset(_offset), x1(_x1), y1(_y1), x2(_x2), y2(_y2), used(true), tileUnitDistanceToAnchor(_tileUnitDistanceToAnchor) {}

    // the box is centered around the anchor point
    Point<float> anchor;
    
    // the offset of the box from the label's anchor point
    Point<float> offset;

    // distances to the edges from the anchor
    float x1;
    float y1;
    float x2;
    float y2;

    // TODO Where's the right place to store the projected bounding box?
    float px1;
    float py1;
    float px2;
    float py2;
    // Placeholder for center of circles (can be derived from bounding box)
    float px;
    float py;
    bool used;

    float tileUnitDistanceToAnchor;

    
    // TODO Placeholders for old collision tiles
    float maxScale;
    float placementScale;
    float adjustedMaxScale(const std::array<float, 4>& , const float) const { return 1; }

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
        : CollisionFeature(line, anchor, shapedText.top, shapedText.bottom, shapedText.left, shapedText.right, boxScale, padding, placement, indexedFeature_) {}

    // for icons
    CollisionFeature(const GeometryCoordinates& line,
                     const Anchor& anchor,
                     optional<PositionedIcon> shapedIcon,
                     const float boxScale,
                     const float padding,
                     const style::SymbolPlacementType placement,
                     const IndexedSubfeature& indexedFeature_)
        : CollisionFeature(line, anchor,
                           (shapedIcon ? shapedIcon->top() : 0),
                           (shapedIcon ? shapedIcon->bottom() : 0),
                           (shapedIcon ? shapedIcon->left() : 0),
                           (shapedIcon ? shapedIcon->right() : 0),
                           boxScale, padding, placement, indexedFeature_) {}

    CollisionFeature(const GeometryCoordinates& line,
                     const Anchor&,
                     const float top,
                     const float bottom,
                     const float left,
                     const float right,
                     const float boxScale,
                     const float padding,
                     const style::SymbolPlacementType,
                     IndexedSubfeature);

    std::vector<CollisionBox> boxes;
    IndexedSubfeature indexedFeature;
    bool alongLine;

private:
    void bboxifyLabel(const GeometryCoordinates& line, GeometryCoordinate& anchorPoint,
                      const int segment, const float length, const float height);
};

} // namespace mbgl
