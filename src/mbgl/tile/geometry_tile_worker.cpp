#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/group_by_layout.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
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
                                       const std::string& sourceID_,
                                       const std::atomic<bool>& obsolete_,
                                       const MapMode mode_,
                                       const float pixelRatio_,
                                       const bool showCollisionBoxes_)
    : self(std::move(self_)),
      parent(std::move(parent_)),
      id(std::move(id_)),
      sourceID(sourceID_),
      obsolete(obsolete_),
      mode(mode_),
      pixelRatio(pixelRatio_),
      showCollisionBoxes(showCollisionBoxes_) {
}

GeometryTileWorker::~GeometryTileWorker() = default;

/*
   NOTE: The comments below are technically correct, but currently
   conceptually misleading. The change to foreground label placement
   means that:
   (1) "placement" here is a misnomer: the remaining role of
    "attemptPlacement" is symbol buffer generation
   (2) Once a tile has completed layout, we will only run
    "attemptPlacement" once
   (3) Tiles won't be rendered until "attemptPlacement" has run once
 
   TODO: Simplify GeometryTileWorker to fit its new role
    https://github.com/mapbox/mapbox-gl-native/issues/10457

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

void GeometryTileWorker::setLayers(std::vector<Immutable<Layer::Impl>> layers_, uint64_t correlationID_) {
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

void GeometryTileWorker::setShowCollisionBoxes(bool showCollisionBoxes_, uint64_t correlationID_) {
    try {
        showCollisionBoxes = showCollisionBoxes_;
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
            if (symbolLayoutsNeedPreparation) {
                attemptPlacement();
                coalesce();
            }
            break;

        case Coalescing:
            if (symbolLayoutsNeedPreparation) {
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

void GeometryTileWorker::onGlyphsAvailable(GlyphMap newGlyphMap) {
    for (auto& newFontGlyphs : newGlyphMap) {
        const FontStack& fontStack = newFontGlyphs.first;
        Glyphs& newGlyphs = newFontGlyphs.second;

        Glyphs& glyphs = glyphMap[fontStack];
        GlyphIDs& pendingGlyphIDs = pendingGlyphDependencies[fontStack];

        for (auto& newGlyph : newGlyphs) {
            const GlyphID& glyphID = newGlyph.first;
            optional<Immutable<Glyph>>& glyph = newGlyph.second;

            if (pendingGlyphIDs.erase(glyphID)) {
                glyphs.emplace(glyphID, std::move(glyph));
            }
        }
    }
    symbolDependenciesChanged();
}

void GeometryTileWorker::onImagesAvailable(ImageMap newImageMap, uint64_t imageCorrelationID_) {
    if (imageCorrelationID != imageCorrelationID_) {
        return; // Ignore outdated image request replies.
    }
    imageMap = std::move(newImageMap);
    pendingImageDependencies.clear();
    symbolDependenciesChanged();
}

void GeometryTileWorker::requestNewGlyphs(const GlyphDependencies& glyphDependencies) {
    for (auto& fontDependencies : glyphDependencies) {
        auto fontGlyphs = glyphMap.find(fontDependencies.first);
        for (auto glyphID : fontDependencies.second) {
            if (fontGlyphs == glyphMap.end() || fontGlyphs->second.find(glyphID) == fontGlyphs->second.end()) {
                pendingGlyphDependencies[fontDependencies.first].insert(glyphID);
            }
        }
    }
    if (!pendingGlyphDependencies.empty()) {
        parent.invoke(&GeometryTile::getGlyphs, pendingGlyphDependencies);
    }
}

void GeometryTileWorker::requestNewImages(const ImageDependencies& imageDependencies) {
    pendingImageDependencies = imageDependencies;
    if (!pendingImageDependencies.empty()) {
        parent.invoke(&GeometryTile::getImages, std::make_pair(pendingImageDependencies, ++imageCorrelationID));
    }
}

static std::vector<std::unique_ptr<RenderLayer>> toRenderLayers(const std::vector<Immutable<style::Layer::Impl>>& layers, float zoom) {
    std::vector<std::unique_ptr<RenderLayer>> renderLayers;
    renderLayers.reserve(layers.size());
    for (auto& layer : layers) {
        renderLayers.push_back(RenderLayer::create(layer));

        renderLayers.back()->transition(TransitionParameters {
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
        if ((*it)->type == LayerType::Symbol) {
            symbolOrder.push_back((*it)->id);
        }
    }

    std::unordered_map<std::string, std::unique_ptr<SymbolLayout>> symbolLayoutMap;
    std::unordered_map<std::string, std::shared_ptr<Bucket>> buckets;
    auto featureIndex = std::make_unique<FeatureIndex>();
    BucketParameters parameters { id, mode, pixelRatio };

    GlyphDependencies glyphDependencies;
    ImageDependencies imageDependencies;

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

        auto geometryLayer = (*data)->getLayer(leader.baseImpl->sourceLayer);
        if (!geometryLayer) {
            continue;
        }

        std::vector<std::string> layerIDs;
        for (const auto& layer : group) {
            layerIDs.push_back(layer->getID());
        }

        featureIndex->setBucketLayerIDs(leader.getID(), layerIDs);

        if (leader.is<RenderSymbolLayer>()) {
            auto layout = leader.as<RenderSymbolLayer>()->createLayout(
                parameters, group, std::move(geometryLayer), glyphDependencies, imageDependencies);
            symbolLayoutMap.emplace(leader.getID(), std::move(layout));
            symbolLayoutsNeedPreparation = true;
        } else {
            const Filter& filter = leader.baseImpl->filter;
            const std::string& sourceLayerID = leader.baseImpl->sourceLayer;
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
    requestNewImages(imageDependencies);

    parent.invoke(&GeometryTile::onLayout, GeometryTile::LayoutResult {
        std::move(buckets),
        std::move(featureIndex),
        *data ? (*data)->clone() : nullptr,
    }, correlationID);

    attemptPlacement();
}

bool GeometryTileWorker::hasPendingSymbolDependencies() const {
    for (auto& glyphDependency : pendingGlyphDependencies) {
        if (!glyphDependency.second.empty()) {
            return true;
        }
    }
    return !pendingImageDependencies.empty();
}

void GeometryTileWorker::attemptPlacement() {
    if (!data || !layers || hasPendingSymbolDependencies()) {
        return;
    }
    
    optional<AlphaImage> glyphAtlasImage;
    optional<PremultipliedImage> iconAtlasImage;

    if (symbolLayoutsNeedPreparation) {
        GlyphAtlas glyphAtlas = makeGlyphAtlas(glyphMap);
        ImageAtlas imageAtlas = makeImageAtlas(imageMap);

        glyphAtlasImage = std::move(glyphAtlas.image);
        iconAtlasImage = std::move(imageAtlas.image);

        for (auto& symbolLayout : symbolLayouts) {
            if (obsolete) {
                return;
            }

            symbolLayout->prepare(glyphMap, glyphAtlas.positions,
                                  imageMap, imageAtlas.positions,
                                  id, sourceID);
        }

        symbolLayoutsNeedPreparation = false;
    }

    std::unordered_map<std::string, std::shared_ptr<Bucket>> buckets;

    for (auto& symbolLayout : symbolLayouts) {
        if (obsolete) {
            return;
        }

        if (!symbolLayout->hasSymbolInstances()) {
            continue;
        }

        std::shared_ptr<SymbolBucket> bucket = symbolLayout->place(showCollisionBoxes);
        for (const auto& pair : symbolLayout->layerPaintProperties) {
            if (!firstLoad) {
                bucket->justReloaded = true;
            }
            buckets.emplace(pair.first, bucket);
        }
    }

    firstLoad = false;

    parent.invoke(&GeometryTile::onPlacement, GeometryTile::PlacementResult {
        std::move(buckets),
        std::move(glyphAtlasImage),
        std::move(iconAtlasImage),
    }, correlationID);
}

} // namespace mbgl
