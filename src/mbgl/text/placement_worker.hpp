#pragma once

#include <unordered_map>
#include <mbgl/util/immutable.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/text/collision_index.hpp>

namespace mbgl {

class PlacementBucket {
    public:
        PlacementBucket(Immutable<SymbolBucket::Core>&, Immutable<std::vector<uint32_t>>& ids, const UnwrappedTileID&, const OverscaledTileID&);
        Immutable<SymbolBucket::Core> symbolBucketCore;
        Immutable<std::vector<uint32_t>> symbolCrossTileIDs;
        UnwrappedTileID unwrappedTileID;
        OverscaledTileID overscaledTileID;
};

class PlacementPair {
    public:
        PlacementPair(bool text_, bool icon_) : text(text_), icon(icon_) {}
        bool text;
        bool icon;
};

class PlacementResult {
    public:
        PlacementResult(const TransformState& state) : collisionIndex(state) {}
        CollisionIndex collisionIndex;
        std::unordered_map<uint32_t,PlacementPair> placements;
};

class PlacementWorker {
    public:
        PlacementWorker(const TransformState&, const mat4& projMatrix, const bool showCollisionBoxes);
        Immutable<PlacementResult> place(Immutable<std::vector<std::vector<PlacementBucket>>> placementBucketLayers);

    private:
        TransformState state;
        const mat4 projMatrix;
        const bool showCollisionBoxes;

        void placeLayerBucket(
                std::unordered_map<uint32_t,PlacementPair>& placements,
                CollisionIndex& collisionIndex,
                const SymbolBucket::Core&,
                const std::vector<uint32_t>& symbolCrossTileIDs,
                const UnwrappedTileID&,
                const OverscaledTileID&,
                std::unordered_set<uint32_t>& seenCrossTileIDs);
};

} // namespace mbgl
