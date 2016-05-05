#include <mbgl/style/style.hpp>
#include <mbgl/source/source.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/layer/custom_layer.hpp>
#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_parser.hpp>
#include <mbgl/style/property_transition.hpp>
#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/style_update_parameters.hpp>
#include <mbgl/style/style_cascade_parameters.hpp>
#include <mbgl/style/style_calculation_parameters.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/layer/background_layer.hpp>
#include <mbgl/math/minmax.hpp>

#include <csscolorparser/csscolorparser.hpp>

#include <algorithm>

namespace mbgl {

bool Style::addClass(const std::string& className, const PropertyTransition& properties) {
    if (std::find(classes.begin(), classes.end(), className) != classes.end()) return false;
    classes.push_back(className);
    transitionProperties = properties;
    return true;
}

bool Style::hasClass(const std::string& className) const {
    return std::find(classes.begin(), classes.end(), className) != classes.end();
}

bool Style::removeClass(const std::string& className, const PropertyTransition& properties) {
    const auto it = std::find(classes.begin(), classes.end(), className);
    if (it != classes.end()) {
        classes.erase(it);
        transitionProperties = properties;
        return true;
    }
    return false;
}

void Style::setClasses(const std::vector<std::string>& classNames, const PropertyTransition& properties) {
    classes = classNames;
    transitionProperties = properties;
}

std::vector<std::string> Style::getClasses() const {
    return classes;
}

Style::Style(FileSource& fileSource_, float pixelRatio)
    : fileSource(fileSource_),
      glyphStore(std::make_unique<GlyphStore>(fileSource)),
      glyphAtlas(std::make_unique<GlyphAtlas>(1024, 1024)),
      spriteStore(std::make_unique<SpriteStore>(pixelRatio)),
      spriteAtlas(std::make_unique<SpriteAtlas>(1024, 1024, pixelRatio, *spriteStore)),
      lineAtlas(std::make_unique<LineAtlas>(256, 512)),
      workers(4) {
    glyphStore->setObserver(this);
    spriteStore->setObserver(this);
}

void Style::setJSON(const std::string& json, const std::string&) {
    sources.clear();
    layers.clear();
    classes.clear();

    StyleParser parser;
    parser.parse(json);

    for (auto& source : parser.sources) {
        addSource(std::move(source));
    }

    for (auto& layer : parser.layers) {
        addLayer(std::move(layer));
    }

    glyphStore->setURL(parser.glyphURL);
    spriteStore->load(parser.spriteURL, fileSource);

    loaded = true;
}

Style::~Style() {
    for (const auto& source : sources) {
        source->setObserver(nullptr);
    }

    glyphStore->setObserver(nullptr);
    spriteStore->setObserver(nullptr);
}

void Style::addSource(std::unique_ptr<Source> source) {
    source->setObserver(this);
    sources.emplace_back(std::move(source));
}

std::vector<std::unique_ptr<StyleLayer>> Style::getLayers() const {
    std::vector<std::unique_ptr<StyleLayer>> result;
    result.reserve(layers.size());
    for (const auto& layer : layers) {
        result.push_back(layer->clone());
    }
    return result;
}

std::vector<std::unique_ptr<StyleLayer>>::const_iterator Style::findLayer(const std::string& id) const {
    return std::find_if(layers.begin(), layers.end(), [&](const auto& layer) {
        return layer->id == id;
    });
}

StyleLayer* Style::getLayer(const std::string& id) const {
    auto it = findLayer(id);
    return it != layers.end() ? it->get() : nullptr;
}

void Style::addLayer(std::unique_ptr<StyleLayer> layer, optional<std::string> before) {
    if (SymbolLayer* symbolLayer = layer->as<SymbolLayer>()) {
        if (!symbolLayer->spriteAtlas) {
            symbolLayer->spriteAtlas = spriteAtlas.get();
        }
    }

    if (CustomLayer* customLayer = layer->as<CustomLayer>()) {
        customLayer->initialize();
    }

    layers.emplace(before ? findLayer(*before) : layers.end(), std::move(layer));
}

void Style::removeLayer(const std::string& id) {
    auto it = findLayer(id);
    if (it == layers.end())
        throw std::runtime_error("no such layer");
    layers.erase(it);
}

void Style::update(const StyleUpdateParameters& parameters) {
    bool allTilesUpdated = true;

    for (const auto& source : sources) {
        if (!source->update(parameters)) {
            allTilesUpdated = false;
        }
    }

    // We can only stop updating "partial" tiles when all of them
    // were notified of the arrival of the new resources.
    if (allTilesUpdated) {
        shouldReparsePartialTiles = false;
    }
}

void Style::cascade(const TimePoint& timePoint, MapMode mode) {
    // When in continuous mode, we can either have user- or style-defined
    // transitions. Still mode is always immediate.
    static const PropertyTransition immediateTransition;

    std::vector<ClassID> classIDs;
    for (const auto& className : classes) {
        classIDs.push_back(ClassDictionary::Get().lookup(className));
    }
    classIDs.push_back(ClassID::Default);
    classIDs.push_back(ClassID::Fallback);

    const StyleCascadeParameters parameters {
        classIDs,
        mode == MapMode::Continuous ? timePoint : Clock::time_point::max(),
        mode == MapMode::Continuous ? transitionProperties.value_or(immediateTransition) : immediateTransition
    };

    transitionProperties = {};

    for (const auto& layer : layers) {
        layer->cascade(parameters);
    }
}

void Style::recalculate(float z, const TimePoint& timePoint, MapMode mode) {
    for (const auto& source : sources) {
        source->enabled = false;
    }

    zoomHistory.update(z, timePoint);

    const StyleCalculationParameters parameters {
        z,
        mode == MapMode::Continuous ? timePoint : Clock::time_point::max(),
        zoomHistory,
        mode == MapMode::Continuous ? util::DEFAULT_FADE_DURATION : Duration::zero()
    };

    hasPendingTransitions = false;
    for (const auto& layer : layers) {
        hasPendingTransitions |= layer->recalculate(parameters);

        Source* source = getSource(layer->source);
        if (source && layer->needsRendering()) {
            source->enabled = true;
            if (!source->loaded && !source->isLoading()) {
                source->load(fileSource);
            }
        }
    }
}

Source* Style::getSource(const std::string& id) const {
    const auto it = std::find_if(sources.begin(), sources.end(), [&](const auto& source) {
        return source->id == id;
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
        if (source->enabled && !source->isLoaded()) return false;
    }

    if (!spriteStore->isLoaded()) {
        return false;
    }

    return true;
}

RenderData Style::getRenderData() const {
    RenderData result;

    for (const auto& source : sources) {
        if (source->enabled) {
            result.sources.insert(source.get());
        }
    }

    for (const auto& layer : layers) {
        if (layer->visibility == VisibilityType::None)
            continue;

        if (const BackgroundLayer* background = layer->as<BackgroundLayer>()) {
            if (layer.get() == layers[0].get() && background->paint.backgroundPattern.value.from.empty()) {
                // This is a solid background. We can use glClear().
                result.backgroundColor = background->paint.backgroundColor;
                result.backgroundColor[0] *= background->paint.backgroundOpacity;
                result.backgroundColor[1] *= background->paint.backgroundOpacity;
                result.backgroundColor[2] *= background->paint.backgroundOpacity;
                result.backgroundColor[3] *= background->paint.backgroundOpacity;
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

        Source* source = getSource(layer->source);
        if (!source) {
            Log::Warning(Event::Render, "can't find source for layer '%s'", layer->id.c_str());
            continue;
        }

        for (auto tile : source->getTiles()) {
            if (!tile->data || !tile->data->isReady())
                continue;

            // We're not clipping symbol layers, so when we have both parents and children of symbol
            // layers, we drop all children in favor of their parent to avoid duplicate labels.
            // See https://github.com/mapbox/mapbox-gl-native/issues/2482
            if (layer->is<SymbolLayer>()) {
                bool skip = false;
                // Look back through the buckets we decided to render to find out whether there is
                // already a bucket from this layer that is a parent of this tile. Tiles are ordered
                // by zoom level when we obtain them from getTiles().
                for (auto it = result.order.rbegin(); it != result.order.rend() && (&it->layer == layer.get()); ++it) {
                    if (tile->id.isChildOf(it->tile->id)) {
                        skip = true;
                        break;
                    }
                }
                if (skip) {
                    continue;
                }
            }

            auto bucket = tile->data->getBucket(*layer);
            if (bucket) {
                result.order.emplace_back(*layer, tile, bucket);
            }
        }
    }

    return result;
}

std::vector<Feature> Style::queryRenderedFeatures(
        const std::vector<TileCoordinate>& queryGeometry,
        const double zoom,
        const double bearing,
        const optional<std::vector<std::string>>& layerIDs) {
    std::vector<std::unordered_map<std::string, std::vector<Feature>>> sourceResults;
    for (const auto& source : sources) {
        sourceResults.emplace_back(source->queryRenderedFeatures(queryGeometry, zoom, bearing, layerIDs));
    }

    std::vector<Feature> features;
    auto featuresInserter = std::back_inserter(features);

    // Combine all results based on the style layer order.
    for (auto& layerPtr : layers) {
        auto& layerID = layerPtr->id;
        for (auto& sourceResult : sourceResults) {
            auto it = sourceResult.find(layerID);
            if (it != sourceResult.end()) {
                auto& layerFeatures = it->second;
                std::move(layerFeatures.begin(), layerFeatures.end(), featuresInserter);
            }
        }
    }

    return features;
}

float Style::getQueryRadius() const {
    float additionalRadius = 0;
    for (auto& layer : layers) {
        additionalRadius = util::max(additionalRadius, layer->getQueryRadius());
    }
    return additionalRadius;
}


void Style::setSourceTileCacheSize(size_t size) {
    for (const auto& source : sources) {
        source->setCacheSize(size);
    }
}

void Style::onLowMemory() {
    for (const auto& source : sources) {
        source->onLowMemory();
    }
}

void Style::setObserver(Observer* observer_) {
    observer = observer_;
}

void Style::onGlyphsLoaded(const FontStack& fontStack, const GlyphRange& glyphRange) {
    shouldReparsePartialTiles = true;
    observer->onGlyphsLoaded(fontStack, glyphRange);
    observer->onResourceLoaded();
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
    observer->onResourceLoaded();
}

void Style::onSourceError(Source& source, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load source %s: %s",
               source.id.c_str(), util::toString(error).c_str());
    observer->onSourceError(source, error);
    observer->onResourceError(error);
}

void Style::onTileLoaded(Source& source, const TileID& tileID, bool isNewTile) {
    if (isNewTile) {
        shouldReparsePartialTiles = true;
    }

    observer->onTileLoaded(source, tileID, isNewTile);
    observer->onResourceLoaded();
}

void Style::onTileError(Source& source, const TileID& tileID, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load tile %s for source %s: %s",
               std::string(tileID).c_str(), source.id.c_str(), util::toString(error).c_str());
    observer->onTileError(source, tileID, error);
    observer->onResourceError(error);
}

void Style::onPlacementRedone() {
    observer->onResourceLoaded();
}

void Style::onSpriteLoaded() {
    shouldReparsePartialTiles = true;
    observer->onSpriteLoaded();
    observer->onResourceLoaded();
}

void Style::onSpriteError(std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load sprite: %s", util::toString(error).c_str());
    observer->onSpriteError(error);
    observer->onResourceError(error);
}

void Style::dumpDebugLogs() const {
    for (const auto& source : sources) {
        source->dumpDebugLogs();
    }

    spriteStore->dumpDebugLogs();
}

} // namespace mbgl
