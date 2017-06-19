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
#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/renderer/cascade_parameters.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_item.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/render_background_layer.hpp>
#include <mbgl/renderer/render_circle_layer.hpp>
#include <mbgl/renderer/render_custom_layer.hpp>
#include <mbgl/renderer/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/render_fill_layer.hpp>
#include <mbgl/renderer/render_line_layer.hpp>
#include <mbgl/renderer/render_raster_layer.hpp>
#include <mbgl/renderer/render_symbol_layer.hpp>
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

struct QueueSourceReloadVisitor {
    UpdateBatch& updateBatch;

    // No need to reload sources for these types; their visibility can change but
    // they don't participate in layout.
    void operator()(CustomLayer&) {}
    void operator()(RasterLayer&) {}
    void operator()(BackgroundLayer&) {}

    template <class VectorLayer>
    void operator()(VectorLayer& layer) {
        updateBatch.sourceIDs.insert(layer.getSourceID());
    }
};

Style::Style(Scheduler& scheduler_, FileSource& fileSource_, float pixelRatio)
    : scheduler(scheduler_),
      fileSource(fileSource_),
      glyphAtlas(std::make_unique<GlyphAtlas>(Size{ 2048, 2048 }, fileSource)),
      spriteAtlas(std::make_unique<SpriteAtlas>(Size{ 1024, 1024 }, pixelRatio)),
      lineAtlas(std::make_unique<LineAtlas>(Size{ 256, 512 })),
      light(std::make_unique<Light>()),
      renderLight(std::make_unique<RenderLight>(light->impl)),
      observer(&nullObserver) {
    glyphAtlas->setObserver(this);
    spriteAtlas->setObserver(this);
    light->impl->setObserver(this);
}

Style::~Style() {
    for (const auto& source : sources) {
        source->baseImpl->setObserver(nullptr);
    }

    for (const auto& layer : layers) {
        if (CustomLayer* customLayer = layer->as<CustomLayer>()) {
            customLayer->impl->deinitialize();
        }
    }

    glyphAtlas->setObserver(nullptr);
    spriteAtlas->setObserver(nullptr);
    light->impl->setObserver(nullptr);
}

bool Style::addClass(const std::string& className) {
    if (hasClass(className)) return false;
    classes.push_back(className);
    return true;
}

bool Style::hasClass(const std::string& className) const {
    return std::find(classes.begin(), classes.end(), className) != classes.end();
}

bool Style::removeClass(const std::string& className) {
    const auto it = std::find(classes.begin(), classes.end(), className);
    if (it != classes.end()) {
        classes.erase(it);
        return true;
    }
    return false;
}

void Style::setClasses(const std::vector<std::string>& classNames) {
    classes = classNames;
}

std::vector<std::string> Style::getClasses() const {
    return classes;
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
    renderLayers.clear();
    classes.clear();
    transitionOptions = {};
    updateBatch = {};

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
    spriteAtlas->load(parser.spriteURL, scheduler, fileSource);

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

    source->baseImpl->setObserver(this);
    source->baseImpl->loadDescription(fileSource);

    std::unique_ptr<RenderSource> renderSource = source->baseImpl->createRenderSource();
    renderSource->setObserver(this);
    renderSources.emplace_back(std::move(renderSource));

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

    util::erase_if(renderSources, [&](const auto& source) {
        return source->baseImpl.id == id;
    });

    auto source = std::move(*it);
    source->baseImpl->setObserver(nullptr);
    sources.erase(it);
    updateBatch.sourceIDs.erase(id);

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

    if (CustomLayer* customLayer = layer->as<CustomLayer>()) {
        customLayer->impl->initialize();
    }

    layer->baseImpl->setObserver(this);
    layer->accept(QueueSourceReloadVisitor { updateBatch });

    auto added = layers.emplace(before ? findLayer(*before) : layers.end(), std::move(layer))->get();
    renderLayers.emplace(before ? findRenderLayer(*before) : renderLayers.end(), added->baseImpl->createRenderLayer());
    return std::move(added);
}

std::unique_ptr<Layer> Style::removeLayer(const std::string& id) {
    auto it = std::find_if(layers.begin(), layers.end(), [&](const auto& layer) {
        return layer->baseImpl->id == id;
    });

    if (it == layers.end())
        return nullptr;

    auto layer = std::move(*it);

    if (CustomLayer* customLayer = layer->as<CustomLayer>()) {
        customLayer->impl->deinitialize();
    }

    layer->baseImpl->setObserver(nullptr);
    layers.erase(it);
    removeRenderLayer(id);
    return layer;
}

std::vector<const RenderLayer*> Style::getRenderLayers() const {
    std::vector<const RenderLayer*> result;
    result.reserve(renderLayers.size());
    for (const auto& layer : renderLayers) {
        result.push_back(layer.get());
    }
    return result;
}

std::vector<RenderLayer*> Style::getRenderLayers() {
    std::vector<RenderLayer*> result;
    result.reserve(renderLayers.size());
    for (auto& layer : renderLayers) {
        result.push_back(layer.get());
    }
    return result;
}

std::vector<std::unique_ptr<RenderLayer>>::const_iterator Style::findRenderLayer(const std::string& id) const {
    return std::find_if(renderLayers.begin(), renderLayers.end(), [&](const auto& layer) {
        return layer->baseImpl.id == id;
    });
}

RenderLayer* Style::getRenderLayer(const std::string& id) const {
    auto it = findRenderLayer(id);
    return it != renderLayers.end() ? it->get() : nullptr;
}

void Style::removeRenderLayer(const std::string& id) {
    auto it = std::find_if(renderLayers.begin(), renderLayers.end(), [&](const auto& layer) {
        return layer->baseImpl.id == id;
    });

    if (it != renderLayers.end()) {
        renderLayers.erase(it);
    }
}

void Style::setLight(std::unique_ptr<Light> light_) {
    light = std::move(light_);
    light->impl->setObserver(this);

    // Copy renderlight to preserve the initialised
    // transitioning light properties
    renderLight = renderLight->copy(light->impl);

    onLightChanged(*light);
}

Light* Style::getLight() const {
    return light.get();
}

RenderLight* Style::getRenderLight() const {
    return renderLight.get();
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
    bool zoomChanged = zoomHistory.update(parameters.transformState.getZoom(), parameters.timePoint);

    std::vector<ClassID> classIDs;
    for (const auto& className : classes) {
        classIDs.push_back(ClassDictionary::Get().lookup(className));
    }
    classIDs.push_back(ClassID::Default);

    const CascadeParameters cascadeParameters {
        classIDs,
        parameters.timePoint,
        parameters.mode == MapMode::Continuous ? transitionOptions : TransitionOptions()
    };

    const PropertyEvaluationParameters evaluationParameters {
        zoomHistory,
        parameters.timePoint,
        parameters.mode == MapMode::Continuous ? util::DEFAULT_FADE_DURATION : Duration::zero()
    };

    const TileParameters tileParameters(parameters.pixelRatio,
                                        parameters.debugOptions,
                                        parameters.transformState,
                                        parameters.scheduler,
                                        parameters.fileSource,
                                        parameters.mode,
                                        parameters.annotationManager,
                                        *this);

    const bool cascade = parameters.updateFlags & Update::Classes;
    const bool evaluate = cascade || zoomChanged || parameters.updateFlags & Update::RecalculateStyle;

    if (cascade) {
        renderLight->transition(cascadeParameters);
    }

    if (evaluate || renderLight->hasTransition()) {
        renderLight->evaluate(evaluationParameters);
    }

    for (const auto& renderSource : renderSources) {
        renderSource->enabled = false;
    }

    for (const auto& layer : renderLayers) {
        if (cascade) {
            layer->cascade(cascadeParameters);
        }

        if (evaluate || layer->hasTransition()) {
            layer->evaluate(evaluationParameters);
        }

        if (layer->needsRendering(zoomHistory.lastZoom)) {
            if (RenderSource* renderSource = getRenderSource(layer->baseImpl.source)) {
                renderSource->enabled = true;
            }
        }
    }

    for (const auto& renderSource : renderSources) {
        bool updated = updateBatch.sourceIDs.count(renderSource->baseImpl.id);
        if (renderSource->enabled) {
            if (updated) {
                renderSource->reloadTiles();
            }
            renderSource->updateTiles(tileParameters);
        } else if (updated) {
            renderSource->invalidateTiles();
        } else {
            renderSource->removeTiles();
        }
    }

    updateBatch.sourceIDs.clear();
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
    const auto it = std::find_if(renderSources.begin(), renderSources.end(), [&](const auto& source) {
        return source->baseImpl.id == id;
    });

    return it != renderSources.end() ? it->get() : nullptr;
}

bool Style::hasTransitions() const {
    if (renderLight->hasTransition()) {
        return true;
    }

    for (const auto& layer : renderLayers) {
        if (layer->hasTransition()) {
            return true;
        }
    }

    return false;
}

bool Style::isLoaded() const {
    if (!loaded) {
        return false;
    }

    for (const auto& source: sources) {
        if (!source->baseImpl->loaded) {
            return false;
        }
    }

    for (const auto& renderSource: renderSources) {
        if (!renderSource->isLoaded()) {
            return false;
        }
    }

    if (!spriteAtlas->isLoaded()) {
        return false;
    }

    return true;
}

RenderData Style::getRenderData(MapDebugOptions debugOptions, float angle) const {
    RenderData result;

    for (const auto& renderSource: renderSources) {
        if (renderSource->enabled) {
            result.sources.insert(renderSource.get());
        }
    }

    for (const auto& layer : renderLayers) {
        if (!layer->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }

        if (const RenderBackgroundLayer* background = layer->as<RenderBackgroundLayer>()) {
            if (debugOptions & MapDebugOptions::Overdraw) {
                // We want to skip glClear optimization in overdraw mode.
                result.order.emplace_back(*layer);
                continue;
            }
            const BackgroundPaintProperties::Evaluated& paint = background->evaluated;
            if (layer.get() == renderLayers[0].get() && paint.get<BackgroundPattern>().from.empty()) {
                // This is a solid background. We can use glClear().
                result.backgroundColor = paint.get<BackgroundColor>() * paint.get<BackgroundOpacity>();
            } else {
                // This is a textured background, or not the bottommost layer. We need to render it with a quad.
                result.order.emplace_back(*layer);
            }
            continue;
        }

        if (layer->is<RenderCustomLayer>()) {
            result.order.emplace_back(*layer);
            continue;
        }

        RenderSource* source = getRenderSource(layer->baseImpl.source);
        if (!source) {
            Log::Warning(Event::Render, "can't find source for layer '%s'", layer->baseImpl.id.c_str());
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
        for (auto tileIt = sortedTiles.begin(); tileIt != sortedTiles.end(); ++tileIt) {
            auto& tile = tileIt->get();
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

            auto bucket = tile.tile.getBucket(*layer);
            if (bucket) {
                sortedTilesForInsertion.emplace_back(tile);
                tile.used = true;
            }
        }

        result.order.emplace_back(*layer, std::move(sortedTilesForInsertion));
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
                auto sourceResults = renderSource->queryRenderedFeatures(geometry, transformState, options);
                std::move(sourceResults.begin(), sourceResults.end(), std::inserter(resultsByLayer, resultsByLayer.begin()));
            }
        }
    } else {
        for (const auto& renderSource : renderSources) {
            auto sourceResults = renderSource->queryRenderedFeatures(geometry, transformState, options);
            std::move(sourceResults.begin(), sourceResults.end(), std::inserter(resultsByLayer, resultsByLayer.begin()));
        }
    }

    std::vector<Feature> result;

    if (resultsByLayer.empty()) {
        return result;
    }

    // Combine all results based on the style layer order.
    for (const auto& layer : renderLayers) {
        if (!layer->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }
        auto it = resultsByLayer.find(layer->baseImpl.id);
        if (it != resultsByLayer.end()) {
            std::move(it->second.begin(), it->second.end(), std::back_inserter(result));
        }
    }

    return result;
}

void Style::setSourceTileCacheSize(size_t size) {
    for (const auto& renderSource : renderSources) {
        renderSource->setCacheSize(size);
    }
}

void Style::onLowMemory() {
    for (const auto& renderSource : renderSources) {
        renderSource->onLowMemory();
    }
}

void Style::setObserver(style::Observer* observer_) {
    observer = observer_;
}

void Style::onGlyphsLoaded(const FontStack& fontStack, const GlyphRange& glyphRange) {
    observer->onGlyphsLoaded(fontStack, glyphRange);
}

void Style::onGlyphsError(const FontStack& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load glyph range %d-%d for font stack %s: %s",
               glyphRange.first, glyphRange.second, fontStackToString(fontStack).c_str(), util::toString(error).c_str());
    observer->onGlyphsError(fontStack, glyphRange, error);
    observer->onResourceError(error);
}

void Style::onSourceLoaded(Source& source) {
    observer->onSourceLoaded(source);
    observer->onUpdate(Update::Repaint);
}

void Style::onSourceChanged(Source& source) {
    observer->onSourceChanged(source);
    observer->onUpdate(Update::Repaint);
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
    if (!source.baseImpl->loaded) {
        source.baseImpl->loadDescription(fileSource);
    }
}

void Style::onTileChanged(RenderSource&, const OverscaledTileID&) {
    observer->onUpdate(Update::Repaint);
}

void Style::onTileError(RenderSource& source, const OverscaledTileID& tileID, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load tile %s for source %s: %s",
               util::toString(tileID).c_str(), source.baseImpl.id.c_str(), util::toString(error).c_str());
    observer->onResourceError(error);
}

void Style::onSpriteLoaded() {
    observer->onSpriteLoaded();
    observer->onUpdate(Update::Repaint); // For *-pattern properties.
}

void Style::onSpriteError(std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load sprite: %s", util::toString(error).c_str());
    observer->onSpriteError(error);
    observer->onResourceError(error);
}

void Style::onLayerFilterChanged(Layer& layer) {
    layer.accept(QueueSourceReloadVisitor { updateBatch });
    observer->onUpdate(Update::Repaint);
}

void Style::onLayerVisibilityChanged(Layer& layer) {
    layer.accept(QueueSourceReloadVisitor { updateBatch });
    observer->onUpdate(Update::RecalculateStyle);
}

void Style::onLayerPaintPropertyChanged(Layer&) {
    observer->onUpdate(Update::RecalculateStyle | Update::Classes);
}

void Style::onLayerDataDrivenPaintPropertyChanged(Layer& layer) {
    layer.accept(QueueSourceReloadVisitor { updateBatch });
    observer->onUpdate(Update::RecalculateStyle | Update::Classes);
}

void Style::onLayerLayoutPropertyChanged(Layer& layer, const char * property) {
    layer.accept(QueueSourceReloadVisitor { updateBatch });

    // Recalculate the style for certain properties
    observer->onUpdate((strcmp(property, "icon-size") == 0 || strcmp(property, "text-size") == 0)
        ? Update::RecalculateStyle
        : Update::Repaint);
}

void Style::onLightChanged(const Light&) {
    observer->onUpdate(Update::Classes | Update::RecalculateStyle);
}

void Style::dumpDebugLogs() const {
    for (const auto& source : sources) {
        source->baseImpl->dumpDebugLogs();
    }

    for (const auto& renderSource : renderSources) {
        renderSource->dumpDebugLogs();
    }

    spriteAtlas->dumpDebugLogs();
}

} // namespace style
} // namespace mbgl
