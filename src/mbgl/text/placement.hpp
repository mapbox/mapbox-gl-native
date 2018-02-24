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

class Placement {
public:
    Placement(const TransformState&, MapMode mapMode);
    void placeLayer(RenderSymbolLayer&, const mat4&, bool showCollisionBoxes);
    bool commit(const Placement& prevPlacement, TimePoint);
    void updateLayerOpacities(RenderSymbolLayer&);
    float symbolFadeChange(TimePoint now) const;
    bool hasTransitions(TimePoint now) const;

    const CollisionIndex& getCollisionIndex() const;

    bool stillRecent(TimePoint now) const;
    void setRecent(TimePoint now);
    void setStale();
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
            const bool holdingForFade);

    void updateBucketOpacities(SymbolBucket&, std::set<uint32_t>&);

    CollisionIndex collisionIndex;

    TransformState state;
    MapMode mapMode;
    TimePoint commitTime;

    std::unordered_map<uint32_t, JointPlacement> placements;
    std::unordered_map<uint32_t, JointOpacityState> opacities;

    TimePoint recentUntil;
    bool stale = false;
};

} // namespace mbgl
