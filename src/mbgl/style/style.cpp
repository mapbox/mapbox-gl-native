#include <mbgl/style/style.hpp>
#include <mbgl/style/observer.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_loader.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/renderer/transition_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_item.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/style_diff.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/map/query.hpp>

#include <algorithm>

namespace mbgl {
namespace style {

static Observer nullObserver;

Style::Style(Scheduler& scheduler_, FileSource& fileSource_, float pixelRatio)
    : scheduler(scheduler_),
      fileSource(fileSource_),
      glyphAtlas(std::make_unique<GlyphAtlas>(Size{ 2048, 2048 }, fileSource)),
      spriteLoader(std::make_unique<SpriteLoader>(pixelRatio)),
      spriteAtlas(std::make_unique<SpriteAtlas>()),
      lineAtlas(std::make_unique<LineAtlas>(Size{ 256, 512 })),
      light(std::make_unique<Light>()),
      renderLight(light->impl),
      observer(&nullObserver) {
    glyphAtlas->setObserver(this);
    spriteLoader->setObserver(this);
    light->setObserver(this);
}

Style::~Style() {
    for (const auto& layer : layers) {
        if (auto* customLayer = layer->as<CustomLayer>()) {
            customLayer->impl().deinitialize();
        }
    }
}

void Style::setTransitionOptions(const TransitionOptions& options) {
    transitionOptions = options;
}

TransitionOptions Style::getTransitionOptions() const {
    return transitionOptions;
}

void Style::setJSON(const std::string& json) {
    sources.clear();
    renderSources.clear();
    layers.clear();
    transitionOptions = {};

    Parser parser;
    auto error = parser.parse(json);

    if (error) {
        std::string message = "Failed to parse style: " + util::toString(error);
        Log::Error(Event::ParseStyle, message.c_str());
        observer->onStyleError(std::make_exception_ptr(util::StyleParseException(message)));
        observer->onResourceError(error);
        return;
    }

    for (auto& source : parser.sources) {
        addSource(std::move(source));
    }

    for (auto& layer : parser.layers) {
        addLayer(std::move(layer));
    }

    name = parser.name;
    defaultLatLng = parser.latLng;
    defaultZoom = parser.zoom;
    defaultBearing = parser.bearing;
    defaultPitch = parser.pitch;
    setLight(std::make_unique<Light>(parser.light));

    glyphAtlas->setURL(parser.glyphURL);
    spriteLoader->load(parser.spriteURL, scheduler, fileSource);

    loaded = true;

    observer->onStyleLoaded();
}

void Style::addSource(std::unique_ptr<Source> source) {
    // Guard against duplicate source ids
    auto it = std::find_if(sources.begin(), sources.end(), [&](const auto& existing) {
        return existing->getID() == source->getID();
    });

    if (it != sources.end()) {
        std::string msg = "Source " + source->getID() + " already exists";
        throw std::runtime_error(msg.c_str());
    }

    source->setObserver(this);
    source->loadDescription(fileSource);

    sources.emplace_back(std::move(source));
}

struct SourceIdUsageEvaluator {
    const std::string& sourceId;

    bool operator()(BackgroundLayer&) { return false; }
    bool operator()(CustomLayer&) { return false; }

    template <class LayerType>
    bool operator()(LayerType& layer) {
        return layer.getSourceID() == sourceId;
    }
};

std::unique_ptr<Source> Style::removeSource(const std::string& id) {
    // Check if source is in use
    SourceIdUsageEvaluator sourceIdEvaluator {id};
    auto layerIt = std::find_if(layers.begin(), layers.end(), [&](const auto& layer) {
        return layer->accept(sourceIdEvaluator);
    });

    if (layerIt != layers.end()) {
        Log::Warning(Event::General, "Source '%s' is in use, cannot remove", id.c_str());
        return nullptr;
    }

    auto it = std::find_if(sources.begin(), sources.end(), [&](const auto& source) {
        return source->getID() == id;
    });

    if (it == sources.end()) {
        return nullptr;
    }

    auto source = std::move(*it);
    source->setObserver(nullptr);
    sources.erase(it);

    return source;
}

std::vector<const Layer*> Style::getLayers() const {
    std::vector<const Layer*> result;
    result.reserve(layers.size());
    for (const auto& layer : layers) {
        result.push_back(layer.get());
    }
    return result;
}

std::vector<Layer*> Style::getLayers() {
    std::vector<Layer*> result;
    result.reserve(layers.size());
    for (auto& layer : layers) {
        result.push_back(layer.get());
    }
    return result;
}

std::vector<std::unique_ptr<Layer>>::const_iterator Style::findLayer(const std::string& id) const {
    return std::find_if(layers.begin(), layers.end(), [&](const auto& layer) {
        return layer->baseImpl->id == id;
    });
}

Layer* Style::getLayer(const std::string& id) const {
    auto it = findLayer(id);
    return it != layers.end() ? it->get() : nullptr;
}

Layer* Style::addLayer(std::unique_ptr<Layer> layer, optional<std::string> before) {
    // TODO: verify source

    // Guard against duplicate layer ids
    auto it = std::find_if(layers.begin(), layers.end(), [&](const auto& existing) {
        return existing->getID() == layer->getID();
    });

    if (it != layers.end()) {
        throw std::runtime_error(std::string{"Layer "} + layer->getID() + " already exists");
    }

    if (auto* customLayer = layer->as<CustomLayer>()) {
        customLayer->impl().initialize();
    }

    layer->setObserver(this);

    return layers.emplace(before ? findLayer(*before) : layers.end(), std::move(layer))->get();
}

std::unique_ptr<Layer> Style::removeLayer(const std::string& id) {
    auto it = std::find_if(layers.begin(), layers.end(), [&](const auto& layer) {
        return layer->baseImpl->id == id;
    });

    if (it == layers.end())
        return nullptr;

    auto layer = std::move(*it);

    if (auto* customLayer = layer->as<CustomLayer>()) {
        customLayer->impl().deinitialize();
    }

    layer->setObserver(nullptr);
    layers.erase(it);
    return layer;
}

std::vector<const RenderLayer*> Style::getRenderLayers() const {
    std::vector<const RenderLayer*> result;
    result.reserve(renderLayers.size());
    for (const auto& entry : renderLayers) {
        result.push_back(entry.second.get());
    }
    return result;
}

std::vector<RenderLayer*> Style::getRenderLayers() {
    std::vector<RenderLayer*> result;
    result.reserve(renderLayers.size());
    for (auto& entry : renderLayers) {
        result.push_back(entry.second.get());
    }
    return result;
}

RenderLayer* Style::getRenderLayer(const std::string& id) const {
    auto it = renderLayers.find(id);
    return it != renderLayers.end() ? it->second.get() : nullptr;
}

void Style::setLight(std::unique_ptr<Light> light_) {
    light = std::move(light_);
    light->setObserver(this);
    onLightChanged(*light);
}

Light* Style::getLight() const {
    return light.get();
}

const RenderLight& Style::getRenderLight() const {
    return renderLight;
}

std::string Style::getName() const {
    return name;
}

LatLng Style::getDefaultLatLng() const {
    return defaultLatLng;
}

double Style::getDefaultZoom() const {
    return defaultZoom;
}

double Style::getDefaultBearing() const {
    return defaultBearing;
}

double Style::getDefaultPitch() const {
    return defaultPitch;
}

void Style::update(const UpdateParameters& parameters) {
    const bool zoomChanged = zoomHistory.update(parameters.transformState.getZoom(), parameters.timePoint);

    const TransitionParameters transitionParameters {
        parameters.mode == MapMode::Continuous ? parameters.timePoint : Clock::time_point::max(),
        parameters.mode == MapMode::Continuous ? transitionOptions : TransitionOptions()
    };

    const PropertyEvaluationParameters evaluationParameters {
        zoomHistory,
        parameters.mode == MapMode::Continuous ? parameters.timePoint : Clock::time_point::max(),
        parameters.mode == MapMode::Continuous ? util::DEFAULT_FADE_DURATION : Duration::zero()
    };

    const TileParameters tileParameters {
        parameters.pixelRatio,
        parameters.debugOptions,
        parameters.transformState,
        parameters.scheduler,
        parameters.fileSource,
        parameters.mode,
        parameters.annotationManager,
        *spriteAtlas,
        *glyphAtlas
    };

    // Update light.
    const bool lightChanged = renderLight.impl != light->impl;

    if (lightChanged) {
        renderLight.impl = light->impl;
        renderLight.transition(transitionParameters);
    }

    if (lightChanged || zoomChanged || renderLight.hasTransition()) {
        renderLight.evaluate(evaluationParameters);
    }


    std::vector<Immutable<Image::Impl>> newImageImpls;
    newImageImpls.reserve(images.size());
    for (const auto& entry : images) {
        newImageImpls.push_back(entry.second->impl);
    }

    const ImageDifference imageDiff = diffImages(imageImpls, newImageImpls);
    imageImpls = std::move(newImageImpls);

    // Remove removed images from sprite atlas.
    for (const auto& entry : imageDiff.removed) {
        spriteAtlas->removeImage(entry.first);
    }

    // Add added images to sprite atlas.
    for (const auto& entry : imageDiff.added) {
        spriteAtlas->addImage(entry.second);
    }

    // Update changed images.
    for (const auto& entry : imageDiff.changed) {
        spriteAtlas->updateImage(entry.second[1]);
    }

    if (spriteLoaded && !spriteAtlas->isLoaded()) {
        spriteAtlas->onSpriteLoaded();
    }


    std::vector<Immutable<Source::Impl>> newSourceImpls;
    newSourceImpls.reserve(sources.size());
    for (const auto& source : sources) {
        newSourceImpls.push_back(source->baseImpl);
    }


    std::vector<Immutable<Layer::Impl>> newLayerImpls;
    newLayerImpls.reserve(layers.size());
    for (const auto& layer : layers) {
        newLayerImpls.push_back(layer->baseImpl);
    }

    const LayerDifference layerDiff = diffLayers(layerImpls, newLayerImpls);
    layerImpls = std::move(newLayerImpls);

    // Remove render layers for removed layers.
    for (const auto& entry : layerDiff.removed) {
        renderLayers.erase(entry.first);
    }

    // Create render layers for newly added layers.
    for (const auto& entry : layerDiff.added) {
        renderLayers.emplace(entry.first, RenderLayer::create(entry.second));
    }

    // Update render layers for changed layers.
    for (const auto& entry : layerDiff.changed) {
        renderLayers.at(entry.first)->setImpl(entry.second[1]);
    }

    // Update layers for class and zoom changes.
    for (const auto& entry : renderLayers) {
        RenderLayer& layer = *entry.second;
        const bool layerAdded = layerDiff.added.count(entry.first);
        const bool layerChanged = layerDiff.changed.count(entry.first);

        if (layerAdded || layerChanged) {
            layer.transition(transitionParameters);
        }

        if (layerAdded || layerChanged || zoomChanged || layer.hasTransition()) {
            layer.evaluate(evaluationParameters);
        }
    }


    const SourceDifference sourceDiff = diffSources(sourceImpls, newSourceImpls);
    sourceImpls = std::move(newSourceImpls);

    // Remove render layers for removed sources.
    for (const auto& entry : sourceDiff.removed) {
        renderSources.erase(entry.first);
    }

    // Create render sources for newly added sources.
    for (const auto& entry : sourceDiff.added) {
        std::unique_ptr<RenderSource> renderSource = RenderSource::create(entry.second);
        renderSource->setObserver(this);
        renderSources.emplace(entry.first, std::move(renderSource));
    }

    // Update all sources.
    for (const auto& source : sourceImpls) {
        std::vector<Immutable<Layer::Impl>> filteredLayers;
        bool needsRendering = false;
        bool needsRelayout = false;

        for (const auto& layer : layerImpls) {
            if (layer->type == LayerType::Background ||
                layer->type == LayerType::Custom ||
                layer->source != source->id) {
                continue;
            }

            if (getRenderLayer(layer->id)->needsRendering(zoomHistory.lastZoom)) {
                needsRendering = true;
            }

            if (hasLayoutDifference(layerDiff, layer->id)) {
                needsRelayout = true;
            }

            filteredLayers.push_back(layer);
        }

        renderSources.at(source->id)->update(source,
                                             filteredLayers,
                                             needsRendering,
                                             needsRelayout,
                                             tileParameters);
    }
}

std::vector<const Source*> Style::getSources() const {
    std::vector<const Source*> result;
    result.reserve(sources.size());
    for (const auto& source : sources) {
        result.push_back(source.get());
    }
    return result;
}

std::vector<Source*> Style::getSources() {
    std::vector<Source*> result;
    result.reserve(sources.size());
    for (auto& source : sources) {
        result.push_back(source.get());
    }
    return result;
}

Source* Style::getSource(const std::string& id) const {
    const auto it = std::find_if(sources.begin(), sources.end(), [&](const auto& source) {
        return source->getID() == id;
    });

    return it != sources.end() ? it->get() : nullptr;
}

RenderSource* Style::getRenderSource(const std::string& id) const {
    auto it = renderSources.find(id);
    return it != renderSources.end() ? it->second.get() : nullptr;
}

bool Style::hasTransitions() const {
    if (renderLight.hasTransition()) {
        return true;
    }

    for (const auto& entry : renderLayers) {
        if (entry.second->hasTransition()) {
            return true;
        }
    }

    return false;
}

bool Style::isLoaded() const {
    if (!loaded) {
        return false;
    }

    if (!spriteLoaded) {
        return false;
    }

    for (const auto& source: sources) {
        if (!source->loaded) {
            return false;
        }
    }

    for (const auto& entry: renderSources) {
        if (!entry.second->isLoaded()) {
            return false;
        }
    }

    return true;
}

void Style::addImage(std::unique_ptr<style::Image> image) {
    std::string id = image->getID();
    auto it = images.find(id);
    if (it != images.end() && it->second->getImage().size != image->getImage().size) {
        Log::Warning(Event::Sprite, "Can't change sprite dimensions for '%s'", id.c_str());
        return;
    }
    images[id] = std::move(image);
}

void Style::removeImage(const std::string& id) {
    images.erase(id);
}

const style::Image* Style::getImage(const std::string& id) const {
    auto it = images.find(id);
    return it == images.end() ? nullptr : it->second.get();
}

RenderData Style::getRenderData(MapDebugOptions debugOptions, float angle) const {
    RenderData result;

    for (const auto& entry : renderSources) {
        if (entry.second->isEnabled()) {
            result.sources.insert(entry.second.get());
        }
    }

    for (auto& layerImpl : layerImpls) {
        RenderLayer* layer = getRenderLayer(layerImpl->id);
        assert(layer);

        if (!layer->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }

        if (const RenderBackgroundLayer* background = layer->as<RenderBackgroundLayer>()) {
            if (debugOptions & MapDebugOptions::Overdraw) {
                // We want to skip glClear optimization in overdraw mode.
                result.order.emplace_back(*layer, nullptr);
                continue;
            }
            const BackgroundPaintProperties::PossiblyEvaluated& paint = background->evaluated;
            if (layerImpl.get() == layerImpls[0].get() && paint.get<BackgroundPattern>().from.empty()) {
                // This is a solid background. We can use glClear().
                result.backgroundColor = paint.get<BackgroundColor>() * paint.get<BackgroundOpacity>();
            } else {
                // This is a textured background, or not the bottommost layer. We need to render it with a quad.
                result.order.emplace_back(*layer, nullptr);
            }
            continue;
        }

        if (layer->is<RenderCustomLayer>()) {
            result.order.emplace_back(*layer, nullptr);
            continue;
        }

        RenderSource* source = getRenderSource(layer->baseImpl->source);
        if (!source) {
            Log::Warning(Event::Render, "can't find source for layer '%s'", layer->getID().c_str());
            continue;
        }

        auto& renderTiles = source->getRenderTiles();
        const bool symbolLayer = layer->is<RenderSymbolLayer>();

        // Sort symbol tiles in opposite y position, so tiles with overlapping
        // symbols are drawn on top of each other, with lower symbols being
        // drawn on top of higher symbols.
        std::vector<std::reference_wrapper<RenderTile>> sortedTiles;
        std::transform(renderTiles.begin(), renderTiles.end(), std::back_inserter(sortedTiles),
                [](auto& pair) { return std::ref(pair.second); });
        if (symbolLayer) {
            std::sort(sortedTiles.begin(), sortedTiles.end(),
                      [angle](const RenderTile& a, const RenderTile& b) {
                Point<float> pa(a.id.canonical.x, a.id.canonical.y);
                Point<float> pb(b.id.canonical.x, b.id.canonical.y);

                auto par = util::rotate(pa, angle);
                auto pbr = util::rotate(pb, angle);

                return std::tie(par.y, par.x) < std::tie(pbr.y, pbr.x);
            });
        }

        std::vector<std::reference_wrapper<RenderTile>> sortedTilesForInsertion;
        for (auto& sortedTile : sortedTiles) {
            auto& tile = sortedTile.get();
            if (!tile.tile.isRenderable()) {
                continue;
            }

            // We're not clipping symbol layers, so when we have both parents and children of symbol
            // layers, we drop all children in favor of their parent to avoid duplicate labels.
            // See https://github.com/mapbox/mapbox-gl-native/issues/2482
            if (symbolLayer) {
                bool skip = false;
                // Look back through the buckets we decided to render to find out whether there is
                // already a bucket from this layer that is a parent of this tile. Tiles are ordered
                // by zoom level when we obtain them from getTiles().
                for (auto it = sortedTilesForInsertion.rbegin();
                     it != sortedTilesForInsertion.rend(); ++it) {
                    if (tile.tile.id.isChildOf(it->get().tile.id)) {
                        skip = true;
                        break;
                    }
                }
                if (skip) {
                    continue;
                }
            }

            auto bucket = tile.tile.getBucket(*layer->baseImpl);
            if (bucket) {
                sortedTilesForInsertion.emplace_back(tile);
                tile.used = true;
            }
        }
        layer->setRenderTiles(std::move(sortedTilesForInsertion));
        result.order.emplace_back(*layer, source);
    }

    return result;
}

std::vector<Feature> Style::queryRenderedFeatures(const ScreenLineString& geometry,
                                                  const TransformState& transformState,
                                                  const RenderedQueryOptions& options) const {
    std::unordered_map<std::string, std::vector<Feature>> resultsByLayer;

    if (options.layerIDs) {
        std::unordered_set<std::string> sourceIDs;
        for (const auto& layerID : *options.layerIDs) {
            if (Layer* layer = getLayer(layerID)) {
                sourceIDs.emplace(layer->baseImpl->source);
            }
        }
        for (const auto& sourceID : sourceIDs) {
            if (RenderSource* renderSource = getRenderSource(sourceID)) {
                auto sourceResults = renderSource->queryRenderedFeatures(geometry, transformState, *this, options);
                std::move(sourceResults.begin(), sourceResults.end(), std::inserter(resultsByLayer, resultsByLayer.begin()));
            }
        }
    } else {
        for (const auto& entry : renderSources) {
            auto sourceResults = entry.second->queryRenderedFeatures(geometry, transformState, *this, options);
            std::move(sourceResults.begin(), sourceResults.end(), std::inserter(resultsByLayer, resultsByLayer.begin()));
        }
    }

    std::vector<Feature> result;

    if (resultsByLayer.empty()) {
        return result;
    }

    // Combine all results based on the style layer order.
    for (const auto& layerImpl : layerImpls) {
        const RenderLayer* layer = getRenderLayer(layerImpl->id);
        if (!layer->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }
        auto it = resultsByLayer.find(layer->baseImpl->id);
        if (it != resultsByLayer.end()) {
            std::move(it->second.begin(), it->second.end(), std::back_inserter(result));
        }
    }

    return result;
}

void Style::setSourceTileCacheSize(size_t size) {
    for (const auto& entry : renderSources) {
        entry.second->setCacheSize(size);
    }
}

void Style::onLowMemory() {
    for (const auto& entry : renderSources) {
        entry.second->onLowMemory();
    }
}

void Style::setObserver(style::Observer* observer_) {
    observer = observer_;
}

void Style::onGlyphsError(const FontStack& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load glyph range %d-%d for font stack %s: %s",
               glyphRange.first, glyphRange.second, fontStackToString(fontStack).c_str(), util::toString(error).c_str());
    observer->onResourceError(error);
}

void Style::onSourceLoaded(Source& source) {
    observer->onSourceLoaded(source);
    observer->onUpdate(Update::Repaint);
}

void Style::onSourceChanged(Source& source) {
    observer->onSourceChanged(source);
}

void Style::onSourceError(Source& source, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load source %s: %s",
               source.getID().c_str(), util::toString(error).c_str());
    observer->onSourceError(source, error);
    observer->onResourceError(error);
}

void Style::onSourceDescriptionChanged(Source& source) {
    observer->onSourceDescriptionChanged(source);
    if (!source.loaded) {
        source.loadDescription(fileSource);
    }
}

void Style::onTileChanged(RenderSource&, const OverscaledTileID&) {
    observer->onUpdate(Update::Repaint);
}

void Style::onTileError(RenderSource& source, const OverscaledTileID& tileID, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load tile %s for source %s: %s",
               util::toString(tileID).c_str(), source.baseImpl->id.c_str(), util::toString(error).c_str());
    observer->onResourceError(error);
}

void Style::onSpriteLoaded(std::vector<std::unique_ptr<Image>>&& images_) {
    for (auto& image : images_) {
        addImage(std::move(image));
    }
    spriteLoaded = true;
    observer->onUpdate(Update::Repaint); // For *-pattern properties.
}

void Style::onSpriteError(std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load sprite: %s", util::toString(error).c_str());
    observer->onResourceError(error);
}

void Style::onLayerChanged(Layer&) {
    observer->onUpdate(Update::Repaint);
}

void Style::onLightChanged(const Light&) {
    observer->onUpdate(Update::Repaint);
}

void Style::dumpDebugLogs() const {
    for (const auto& source : sources) {
        source->dumpDebugLogs();
    }

    for (const auto& entry : renderSources) {
        entry.second->dumpDebugLogs();
    }

    spriteAtlas->dumpDebugLogs();
}

} // namespace style
} // namespace mbgl
