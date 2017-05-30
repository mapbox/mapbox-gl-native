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
#include <mbgl/sprite/sprite_loader.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace style {

static Observer nullObserver;

Style::Style(Scheduler& scheduler_, FileSource& fileSource_, float pixelRatio)
    : scheduler(scheduler_),
      fileSource(fileSource_),
      spriteLoader(std::make_unique<SpriteLoader>(pixelRatio)),
      light(std::make_unique<Light>()),
      observer(&nullObserver) {
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

    spriteLoader->load(parser.spriteURL, scheduler, fileSource);
    glyphURL = parser.glyphURL;

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

void Style::setLight(std::unique_ptr<Light> light_) {
    light = std::move(light_);
    light->setObserver(this);
    onLightChanged(*light);
}

Light* Style::getLight() const {
    return light.get();
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

void Style::setObserver(style::Observer* observer_) {
    observer = observer_;
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
}

const std::string& Style::getGlyphURL() const {
    return glyphURL;
}

std::vector<Immutable<Image::Impl>> Style::getImageImpls() const {
    std::vector<Immutable<style::Image::Impl>> result;
    result.reserve(images.size());
    for (const auto& image : images) {
        result.push_back(image.second->impl);
    }
    return result;
}

std::vector<Immutable<Source::Impl>> Style::getSourceImpls() const {
    std::vector<Immutable<style::Source::Impl>> result;
    result.reserve(sources.size());
    for (const auto& source : sources) {
        result.push_back(source->baseImpl);
    }
    return result;
}

std::vector<Immutable<Layer::Impl>> Style::getLayerImpls() const {
    std::vector<Immutable<style::Layer::Impl>> result;
    result.reserve(layers.size());
    for (const auto& layer : layers) {
        result.push_back(layer->baseImpl);
    }
    return result;
}

} // namespace style
} // namespace mbgl
