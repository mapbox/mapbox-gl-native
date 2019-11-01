#include <mbgl/text/placement.hpp>

#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/math.hpp>
#include <utility>

namespace mbgl {

OpacityState::OpacityState(bool placed_, bool skipFade)
    : opacity((skipFade && placed_) ? 1 : 0)
    , placed(placed_)
{
}

OpacityState::OpacityState(const OpacityState& prevState, float increment, bool placed_) :
    opacity(::fmax(0, ::fmin(1, prevState.opacity + (prevState.placed ? increment : -increment)))),
    placed(placed_) {}

bool OpacityState::isHidden() const {
    return opacity == 0 && !placed;
}

JointOpacityState::JointOpacityState(bool placedText, bool placedIcon, bool skipFade) :
    icon(OpacityState(placedIcon, skipFade)),
    text(OpacityState(placedText, skipFade)) {}

JointOpacityState::JointOpacityState(const JointOpacityState& prevOpacityState, float increment, bool placedText, bool placedIcon) :
    icon(OpacityState(prevOpacityState.icon, increment, placedIcon)),
    text(OpacityState(prevOpacityState.text, increment, placedText)) {}

bool JointOpacityState::isHidden() const {
    return icon.isHidden() && text.isHidden();
}
    
const CollisionGroups::CollisionGroup& CollisionGroups::get(const std::string& sourceID) {
    // The predicate/groupID mechanism allows for arbitrary grouping,
    // but the current interface defines one source == one group when
    // crossSourceCollisions == true.
    if (!crossSourceCollisions) {
        if (collisionGroups.find(sourceID) == collisionGroups.end()) {
            uint16_t nextGroupID = ++maxGroupID;
            collisionGroups.emplace(sourceID, CollisionGroup(
                nextGroupID,
                optional<Predicate>([nextGroupID](const IndexedSubfeature& feature) -> bool {
                    return feature.collisionGroupId == nextGroupID;
                })
            ));
        }
        return collisionGroups[sourceID];
    } else {
        static CollisionGroup nullGroup{0, nullopt};
        return nullGroup;
    }
}

// PlacementController implemenation

PlacementController::PlacementController()
    : placement(makeMutable<Placement>(TransformState{}, MapMode::Static, style::TransitionOptions{}, true, nullopt)) {}

void PlacementController::setPlacement(Immutable<Placement> placement_) {
    placement = std::move(placement_);
    stale = false;
}

bool PlacementController::placementIsRecent(TimePoint now, const float zoom, optional<Duration> maximumDuration) const {
    if (!placement->transitionsEnabled()) return false;

    auto updatePeriod = placement->getUpdatePeriod(zoom);

    if (maximumDuration) {
        updatePeriod = std::min(*maximumDuration, updatePeriod);
    }

    return placement->getCommitTime() + updatePeriod > now;
}

bool PlacementController::hasTransitions(TimePoint now) const {
    if (!placement->transitionsEnabled()) return false;

    if (stale) return true;

    return placement->hasTransitions(now);
}

// Placement implementation

Placement::Placement(const TransformState& state_,
                     MapMode mapMode_,
                     style::TransitionOptions transitionOptions_,
                     const bool crossSourceCollisions,
                     optional<Immutable<Placement>> prevPlacement_)
    : collisionIndex(state_),
      mapMode(mapMode_),
      transitionOptions(std::move(transitionOptions_)),
      placementZoom(state_.getZoom()),
      collisionGroups(crossSourceCollisions),
      prevPlacement(std::move(prevPlacement_)) {
    if (prevPlacement) {
        prevPlacement->get()->prevPlacement = nullopt; // Only hold on to one placement back
    }
}

void Placement::placeLayer(const RenderLayer& layer, const mat4& projMatrix, bool showCollisionBoxes) {
    std::set<uint32_t> seenCrossTileIDs;
    for (const auto& item : layer.getPlacementData()) {
        Bucket& bucket = item.bucket;
        BucketPlacementParameters params{
                item.tile,
                projMatrix,
                layer.baseImpl->source,
                item.featureIndex,
                showCollisionBoxes};
        bucket.place(*this, params, seenCrossTileIDs);
    }
}

namespace {
Point<float> calculateVariableLayoutOffset(style::SymbolAnchorType anchor, float width, float height, std::array<float, 2> offset, float textBoxScale) {
    AnchorAlignment alignment = AnchorAlignment::getAnchorAlignment(anchor);
    float shiftX = -(alignment.horizontalAlign - 0.5f) * width;
    float shiftY = -(alignment.verticalAlign - 0.5f) * height;
    auto variableOffset = SymbolLayout::evaluateVariableOffset(anchor, offset);
    return { shiftX + variableOffset[0] * textBoxScale,
             shiftY + variableOffset[1] * textBoxScale };
}
} // namespace

void Placement::placeBucket(
        const SymbolBucket& bucket,
        const BucketPlacementParameters& params,
        std::set<uint32_t>& seenCrossTileIDs) {
    const auto& layout = *bucket.layout;
    const auto& renderTile = params.tile;
    const auto& state = collisionIndex.getTransformState();
    const float pixelsToTileUnits = renderTile.id.pixelsToTileUnits(1, state.getZoom());
    const OverscaledTileID& overscaledID = renderTile.getOverscaledTileID();
    const float scale = std::pow(2, state.getZoom() - overscaledID.overscaledZ);
    const float pixelRatio = (util::tileSize * overscaledID.overscaleFactor()) / util::EXTENT;

    mat4 posMatrix;
    state.matrixFor(posMatrix, renderTile.id);
    matrix::multiply(posMatrix, params.projMatrix, posMatrix);

    mat4 textLabelPlaneMatrix = getLabelPlaneMatrix(posMatrix,
            layout.get<style::TextPitchAlignment>() == style::AlignmentType::Map,
            layout.get<style::TextRotationAlignment>() == style::AlignmentType::Map,
            state,
            pixelsToTileUnits);

    mat4 iconLabelPlaneMatrix = getLabelPlaneMatrix(posMatrix,
            layout.get<style::IconPitchAlignment>() == style::AlignmentType::Map,
            layout.get<style::IconRotationAlignment>() == style::AlignmentType::Map,
            state,
            pixelsToTileUnits);

    const auto& collisionGroup = collisionGroups.get(params.sourceId);
    auto partiallyEvaluatedTextSize = bucket.textSizeBinder->evaluateForZoom(state.getZoom());
    auto partiallyEvaluatedIconSize = bucket.iconSizeBinder->evaluateForZoom(state.getZoom());

    optional<CollisionTileBoundaries> avoidEdges;
    if (mapMode == MapMode::Tile && (layout.get<style::SymbolAvoidEdges>() ||
                                     layout.get<style::SymbolPlacement>() == style::SymbolPlacementType::Line ||
                                     !layout.get<style::TextVariableAnchor>().empty())) {
        avoidEdges = collisionIndex.projectTileBoundaries(posMatrix);
    }

    const bool textAllowOverlap = layout.get<style::TextAllowOverlap>();
    const bool iconAllowOverlap = layout.get<style::IconAllowOverlap>();
    // This logic is similar to the "defaultOpacityState" logic below in updateBucketOpacities
    // If we know a symbol is always supposed to show, force it to be marked visible even if
    // it wasn't placed into the collision index (because some or all of it was outside the range
    // of the collision grid).
    // There is a subtle edge case here we're accepting:
    //  Symbol A has text-allow-overlap: true, icon-allow-overlap: true, icon-optional: false
    //  A's icon is outside the grid, so doesn't get placed
    //  A's text would be inside grid, but doesn't get placed because of icon-optional: false
    //  We still show A because of the allow-overlap settings.
    //  Symbol B has allow-overlap: false, and gets placed where A's text would be
    //  On panning in, there is a short period when Symbol B and Symbol A will overlap
    //  This is the reverse of our normal policy of "fade in on pan", but should look like any other
    //  collision and hopefully not be too noticeable.
    // See https://github.com/mapbox/mapbox-gl-native/issues/12683
    const bool alwaysShowText = textAllowOverlap && (iconAllowOverlap || !(bucket.hasIconData() || bucket.hasSdfIconData()) || layout.get<style::IconOptional>());
    const bool alwaysShowIcon = iconAllowOverlap && (textAllowOverlap || !bucket.hasTextData() || layout.get<style::TextOptional>());
    std::vector<style::TextVariableAnchorType> variableTextAnchors = layout.get<style::TextVariableAnchor>();
    const bool rotateWithMap = layout.get<style::TextRotationAlignment>() == style::AlignmentType::Map;
    const bool pitchWithMap = layout.get<style::TextPitchAlignment>() == style::AlignmentType::Map;
    const bool hasIconTextFit = layout.get<style::IconTextFit>() != style::IconTextFitType::None;

    const bool zOrderByViewportY = layout.get<style::SymbolZOrder>() == style::SymbolZOrderType::ViewportY;
    std::vector<ProjectedCollisionBox> textBoxes;
    std::vector<ProjectedCollisionBox> iconBoxes;

    auto placeSymbol = [&] (const SymbolInstance& symbolInstance) {
        if (seenCrossTileIDs.count(symbolInstance.crossTileID) != 0u) return;

        if (renderTile.holdForFade()) {
            // Mark all symbols from this tile as "not placed", but don't add to seenCrossTileIDs, because we don't
            // know yet if we have a duplicate in a parent tile that _should_ be placed.
            placements.emplace(symbolInstance.crossTileID, JointPlacement(false, false, false));
            return;
        }
        textBoxes.clear();
        iconBoxes.clear();

        bool placeText = false;
        bool placeIcon = false;
        bool offscreen = true;
        std::pair<bool, bool> placed{ false, false };
        std::pair<bool, bool> placedVerticalText{ false, false };
        std::pair<bool, bool> placedVerticalIcon{ false, false };
        Point<float> shift{0.0f, 0.0f};
        optional<size_t> horizontalTextIndex = symbolInstance.getDefaultHorizontalPlacedTextIndex();
        if (horizontalTextIndex) {
            const PlacedSymbol& placedSymbol = bucket.text.placedSymbols.at(*horizontalTextIndex);
            const float fontSize = evaluateSizeForFeature(partiallyEvaluatedTextSize, placedSymbol);

            const auto updatePreviousOrientationIfNotPlaced = [&](bool isPlaced) {
                if (bucket.allowVerticalPlacement && !isPlaced && getPrevPlacement()) {
                    auto prevOrientation = getPrevPlacement()->placedOrientations.find(symbolInstance.crossTileID);
                    if (prevOrientation != getPrevPlacement()->placedOrientations.end()) {
                        placedOrientations[symbolInstance.crossTileID] = prevOrientation->second;
                    }
                }
            };

            const auto placeTextForPlacementModes = [&] (auto& placeHorizontalFn, auto& placeVerticalFn) {
                if (bucket.allowVerticalPlacement && symbolInstance.writingModes & WritingModeType::Vertical) {
                    assert(!bucket.placementModes.empty());
                    for (auto& placementMode : bucket.placementModes) {
                        if (placementMode == style::TextWritingModeType::Vertical) {
                            placedVerticalText = placed = placeVerticalFn();
                        } else {
                            placed = placeHorizontalFn();
                        }

                        if (placed.first) {
                            break;
                        }
                    }
                } else {
                    placed = placeHorizontalFn();
                }
            };

            // Line or point label placement
            if (variableTextAnchors.empty()) {
                const auto placeFeature = [&] (const CollisionFeature& collisionFeature, style::TextWritingModeType orientation) {
                    textBoxes.clear();
                    auto placedFeature = collisionIndex.placeFeature(collisionFeature, {},
                                                                     posMatrix, textLabelPlaneMatrix, pixelRatio,
                                                                     placedSymbol, scale, fontSize,
                                                                     layout.get<style::TextAllowOverlap>(),
                                                                     pitchWithMap,
                                                                     params.showCollisionBoxes, avoidEdges, collisionGroup.second, textBoxes);
                    if (placedFeature.first) {
                        placedOrientations.emplace(symbolInstance.crossTileID, orientation);
                    }
                    return placedFeature;
                };

                const auto placeHorizontal = [&] {
                    return placeFeature(symbolInstance.textCollisionFeature, style::TextWritingModeType::Horizontal);
                };

                const auto placeVertical = [&] {
                    if (bucket.allowVerticalPlacement && symbolInstance.verticalTextCollisionFeature) {
                        return placeFeature(*symbolInstance.verticalTextCollisionFeature, style::TextWritingModeType::Vertical);
                    }
                    return std::pair<bool, bool>{false, false};
                };

                placeTextForPlacementModes(placeHorizontal, placeVertical);
                updatePreviousOrientationIfNotPlaced(placed.first);

                placeText = placed.first;
                offscreen &= placed.second;
            } else if (!symbolInstance.textCollisionFeature.alongLine &&
                       !symbolInstance.textCollisionFeature.boxes.empty()) {
                // If this symbol was in the last placement, shift the previously used
                // anchor to the front of the anchor list, only if the previous anchor
                // is still in the anchor list.
                if (getPrevPlacement()) {
                    auto prevOffset = getPrevPlacement()->variableOffsets.find(symbolInstance.crossTileID);
                    if (prevOffset != getPrevPlacement()->variableOffsets.end()) {
                        const auto prevAnchor = prevOffset->second.anchor;
                        auto found = std::find(variableTextAnchors.begin(), variableTextAnchors.end(), prevAnchor);
                        if (found != variableTextAnchors.begin() && found != variableTextAnchors.end()) {
                            std::vector<style::TextVariableAnchorType> filtered;
                            filtered.reserve(variableTextAnchors.size());
                            filtered.push_back(prevAnchor);
                            for (auto anchor : variableTextAnchors) {
                                if (anchor != prevAnchor) {
                                    filtered.push_back(anchor);
                                }
                            }
                            variableTextAnchors = std::move(filtered);
                        }
                    }
                }

                const bool doVariableIconPlacement =
                    hasIconTextFit && !iconAllowOverlap && symbolInstance.placedIconIndex;

                const auto placeFeatureForVariableAnchors = [&](const CollisionFeature& textCollisionFeature,
                                                                style::TextWritingModeType orientation,
                                                                const CollisionFeature& iconCollisionFeature) {
                    const CollisionBox& textBox = textCollisionFeature.boxes[0];
                    const float width = textBox.x2 - textBox.x1;
                    const float height = textBox.y2 - textBox.y1;
                    const float textBoxScale = symbolInstance.textBoxScale;
                    std::pair<bool, bool> placedFeature = {false, false};
                    const size_t anchorsSize = variableTextAnchors.size();
                    const size_t placementAttempts = textAllowOverlap ? anchorsSize * 2 : anchorsSize;
                    for (size_t i = 0u; i < placementAttempts; ++i) {
                        auto anchor = variableTextAnchors[i % anchorsSize];
                        const bool allowOverlap = (i >= anchorsSize);
                        shift = calculateVariableLayoutOffset(anchor, width, height, symbolInstance.variableTextOffset, textBoxScale);
                        if (rotateWithMap) {
                            float angle = pitchWithMap ? state.getBearing() : -state.getBearing();
                            shift = util::rotate(shift, angle);
                        }

                        textBoxes.clear();
                        placedFeature = collisionIndex.placeFeature(textCollisionFeature,
                                                                    shift,
                                                                    posMatrix,
                                                                    mat4(),
                                                                    pixelRatio,
                                                                    placedSymbol,
                                                                    scale,
                                                                    fontSize,
                                                                    allowOverlap,
                                                                    pitchWithMap,
                                                                    params.showCollisionBoxes,
                                                                    avoidEdges,
                                                                    collisionGroup.second,
                                                                    textBoxes);

                        if (doVariableIconPlacement) {
                            auto placedIconFeature = collisionIndex.placeFeature(iconCollisionFeature,
                                                                                 shift,
                                                                                 posMatrix,
                                                                                 iconLabelPlaneMatrix,
                                                                                 pixelRatio,
                                                                                 placedSymbol,
                                                                                 scale,
                                                                                 fontSize,
                                                                                 iconAllowOverlap,
                                                                                 pitchWithMap,
                                                                                 params.showCollisionBoxes,
                                                                                 avoidEdges,
                                                                                 collisionGroup.second,
                                                                                 iconBoxes);
                            iconBoxes.clear();
                            if (!placedIconFeature.first) continue;
                        }

                        if (placedFeature.first) {
                            assert(symbolInstance.crossTileID != 0u);
                            optional<style::TextVariableAnchorType> prevAnchor;

                            // If this label was placed in the previous placement, record the anchor position
                            // to allow us to animate the transition
                            if (getPrevPlacement()) {
                                auto prevOffset = getPrevPlacement()->variableOffsets.find(symbolInstance.crossTileID);
                                auto prevPlacements = getPrevPlacement()->placements.find(symbolInstance.crossTileID);
                                if (prevOffset != getPrevPlacement()->variableOffsets.end() &&
                                    prevPlacements != getPrevPlacement()->placements.end() &&
                                    prevPlacements->second.text) {
                                    // TODO: The prevAnchor seems to be unused, needs to be fixed.
                                    prevAnchor = prevOffset->second.anchor;
                                }
                            }

                            variableOffsets.insert(std::make_pair(symbolInstance.crossTileID, VariableOffset{
                                symbolInstance.variableTextOffset,
                                width,
                                height,
                                anchor,
                                textBoxScale,
                                prevAnchor
                            }));

                            if (bucket.allowVerticalPlacement) {
                                placedOrientations.emplace(symbolInstance.crossTileID, orientation);
                            }
                            break;
                        }
                    }

                    return placedFeature;
                };

                const auto placeHorizontal = [&] {
                    return placeFeatureForVariableAnchors(symbolInstance.textCollisionFeature,
                                                          style::TextWritingModeType::Horizontal,
                                                          symbolInstance.iconCollisionFeature);
                };

                const auto placeVertical = [&] {
                    if (bucket.allowVerticalPlacement && !placed.first && symbolInstance.verticalTextCollisionFeature) {
                        return placeFeatureForVariableAnchors(*symbolInstance.verticalTextCollisionFeature,
                                                              style::TextWritingModeType::Vertical,
                                                              symbolInstance.verticalIconCollisionFeature
                                                                  ? *symbolInstance.verticalIconCollisionFeature
                                                                  : symbolInstance.iconCollisionFeature);
                    }
                    return std::pair<bool, bool>{false, false};
                };

                placeTextForPlacementModes(placeHorizontal, placeVertical);

                placeText = placed.first;
                offscreen &= placed.second;

                updatePreviousOrientationIfNotPlaced(placed.first);

                // If we didn't get placed, we still need to copy our position from the last placement for
                // fade animations
                if (!placeText && getPrevPlacement()) {
                    auto prevOffset = getPrevPlacement()->variableOffsets.find(symbolInstance.crossTileID);
                    if (prevOffset != getPrevPlacement()->variableOffsets.end()) {
                        variableOffsets[symbolInstance.crossTileID] = prevOffset->second;
                    }
                }
            }
        }

        if (symbolInstance.placedIconIndex) {
            if (!hasIconTextFit || !placeText || variableTextAnchors.empty()) {
                shift = {0.0f, 0.0f};
            }

            const auto& iconBuffer = symbolInstance.hasSdfIcon() ? bucket.sdfIcon : bucket.icon;
            const PlacedSymbol& placedSymbol = iconBuffer.placedSymbols.at(*symbolInstance.placedIconIndex);
            const float fontSize = evaluateSizeForFeature(partiallyEvaluatedIconSize, placedSymbol);
            const auto& placeIconFeature = [&] (const CollisionFeature& collisionFeature) {
                return collisionIndex.placeFeature(collisionFeature, shift,
                                                   posMatrix, iconLabelPlaneMatrix, pixelRatio,
                                                   placedSymbol, scale, fontSize,
                                                   layout.get<style::IconAllowOverlap>(),
                                                   pitchWithMap,
                                                   params.showCollisionBoxes, avoidEdges,
                                                   collisionGroup.second, iconBoxes);
            };

            std::pair<bool, bool> placedIcon = {false, false};
            if (placedVerticalText.first && symbolInstance.verticalIconCollisionFeature) {
                placedIcon = placedVerticalIcon = placeIconFeature(*symbolInstance.verticalIconCollisionFeature);
            } else {
                placedIcon = placeIconFeature(symbolInstance.iconCollisionFeature);
            }
            placeIcon = placedIcon.first;
            offscreen &= placedIcon.second;
        }

        const bool iconWithoutText = !symbolInstance.hasText() || layout.get<style::TextOptional>();
        const bool textWithoutIcon = !symbolInstance.hasIcon() || layout.get<style::IconOptional>();

        // combine placements for icon and text
        if (!iconWithoutText && !textWithoutIcon) {
            placeText = placeIcon = placeText && placeIcon;
        } else if (!textWithoutIcon) {
            placeText = placeText && placeIcon;
        } else if (!iconWithoutText) {
            placeIcon = placeText && placeIcon;
        }

        if (placeText) {
            if (placedVerticalText.first && symbolInstance.verticalTextCollisionFeature) {
                collisionIndex.insertFeature(*symbolInstance.verticalTextCollisionFeature, textBoxes, layout.get<style::TextIgnorePlacement>(), bucket.bucketInstanceId, collisionGroup.first);
            } else {
                collisionIndex.insertFeature(symbolInstance.textCollisionFeature, textBoxes, layout.get<style::TextIgnorePlacement>(), bucket.bucketInstanceId, collisionGroup.first);
            }
        }

        if (placeIcon) {
            if (placedVerticalIcon.first && symbolInstance.verticalIconCollisionFeature) {
                collisionIndex.insertFeature(*symbolInstance.verticalIconCollisionFeature, iconBoxes, layout.get<style::IconIgnorePlacement>(), bucket.bucketInstanceId, collisionGroup.first);
            } else {
                collisionIndex.insertFeature(symbolInstance.iconCollisionFeature, iconBoxes, layout.get<style::IconIgnorePlacement>(), bucket.bucketInstanceId, collisionGroup.first);
            }
        }

        const bool hasIconCollisionCircleData = bucket.hasIconCollisionCircleData();
        const bool hasTextCollisionCircleData = bucket.hasTextCollisionCircleData();

        if (hasIconCollisionCircleData && symbolInstance.iconCollisionFeature.alongLine && !iconBoxes.empty()) {
            collisionCircles[&symbolInstance.iconCollisionFeature] = iconBoxes;
        }
        if (hasTextCollisionCircleData && symbolInstance.textCollisionFeature.alongLine && !textBoxes.empty()) {
            collisionCircles[&symbolInstance.textCollisionFeature] = textBoxes;
        }

        assert(symbolInstance.crossTileID != 0);

        if (placements.find(symbolInstance.crossTileID) != placements.end()) {
            // If there's a previous placement with this ID, it comes from a tile that's fading out
            // Erase it so that the placement result from the non-fading tile supersedes it
            placements.erase(symbolInstance.crossTileID);
        }
        
        placements.emplace(symbolInstance.crossTileID, JointPlacement(placeText || alwaysShowText, placeIcon || alwaysShowIcon, offscreen || bucket.justReloaded));
        seenCrossTileIDs.insert(symbolInstance.crossTileID);
    };

    if (zOrderByViewportY) {
        const auto& sortedSymbols = bucket.getSortedSymbols(state.getBearing());
        // Place in the reverse order than draw i.e., starting from the foreground elements.
        for (auto it = sortedSymbols.rbegin(); it != sortedSymbols.rend(); ++it) {
            placeSymbol(*it);
        }
    } else {
        for (const SymbolInstance& symbol : bucket.symbolInstances) {
            placeSymbol(symbol);
        }
    }

    bucket.justReloaded = false;

    // As long as this placement lives, we have to hold onto this bucket's
    // matching FeatureIndex/data for querying purposes
    retainedQueryData.emplace(std::piecewise_construct,
                                std::forward_as_tuple(bucket.bucketInstanceId),
                                std::forward_as_tuple(bucket.bucketInstanceId, params.featureIndex, overscaledID));
}

void Placement::commit(TimePoint now, const double zoom) {
    commitTime = now;

    bool placementChanged = false;

    prevZoomAdjustment = getPrevPlacement()->zoomAdjustment(zoom);

    float increment = getPrevPlacement()->symbolFadeChange(commitTime);

    // add the opacities from the current placement, and copy their current values from the previous placement
    for (auto& jointPlacement : placements) {
        auto prevOpacity = getPrevPlacement()->opacities.find(jointPlacement.first);
        if (prevOpacity != getPrevPlacement()->opacities.end()) {
            opacities.emplace(jointPlacement.first, JointOpacityState(prevOpacity->second, increment, jointPlacement.second.text, jointPlacement.second.icon));
            placementChanged = placementChanged ||
                jointPlacement.second.icon != prevOpacity->second.icon.placed ||
                jointPlacement.second.text != prevOpacity->second.text.placed;
        } else {
            opacities.emplace(jointPlacement.first, JointOpacityState(jointPlacement.second.text, jointPlacement.second.icon, jointPlacement.second.skipFade));
            placementChanged = placementChanged || jointPlacement.second.icon || jointPlacement.second.text;
        }
    }

    // copy and update values from the previous placement that aren't in the current placement but haven't finished fading
    for (auto& prevOpacity : getPrevPlacement()->opacities) {
        if (opacities.find(prevOpacity.first) == opacities.end()) {
            JointOpacityState jointOpacity(prevOpacity.second, increment, false, false);
            if (!jointOpacity.isHidden()) {
                opacities.emplace(prevOpacity.first, jointOpacity);
                placementChanged = placementChanged || prevOpacity.second.icon.placed || prevOpacity.second.text.placed;
            }
        }
    }

    for (auto& prevOffset : getPrevPlacement()->variableOffsets) {
        const uint32_t crossTileID = prevOffset.first;
        auto foundOffset = variableOffsets.find(crossTileID);
        auto foundOpacity = opacities.find(crossTileID);
        if (foundOffset == variableOffsets.end() && foundOpacity != opacities.end() && !foundOpacity->second.isHidden()) {
            variableOffsets[prevOffset.first] = prevOffset.second;
        }
    }

    for (auto& prevOrientation : getPrevPlacement()->placedOrientations) {
        const uint32_t crossTileID = prevOrientation.first;
        auto foundOrientation = placedOrientations.find(crossTileID);
        auto foundOpacity = opacities.find(crossTileID);
        if (foundOrientation == placedOrientations.end() && foundOpacity != opacities.end() && !foundOpacity->second.isHidden()) {
            placedOrientations[prevOrientation.first] = prevOrientation.second;
        }
    }

    fadeStartTime = placementChanged ? commitTime : getPrevPlacement()->fadeStartTime;
}

void Placement::updateLayerBuckets(const RenderLayer& layer, const TransformState& state, bool updateOpacities) const {
    std::set<uint32_t> seenCrossTileIDs;
    for (const auto& item : layer.getPlacementData()) {
        item.bucket.get().updateVertices(*this, updateOpacities, state, item.tile, seenCrossTileIDs);
    }
}

namespace {
Point<float> calculateVariableRenderShift(style::SymbolAnchorType anchor, float width, float height, std::array<float, 2> textOffset, float textBoxScale, float renderTextSize) {
    AnchorAlignment alignment = AnchorAlignment::getAnchorAlignment(anchor);
    float shiftX = -(alignment.horizontalAlign - 0.5f) * width;
    float shiftY = -(alignment.verticalAlign - 0.5f) * height;
    auto variablOffset = SymbolLayout::evaluateVariableOffset(anchor, textOffset);
    return { (shiftX / textBoxScale + variablOffset[0]) * renderTextSize,
             (shiftY / textBoxScale + variablOffset[1]) * renderTextSize };
}
} // namespace

bool Placement::updateBucketDynamicVertices(SymbolBucket& bucket, const TransformState& state, const RenderTile& tile) const {
    using namespace style;
    const auto& layout = *bucket.layout;
    const bool alongLine = layout.get<SymbolPlacement>() != SymbolPlacementType::Point;
    const bool hasVariableAnchors = !layout.get<TextVariableAnchor>().empty() && bucket.hasTextData();
    const bool updateTextFitIcon = layout.get<IconTextFit>() != IconTextFitType::None && (bucket.allowVerticalPlacement || hasVariableAnchors) && (bucket.hasIconData() || bucket.hasSdfIconData());
    bool result = false;

    if (alongLine) {
        if (layout.get<IconRotationAlignment>() == AlignmentType::Map) {
            const bool pitchWithMap = layout.get<style::IconPitchAlignment>() == style::AlignmentType::Map;
            const bool keepUpright = layout.get<style::IconKeepUpright>();
            if (bucket.hasSdfIconData()) {
                reprojectLineLabels(bucket.sdfIcon.dynamicVertices, bucket.sdfIcon.placedSymbols,
                                    tile.matrix, pitchWithMap, true /*rotateWithMap*/, keepUpright,
                                    tile, *bucket.iconSizeBinder, state);
                result = true;
            }
            if (bucket.hasIconData()) {
                reprojectLineLabels(bucket.icon.dynamicVertices, bucket.icon.placedSymbols,
                                    tile.matrix, pitchWithMap, true /*rotateWithMap*/, keepUpright,
                                    tile, *bucket.iconSizeBinder, state);
                result = true;
            }
        }

        if (bucket.hasTextData() && layout.get<TextRotationAlignment>() == AlignmentType::Map) {
            const bool pitchWithMap = layout.get<style::TextPitchAlignment>() == style::AlignmentType::Map;
            const bool keepUpright = layout.get<style::TextKeepUpright>();
            reprojectLineLabels(bucket.text.dynamicVertices, bucket.text.placedSymbols,
                tile.matrix, pitchWithMap, true /*rotateWithMap*/, keepUpright,
                tile, *bucket.textSizeBinder, state);
            result = true;
        }
    } else if (hasVariableAnchors) {
        bucket.text.dynamicVertices.clear();
        bucket.hasVariablePlacement = false;

        const auto partiallyEvaluatedSize = bucket.textSizeBinder->evaluateForZoom(state.getZoom());
        const float tileScale = std::pow(2, state.getZoom() - tile.getOverscaledTileID().overscaledZ);
        const bool rotateWithMap = layout.get<TextRotationAlignment>() == AlignmentType::Map;
        const bool pitchWithMap = layout.get<TextPitchAlignment>() == AlignmentType::Map;
        const float pixelsToTileUnits = tile.id.pixelsToTileUnits(1.0, state.getZoom());
        const auto labelPlaneMatrix = getLabelPlaneMatrix(tile.matrix, pitchWithMap, rotateWithMap, state, pixelsToTileUnits);
        std::unordered_map<std::size_t, std::pair<std::size_t, Point<float>>> placedTextShifts;

        for (std::size_t i = 0; i < bucket.text.placedSymbols.size(); ++i) {
            const PlacedSymbol& symbol = bucket.text.placedSymbols[i];
            optional<VariableOffset> variableOffset;
            const bool skipOrientation = bucket.allowVerticalPlacement && !symbol.placedOrientation;
            if (!symbol.hidden && symbol.crossTileID != 0u && !skipOrientation) {
                auto it = variableOffsets.find(symbol.crossTileID);
                if (it != variableOffsets.end()) {
                    bucket.hasVariablePlacement = true;
                    variableOffset = it->second;
                }
            }

            if (!variableOffset) {
                // These symbols are from a justification that is not being used, or a label that wasn't placed
                // so we don't need to do the extra math to figure out what incremental shift to apply.
                hideGlyphs(symbol.glyphOffsets.size(), bucket.text.dynamicVertices);
            } else {
                const Point<float> tileAnchor = symbol.anchorPoint;
                const auto projectedAnchor = project(tileAnchor, pitchWithMap ? tile.matrix : labelPlaneMatrix);
                const float perspectiveRatio = 0.5f + 0.5f * (state.getCameraToCenterDistance() / projectedAnchor.second);
                float renderTextSize = evaluateSizeForFeature(partiallyEvaluatedSize, symbol) * perspectiveRatio / util::ONE_EM;
                if (pitchWithMap) {
                    // Go from size in pixels to equivalent size in tile units
                    renderTextSize *= bucket.tilePixelRatio / tileScale;
                }

                auto shift = calculateVariableRenderShift(
                        (*variableOffset).anchor,
                        (*variableOffset).width,
                        (*variableOffset).height,
                        (*variableOffset).offset,
                        (*variableOffset).textBoxScale,
                        renderTextSize);

                // Usual case is that we take the projected anchor and add the pixel-based shift
                // calculated above. In the (somewhat weird) case of pitch-aligned text, we add an equivalent
                // tile-unit based shift to the anchor before projecting to the label plane.
                Point<float> shiftedAnchor;
                if (pitchWithMap) {
                    shiftedAnchor = project(Point<float>(tileAnchor.x + shift.x, tileAnchor.y + shift.y),
                                            labelPlaneMatrix).first;
                } else if (rotateWithMap) {
                    auto rotated = util::rotate(shift, -state.getPitch());
                    shiftedAnchor = Point<float>(projectedAnchor.first.x + rotated.x,
                                                projectedAnchor.first.y + rotated.y);
                } else {
                    shiftedAnchor = Point<float>(projectedAnchor.first.x + shift.x,
                                                 projectedAnchor.first.y + shift.y);
                }

                if (updateTextFitIcon && symbol.placedIconIndex) {
                    placedTextShifts.emplace(*symbol.placedIconIndex,
                                             std::pair<std::size_t, Point<float>>{i, shiftedAnchor});
                }

                for (std::size_t j = 0; j < symbol.glyphOffsets.size(); ++j) {
                    addDynamicAttributes(shiftedAnchor, symbol.angle, bucket.text.dynamicVertices);
                }
            }
        }

        if (updateTextFitIcon && bucket.hasVariablePlacement) {
            auto updateIcon = [&](SymbolBucket::Buffer& iconBuffer) {
                iconBuffer.dynamicVertices.clear();
                for (std::size_t i = 0; i < iconBuffer.placedSymbols.size(); ++i) {
                    const PlacedSymbol& placedIcon = iconBuffer.placedSymbols[i];
                    if (placedIcon.hidden || (!placedIcon.placedOrientation && bucket.allowVerticalPlacement)) {
                        hideGlyphs(placedIcon.glyphOffsets.size(), iconBuffer.dynamicVertices);
                    } else {
                        const auto& pair = placedTextShifts.find(i);
                        if (pair == placedTextShifts.end()) {
                            hideGlyphs(placedIcon.glyphOffsets.size(), iconBuffer.dynamicVertices);
                        } else {
                            for (std::size_t j = 0; j < placedIcon.glyphOffsets.size(); ++j) {
                                addDynamicAttributes(pair->second.second, placedIcon.angle, iconBuffer.dynamicVertices);
                            }
                        }
                    }
                }
            };
            updateIcon(bucket.icon);
            updateIcon(bucket.sdfIcon);
        }

        result = true;
    } else if (bucket.allowVerticalPlacement && bucket.hasTextData()) {
        const auto updateDynamicVertices = [](SymbolBucket::Buffer& buffer) {
            buffer.dynamicVertices.clear();
            for (const PlacedSymbol& symbol : buffer.placedSymbols) {
                if (symbol.hidden || !symbol.placedOrientation) {
                    hideGlyphs(symbol.glyphOffsets.size(), buffer.dynamicVertices);
                } else {
                    for (std::size_t j = 0; j < symbol.glyphOffsets.size(); ++j) {
                        addDynamicAttributes(symbol.anchorPoint, symbol.angle, buffer.dynamicVertices);
                    }
                }
            }
        };

        updateDynamicVertices(bucket.text);
        // When text box is rotated, icon-text-fit icon must be rotated as well.
        if (updateTextFitIcon) {
            updateDynamicVertices(bucket.icon);
            updateDynamicVertices(bucket.sdfIcon);
        }

        result = true;
    }

    return result;
}

void Placement::updateBucketOpacities(SymbolBucket& bucket,
                                      const TransformState& state,
                                      std::set<uint32_t>& seenCrossTileIDs) const {
    if (bucket.hasTextData()) bucket.text.opacityVertices.clear();
    if (bucket.hasIconData()) bucket.icon.opacityVertices.clear();
    if (bucket.hasSdfIconData()) bucket.sdfIcon.opacityVertices.clear();
    if (bucket.hasIconCollisionBoxData()) bucket.iconCollisionBox->dynamicVertices.clear();
    if (bucket.hasIconCollisionCircleData()) bucket.iconCollisionCircle->dynamicVertices.clear();
    if (bucket.hasTextCollisionBoxData()) bucket.textCollisionBox->dynamicVertices.clear();
    if (bucket.hasTextCollisionCircleData()) bucket.textCollisionCircle->dynamicVertices.clear();

    const JointOpacityState duplicateOpacityState(false, false, true);

    const bool textAllowOverlap = bucket.layout->get<style::TextAllowOverlap>();
    const bool iconAllowOverlap = bucket.layout->get<style::IconAllowOverlap>();
    const bool variablePlacement = !bucket.layout->get<style::TextVariableAnchor>().empty();
    const bool rotateWithMap = bucket.layout->get<style::TextRotationAlignment>() == style::AlignmentType::Map;
    const bool pitchWithMap = bucket.layout->get<style::TextPitchAlignment>() == style::AlignmentType::Map;
    const bool hasIconTextFit = bucket.layout->get<style::IconTextFit>() != style::IconTextFitType::None;

    // If allow-overlap is true, we can show symbols before placement runs on them
    // But we have to wait for placement if we potentially depend on a paired icon/text
    // with allow-overlap: false.
    // See https://github.com/mapbox/mapbox-gl-native/issues/12483
    const JointOpacityState defaultOpacityState(
            textAllowOverlap && (iconAllowOverlap || !(bucket.hasIconData() || bucket.hasSdfIconData()) || bucket.layout->get<style::IconOptional>()),
            iconAllowOverlap && (textAllowOverlap || !bucket.hasTextData() || bucket.layout->get<style::TextOptional>()),
            true);

    for (SymbolInstance& symbolInstance : bucket.symbolInstances) {
        bool isDuplicate = seenCrossTileIDs.count(symbolInstance.crossTileID) > 0;

        auto it = opacities.find(symbolInstance.crossTileID);
        auto opacityState = defaultOpacityState;
        if (isDuplicate) {
            opacityState = duplicateOpacityState;
        } else if (it != opacities.end()) {
            opacityState = it->second;
        }

        seenCrossTileIDs.insert(symbolInstance.crossTileID);

        if (symbolInstance.hasText()) {
            size_t textOpacityVerticesSize = 0u;
            const auto& opacityVertex = SymbolSDFTextProgram::opacityVertex(opacityState.text.placed, opacityState.text.opacity);
            if (symbolInstance.placedRightTextIndex) {
                textOpacityVerticesSize += symbolInstance.rightJustifiedGlyphQuadsSize * 4;
                PlacedSymbol& placed = bucket.text.placedSymbols[*symbolInstance.placedRightTextIndex];
                placed.hidden = opacityState.isHidden();
            }
            if (symbolInstance.placedCenterTextIndex && !symbolInstance.singleLine) {
                textOpacityVerticesSize += symbolInstance.centerJustifiedGlyphQuadsSize * 4;
                PlacedSymbol& placed = bucket.text.placedSymbols[*symbolInstance.placedCenterTextIndex];
                placed.hidden = opacityState.isHidden();
            }
            if (symbolInstance.placedLeftTextIndex && !symbolInstance.singleLine) {
                textOpacityVerticesSize += symbolInstance.leftJustifiedGlyphQuadsSize * 4;
                PlacedSymbol& placed = bucket.text.placedSymbols[*symbolInstance.placedLeftTextIndex];
                placed.hidden = opacityState.isHidden();
            }
            if (symbolInstance.placedVerticalTextIndex) {
                textOpacityVerticesSize += symbolInstance.verticalGlyphQuadsSize * 4;
                bucket.text.placedSymbols[*symbolInstance.placedVerticalTextIndex].hidden = opacityState.isHidden();
            }

            bucket.text.opacityVertices.extend(textOpacityVerticesSize, opacityVertex);

            style::TextWritingModeType previousOrientation = style::TextWritingModeType::Horizontal;
            if (bucket.allowVerticalPlacement) {
                auto prevOrientation = placedOrientations.find(symbolInstance.crossTileID);
                if (prevOrientation != placedOrientations.end()) {
                    previousOrientation = prevOrientation->second;
                    markUsedOrientation(bucket, prevOrientation->second, symbolInstance);
                }
            }

            auto prevOffset = variableOffsets.find(symbolInstance.crossTileID);
            if (prevOffset != variableOffsets.end()) {
                markUsedJustification(bucket, prevOffset->second.anchor, symbolInstance, previousOrientation);
            }
        }
        if (symbolInstance.hasIcon()) {
            const auto& opacityVertex = SymbolIconProgram::opacityVertex(opacityState.icon.placed, opacityState.icon.opacity);
            auto& iconBuffer = symbolInstance.hasSdfIcon() ? bucket.sdfIcon : bucket.icon;
            
            if (symbolInstance.placedIconIndex) {
                iconBuffer.opacityVertices.extend(4, opacityVertex);
                iconBuffer.placedSymbols[*symbolInstance.placedIconIndex].hidden = opacityState.isHidden();
            }

            if (symbolInstance.placedVerticalIconIndex) {
                iconBuffer.opacityVertices.extend(4, opacityVertex);
                iconBuffer.placedSymbols[*symbolInstance.placedVerticalIconIndex].hidden = opacityState.isHidden();
            }
        }
        
        auto updateIconCollisionBox = [&](const auto& feature, const bool placed, const Point<float>& shift) {
            if (feature.alongLine) {
                return;
            }
            const auto& dynamicVertex = CollisionBoxProgram::dynamicVertex(placed, false, shift);
            bucket.iconCollisionBox->dynamicVertices.extend(feature.boxes.size() * 4, dynamicVertex);
        };

        auto updateTextCollisionBox = [this, &bucket, &symbolInstance, &state, variablePlacement, rotateWithMap, pitchWithMap](const auto& feature, const bool placed) {
            Point<float> shift{0.0f, 0.0f};
            if (feature.alongLine) {
                return shift;
            }
            bool used = true;
            if (variablePlacement) {
                auto foundOffset = variableOffsets.find(symbolInstance.crossTileID);
                if (foundOffset != variableOffsets.end()) {
                    const VariableOffset& variableOffset = foundOffset->second;
                    // This will show either the currently placed position or the last
                    // successfully placed position (so you can visualize what collision
                    // just made the symbol disappear, and the most likely place for the
                    // symbol to come back)
                    shift = calculateVariableLayoutOffset(variableOffset.anchor,
                                                          variableOffset.width,
                                                          variableOffset.height,
                                                          variableOffset.offset,
                                                          variableOffset.textBoxScale);
                    if (rotateWithMap) {
                        shift = util::rotate(shift, pitchWithMap ? state.getBearing() : -state.getBearing());
                    }
                } else {
                    // No offset -> this symbol hasn't been placed since coming on-screen
                    // No single box is particularly meaningful and all of them would be too noisy
                    // Use the center box just to show something's there, but mark it "not used"
                    used = false;
                }
            }
            const auto& dynamicVertex = CollisionBoxProgram::dynamicVertex(placed, !used, shift);
            bucket.textCollisionBox->dynamicVertices.extend(feature.boxes.size() * 4, dynamicVertex);
            return shift;
        };
        
        auto updateCollisionCircles = [&](const auto& feature, const bool placed, bool isText) {
            if (!feature.alongLine) {
                return;
            }
            auto circles = collisionCircles.find(&feature);
            if (circles != collisionCircles.end()) {
                for (const auto& circle : circles->second) {
                    const auto& dynamicVertex = CollisionBoxProgram::dynamicVertex(placed, !circle.isCircle(), {});
                    isText ? bucket.textCollisionCircle->dynamicVertices.extend(4, dynamicVertex):
                             bucket.iconCollisionCircle->dynamicVertices.extend(4, dynamicVertex);
                }
            } else {
                // This feature was not placed, because it was not loaded or from a fading tile. Apply default values.
                static const auto dynamicVertex = CollisionBoxProgram::dynamicVertex(placed, false /*not used*/, {});
                isText ? bucket.textCollisionCircle->dynamicVertices.extend(4 * feature.boxes.size(), dynamicVertex):
                         bucket.iconCollisionCircle->dynamicVertices.extend(4 * feature.boxes.size(), dynamicVertex);
            }
        };
        Point<float> textShift{0.0f, 0.0f};
        Point<float> verticalTextShift{0.0f, 0.0f};
        if (bucket.hasTextCollisionBoxData()) {
            textShift = updateTextCollisionBox(symbolInstance.textCollisionFeature, opacityState.text.placed);
            if (bucket.allowVerticalPlacement && symbolInstance.verticalTextCollisionFeature) {
                verticalTextShift = updateTextCollisionBox(*symbolInstance.verticalTextCollisionFeature, opacityState.text.placed);
            }
        }
        if (bucket.hasIconCollisionBoxData()) {
            updateIconCollisionBox(symbolInstance.iconCollisionFeature, opacityState.icon.placed, hasIconTextFit ? textShift : Point<float>{0.0f, 0.0f});
            if (bucket.allowVerticalPlacement && symbolInstance.verticalIconCollisionFeature) {
                updateIconCollisionBox(*symbolInstance.verticalIconCollisionFeature, opacityState.text.placed, hasIconTextFit ? verticalTextShift : Point<float>{0.0f, 0.0f});
            }
        }

        if (bucket.hasIconCollisionCircleData()) {
            updateCollisionCircles(symbolInstance.iconCollisionFeature, opacityState.icon.placed, false);
        }
        if (bucket.hasTextCollisionCircleData()) {
            updateCollisionCircles(symbolInstance.textCollisionFeature, opacityState.text.placed, true);
        }
    }

    bucket.sortFeatures(state.getBearing());
    auto retainedData = retainedQueryData.find(bucket.bucketInstanceId);
    if (retainedData != retainedQueryData.end()) {
        retainedData->second.featureSortOrder = bucket.featureSortOrder;
    }
}

namespace {
optional<size_t> justificationToIndex(style::TextJustifyType justify, const SymbolInstance& symbolInstance, style::TextWritingModeType orientation) {
    // Vertical symbol has just one justification, style::TextJustifyType::Left.
    if (orientation == style::TextWritingModeType::Vertical) {
        return symbolInstance.placedVerticalTextIndex;
    }

    switch(justify) {
        case style::TextJustifyType::Right: return symbolInstance.placedRightTextIndex;
        case style::TextJustifyType::Center: return symbolInstance.placedCenterTextIndex;
        case style::TextJustifyType::Left: return symbolInstance.placedLeftTextIndex;
        case style::TextJustifyType::Auto: break;
    }
    assert(false);
    return nullopt;
}

const style::TextJustifyType justifyTypes[] = {
    style::TextJustifyType::Right, style::TextJustifyType::Center, style::TextJustifyType::Left};

}  // namespace

void Placement::markUsedJustification(SymbolBucket& bucket,
                                      style::TextVariableAnchorType placedAnchor,
                                      const SymbolInstance& symbolInstance,
                                      style::TextWritingModeType orientation) const {
    style::TextJustifyType anchorJustify = getAnchorJustification(placedAnchor);
    assert(anchorJustify != style::TextJustifyType::Auto);
    const optional<size_t>& autoIndex = justificationToIndex(anchorJustify, symbolInstance, orientation);

    for (auto& justify : justifyTypes) {
        const optional<size_t> index = justificationToIndex(justify, symbolInstance, orientation);
        if (index) {
            assert(bucket.text.placedSymbols.size() > *index);
            if (autoIndex && *index != *autoIndex) {
                // There are multiple justifications and this one isn't it: shift offscreen
                bucket.text.placedSymbols.at(*index).crossTileID = 0u;
            } else {
                // Either this is the chosen justification or the justification is hardwired: use this one
                bucket.text.placedSymbols.at(*index).crossTileID = symbolInstance.crossTileID;
            }
        }
    }
}

void Placement::markUsedOrientation(SymbolBucket& bucket,
                                    style::TextWritingModeType orientation,
                                    const SymbolInstance& symbolInstance) const {
    auto horizontal = orientation == style::TextWritingModeType::Horizontal ?
                                     optional<style::TextWritingModeType>(orientation) : nullopt;
    auto vertical = orientation == style::TextWritingModeType::Vertical ?
                                     optional<style::TextWritingModeType>(orientation) : nullopt;

    if (symbolInstance.placedRightTextIndex) {
        bucket.text.placedSymbols.at(*symbolInstance.placedRightTextIndex).placedOrientation = horizontal;
    }

    if (symbolInstance.placedCenterTextIndex && !symbolInstance.singleLine) {
        bucket.text.placedSymbols.at(*symbolInstance.placedCenterTextIndex).placedOrientation = horizontal;
    }

    if (symbolInstance.placedLeftTextIndex && !symbolInstance.singleLine) {
        bucket.text.placedSymbols.at(*symbolInstance.placedLeftTextIndex).placedOrientation = horizontal;
    }

    if (symbolInstance.placedVerticalTextIndex) {
        bucket.text.placedSymbols.at(*symbolInstance.placedVerticalTextIndex).placedOrientation = vertical;
    }

    auto& iconBuffer = symbolInstance.hasSdfIcon() ? bucket.sdfIcon : bucket.icon;
    if (symbolInstance.placedIconIndex) {
        iconBuffer.placedSymbols.at(*symbolInstance.placedIconIndex).placedOrientation = horizontal;
    }

    if (symbolInstance.placedVerticalIconIndex) {
        iconBuffer.placedSymbols.at(*symbolInstance.placedVerticalIconIndex).placedOrientation = vertical;
    }
}

float Placement::symbolFadeChange(TimePoint now) const {
    if (transitionsEnabled() &&
        transitionOptions.duration.value_or(util::DEFAULT_TRANSITION_DURATION) > Milliseconds(0)) {
        return std::chrono::duration<float>(now - commitTime) / transitionOptions.duration.value_or(util::DEFAULT_TRANSITION_DURATION) + prevZoomAdjustment;
    }
    return 1.0;
}

float Placement::zoomAdjustment(const float zoom) const {
    // When zooming out labels can overlap each other quickly. This
    // adjustment is used to reduce the fade duration for symbols while zooming out quickly.
    // It is also used to reduce the interval between placement calculations. Reducing the
    // interval between placements means collisions are discovered and eliminated sooner.
    return std::max(0.0, (placementZoom - zoom) / 1.5);
}

Duration Placement::getUpdatePeriod(const float zoom) const {
    // Even if transitionOptions.duration is set to a value < 300ms, we still wait for this default transition duration
    // before attempting another placement operation.
    const auto fadeDuration = std::max(util::DEFAULT_TRANSITION_DURATION,
                                       transitionOptions.duration.value_or(util::DEFAULT_TRANSITION_DURATION));
    return std::chrono::duration_cast<Duration>(fadeDuration * (1.0 - zoomAdjustment(zoom)));
}

bool Placement::transitionsEnabled() const {
    return mapMode == MapMode::Continuous && transitionOptions.enablePlacementTransitions;
}

bool Placement::hasTransitions(TimePoint now) const {
    assert(transitionsEnabled());
    return std::chrono::duration<float>(now - fadeStartTime) <
           transitionOptions.duration.value_or(util::DEFAULT_TRANSITION_DURATION);
}

const CollisionIndex& Placement::getCollisionIndex() const {
    return collisionIndex;
}
    
const RetainedQueryData& Placement::getQueryData(uint32_t bucketInstanceId) const {
    auto it = retainedQueryData.find(bucketInstanceId);
    if (it == retainedQueryData.end()) {
        throw std::runtime_error("Placement::getQueryData with unrecognized bucketInstanceId");
    }
    return it->second;
}

} // namespace mbgl
