#include <mbgl/style/style.hpp>
#include <mbgl/style/observer.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/style/cascade_parameters.hpp>
#include <mbgl/style/calculation_parameters.hpp>
#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/renderer/render_item.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/math/minmax.hpp>

#include <algorithm>

namespace mbgl {
namespace style {

static Observer nullObserver;

Style::Style(FileSource& fileSource_, float pixelRatio)
    : fileSource(fileSource_),
      glyphStore(std::make_unique<GlyphStore>(fileSource)),
      glyphAtlas(std::make_unique<GlyphAtlas>(1024, 1024)),
      spriteStore(std::make_unique<SpriteStore>(pixelRatio)),
      spriteAtlas(std::make_unique<SpriteAtlas>(1024, 1024, pixelRatio, *spriteStore)),
      lineAtlas(std::make_unique<LineAtlas>(256, 512)),
      observer(&nullObserver),
      workers(4) {
    glyphStore->setObserver(this);
    spriteStore->setObserver(this);
}

Style::~Style() {
    for (const auto& source : sources) {
        source->setObserver(nullptr);
    }

    glyphStore->setObserver(nullptr);
    spriteStore->setObserver(nullptr);
}

bool Style::addClass(const std::string& className, const TransitionOptions& properties) {
    if (hasClass(className)) return false;
    classes.push_back(className);
    transitionProperties = properties;
    return true;
}

bool Style::hasClass(const std::string& className) const {
    return std::find(classes.begin(), classes.end(), className) != classes.end();
}

bool Style::removeClass(const std::string& className, const TransitionOptions& properties) {
    const auto it = std::find(classes.begin(), classes.end(), className);
    if (it != classes.end()) {
        classes.erase(it);
        transitionProperties = properties;
        return true;
    }
    return false;
}

void Style::setClasses(const std::vector<std::string>& classNames, const TransitionOptions& properties) {
    classes = classNames;
    transitionProperties = properties;
}

std::vector<std::string> Style::getClasses() const {
    return classes;
}

void Style::setJSON(const std::string& json) {
    sources.clear();
    layers.clear();
    classes.clear();

    Parser parser;
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

void Style::addSource(std::unique_ptr<Source> source) {
    source->setObserver(this);
    sources.emplace_back(std::move(source));
}

std::vector<std::unique_ptr<Layer>> Style::getLayers() const {
    std::vector<std::unique_ptr<Layer>> result;
    result.reserve(layers.size());
    for (const auto& layer : layers) {
        result.push_back(layer->baseImpl->clone());
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

void Style::addLayer(std::unique_ptr<Layer> layer, optional<std::string> before) {
    if (SymbolLayer* symbolLayer = layer->as<SymbolLayer>()) {
        if (!symbolLayer->impl->spriteAtlas) {
            symbolLayer->impl->spriteAtlas = spriteAtlas.get();
        }
    }

    if (CustomLayer* customLayer = layer->as<CustomLayer>()) {
        customLayer->impl->initialize();
    }

    layers.emplace(before ? findLayer(*before) : layers.end(), std::move(layer));
}

void Style::removeLayer(const std::string& id) {
    auto it = findLayer(id);
    if (it == layers.end())
        throw std::runtime_error("no such layer");
    layers.erase(it);
}

void Style::update(const UpdateParameters& parameters) {
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
    static const TransitionOptions immediateTransition;

    std::vector<ClassID> classIDs;
    for (const auto& className : classes) {
        classIDs.push_back(ClassDictionary::Get().lookup(className));
    }
    classIDs.push_back(ClassID::Default);
    classIDs.push_back(ClassID::Fallback);

    const CascadeParameters parameters {
        classIDs,
        mode == MapMode::Continuous ? timePoint : Clock::time_point::max(),
        mode == MapMode::Continuous ? transitionProperties.value_or(immediateTransition) : immediateTransition
    };

    transitionProperties = {};

    for (const auto& layer : layers) {
        layer->baseImpl->cascade(parameters);
    }
}

void Style::recalculate(float z, const TimePoint& timePoint, MapMode mode) {
    for (const auto& source : sources) {
        source->enabled = false;
    }

    zoomHistory.update(z, timePoint);

    const CalculationParameters parameters {
        z,
        mode == MapMode::Continuous ? timePoint : Clock::time_point::max(),
        zoomHistory,
        mode == MapMode::Continuous ? util::DEFAULT_FADE_DURATION : Duration::zero()
    };

    hasPendingTransitions = false;
    for (const auto& layer : layers) {
        hasPendingTransitions |= layer->baseImpl->recalculate(parameters);

        Source* source = getSource(layer->baseImpl->source);
        if (source && layer->baseImpl->needsRendering()) {
            source->enabled = true;
            if (!source->loaded) {
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
        if (layer->baseImpl->visibility == VisibilityType::None)
            continue;

        if (const BackgroundLayer* background = layer->as<BackgroundLayer>()) {
            const BackgroundPaintProperties& paint = background->impl->paint;
            if (layer.get() == layers[0].get() && paint.backgroundPattern.value.from.empty()) {
                // This is a solid background. We can use glClear().
                result.backgroundColor = paint.backgroundColor;
                result.backgroundColor.r *= paint.backgroundOpacity;
                result.backgroundColor.g *= paint.backgroundOpacity;
                result.backgroundColor.b *= paint.backgroundOpacity;
                result.backgroundColor.a *= paint.backgroundOpacity;
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

        for (auto& pair : source->getRenderTiles()) {
            auto& tile = pair.second;
            if (!tile.tile.isRenderable()) {
                continue;
            }

            // We're not clipping symbol layers, so when we have both parents and children of symbol
            // layers, we drop all children in favor of their parent to avoid duplicate labels.
            // See https://github.com/mapbox/mapbox-gl-native/issues/2482
            if (layer->is<SymbolLayer>()) {
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
            }
        }
    }

    return result;
}

std::vector<Feature> Style::queryRenderedFeatures(const QueryParameters& parameters) const {
    std::unordered_map<std::string, std::vector<Feature>> resultsByLayer;

    for (const auto& source : sources) {
        auto sourceResults = source->queryRenderedFeatures(parameters);
        std::move(sourceResults.begin(), sourceResults.end(), std::inserter(resultsByLayer, resultsByLayer.begin()));
    }

    std::vector<Feature> result;

    // Combine all results based on the style layer order.
    for (const auto& layer : layers) {
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
        additionalRadius = util::max(additionalRadius, layer->baseImpl->getQueryRadius());
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

void Style::setObserver(style::Observer* observer_) {
    observer = observer_;
}

void Style::onGlyphsLoaded(const FontStack& fontStack, const GlyphRange& glyphRange) {
    shouldReparsePartialTiles = true;
    observer->onGlyphsLoaded(fontStack, glyphRange);
    observer->onNeedsRepaint();
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
    observer->onNeedsRepaint();
}

void Style::onSourceError(Source& source, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load source %s: %s",
               source.id.c_str(), util::toString(error).c_str());
    observer->onSourceError(source, error);
    observer->onResourceError(error);
}

void Style::onTileLoaded(Source& source, const OverscaledTileID& tileID, bool isNewTile) {
    if (isNewTile) {
        shouldReparsePartialTiles = true;
    }

    observer->onTileLoaded(source, tileID, isNewTile);
    observer->onNeedsRepaint();
}

void Style::onTileError(Source& source, const OverscaledTileID& tileID, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load tile %s for source %s: %s",
               util::toString(tileID).c_str(), source.id.c_str(), util::toString(error).c_str());
    observer->onTileError(source, tileID, error);
    observer->onResourceError(error);
}

void Style::onNeedsRepaint() {
    observer->onNeedsRepaint();
}

void Style::onSpriteLoaded() {
    shouldReparsePartialTiles = true;
    observer->onSpriteLoaded();
    observer->onNeedsRepaint();
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

} // namespace style
} // namespace mbgl
