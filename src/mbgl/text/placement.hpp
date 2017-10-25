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
            OpacityState(float targetOpacity);
            OpacityState(OpacityState& prevOpacityState, float increment, float targetOpacity);
            float opacity;
            float targetOpacity;

            bool isHidden() const;
    };

    class JointOpacityState {
        public:
            JointOpacityState(float iconOpacity, float textOpacity);
            JointOpacityState(JointOpacityState& prevOpacityState, float increment, float iconOpacity, float textOpacity);
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
            void commit(std::unique_ptr<Placement> prevPlacement, TimePoint);
            void updateLayerOpacities(RenderSymbolLayer&, gl::Context&);
            JointOpacityState getOpacity(uint32_t crossTileSymbolID) const;

        private:

            void placeLayerBucket(
                    RenderSymbolLayer&,
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
