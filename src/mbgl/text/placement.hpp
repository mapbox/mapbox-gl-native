#pragma once

#include <mbgl/layout/symbol_projection.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/text/collision_index.hpp>
#include <mbgl/util/chrono.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace mbgl {

class SymbolBucket;
class SymbolInstance;
using SymbolInstanceReferences = std::vector<std::reference_wrapper<const SymbolInstance>>;
class UpdateParameters;
enum class PlacedSymbolOrientation : bool;

class OpacityState {
public:
    OpacityState(bool placed, bool skipFade);
    OpacityState(const OpacityState& prevState, float increment, bool placed);
    bool isHidden() const;
    float opacity;
    bool placed;
};

class JointOpacityState {
public:
    JointOpacityState(bool placedText, bool placedIcon, bool skipFade);
    JointOpacityState(const JointOpacityState& prevOpacityState, float increment, bool placedText, bool placedIcon);
    bool isHidden() const;
    OpacityState icon;
    OpacityState text;
};

class VariableOffset {
public:
    std::array<float, 2> offset;
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

    bool placed() const { return text || icon; }

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
    mutable FeatureSortOrder featureSortOrder;

    RetainedQueryData(uint32_t bucketInstanceId_, std::shared_ptr<FeatureIndex> featureIndex_, OverscaledTileID tileID_)
        : bucketInstanceId(bucketInstanceId_), featureIndex(std::move(featureIndex_)), tileID(tileID_) {}
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

class Placement;
class PlacementContext;
class PlacementController {
public:
    PlacementController();
    void setPlacement(Immutable<Placement>);
    const Immutable<Placement>& getPlacement() const { return placement; }
    void setPlacementStale() { stale = true; }
    bool placementIsRecent(TimePoint now, float zoom, optional<Duration> periodOverride = nullopt) const;
    bool hasTransitions(TimePoint now) const;

private:
    Immutable<Placement> placement;
    bool stale = false;
};

class Placement {
public:
    /**
     * @brief creates a new placement instance, from the given update parameters and the previous placement instance.
     *
     * Different placement implementations are created based on `updateParameters->mapMode`.
     * In Continuous map mode, `prevPlacement` must be provided.
     */
    static Mutable<Placement> create(std::shared_ptr<const UpdateParameters> updateParameters,
                                     optional<Immutable<Placement>> prevPlacement = nullopt);

    virtual ~Placement();
    virtual void placeLayers(const RenderLayerReferences&);
    void updateLayerBuckets(const RenderLayer&, const TransformState&, bool updateOpacities) const;
    virtual float symbolFadeChange(TimePoint now) const;
    virtual bool hasTransitions(TimePoint now) const;
    virtual bool transitionsEnabled() const;
    virtual void collectPlacedSymbolData(bool /*enable*/) {}
    virtual const std::vector<PlacedSymbolData>& getPlacedSymbolsData() const;

    const CollisionIndex& getCollisionIndex() const;
    TimePoint getCommitTime() const { return commitTime; }
    Duration getUpdatePeriod(float zoom) const;

    float zoomAdjustment(float zoom) const;
    const JointPlacement* getSymbolPlacement(const SymbolInstance&) const;

    const RetainedQueryData& getQueryData(uint32_t bucketInstanceId) const;

    // Public constructors are required for makeMutable(), shall not be called directly.
    Placement();
    Placement(std::shared_ptr<const UpdateParameters>, optional<Immutable<Placement>> prevPlacement);

protected:
    friend SymbolBucket;
    virtual void placeSymbolBucket(const BucketPlacementData&, std::set<uint32_t>& seenCrossTileIDs);
    JointPlacement placeSymbol(const SymbolInstance& symbolInstance, const PlacementContext&);
    void placeLayer(const RenderLayer&, std::set<uint32_t>&);
    virtual void commit();
    virtual void newSymbolPlaced(const SymbolInstance&,
                                 const PlacementContext&,
                                 const JointPlacement&,
                                 style::SymbolPlacementType,
                                 const std::vector<ProjectedCollisionBox>& /*textBoxes*/,
                                 const std::vector<ProjectedCollisionBox>& /*iconBoxes*/) {}
    // Implentation specific hooks, which get called during a symbol bucket placement.
    virtual optional<CollisionBoundaries> getAvoidEdges(const SymbolBucket&, const mat4& /*posMatrix*/) {
        return nullopt;
    }
    SymbolInstanceReferences getSortedSymbols(const BucketPlacementData&, float pixelRatio);
    virtual bool canPlaceAtVariableAnchor(const CollisionBox&,
                                          style::TextVariableAnchorType,
                                          Point<float> /*shift*/,
                                          std::vector<style::TextVariableAnchorType>&,
                                          const mat4& /*posMatrix*/,
                                          float /*textPixelRatio*/) {
        return true;
    }

    // Returns `true` if bucket vertices were updated; returns `false` otherwise.
    bool updateBucketDynamicVertices(SymbolBucket&, const TransformState&, const RenderTile& tile) const;
    void updateBucketOpacities(SymbolBucket&, const TransformState&, std::set<uint32_t>&) const;
    void markUsedJustification(SymbolBucket&,
                               style::TextVariableAnchorType,
                               const SymbolInstance&,
                               style::TextWritingModeType orientation) const;
    void markUsedOrientation(SymbolBucket&, style::TextWritingModeType, const SymbolInstance&) const;
    const Placement* getPrevPlacement() const { return prevPlacement ? prevPlacement->get() : nullptr; }
    bool isTiltedView() const;

    std::shared_ptr<const UpdateParameters> updateParameters;
    CollisionIndex collisionIndex;

    style::TransitionOptions transitionOptions;

    TimePoint fadeStartTime;
    TimePoint commitTime;
    float placementZoom = 0.0f;
    float prevZoomAdjustment = 0.0f;

    std::unordered_map<uint32_t, JointPlacement> placements;
    std::unordered_map<uint32_t, JointOpacityState> opacities;
    std::unordered_map<uint32_t, VariableOffset> variableOffsets;
    std::unordered_map<uint32_t, style::TextWritingModeType> placedOrientations;

    std::unordered_map<uint32_t, RetainedQueryData> retainedQueryData;
    CollisionGroups collisionGroups;
    mutable optional<Immutable<Placement>> prevPlacement;
    bool showCollisionBoxes = false;

    // Cache being used by placeSymbol()
    std::vector<ProjectedCollisionBox> textBoxes;
    std::vector<ProjectedCollisionBox> iconBoxes;
    // Used for debug purposes.
    std::unordered_map<const CollisionFeature*, std::vector<ProjectedCollisionBox>> collisionCircles;
};

} // namespace mbgl
