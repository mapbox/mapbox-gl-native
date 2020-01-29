#include <mbgl/tile/geometry_tile_worker.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/layermanager/layer_manager.hpp>
#include <mbgl/layout/layout.hpp>
#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/layout/pattern_layout.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/group_by_layout.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/stopwatch.hpp>

#include <unordered_set>
#include <utility>

namespace mbgl {

using namespace style;

GeometryTileWorker::GeometryTileWorker(ActorRef<GeometryTileWorker> self_,
                                       ActorRef<GeometryTile> parent_,
                                       OverscaledTileID id_,
                                       std::string sourceID_,
                                       const std::atomic<bool>& obsolete_,
                                       const MapMode mode_,
                                       const float pixelRatio_,
                                       const bool showCollisionBoxes_)
    : self(std::move(self_)),
      parent(std::move(parent_)),
      id(id_),
      sourceID(std::move(sourceID_)),
      obsolete(obsolete_),
      mode(mode_),
      pixelRatio(pixelRatio_),
      showCollisionBoxes(showCollisionBoxes_) {}

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
        sends getGlyphs, hasPendingDependencies() is true
        enters [Coalescing], sends coalesced
    - [Coalescing] coalesced -> [Idle]
    - [Idle] setData -> new parse(), interrupts old parse()
        sends getGlyphs, hasPendingDependencies() is true
        enters [Coalescing], sends coalesced
    - [Coalescing] onGlyphsAvailable -> [NeedsSymbolLayout]
           hasPendingDependencies() may or may not be true
    - [NeedsSymbolLayout] coalesced -> finalizeLayout()
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

void GeometryTileWorker::setData(std::unique_ptr<const GeometryTileData> data_,
                                 std::set<std::string> availableImages_,
                                 uint64_t correlationID_) {
    try {
        data = std::move(data_);
        correlationID = correlationID_;
        availableImages = std::move(availableImages_);

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

void GeometryTileWorker::setLayers(std::vector<Immutable<LayerProperties>> layers_,
                                   std::set<std::string> availableImages_,
                                   uint64_t correlationID_) {
    try {
        layers = std::move(layers_);
        correlationID = correlationID_;
        availableImages = std::move(availableImages_);

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

void GeometryTileWorker::reset(uint64_t correlationID_) {
    layers = nullopt;
    data = nullopt;
    correlationID = correlationID_;

    switch (state) {
        case Idle:
        case NeedsParse:
            break;
        case Coalescing:
        case NeedsSymbolLayout:
            state = NeedsParse;
            break;
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
            if (!layouts.empty()) {
                // Layouts are created only by parsing and the parse result can only be
                // cleared by performLayout, which also clears the layouts.
                assert(hasPendingParseResult());
                finalizeLayout();
                coalesce();
            }
            break;

        case Coalescing:
            if (!layouts.empty()) {
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
            // after a performLayout. In that case, we need to
            // start over with parsing in order to do another layout.
            hasPendingParseResult() ? finalizeLayout() : parse();
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
        FontStackHash fontStack = newFontGlyphs.first;
        Glyphs& newGlyphs = newFontGlyphs.second;

        Glyphs& glyphs = glyphMap[fontStack];
        for (auto& pendingGlyphDependency : pendingGlyphDependencies) {
            // Linear lookup here to handle reverse of FontStackHash -> FontStack,
            // since dependencies need the full font stack name to make a request
            // There should not be many fontstacks to look through
            if (FontStackHasher()(pendingGlyphDependency.first) == fontStack) {
                GlyphIDs& pendingGlyphIDs = pendingGlyphDependency.second;
                for (auto& newGlyph : newGlyphs) {
                    const GlyphID& glyphID = newGlyph.first;
                    optional<Immutable<Glyph>>& glyph = newGlyph.second;

                    if (pendingGlyphIDs.erase(glyphID)) {
                        glyphs.emplace(glyphID, std::move(glyph));
                    }
                }
            }
        }
    }
    symbolDependenciesChanged();
}

void GeometryTileWorker::onImagesAvailable(ImageMap newIconMap, ImageMap newPatternMap, ImageVersionMap newVersionMap, uint64_t imageCorrelationID_) {
    if (imageCorrelationID != imageCorrelationID_) {
        return; // Ignore outdated image request replies.
    }
    imageMap = std::move(newIconMap);
    patternMap = std::move(newPatternMap);
    versionMap = std::move(newVersionMap);
    pendingImageDependencies.clear();
    symbolDependenciesChanged();
}

void GeometryTileWorker::requestNewGlyphs(const GlyphDependencies& glyphDependencies) {
    for (auto& fontDependencies : glyphDependencies) {
        auto fontGlyphs = glyphMap.find(FontStackHasher()(fontDependencies.first));
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

void GeometryTileWorker::parse() {
    if (!data || !layers) {
        return;
    }

    MBGL_TIMING_START(watch)

    std::unordered_map<std::string, std::unique_ptr<SymbolLayout>> symbolLayoutMap;

    renderData.clear();
    layouts.clear();

    featureIndex = std::make_unique<FeatureIndex>(*data ? (*data)->clone() : nullptr);

    GlyphDependencies glyphDependencies;
    ImageDependencies imageDependencies;

    // Create render layers and group by layout
    std::unordered_map<std::string, std::vector<Immutable<style::LayerProperties>>> groupMap;
    for (auto layer : *layers) {
        groupMap[layoutKey(*layer->baseImpl)].push_back(std::move(layer));
    }

    for (auto& pair : groupMap) {
        const auto& group = pair.second;
        if (obsolete) {
            return;
        }

        if (!*data) {
            continue; // Tile has no data.
        }

        const style::Layer::Impl& leaderImpl = *(group.at(0)->baseImpl);
        BucketParameters parameters { id, mode, pixelRatio, leaderImpl.getTypeInfo() };

        auto geometryLayer = (*data)->getLayer(leaderImpl.sourceLayer);
        if (!geometryLayer) {
            continue;
        }

        std::vector<std::string> layerIDs(group.size());
        for (const auto& layer : group) {
            layerIDs.push_back(layer->baseImpl->id);
        }

        featureIndex->setBucketLayerIDs(leaderImpl.id, layerIDs);

        // Symbol layers and layers that support pattern properties have an extra step at layout time to figure out what images/glyphs
        // are needed to render the layer. They use the intermediate Layout data structure to accomplish this,
        // and either immediately create a bucket if no images/glyphs are used, or the Layout is stored until
        // the images/glyphs are available to add the features to the buckets.
        if (leaderImpl.getTypeInfo()->layout == LayerTypeInfo::Layout::Required) {
            std::unique_ptr<Layout> layout = LayerManager::get()->createLayout(
                {parameters, glyphDependencies, imageDependencies, availableImages}, std::move(geometryLayer), group);
            if (layout->hasDependencies()) {
                layouts.push_back(std::move(layout));
            } else {
                layout->createBucket({}, featureIndex, renderData, firstLoad, showCollisionBoxes, id.canonical);
            }
        } else {
            const Filter& filter = leaderImpl.filter;
            const std::string& sourceLayerID = leaderImpl.sourceLayer;
            std::shared_ptr<Bucket> bucket = LayerManager::get()->createBucket(parameters, group);

            for (std::size_t i = 0; !obsolete && i < geometryLayer->featureCount(); i++) {
                std::unique_ptr<GeometryTileFeature> feature = geometryLayer->getFeature(i);

                if (!filter(expression::EvaluationContext { static_cast<float>(this->id.overscaledZ), feature.get() }))
                    continue;

                const GeometryCollection& geometries = feature->getGeometries();
                bucket->addFeature(*feature, geometries, {}, PatternLayerMap(), i, id.canonical);
                featureIndex->insert(geometries, i, sourceLayerID, leaderImpl.id);
            }

            if (!bucket->hasData()) {
                continue;
            }

            for (const auto& layer : group) {
                renderData.emplace(layer->baseImpl->id, LayerRenderData{bucket, layer});
            }
        }
    }

    requestNewGlyphs(glyphDependencies);
    requestNewImages(imageDependencies);

    MBGL_TIMING_FINISH(watch,
                       " Action: " << "Parsing," <<
                       " SourceID: " << sourceID.c_str() <<
                       " Canonical: " << static_cast<int>(id.canonical.z) << "/" << id.canonical.x << "/" << id.canonical.y <<
                       " Time");
    finalizeLayout();
}

bool GeometryTileWorker::hasPendingDependencies() const {
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

void GeometryTileWorker::finalizeLayout() {
    if (!data || !layers || !hasPendingParseResult() || hasPendingDependencies()) {
        return;
    }
    
    MBGL_TIMING_START(watch)
    optional<AlphaImage> glyphAtlasImage;
    ImageAtlas iconAtlas = makeImageAtlas(imageMap, patternMap, versionMap);
    if (!layouts.empty()) {
        GlyphAtlas glyphAtlas = makeGlyphAtlas(glyphMap);
        glyphAtlasImage = std::move(glyphAtlas.image);

        for (auto& layout : layouts) {
            if (obsolete) {
                return;
            }

            layout->prepareSymbols(glyphMap, glyphAtlas.positions, imageMap, iconAtlas.iconPositions);

            if (!layout->hasSymbolInstances()) {
                continue;
            }

            // layout adds the bucket to buckets
            layout->createBucket(
                iconAtlas.patternPositions, featureIndex, renderData, firstLoad, showCollisionBoxes, id.canonical);
        }
    }

    layouts.clear();

    firstLoad = false;
    
    MBGL_TIMING_FINISH(watch,
                       " Action: " << "SymbolLayout," <<
                       " SourceID: " << sourceID.c_str() <<
                       " Canonical: " << static_cast<int>(id.canonical.z) << "/" << id.canonical.x << "/" << id.canonical.y <<
                       " Time");

    parent.invoke(&GeometryTile::onLayout, std::make_shared<GeometryTile::LayoutResult>(
        std::move(renderData),
        std::move(featureIndex),
        std::move(glyphAtlasImage),
        std::move(iconAtlas)
    ), correlationID);
}

} // namespace mbgl
