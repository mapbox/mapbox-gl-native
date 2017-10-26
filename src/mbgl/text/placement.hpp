#pragma once

#include <string>
#include <unordered_map>
#include <mbgl/util/chrono.hpp>
#include <mbgl/text/collision_index.hpp>
#include <mbgl/layout/symbol_projection.hpp>

namespace mbgl {

    class RenderSymbolLayer;
    class SymbolBucket;

    class OpacityState {
        public:
            OpacityState(bool placed);
            OpacityState(OpacityState& prevOpacityState, float increment, bool placed);
            bool isHidden() const;
            float opacity;
            bool placed;
    };

    class JointOpacityState {
        public:
            JointOpacityState(bool placedIcon, bool placedText);
            JointOpacityState(JointOpacityState& prevOpacityState, float increment, bool placedIcon, bool placedText);
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
            Placement(const TransformState&);
            void placeLayer(RenderSymbolLayer&, bool showCollisionBoxes);
            bool commit(std::unique_ptr<Placement> prevPlacement, TimePoint);
            void updateLayerOpacities(RenderSymbolLayer&, gl::Context&);
            JointOpacityState getOpacity(uint32_t crossTileSymbolID) const;

        private:

            void placeLayerBucket(
                    SymbolBucket&,
                    const mat4& posMatrix,
                    const mat4& textLabelPlaneMatrix,
                    const mat4& iconLabelPlaneMatrix,
                    const float scale,
                    const bool showCollisionBoxes);

            void updateBucketOpacities(SymbolBucket&, gl::Context&);

            CollisionIndex collisionIndex;
            TransformState state;
            TimePoint commitTime;

            static uint32_t maxCrossTileID; // TODO remove
            std::unordered_map<uint32_t,PlacementPair> placements;
            std::unordered_map<uint32_t,JointOpacityState> opacities;
    };

} // namespace mbgl
