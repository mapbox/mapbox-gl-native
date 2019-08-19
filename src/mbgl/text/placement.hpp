#pragma once

#include <string>
#include <unordered_map>
#include <mbgl/util/chrono.hpp>
#include <mbgl/text/collision_index.hpp>
#include <mbgl/layout/symbol_projection.hpp>
#include <mbgl/style/transition_options.hpp>
#include <unordered_set>

namespace mbgl {

class SymbolBucket;
class SymbolInstance;
enum class PlacedSymbolOrientation : bool;

class OpacityState {
public:
    OpacityState(bool placed, bool skipFade);
    OpacityState(const OpacityState& prevOpacityState, float increment, bool placed);
    bool isHidden() const;
    float opacity;
    bool placed;
};

class JointOpacityState {
public:
    JointOpacityState(bool placedIcon, bool placedText, bool skipFade);
    JointOpacityState(const JointOpacityState& prevOpacityState, float increment, bool placedIcon, bool placedText);
    bool isHidden() const;
    OpacityState icon;
    OpacityState text;
};

class VariableOffset {
public:
    float radialOffset;
    float width;
    float height;
    style::TextVariableAnchorType anchor;
    float textBoxScale;
    optional<style::TextVariableAnchorType> prevAnchor;
};

class JointPlacement {
public:
    JointPlacement(bool text_, bool icon_, bool skipFade_)
        : text(text_), icon(icon_), skipFade(skipFade_)
    {}

    const bool text;
    const bool icon;
    // skipFade = outside viewport, but within CollisionIndex::viewportPadding px of the edge
    // Because these symbols aren't onscreen yet, we can skip the "fade in" animation,
    // and if a subsequent viewport change brings them into view, they'll be fully
    // visible right away.
    const bool skipFade;
};
  
struct RetainedQueryData {
    uint32_t bucketInstanceId;
    std::shared_ptr<FeatureIndex> featureIndex;
    OverscaledTileID tileID;
    std::shared_ptr<std::vector<size_t>> featureSortOrder;
    
    RetainedQueryData(uint32_t bucketInstanceId_,
                      std::shared_ptr<FeatureIndex> featureIndex_,
                      OverscaledTileID tileID_)
        : bucketInstanceId(bucketInstanceId_)
        , featureIndex(std::move(featureIndex_))
        , tileID(std::move(tileID_)) {}
};
    
class CollisionGroups {
public:
    using Predicate = std::function<bool(const IndexedSubfeature&)>;
    using CollisionGroup = std::pair<uint16_t, optional<Predicate>>;
    
    CollisionGroups(const bool crossSourceCollisions_)
        : maxGroupID(0)
        , crossSourceCollisions(crossSourceCollisions_)
    {}
    
    const CollisionGroup& get(const std::string& sourceID);
    
private:
    std::map<std::string, CollisionGroup> collisionGroups;
    uint16_t maxGroupID;
    bool crossSourceCollisions;
};

class BucketPlacementParameters {
public:
    const RenderTile& tile;
    const mat4& projMatrix;
    std::string sourceId;
    std::shared_ptr<FeatureIndex> featureIndex;
    bool showCollisionBoxes;
};
    
class Placement {
public:
    Placement(const TransformState&, MapMode, style::TransitionOptions, const bool crossSourceCollisions, std::unique_ptr<Placement> prevPlacementOrNull = nullptr);
    void placeLayer(const RenderLayer&, const mat4&, bool showCollisionBoxes);
    void commit(TimePoint, const double zoom);
    void updateLayerBuckets(const RenderLayer&, const TransformState&,  bool updateOpacities);
    float symbolFadeChange(TimePoint now) const;
    bool hasTransitions(TimePoint now) const;

    const CollisionIndex& getCollisionIndex() const;

    bool stillRecent(TimePoint now, const float zoom) const;
    void setRecent(TimePoint now);
    void setStale();
    
    const RetainedQueryData& getQueryData(uint32_t bucketInstanceId) const;
private:
    friend SymbolBucket;
    void placeBucket(
            const SymbolBucket&,
            const BucketPlacementParameters&,
            std::set<uint32_t>& seenCrossTileIDs);
    // Returns `true` if bucket vertices were updated; returns `false` otherwise.
    bool updateBucketDynamicVertices(SymbolBucket&, const TransformState&, const RenderTile& tile) const;
    void updateBucketOpacities(SymbolBucket&, const TransformState&, std::set<uint32_t>&);
    void markUsedJustification(SymbolBucket&, style::TextVariableAnchorType, const SymbolInstance&, style::TextWritingModeType orientation);
    void markUsedOrientation(SymbolBucket&, style::TextWritingModeType, const SymbolInstance&);
    float zoomAdjustment(const float zoom) const;

    CollisionIndex collisionIndex;

    MapMode mapMode;
    style::TransitionOptions transitionOptions;

    TimePoint fadeStartTime;
    TimePoint commitTime;
    float placementZoom;
    float prevZoomAdjustment = 0;

    std::unordered_map<uint32_t, JointPlacement> placements;
    std::unordered_map<uint32_t, JointOpacityState> opacities;
    std::unordered_map<uint32_t, VariableOffset> variableOffsets;
    std::unordered_map<uint32_t, style::TextWritingModeType> placedOrientations;

    bool stale = false;
    
    std::unordered_map<uint32_t, RetainedQueryData> retainedQueryData;
    CollisionGroups collisionGroups;
    std::unique_ptr<Placement> prevPlacement;

    // Used for debug purposes.
    std::unordered_map<const CollisionFeature*, std::vector<ProjectedCollisionBox>> collisionCircles;
};

} // namespace mbgl
