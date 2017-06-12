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
    images.clear();

    transitionOptions = {};
    transitionOptions.duration = util::DEFAULT_TRANSITION_DURATION;

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
    if (sources.get(source->getID())) {
        std::string msg = "Source " + source->getID() + " already exists";
        throw std::runtime_error(msg.c_str());
    }

    source->setObserver(this);
    source->loadDescription(fileSource);

    sources.add(std::move(source));
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

    std::unique_ptr<Source> source = sources.remove(id);

    if (source) {
        source->setObserver(nullptr);
    }

    return source;
}

std::vector<Layer*> Style::getLayers() {
    return layers.getWrappers();
}

Layer* Style::getLayer(const std::string& id) const {
    return layers.get(id);
}

Layer* Style::addLayer(std::unique_ptr<Layer> layer, optional<std::string> before) {
    // TODO: verify source

    if (layers.get(layer->getID())) {
        throw std::runtime_error(std::string{"Layer "} + layer->getID() + " already exists");
    }

    if (auto* customLayer = layer->as<CustomLayer>()) {
        customLayer->impl().initialize();
    }

    layer->setObserver(this);

    return layers.add(std::move(layer), before);
}

std::unique_ptr<Layer> Style::removeLayer(const std::string& id) {
    std::unique_ptr<Layer> layer = layers.remove(id);

    if (layer) {
        layer->setObserver(nullptr);

        if (auto* customLayer = layer->as<CustomLayer>()) {
            customLayer->impl().deinitialize();
        }
    }

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
    return sources.getWrappers();
}

Source* Style::getSource(const std::string& id) const {
    return sources.get(id);
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
    images.remove(image->getID()); // We permit using addImage to update.
    images.add(std::move(image));
}

void Style::removeImage(const std::string& id) {
    images.remove(id);
}

const style::Image* Style::getImage(const std::string& id) const {
    return images.get(id);
}

void Style::setObserver(style::Observer* observer_) {
    observer = observer_;
}

void Style::onSourceLoaded(Source& source) {
    sources.update(source);
    observer->onSourceLoaded(source);
    observer->onUpdate(Update::Repaint);
}

void Style::onSourceChanged(Source& source) {
    sources.update(source);
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
    sources.update(source);
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

void Style::onLayerChanged(Layer& layer) {
    layers.update(layer);
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

Immutable<std::vector<Immutable<Image::Impl>>> Style::getImageImpls() const {
    return images.getImpls();
}

Immutable<std::vector<Immutable<Source::Impl>>> Style::getSourceImpls() const {
    return sources.getImpls();
}

Immutable<std::vector<Immutable<Layer::Impl>>> Style::getLayerImpls() const {
    return layers.getImpls();
}

} // namespace style
} // namespace mbgl
