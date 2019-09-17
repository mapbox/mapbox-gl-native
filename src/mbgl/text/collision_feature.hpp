#pragma once

#include <mbgl/geometry/anchor.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/geometry/feature_index.hpp>

#include <vector>

namespace mbgl {

class ProjectedCollisionBox {
public:
    enum class Type : char {
        Unknown,
        Box,
        Circle
    };

    ProjectedCollisionBox() = default;
    ProjectedCollisionBox(float x1, float y1, float x2, float y2) : geometry(x1, y1, x2, y2), type(Type::Box) {}
    ProjectedCollisionBox(float x, float y, float r) : geometry(x, y, r), type(Type::Circle) {}

    const mapbox::geometry::box<float>& box() const {
        assert(isBox());
        return geometry.box;
    }

    const geometry::circle<float>& circle() const {
        assert(isCircle());
        return geometry.circle;
    }

    bool isCircle() const { return type == Type::Circle; }
    bool isBox() const { return type == Type::Box; }

private:
    union Geometry {
        Geometry(float x1, float y1, float x2, float y2) : box({x1, y1}, {x2, y2}) {}
        Geometry(float x, float y, float r) : circle({x, y}, r) {}
        Geometry() {}
        mapbox::geometry::box<float> box;
        geometry::circle<float> circle;
    } geometry;
    Type type = Type::Unknown;
};

class CollisionBox {
public:
    CollisionBox(Point<float> _anchor, float _x1, float _y1, float _x2, float _y2, float _signedDistanceFromAnchor = 0) :
        anchor(std::move(_anchor)), x1(_x1), y1(_y1), x2(_x2), y2(_y2), signedDistanceFromAnchor(_signedDistanceFromAnchor) {}

    // the box is centered around the anchor point
    Point<float> anchor;

    // the offset of the box from the label's anchor point.
    // TODO: might be needed for #13526
    // Point<float> offset;

    // distances to the edges from the anchor
    float x1;
    float y1;
    float x2;
    float y2;

    float signedDistanceFromAnchor;
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
                     const float overscaling,
                     const float rotate)
        : CollisionFeature(line, anchor, shapedText.top, shapedText.bottom, shapedText.left, shapedText.right, boxScale, padding, placement, indexedFeature_, overscaling, rotate) {}

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
                     const IndexedSubfeature& indexedFeature_,
                     const float rotate)
        : CollisionFeature(line, anchor,
                           (shapedIcon ? shapedIcon->top() : 0),
                           (shapedIcon ? shapedIcon->bottom() : 0),
                           (shapedIcon ? shapedIcon->left() : 0),
                           (shapedIcon ? shapedIcon->right() : 0),
                           boxScale,
                           padding,
                           style::SymbolPlacementType::Point,
                           indexedFeature_, 1, rotate) {}

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
                     const float overscaling,
                     const float rotate);

    std::vector<CollisionBox> boxes;
    IndexedSubfeature indexedFeature;
    bool alongLine;

private:
    void bboxifyLabel(const GeometryCoordinates& line, GeometryCoordinate& anchorPoint,
                      const int segment, const float length, const float height, const float overscaling);
};

} // namespace mbgl
