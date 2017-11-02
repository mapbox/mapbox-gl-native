#pragma once

#include <string>
#include <unordered_map>
#include <mbgl/util/chrono.hpp>
#include <mbgl/text/collision_index.hpp>
#include <mbgl/layout/symbol_projection.hpp>
#include <unordered_set>

namespace mbgl {

    class RenderSymbolLayer;
    class SymbolBucket;

    class OpacityState {
        public:
            OpacityState(bool placed);
            OpacityState(const OpacityState& prevOpacityState, float increment, bool placed);
            bool isHidden() const;
            float opacity;
            bool placed;
    };

    class JointOpacityState {
        public:
            JointOpacityState(bool placedIcon, bool placedText);
            JointOpacityState(const JointOpacityState& prevOpacityState, float increment, bool placedIcon, bool placedText);
            bool isHidden() const;
            OpacityState icon;
            OpacityState text;
    };

    class PlacementPair {
        public:
            PlacementPair(bool text_, bool icon_) : text(text_), icon(icon_) {}
            bool text;
            bool icon;
    };

    class Placement {
        public:
            Placement(const TransformState&, MapMode mapMode);
            void placeLayer(RenderSymbolLayer&, const mat4&, bool showCollisionBoxes);
            bool commit(const Placement& prevPlacement, TimePoint);
            void updateLayerOpacities(RenderSymbolLayer&);
            JointOpacityState getOpacity(uint32_t crossTileSymbolID) const;
            float symbolFadeChange(TimePoint now) const;
            bool hasTransitions(TimePoint now) const;

            // TODO: public for queryRenderedFeatures
            CollisionIndex collisionIndex;

        private:

            void placeLayerBucket(
                    SymbolBucket&,
                    const mat4& posMatrix,
                    const mat4& textLabelPlaneMatrix,
                    const mat4& iconLabelPlaneMatrix,
                    const float scale,
                    const float pixelRatio,
                    const bool showCollisionBoxes,
                    std::unordered_set<uint32_t>& seenCrossTileIDs);

            void updateBucketOpacities(SymbolBucket&);

            TransformState state;
            MapMode mapMode;
            TimePoint commitTime;

            std::unordered_map<uint32_t,PlacementPair> placements;
            std::unordered_map<uint32_t,JointOpacityState> opacities;
    };

} // namespace mbgl
