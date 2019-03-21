#pragma once

#include <string>
#include <unordered_map>
#include <mbgl/util/chrono.hpp>
#include <mbgl/text/collision_index.hpp>
#include <mbgl/layout/symbol_projection.hpp>
#include <mbgl/style/transition_options.hpp>
#include <unordered_set>

namespace mbgl {

class RenderLayerSymbolInterface;
class SymbolBucket;

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
    
class Placement {
public:
    Placement(const TransformState&, MapMode, style::TransitionOptions, const bool crossSourceCollisions);
    void placeLayer(const RenderLayerSymbolInterface&, const mat4&, bool showCollisionBoxes);
    void commit(const Placement& prevPlacement, TimePoint);
    void updateLayerOpacities(const RenderLayerSymbolInterface&);
    float symbolFadeChange(TimePoint now) const;
    bool hasTransitions(TimePoint now) const;

    const CollisionIndex& getCollisionIndex() const;

    bool stillRecent(TimePoint now) const;
    void setRecent(TimePoint now);
    void setStale();
    
    const RetainedQueryData& getQueryData(uint32_t bucketInstanceId) const;
private:

    void placeLayerBucket(
            SymbolBucket&,
            const mat4& posMatrix,
            const mat4& textLabelPlaneMatrix,
            const mat4& iconLabelPlaneMatrix,
            const float scale,
            const float pixelRatio,
            const bool showCollisionBoxes,
            std::unordered_set<uint32_t>& seenCrossTileIDs,
            const bool holdingForFade,
            const CollisionGroups::CollisionGroup& collisionGroup);

    void updateBucketOpacities(SymbolBucket&, std::set<uint32_t>&);

    CollisionIndex collisionIndex;

    TransformState state;
    MapMode mapMode;
    style::TransitionOptions transitionOptions;

    TimePoint fadeStartTime;
    TimePoint commitTime;

    std::unordered_map<uint32_t, JointPlacement> placements;
    std::unordered_map<uint32_t, JointOpacityState> opacities;

    bool stale = false;
    
    std::unordered_map<uint32_t, RetainedQueryData> retainedQueryData;
    CollisionGroups collisionGroups;
};

Point<float> calculateVariableLayoutOffset(style::SymbolAnchorType anchor,
                                           float width,
                                           float height,
                                           float radialOffset,
                                           float textBoxScale);

} // namespace mbgl
