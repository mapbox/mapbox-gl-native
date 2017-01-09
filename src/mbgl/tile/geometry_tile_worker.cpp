#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/style/bucket_parameters.hpp>
#include <mbgl/style/group_by_layout.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/renderer/symbol_bucket.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/exception.hpp>

#include <unordered_set>

namespace mbgl {

using namespace style;

GeometryTileWorker::GeometryTileWorker(ActorRef<GeometryTileWorker> self_,
                                       ActorRef<GeometryTile> parent_,
                                       OverscaledTileID id_,
                                       GlyphAtlas& glyphAtlas_,
                                       const std::atomic<bool>& obsolete_,
                                       const MapMode mode_)
    : self(std::move(self_)),
      parent(std::move(parent_)),
      id(std::move(id_)),
      glyphAtlas(glyphAtlas_),
      obsolete(obsolete_),
      mode(mode_) {
}

GeometryTileWorker::~GeometryTileWorker() {
    glyphAtlas.removeGlyphs(reinterpret_cast<uintptr_t>(this));
}

/*
   GeometryTileWorker is a state machine. This is its transition diagram.
   States are indicated by [state], lines are transitions triggered by
   messages, (parentheses) are actions taken on transition.

                              [idle] <----------------------------.
                                 |                                |
      set{Data,Layers,Placement}, symbolDependenciesChanged       |
                                 |                                |
           (do layout/placement; self-send "coalesced")           |
                                 v                                |
                           [coalescing] --- coalesced ------------.
                               |   |
             .-----------------.   .---------------.
             |                                     |
   .--- set{Data,Layers}                      setPlacement -----.
   |         |                                     |            |
   |         v                                     v            |
   .-- [need layout] <-- set{Data,Layers} -- [need placement] --.
             |                                     |
         coalesced                             coalesced
             |                                     |
             v                                     v
    (do layout or placement; self-send "coalesced"; goto [coalescing])

   The idea is that in the [idle] state, layout or placement happens immediately
   in response to a "set" message. During this processing, multiple "set" messages
   might get queued in the mailbox. At the end of processing, we self-send "coalesced",
   read all the queued messages until we get to "coalesced", and then redo either
   layout or placement if there were one or more "set"s (with layout taking priority,
   since it will trigger placement when complete), or return to the [idle] state if not.
*/

void GeometryTileWorker::setData(std::unique_ptr<const GeometryTileData> data_, uint64_t correlationID_) {
    try {
        data = std::move(data_);
        correlationID = correlationID_;

        switch (state) {
        case Idle:
            redoLayout();
            coalesce();
            break;

        case Coalescing:
        case NeedLayout:
        case NeedPlacement:
            state = NeedLayout;
            break;
        }
    } catch (...) {
        parent.invoke(&GeometryTile::onError, std::current_exception());
    }
}

void GeometryTileWorker::setLayers(std::vector<std::unique_ptr<Layer>> layers_, uint64_t correlationID_) {
    try {
        layers = std::move(layers_);
        correlationID = correlationID_;

        switch (state) {
        case Idle:
            redoLayout();
            coalesce();
            break;

        case Coalescing:
        case NeedPlacement:
            state = NeedLayout;
            break;

        case NeedLayout:
            break;
        }
    } catch (...) {
        parent.invoke(&GeometryTile::onError, std::current_exception());
    }
}

void GeometryTileWorker::setPlacementConfig(PlacementConfig placementConfig_, uint64_t correlationID_) {
    try {
        placementConfig = std::move(placementConfig_);
        correlationID = correlationID_;

        switch (state) {
        case Idle:
            attemptPlacement();
            coalesce();
            break;

        case Coalescing:
            state = NeedPlacement;
            break;

        case NeedPlacement:
        case NeedLayout:
            break;
        }
    } catch (...) {
        parent.invoke(&GeometryTile::onError, std::current_exception());
    }
}

void GeometryTileWorker::symbolDependenciesChanged() {
    try {
        switch (state) {
        case Idle:
            if (hasPendingSymbolDependencies()) {
                attemptPlacement();
                coalesce();
            }
            break;

        case Coalescing:
            if (hasPendingSymbolDependencies()) {
                state = NeedPlacement;
            }
            break;

        case NeedPlacement:
        case NeedLayout:
            break;
        }
    } catch (...) {
        parent.invoke(&GeometryTile::onError, std::current_exception());
    }
}

void GeometryTileWorker::coalesced() {
    try {
        switch (state) {
        case Idle:
            assert(false);
            break;

        case Coalescing:
            state = Idle;
            break;

        case NeedLayout:
            redoLayout();
            coalesce();
            break;

        case NeedPlacement:
            attemptPlacement();
            coalesce();
            break;
        }
    } catch (...) {
        parent.invoke(&GeometryTile::onError, std::current_exception());
    }
}

void GeometryTileWorker::coalesce() {
    state = Coalescing;
    self.invoke(&GeometryTileWorker::coalesced);
}

void GeometryTileWorker::redoLayout() {
    if (!data || !layers) {
        return;
    }

    std::vector<std::string> symbolOrder;
    for (auto it = layers->rbegin(); it != layers->rend(); it++) {
        if ((*it)->is<SymbolLayer>()) {
            symbolOrder.push_back((*it)->getID());
        }
    }

    std::unordered_map<std::string, std::unique_ptr<SymbolLayout>> symbolLayoutMap;
    std::unordered_map<std::string, std::shared_ptr<Bucket>> buckets;
    auto featureIndex = std::make_unique<FeatureIndex>();
    BucketParameters parameters { id, obsolete, *featureIndex, mode };

    std::vector<std::vector<const Layer*>> groups = groupByLayout(*layers);
    for (auto& group : groups) {
        if (obsolete) {
            return;
        }

        if (!*data) {
            continue; // Tile has no data.
        }

        const Layer& leader = *group.at(0);

        auto geometryLayer = (*data)->getLayer(leader.baseImpl->sourceLayer);
        if (!geometryLayer) {
            continue;
        }

        std::vector<std::string> layerIDs;
        for (const auto& layer : group) {
            layerIDs.push_back(layer->getID());
        }

        featureIndex->setBucketLayerIDs(leader.getID(), layerIDs);

        if (leader.is<SymbolLayer>()) {
            symbolLayoutMap.emplace(leader.getID(),
                leader.as<SymbolLayer>()->impl->createLayout(parameters, *geometryLayer, layerIDs));
        } else {
            std::shared_ptr<Bucket> bucket = leader.baseImpl->createBucket(parameters, *geometryLayer);
            if (!bucket->hasData()) {
                continue;
            }
            for (const auto& layer : group) {
                buckets.emplace(layer->getID(), bucket);
            }
        }
    }

    symbolLayouts.clear();
    for (const auto& symbolLayerID : symbolOrder) {
        auto it = symbolLayoutMap.find(symbolLayerID);
        if (it != symbolLayoutMap.end()) {
            symbolLayouts.push_back(std::move(it->second));
        }
    }

    parent.invoke(&GeometryTile::onLayout, GeometryTile::LayoutResult {
        std::move(buckets),
        std::move(featureIndex),
        *data ? (*data)->clone() : nullptr,
        correlationID
    });

    attemptPlacement();
}

bool GeometryTileWorker::hasPendingSymbolDependencies() const {
    bool result = false;

    for (const auto& symbolLayout : symbolLayouts) {
        if (symbolLayout->state == SymbolLayout::Pending) {
            result = true;
        }
    }

    return result;
}

void GeometryTileWorker::attemptPlacement() {
    if (!data || !layers || !placementConfig) {
        return;
    }

    bool canPlace = true;

    // Prepare as many SymbolLayouts as possible.
    for (auto& symbolLayout : symbolLayouts) {
        if (obsolete) {
            return;
        }

        if (symbolLayout->state == SymbolLayout::Pending) {
            if (symbolLayout->canPrepare(glyphAtlas)) {
                symbolLayout->state = SymbolLayout::Prepared;
                symbolLayout->prepare(reinterpret_cast<uintptr_t>(this),
                                      glyphAtlas);
            } else {
                canPlace = false;
            }
        }
    }

    if (!canPlace) {
        return; // We'll be notified (via `setPlacementConfig`) when it's time to try again.
    }

    auto collisionTile = std::make_unique<CollisionTile>(*placementConfig);
    std::unordered_map<std::string, std::shared_ptr<Bucket>> buckets;

    for (auto& symbolLayout : symbolLayouts) {
        if (obsolete) {
            return;
        }

        symbolLayout->state = SymbolLayout::Placed;
        if (!symbolLayout->hasSymbolInstances()) {
            continue;
        }

        std::shared_ptr<Bucket> bucket = symbolLayout->place(*collisionTile);
        for (const auto& layerID : symbolLayout->layerIDs) {
            buckets.emplace(layerID, bucket);
        }
    }

    parent.invoke(&GeometryTile::onPlacement, GeometryTile::PlacementResult {
        std::move(buckets),
        std::move(collisionTile),
        correlationID
    });
}

} // namespace mbgl
