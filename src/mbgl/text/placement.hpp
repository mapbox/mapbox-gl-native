#pragma once

#include <string>
#include <unordered_map>
#include <mbgl/actor/actor.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/text/placement_worker.hpp>
#include <mbgl/layout/symbol_projection.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <unordered_set>

namespace mbgl {

    class RenderSymbolLayer;

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

    class Placement {
        public:
            Placement(const TransformState&, MapMode mapMode, const mat4& projMatrix, const bool showCollisionBoxes, Scheduler&);
            void addLayer(RenderSymbolLayer&);
            void place();
            bool isReady();
            bool commit(const Placement& prevPlacement, TimePoint);
            void updateLayerOpacities(RenderSymbolLayer&);
            JointOpacityState getOpacity(uint32_t crossTileSymbolID) const;
            float symbolFadeChange(TimePoint now) const;
            bool hasTransitions(TimePoint now) const;

            // TODO: public for queryRenderedFeatures
            Immutable<PlacementResult> result;
        
            bool stillRecent(TimePoint now) const;
            void setRecent(TimePoint now);
            void setStale();
        private:

            void placeLayerBucket(
                    const SymbolBucket::Core&,
                    const std::vector<uint32_t>& symbolCrossTileIDs,
                    const UnwrappedTileID&,
                    const OverscaledTileID&,
                    std::unordered_set<uint32_t>& seenCrossTileIDs);

            void updateBucketOpacities(SymbolBucket&, std::unordered_set<uint32_t>&);

            TransformState state;
            const MapMode mapMode;
            TimePoint commitTime;
            const bool showCollisionBoxes;
            const mat4 projMatrix;

            Actor<PlacementWorker> worker;
            std::future<Immutable<PlacementResult>> future;

            Mutable<std::vector<std::vector<PlacementBucket>>> placementBucketLayers;

            std::unordered_map<uint32_t,JointOpacityState> opacities;
        
            TimePoint recentUntil;
            bool stale = false;
    };

} // namespace mbgl
