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
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/style/cascade_parameters.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/renderer/render_item.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/map/query.hpp>

#include <algorithm>

namespace mbgl {
namespace style {

static Observer nullObserver;

Style::Style(FileSource& fileSource_, float pixelRatio)
    : fileSource(fileSource_),
      glyphAtlas(std::make_unique<GlyphAtlas>(Size{ 2048, 2048 }, fileSource)),
      spriteAtlas(std::make_unique<SpriteAtlas>(Size{ 1024, 1024 }, pixelRatio)),
      lineAtlas(std::make_unique<LineAtlas>(Size{ 256, 512 })),
      observer(&nullObserver) {
    glyphAtlas->setObserver(this);
    spriteAtlas->setObserver(this);
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
    layers.clear();
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

    glyphAtlas->setURL(parser.glyphURL);
    spriteAtlas->load(parser.spriteURL, fileSource);

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
    sources.emplace_back(std::move(source));
}

std::unique_ptr<Source> Style::removeSource(const std::string& id) {
    auto it = std::find_if(sources.begin(), sources.end(), [&](const auto& source) {
        return source->getID() == id;
    });

    if (it == sources.end()) {
        return nullptr;
    }

    auto source = std::move(*it);
    sources.erase(it);
    updateBatch.sourceIDs.erase(id);

    source->baseImpl->detach();
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

    if (SymbolLayer* symbolLayer = layer->as<SymbolLayer>()) {
        if (!symbolLayer->impl->spriteAtlas) {
            symbolLayer->impl->spriteAtlas = spriteAtlas.get();
        }
    }

    if (CustomLayer* customLayer = layer->as<CustomLayer>()) {
        customLayer->impl->initialize();
    }

    layer->baseImpl->setObserver(this);

    return layers.emplace(before ? findLayer(*before) : layers.end(), std::move(layer))->get();
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

    layers.erase(it);
    return layer;
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

void Style::updateTiles(const UpdateParameters& parameters) {
    for (const auto& source : sources) {
        if (source->baseImpl->enabled) {
            source->baseImpl->updateTiles(parameters);
        }
    }
}

void Style::updateSymbolDependentTiles() {
    for (const auto& source : sources) {
        source->baseImpl->updateSymbolDependentTiles();
    }
}

void Style::relayout() {
    for (const auto& sourceID : updateBatch.sourceIDs) {
        Source* source = getSource(sourceID);
        if (source && source->baseImpl->enabled) {
            source->baseImpl->reloadTiles();
        }
    }
    updateBatch.sourceIDs.clear();
}

void Style::cascade(const TimePoint& timePoint, MapMode mode) {
    // When in continuous mode, we can either have user- or style-defined
    // transitions. Still mode is always immediate.
    static const TransitionOptions immediateTransition {};

    std::vector<ClassID> classIDs;
    for (const auto& className : classes) {
        classIDs.push_back(ClassDictionary::Get().lookup(className));
    }
    classIDs.push_back(ClassID::Default);

    const CascadeParameters parameters {
        classIDs,
        mode == MapMode::Continuous ? timePoint : Clock::time_point::max(),
        mode == MapMode::Continuous ? transitionOptions : immediateTransition
    };

    for (const auto& layer : layers) {
        layer->baseImpl->cascade(parameters);
    }
}

void Style::recalculate(float z, const TimePoint& timePoint, MapMode mode) {
    // Disable all sources first. If we find an enabled layer that uses this source, we will
    // re-enable it later.
    for (const auto& source : sources) {
        source->baseImpl->enabled = false;
    }

    zoomHistory.update(z, timePoint);

    const PropertyEvaluationParameters parameters {
        z,
        mode == MapMode::Continuous ? timePoint : Clock::time_point::max(),
        zoomHistory,
        mode == MapMode::Continuous ? util::DEFAULT_FADE_DURATION : Duration::zero()
    };

    hasPendingTransitions = false;
    for (const auto& layer : layers) {
        const bool hasTransitions = layer->baseImpl->evaluate(parameters);

        // Disable this layer if it doesn't need to be rendered.
        const bool needsRendering = layer->baseImpl->needsRendering(zoomHistory.lastZoom);
        if (!needsRendering) {
            continue;
        }

        hasPendingTransitions |= hasTransitions;

        // If this layer has a source, make sure that it gets loaded.
        if (Source* source = getSource(layer->baseImpl->source)) {
            source->baseImpl->enabled = true;
            if (!source->baseImpl->loaded) {
                source->baseImpl->loadDescription(fileSource);
            }
        }
    }

    // Remove the existing tiles if we didn't end up re-enabling the source.
    for (const auto& source : sources) {
        if (!source->baseImpl->enabled) {
            source->baseImpl->removeTiles();
        }
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

bool Style::hasTransitions() const {
    return hasPendingTransitions;
}

bool Style::isLoaded() const {
    if (!loaded) {
        return false;
    }

    for (const auto& source: sources) {
        if (source->baseImpl->enabled && !source->baseImpl->isLoaded()) {
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

    for (const auto& source : sources) {
        if (source->baseImpl->enabled) {
            result.sources.insert(source.get());
        }
    }

    for (const auto& layer : layers) {
        if (!layer->baseImpl->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }

        if (const BackgroundLayer* background = layer->as<BackgroundLayer>()) {
            if (debugOptions & MapDebugOptions::Overdraw) {
                // We want to skip glClear optimization in overdraw mode.
                result.order.emplace_back(*layer);
                continue;
            }
            const BackgroundPaintProperties::Evaluated& paint = background->impl->paint.evaluated;
            if (layer.get() == layers[0].get() && paint.get<BackgroundPattern>().from.empty()) {
                // This is a solid background. We can use glClear().
                result.backgroundColor = paint.get<BackgroundColor>() * paint.get<BackgroundOpacity>();
            } else {
                // This is a textured background, or not the bottommost layer. We need to render it with a quad.
                result.order.emplace_back(*layer);
            }
            continue;
        }

        if (layer->is<CustomLayer>()) {
            result.order.emplace_back(*layer);
            continue;
        }

        Source* source = getSource(layer->baseImpl->source);
        if (!source) {
            Log::Warning(Event::Render, "can't find source for layer '%s'", layer->baseImpl->id.c_str());
            continue;
        }

        auto& renderTiles = source->baseImpl->getRenderTiles();
        const bool symbolLayer = layer->is<SymbolLayer>();

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

        for (auto& tileRef : sortedTiles) {
            auto& tile = tileRef.get();
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
                for (auto it = result.order.rbegin(); it != result.order.rend() && (&it->layer == layer.get()); ++it) {
                    if (tile.tile.id.isChildOf(it->tile->tile.id)) {
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
                result.order.emplace_back(*layer, &tile, bucket);
                tile.used = true;
            }
        }
    }

    return result;
}

std::vector<Feature> Style::queryRenderedFeatures(const ScreenLineString& geometry,
                                                  const TransformState& transformState,
                                                  const RenderedQueryOptions& options) const {
    std::unordered_set<std::string> sourceFilter;

    if (options.layerIDs) {
        for (const auto& layerID : *options.layerIDs) {
            auto layer = getLayer(layerID);
            if (layer) sourceFilter.emplace(layer->baseImpl->source);
        }
    }

    std::vector<Feature> result;
    std::unordered_map<std::string, std::vector<Feature>> resultsByLayer;

    for (const auto& source : sources) {
        if (!sourceFilter.empty() && sourceFilter.find(source->getID()) == sourceFilter.end()) {
            continue;
        }

        auto sourceResults = source->baseImpl->queryRenderedFeatures(geometry, transformState, options);
        std::move(sourceResults.begin(), sourceResults.end(), std::inserter(resultsByLayer, resultsByLayer.begin()));
    }

    if (resultsByLayer.empty()) {
        return result;
    }

    // Combine all results based on the style layer order.
    for (const auto& layer : layers) {
        if (!layer->baseImpl->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }
        auto it = resultsByLayer.find(layer->baseImpl->id);
        if (it != resultsByLayer.end()) {
            std::move(it->second.begin(), it->second.end(), std::back_inserter(result));
        }
    }

    return result;
}

float Style::getQueryRadius() const {
    float additionalRadius = 0;
    for (auto& layer : layers) {
        if (!layer->baseImpl->needsRendering(zoomHistory.lastZoom)) {
            continue;
        }
        additionalRadius = util::max(additionalRadius, layer->baseImpl->getQueryRadius());
    }
    return additionalRadius;
}


void Style::setSourceTileCacheSize(size_t size) {
    for (const auto& source : sources) {
        source->baseImpl->setCacheSize(size);
    }
}

void Style::onLowMemory() {
    for (const auto& source : sources) {
        source->baseImpl->onLowMemory();
    }
}

void Style::setObserver(style::Observer* observer_) {
    observer = observer_;
}

void Style::onGlyphsLoaded(const FontStack& fontStack, const GlyphRange& glyphRange) {
    observer->onGlyphsLoaded(fontStack, glyphRange);
    updateSymbolDependentTiles();
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

void Style::onSourceAttributionChanged(Source& source, const std::string& attribution) {
    observer->onSourceAttributionChanged(source, attribution);
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

void Style::onTileChanged(Source& source, const OverscaledTileID& tileID) {
    observer->onTileChanged(source, tileID);
    observer->onUpdate(Update::Repaint);
}

void Style::onTileError(Source& source, const OverscaledTileID& tileID, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load tile %s for source %s: %s",
               util::toString(tileID).c_str(), source.getID().c_str(), util::toString(error).c_str());
    observer->onTileError(source, tileID, error);
    observer->onResourceError(error);
}

void Style::onSpriteLoaded() {
    observer->onSpriteLoaded();
    observer->onUpdate(Update::Repaint); // For *-pattern properties.
    updateSymbolDependentTiles();
}

void Style::onSpriteError(std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load sprite: %s", util::toString(error).c_str());
    observer->onSpriteError(error);
    observer->onResourceError(error);
}

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

void Style::onLayerFilterChanged(Layer& layer) {
    layer.accept(QueueSourceReloadVisitor { updateBatch });
    observer->onUpdate(Update::Layout);
}

void Style::onLayerVisibilityChanged(Layer& layer) {
    layer.accept(QueueSourceReloadVisitor { updateBatch });
    observer->onUpdate(Update::RecalculateStyle | Update::Layout);
}

void Style::onLayerPaintPropertyChanged(Layer&) {
    observer->onUpdate(Update::RecalculateStyle | Update::Classes);
}

void Style::onLayerDataDrivenPaintPropertyChanged(Layer& layer) {
    layer.accept(QueueSourceReloadVisitor { updateBatch });
    observer->onUpdate(Update::RecalculateStyle | Update::Classes | Update::Layout);
}

void Style::onLayerLayoutPropertyChanged(Layer& layer, const char * property) {
    layer.accept(QueueSourceReloadVisitor { updateBatch });

    auto update = Update::Layout;

    // Recalculate the style for certain properties
    bool needsRecalculation = strcmp(property, "icon-size") == 0 || strcmp(property, "text-size") == 0;
    if (needsRecalculation) {
        update |= Update::RecalculateStyle;
    }
    observer->onUpdate(update);
}

void Style::dumpDebugLogs() const {
    for (const auto& source : sources) {
        source->baseImpl->dumpDebugLogs();
    }

    spriteAtlas->dumpDebugLogs();
}

} // namespace style
} // namespace mbgl
