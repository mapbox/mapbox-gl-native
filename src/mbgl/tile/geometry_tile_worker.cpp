#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/group_by_layout.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/renderer/render_symbol_layer.hpp>
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
                                       const std::atomic<bool>& obsolete_,
                                       const MapMode mode_)
    : self(std::move(self_)),
      parent(std::move(parent_)),
      id(std::move(id_)),
      obsolete(obsolete_),
      mode(mode_) {
}

GeometryTileWorker::~GeometryTileWorker() {
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
        parent.invoke(&GeometryTile::onError, std::current_exception(), correlationID);
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
        parent.invoke(&GeometryTile::onError, std::current_exception(), correlationID);
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
        parent.invoke(&GeometryTile::onError, std::current_exception(), correlationID);
    }
}

void GeometryTileWorker::symbolDependenciesChanged() {
    try {
        switch (state) {
        case Idle:
            if (hasPendingSymbolLayouts()) {
                attemptPlacement();
                coalesce();
            }
            break;

        case Coalescing:
            if (hasPendingSymbolLayouts()) {
                state = NeedPlacement;
            }
            break;

        case NeedPlacement:
        case NeedLayout:
            break;
        }
    } catch (...) {
        parent.invoke(&GeometryTile::onError, std::current_exception(), correlationID);
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
        parent.invoke(&GeometryTile::onError, std::current_exception(), correlationID);
    }
}

void GeometryTileWorker::coalesce() {
    state = Coalescing;
    self.invoke(&GeometryTileWorker::coalesced);
}

void GeometryTileWorker::onGlyphsAvailable(GlyphPositionMap newGlyphPositions) {
    for (auto& newFontGlyphs : newGlyphPositions) {
        const FontStack& fontStack = newFontGlyphs.first;
        GlyphPositions& newPositions = newFontGlyphs.second;

        GlyphPositions& positions = glyphPositions[fontStack];
        GlyphIDs& pendingGlyphIDs = pendingGlyphDependencies[fontStack];

        for (auto& newPosition : newPositions) {
            const GlyphID& glyphID = newPosition.first;
            optional<Glyph>& glyph = newPosition.second;

            if (pendingGlyphIDs.erase(glyphID)) {
                positions.emplace(glyphID, std::move(glyph));
            }
        }
    }
    symbolDependenciesChanged();
}

void GeometryTileWorker::onIconsAvailable(IconMap newIcons) {
    icons = std::move(newIcons);
    pendingIconDependencies.clear();
    symbolDependenciesChanged();
}

void GeometryTileWorker::requestNewGlyphs(const GlyphDependencies& glyphDependencies) {
    for (auto& fontDependencies : glyphDependencies) {
        auto fontGlyphs = glyphPositions.find(fontDependencies.first);
        for (auto glyphID : fontDependencies.second) {
            if (fontGlyphs == glyphPositions.end() || fontGlyphs->second.find(glyphID) == fontGlyphs->second.end()) {
                pendingGlyphDependencies[fontDependencies.first].insert(glyphID);
            }
        }
    }
    if (!pendingGlyphDependencies.empty()) {
        parent.invoke(&GeometryTile::getGlyphs, pendingGlyphDependencies);
    }
}

void GeometryTileWorker::requestNewIcons(const IconDependencies& iconDependencies) {
    pendingIconDependencies = iconDependencies;
    if (!pendingIconDependencies.empty()) {
        parent.invoke(&GeometryTile::getIcons, pendingIconDependencies);
    }
}

static std::vector<std::unique_ptr<RenderLayer>> toRenderLayers(const std::vector<std::unique_ptr<style::Layer>>& layers, float zoom) {
    std::vector<std::unique_ptr<RenderLayer>> renderLayers;
    renderLayers.reserve(layers.size());
    for (auto& layer : layers) {
        renderLayers.push_back(layer->baseImpl->createRenderLayer());

        renderLayers.back()->cascade(CascadeParameters {
            { ClassID::Default },
            Clock::time_point::max(),
            TransitionOptions()
        });

        renderLayers.back()->evaluate(PropertyEvaluationParameters {
            zoom
        });
    }
    return renderLayers;
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
    BucketParameters parameters { id, mode };

    GlyphDependencies glyphDependencies;
    IconDependencies iconDependencies;

    // Create render layers and group by layout
    std::vector<std::unique_ptr<RenderLayer>> renderLayers = toRenderLayers(*layers, id.overscaledZ);
    std::vector<std::vector<const RenderLayer*>> groups = groupByLayout(renderLayers);

    for (auto& group : groups) {
        if (obsolete) {
            return;
        }

        if (!*data) {
            continue; // Tile has no data.
        }

        const RenderLayer& leader = *group.at(0);

        auto geometryLayer = (*data)->getLayer(leader.baseImpl.sourceLayer);
        if (!geometryLayer) {
            continue;
        }

        std::vector<std::string> layerIDs;
        for (const auto& layer : group) {
            layerIDs.push_back(layer->getID());
        }

        featureIndex->setBucketLayerIDs(leader.getID(), layerIDs);

        if (leader.is<RenderSymbolLayer>()) {
            symbolLayoutMap.emplace(leader.getID(),
                leader.as<RenderSymbolLayer>()->createLayout(parameters, group, *geometryLayer, glyphDependencies, iconDependencies));
        } else {
            const Filter& filter = leader.baseImpl.filter;
            const std::string& sourceLayerID = leader.baseImpl.sourceLayer;
            std::shared_ptr<Bucket> bucket = leader.createBucket(parameters, group);

            for (std::size_t i = 0; !obsolete && i < geometryLayer->featureCount(); i++) {
                std::unique_ptr<GeometryTileFeature> feature = geometryLayer->getFeature(i);

                if (!filter(feature->getType(), feature->getID(), [&] (const auto& key) { return feature->getValue(key); }))
                    continue;

                GeometryCollection geometries = feature->getGeometries();
                bucket->addFeature(*feature, geometries);
                featureIndex->insert(geometries, i, sourceLayerID, leader.getID());
            }

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

    requestNewGlyphs(glyphDependencies);
    requestNewIcons(iconDependencies);

    parent.invoke(&GeometryTile::onLayout, GeometryTile::LayoutResult {
        std::move(buckets),
        std::move(featureIndex),
        *data ? (*data)->clone() : nullptr,
    }, correlationID);

    attemptPlacement();
}

bool GeometryTileWorker::hasPendingSymbolLayouts() const {
    for (const auto& symbolLayout : symbolLayouts) {
        if (symbolLayout->state == SymbolLayout::Pending) {
            return true;
        }
    }

    return false;
}

bool GeometryTileWorker::hasPendingSymbolDependencies() const {
    for (auto& glyphDependency : pendingGlyphDependencies) {
        if (!glyphDependency.second.empty()) {
            return true;
        }
    }
    return !pendingIconDependencies.empty();
}


void GeometryTileWorker::attemptPlacement() {
    if (!data || !layers || !placementConfig || hasPendingSymbolDependencies()) {
        return;
    }
    
    auto collisionTile = std::make_unique<CollisionTile>(*placementConfig);
    std::unordered_map<std::string, std::shared_ptr<Bucket>> buckets;

    for (auto& symbolLayout : symbolLayouts) {
        if (obsolete) {
            return;
        }

        if (symbolLayout->state == SymbolLayout::Pending) {
            symbolLayout->prepare(glyphPositions, icons);
            symbolLayout->state = SymbolLayout::Placed;
        }

        if (!symbolLayout->hasSymbolInstances()) {
            continue;
        }

        std::shared_ptr<Bucket> bucket = symbolLayout->place(*collisionTile);
        for (const auto& pair : symbolLayout->layerPaintProperties) {
            buckets.emplace(pair.first, bucket);
        }
    }

    parent.invoke(&GeometryTile::onPlacement, GeometryTile::PlacementResult {
        std::move(buckets),
        std::move(collisionTile),        
    }, correlationID);
}

} // namespace mbgl
