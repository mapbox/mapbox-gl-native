#include <mbgl/style/style.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/map/tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/layer/symbol_layer.hpp>
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
#include <mbgl/platform/log.hpp>
#include <mbgl/layer/background_layer.hpp>

#include <csscolorparser/csscolorparser.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <algorithm>

namespace mbgl {

Style::Style(MapData& data_)
    : data(data_),
      glyphStore(std::make_unique<GlyphStore>()),
      glyphAtlas(std::make_unique<GlyphAtlas>(1024, 1024)),
      spriteStore(std::make_unique<SpriteStore>(data.pixelRatio)),
      spriteAtlas(std::make_unique<SpriteAtlas>(512, 512, data.pixelRatio, *spriteStore)),
      lineAtlas(std::make_unique<LineAtlas>(512, 512)),
      workers(4) {
    glyphStore->setObserver(this);
    spriteStore->setObserver(this);
}

void Style::setJSON(const std::string& json, const std::string&) {
    rapidjson::Document doc;
    doc.Parse<0>((const char *const)json.c_str());
    if (doc.HasParseError()) {
        Log::Error(Event::ParseStyle, "Error parsing style JSON at %i: %s", doc.GetErrorOffset(), rapidjson::GetParseError_En(doc.GetParseError()));
        return;
    }

    StyleParser parser;
    parser.parse(doc);

    for (auto& source : parser.sources) {
        addSource(std::move(source));
    }

    for (auto& layer : parser.layers) {
        addLayer(std::move(layer));
    }

    glyphStore->setURL(parser.glyphURL);
    spriteStore->setURL(parser.spriteURL);

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
    source->load();
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

void Style::addLayer(std::unique_ptr<StyleLayer> layer) {
    if (SymbolLayer* symbolLayer = dynamic_cast<SymbolLayer*>(layer.get())) {
        if (!symbolLayer->spriteAtlas) {
            symbolLayer->spriteAtlas = spriteAtlas.get();
        }
    }

    layers.emplace_back(std::move(layer));
}

void Style::addLayer(std::unique_ptr<StyleLayer> layer, const std::string& before) {
    layers.emplace(findLayer(before), std::move(layer));
}

void Style::removeLayer(const std::string& id) {
    auto it = findLayer(id);
    if (it == layers.end())
        throw std::runtime_error("no such layer");
    layers.erase(it);
}

void Style::update(const TransformState& transform,
                   TexturePool& texturePool) {
    bool allTilesUpdated = true;
    StyleUpdateParameters parameters(data.pixelRatio,
                                     data.getDebug(),
                                     data.getAnimationTime(),
                                     transform,
                                     workers,
                                     texturePool,
                                     shouldReparsePartialTiles,
                                     data,
                                     *this);

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

void Style::cascade() {
    std::vector<ClassID> classes;

    std::vector<std::string> classNames = data.getClasses();
    for (auto it = classNames.rbegin(); it != classNames.rend(); it++) {
        classes.push_back(ClassDictionary::Get().lookup(*it));
    }
    classes.push_back(ClassID::Default);
    classes.push_back(ClassID::Fallback);

    StyleCascadeParameters parameters(classes,
                                      data.getAnimationTime(),
                                      PropertyTransition { data.getDefaultTransitionDuration(),
                                                           data.getDefaultTransitionDelay() });

    for (const auto& layer : layers) {
        layer->cascade(parameters);
    }
}

void Style::recalculate(float z) {
    for (const auto& source : sources) {
        source->enabled = false;
    }

    zoomHistory.update(z, data.getAnimationTime());

    StyleCalculationParameters parameters(z,
                                          data.getAnimationTime(),
                                          zoomHistory,
                                          data.getDefaultFadeDuration());

    for (const auto& layer : layers) {
        hasPendingTransitions |= layer->recalculate(parameters);

        Source* source = getSource(layer->source);
        if (!source) {
            continue;
        }

        source->enabled = true;
    }
}

Source* Style::getSource(const std::string& id) const {
    const auto it = std::find_if(sources.begin(), sources.end(), [&](const auto& source) {
        return source->info.source_id == id;
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

    for (const auto& source : sources) {
        if (!source->isLoaded()) {
            return false;
        }
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

        if (const BackgroundLayer* background = dynamic_cast<const BackgroundLayer*>(layer.get())) {
            if (background->paint.pattern.value.from.empty()) {
                // This is a solid background. We can use glClear().
                result.backgroundColor = background->paint.color;
                result.backgroundColor[0] *= background->paint.opacity;
                result.backgroundColor[1] *= background->paint.opacity;
                result.backgroundColor[2] *= background->paint.opacity;
                result.backgroundColor[3] *= background->paint.opacity;
            } else {
                // This is a textured background. We need to render it with a quad.
                result.order.emplace_back(*layer);
            }
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
            if (layer->type == StyleLayerType::Symbol) {
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
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));
    assert(!observer);

    observer = observer_;
}

void Style::onGlyphRangeLoaded() {
    shouldReparsePartialTiles = true;

    emitTileDataChanged();
}

void Style::onGlyphRangeLoadingFailed(std::exception_ptr error) {
    emitResourceLoadingFailed(error);
}

void Style::onSourceLoaded() {
    emitTileDataChanged();
}

void Style::onSourceLoadingFailed(std::exception_ptr error) {
    emitResourceLoadingFailed(error);
}

void Style::onTileLoaded(bool isNewTile) {
    if (isNewTile) {
        shouldReparsePartialTiles = true;
    }

    emitTileDataChanged();
}

void Style::onTileLoadingFailed(std::exception_ptr error) {
    emitResourceLoadingFailed(error);
}

void Style::onSpriteLoaded() {
    shouldReparsePartialTiles = true;
    emitTileDataChanged();
}

void Style::onSpriteLoadingFailed(std::exception_ptr error) {
    emitResourceLoadingFailed(error);
}

void Style::emitTileDataChanged() {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    if (observer) {
        observer->onTileDataChanged();
    }
}

void Style::emitResourceLoadingFailed(std::exception_ptr error) {
    assert(util::ThreadContext::currentlyOn(util::ThreadType::Map));

    try {
        if (error) {
            lastError = error;
            std::rethrow_exception(error);
        }
    } catch(const std::exception& e) {
        Log::Error(Event::Style, "%s", e.what());
    }

    if (observer) {
        observer->onResourceLoadingFailed(error);
    }
}

void Style::dumpDebugLogs() const {
    for (const auto& source : sources) {
        source->dumpDebugLogs();
    }

    spriteStore->dumpDebugLogs();
}

} // namespace mbgl
