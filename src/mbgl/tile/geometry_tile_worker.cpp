#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/group_by_layout.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/stopwatch.hpp>

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
   GeometryTileWorker is a state machine. This is its transition diagram.
   States are indicated by [state], lines are transitions triggered by
   messages, (parentheses) are actions taken on transition.

                         [Idle] <-------------------------.
                            |                             |
        set{Data,Layers}, symbolDependenciesChanged,      |
                    setShowCollisionBoxes                 |
                            |                             |
   (do parse and/or symbol layout; self-send "coalesced") |
                            v                             |
                      [Coalescing] --- coalesced ---------.
                         |   |
             .-----------.   .---------------------.
             |                                     |
   .--- set{Data,Layers}                setShowCollisionBoxes,
   |         |                         symbolDependenciesChanged --.
   |         |                                     |               |
   |         v                                     v               |
   .-- [NeedsParse] <-- set{Data,Layers} -- [NeedsSymbolLayout] ---.
             |                                     |
         coalesced                             coalesced
             |                                     |
             v                                     v
   (do parse or symbol layout; self-send "coalesced"; goto [coalescing])

   The idea is that in the [idle] state, parsing happens immediately in response to
   a "set" message, and symbol layout happens once all symbol dependencies are met.
   During this processing, multiple "set" messages might get queued in the mailbox.
   At the end of processing, we self-send "coalesced", read all the queued messages
   until we get to "coalesced", and then re-parse if there were one or more "set"s or
   return to the [idle] state if not.
 
   One important goal of the design is to prevent starvation. Under heavy load new
   requests for tiles should not prevent in progress request from completing.
   It is nevertheless possible to restart an in-progress request:
 
    - [Idle] setData -> parse()
        sends getGlyphs, hasPendingSymbolDependencies() is true
        enters [Coalescing], sends coalesced
    - [Coalescing] coalesced -> [Idle]
    - [Idle] setData -> new parse(), interrupts old parse()
        sends getGlyphs, hasPendingSymbolDependencies() is true
        enters [Coalescing], sends coalesced
    - [Coalescing] onGlyphsAvailable -> [NeedsSymbolLayout]
           hasPendingSymbolDependencies() may or may not be true
    - [NeedsSymbolLayout] coalesced -> performSymbolLayout()
           Generates result depending on whether dependencies are met
           -> [Idle]
 
   In this situation, we are counting on the idea that even with rapid changes to
   the tile's data, the set of glyphs/images it requires will not keep growing without
   limit.
 
   Although parsing (which populates all non-symbol buckets and requests dependencies
   for symbol buckets) is internally separate from symbol layout, we only return
   results to the foreground when we have completed both steps. Because we _move_
   the result buckets to the foreground, it is necessary to re-generate all buckets from
   scratch for `setShowCollisionBoxes`, even though it only affects symbol layers.
 
   The GL JS equivalent (in worker_tile.js and vector_tile_worker_source.js)
   is somewhat simpler because it relies on getGlyphs/getImages calls that transfer
   an entire set of glyphs/images on every tile load, while the native logic
   maintains a local state that can be incrementally updated. Because each tile load
   call becomes self-contained, the equivalent of the coalescing logic is handled by
   'reloadTile' queueing a single extra 'reloadTile' callback to run after the next
   completed parse.
*/

void GeometryTileWorker::setData(std::unique_ptr<const GeometryTileData> data_, uint64_t correlationID_) {
    try {
        data = std::move(data_);
        correlationID = correlationID_;

        switch (state) {
        case Idle:
            parse();
            coalesce();
            break;

        case Coalescing:
        case NeedsParse:
        case NeedsSymbolLayout:
            state = NeedsParse;
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
            parse();
            coalesce();
            break;

        case Coalescing:
        case NeedsSymbolLayout:
            state = NeedsParse;
            break;

        case NeedsParse:
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
            if (!hasPendingParseResult()) {
                // Trigger parse if nothing is in flight, otherwise symbol layout will automatically
                // pick up the change
                parse();
                coalesce();
            }
            break;

        case Coalescing:
            state = NeedsSymbolLayout;
            break;

        case NeedsSymbolLayout:
        case NeedsParse:
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
                // symbolLayoutsNeedPreparation can only be set true by parsing
                // and the parse result can only be cleared by performSymbolLayout
                // which also clears symbolLayoutsNeedPreparation
                assert(hasPendingParseResult());
                performSymbolLayout();
                coalesce();
            }
            break;

        case Coalescing:
            if (symbolLayoutsNeedPreparation) {
                state = NeedsSymbolLayout;
            }
            break;

        case NeedsSymbolLayout:
        case NeedsParse:
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

        case NeedsParse:
            parse();
            coalesce();
            break;

        case NeedsSymbolLayout:
            // We may have entered NeedsSymbolLayout while coalescing
            // after a performSymbolLayout. In that case, we need to
            // start over with parsing in order to do another layout.
            hasPendingParseResult() ? performSymbolLayout() : parse();
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

void GeometryTileWorker::parse() {
    if (!data || !layers) {
        return;
    }

    MBGL_TIMING_START(watch)
    std::vector<std::string> symbolOrder;
    for (auto it = layers->rbegin(); it != layers->rend(); it++) {
        if ((*it)->type == LayerType::Symbol) {
            symbolOrder.push_back((*it)->id);
        }
    }

    std::unordered_map<std::string, std::unique_ptr<SymbolLayout>> symbolLayoutMap;
    buckets.clear();
    featureIndex = std::make_unique<FeatureIndex>(*data ? (*data)->clone() : nullptr);
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

                if (!filter(expression::EvaluationContext { static_cast<float>(this->id.overscaledZ), feature.get() }))
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

    MBGL_TIMING_FINISH(watch,
                       " Action: " << "Parsing," <<
                       " SourceID: " << sourceID.c_str() <<
                       " Canonical: " << static_cast<int>(id.canonical.z) << "/" << id.canonical.x << "/" << id.canonical.y <<
                       " Time");
    performSymbolLayout();
}

bool GeometryTileWorker::hasPendingSymbolDependencies() const {
    for (auto& glyphDependency : pendingGlyphDependencies) {
        if (!glyphDependency.second.empty()) {
            return true;
        }
    }
    return !pendingImageDependencies.empty();
}
    
bool GeometryTileWorker::hasPendingParseResult() const {
    return bool(featureIndex);
}

void GeometryTileWorker::performSymbolLayout() {
    if (!data || !layers || !hasPendingParseResult() || hasPendingSymbolDependencies()) {
        return;
    }
    
    MBGL_TIMING_START(watch)
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
                                  imageMap, imageAtlas.positions);
        }

        symbolLayoutsNeedPreparation = false;
    }

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
    
    MBGL_TIMING_FINISH(watch,
                       " Action: " << "SymbolLayout," <<
                       " SourceID: " << sourceID.c_str() <<
                       " Canonical: " << static_cast<int>(id.canonical.z) << "/" << id.canonical.x << "/" << id.canonical.y <<
                       " Time");
    parent.invoke(&GeometryTile::onLayout, GeometryTile::LayoutResult {
        std::move(buckets),
        std::move(featureIndex),
        std::move(glyphAtlasImage),
        std::move(iconAtlasImage)
    }, correlationID);
}

} // namespace mbgl
