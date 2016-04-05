#ifndef MBGL_TEXT_COLLISION_FEATURE
#define MBGL_TEXT_COLLISION_FEATURE

#include <mbgl/util/vec.hpp>
#include <mbgl/geometry/anchor.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/geometry/feature_index.hpp>

#include <vector>

namespace mbgl {
    class CollisionBox {
        public:
            explicit CollisionBox(const vec2<float> &_anchor, float _x1, float _y1, float _x2, float _y2, float _maxScale,
                    const IndexedSubfeature& indexedFeature_ = { 0, "", "", 0 }) :
                anchor(_anchor), x1(_x1), y1(_y1), x2(_x2), y2(_y2), maxScale(_maxScale), indexedFeature(indexedFeature_) {}

            // the box is centered around the anchor point
            vec2<float> anchor;

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

            IndexedSubfeature indexedFeature;
    };

    class CollisionFeature {
        public:
            // for text
            inline explicit CollisionFeature(const GeometryCoordinates &line, const Anchor &anchor,
                    const Shaping &shapedText,
                    const float boxScale, const float padding, const bool alongLine, const IndexedSubfeature& indexedFeature)
                : CollisionFeature(line, anchor,
                        shapedText.top, shapedText.bottom, shapedText.left, shapedText.right,
                        boxScale, padding, alongLine, indexedFeature, false) {}

            // for icons
            inline explicit CollisionFeature(const GeometryCoordinates &line, const Anchor &anchor,
                    const PositionedIcon &shapedIcon,
                    const float boxScale, const float padding, const bool alongLine, const IndexedSubfeature& indexedFeature)
                : CollisionFeature(line, anchor,
                        shapedIcon.top, shapedIcon.bottom, shapedIcon.left, shapedIcon.right,
                        boxScale, padding, alongLine, indexedFeature, true) {}

            explicit CollisionFeature(const GeometryCoordinates &line, const Anchor &anchor,
                    const float top, const float bottom, const float left, const float right,
                    const float boxScale, const float padding, const bool alongLine,
                    const IndexedSubfeature&, const bool straight);


            std::vector<CollisionBox> boxes;

        private:
            void bboxifyLabel(const GeometryCoordinates &line, GeometryCoordinate &anchorPoint,
                    const int segment, const float length, const float height,
                    const IndexedSubfeature&);
    };
} // namespace mbgl

#endif
