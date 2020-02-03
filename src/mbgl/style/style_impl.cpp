#include <mbgl/gl/custom_layer.hpp>
#include <mbgl/sprite/sprite_loader.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/observer.hpp>
#include <mbgl/style/parser.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/style_impl.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/util/async_request.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/string.hpp>
#include <sstream>

namespace mbgl {
namespace style {

static Observer nullObserver;

Style::Impl::Impl(std::shared_ptr<FileSource> fileSource_, float pixelRatio)
    : fileSource(std::move(fileSource_)),
      spriteLoader(std::make_unique<SpriteLoader>(pixelRatio)),
      light(std::make_unique<Light>()),
      observer(&nullObserver) {
    spriteLoader->setObserver(this);
    light->setObserver(this);
}

Style::Impl::~Impl() = default;

void Style::Impl::loadJSON(const std::string& json_) {
    lastError = nullptr;
    observer->onStyleLoading();

    url.clear();
    parse(json_);
}

void Style::Impl::loadURL(const std::string& url_) {
    if (!fileSource) {
        observer->onStyleError(
            std::make_exception_ptr(util::StyleLoadException("Unable to find resource provider for style url.")));
        return;
    }

    lastError = nullptr;
    observer->onStyleLoading();

    loaded = false;
    url = url_;

    styleRequest = fileSource->request(Resource::style(url), [this](Response res) {
        // Don't allow a loaded, mutated style to be overwritten with a new version.
        if (mutated && loaded) {
            return;
        }

        if (res.error) {
            const std::string message = "loading style failed: " + res.error->message;
            Log::Error(Event::Setup, message.c_str());
            observer->onStyleError(std::make_exception_ptr(util::StyleLoadException(message)));
            observer->onResourceError(std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified || res.noContent) {
            return;
        } else {
            parse(*res.data);
        }
    });
}

void Style::Impl::parse(const std::string& json_) {
    Parser parser;

    if (auto error = parser.parse(json_)) {
        std::string message = "Failed to parse style: " + util::toString(error);
        Log::Error(Event::ParseStyle, message.c_str());
        observer->onStyleError(std::make_exception_ptr(util::StyleParseException(message)));
        observer->onResourceError(error);
        return;
    }

    mutated = false;
    loaded = false;
    json = json_;

    sources.clear();
    layers.clear();
    images.clear();

    transitionOptions = parser.transition;

    for (auto& source : parser.sources) {
        addSource(std::move(source));
    }

    for (auto& layer : parser.layers) {
        addLayer(std::move(layer));
    }

    name = parser.name;
    defaultCamera.center = parser.latLng;
    defaultCamera.zoom = parser.zoom;
    defaultCamera.bearing = parser.bearing;
    defaultCamera.pitch = parser.pitch;

    setLight(std::make_unique<Light>(parser.light));

    spriteLoaded = false;
    if (fileSource) {
        spriteLoader->load(parser.spriteURL, *fileSource);
    } else {
        onSpriteError(std::make_exception_ptr(std::runtime_error("Unable to find resource provider for sprite url.")));
    }
    glyphURL = parser.glyphURL;

    loaded = true;
    observer->onStyleLoaded();
}

std::string Style::Impl::getJSON() const {
    return json;
}

std::string Style::Impl::getURL() const {
    return url;
}

void Style::Impl::setTransitionOptions(const TransitionOptions& options) {
    transitionOptions = options;
}

TransitionOptions Style::Impl::getTransitionOptions() const {
    return transitionOptions;
}

void Style::Impl::addSource(std::unique_ptr<Source> source) {
    if (sources.get(source->getID())) {
        std::string msg = "Source " + source->getID() + " already exists";
        throw std::runtime_error(msg.c_str());
    }

    source->setObserver(this);
    auto item = sources.add(std::move(source));
    if (fileSource) {
        item->loadDescription(*fileSource);
    }
}

std::unique_ptr<Source> Style::Impl::removeSource(const std::string& id) {
    // Check if source is in use
    for (const auto& layer: layers) {
        if (layer->getSourceID() == id) {
            Log::Warning(Event::General, "Source '%s' is in use, cannot remove", id.c_str());
            return nullptr;
        }
    }

    std::unique_ptr<Source> source = sources.remove(id);

    if (source) {
        source->setObserver(nullptr);
    }

    return source;
}

std::vector<Layer*> Style::Impl::getLayers() {
    return layers.getWrappers();
}

std::vector<const Layer*> Style::Impl::getLayers() const {
    auto wrappers = layers.getWrappers();
    return std::vector<const Layer*>(wrappers.begin(), wrappers.end());
}

Layer* Style::Impl::getLayer(const std::string& id) const {
    return layers.get(id);
}

Layer* Style::Impl::addLayer(std::unique_ptr<Layer> layer, optional<std::string> before) {
    // TODO: verify source
    if (Source* source = sources.get(layer->getSourceID())) {
        if (!source->supportsLayerType(layer->baseImpl->getTypeInfo())) {
            std::ostringstream message;
            message << "Layer '" << layer->getID() << "' is not compatible with source '" << layer->getSourceID()
                    << "'";

            throw std::runtime_error(message.str());
        }
    }

    if (layers.get(layer->getID())) {
        throw std::runtime_error(std::string{"Layer "} + layer->getID() + " already exists");
    }

    layer->setObserver(this);
    Layer* result = layers.add(std::move(layer), before);
    observer->onUpdate();

    return result;
}

std::unique_ptr<Layer> Style::Impl::removeLayer(const std::string& id) {
    std::unique_ptr<Layer> layer = layers.remove(id);

    if (layer) {
        layer->setObserver(nullptr);
        observer->onUpdate();
    }

    return layer;
}

void Style::Impl::setLight(std::unique_ptr<Light> light_) {
    light = std::move(light_);
    light->setObserver(this);
    onLightChanged(*light);
}

Light* Style::Impl::getLight() const {
    return light.get();
}

std::string Style::Impl::getName() const {
    return name;
}

CameraOptions Style::Impl::getDefaultCamera() const {
    return defaultCamera;
}

std::vector<Source*> Style::Impl::getSources() {
    return sources.getWrappers();
}

std::vector<const Source*> Style::Impl::getSources() const {
    auto wrappers = sources.getWrappers();
    return std::vector<const Source*>(wrappers.begin(), wrappers.end());
}

Source* Style::Impl::getSource(const std::string& id) const {
    return sources.get(id);
}

bool Style::Impl::isLoaded() const {
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

void Style::Impl::addImage(std::unique_ptr<style::Image> image) {
    images.remove(image->getID()); // We permit using addImage to update.
    images.add(std::move(image));
    observer->onUpdate();
}

void Style::Impl::removeImage(const std::string& id) {
    images.remove(id);
}

const style::Image* Style::Impl::getImage(const std::string& id) const {
    return images.get(id);
}

void Style::Impl::setObserver(style::Observer* observer_) {
    observer = observer_;
}

void Style::Impl::onSourceLoaded(Source& source) {
    sources.update(source);
    observer->onSourceLoaded(source);
    observer->onUpdate();
}

void Style::Impl::onSourceChanged(Source& source) {
    sources.update(source);
    observer->onSourceChanged(source);
    observer->onUpdate();
}

void Style::Impl::onSourceError(Source& source, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load source %s: %s",
               source.getID().c_str(), util::toString(error).c_str());
    observer->onSourceError(source, error);
    observer->onResourceError(error);
}

void Style::Impl::onSourceDescriptionChanged(Source& source) {
    sources.update(source);
    observer->onSourceDescriptionChanged(source);
    if (!source.loaded && fileSource) {
        source.loadDescription(*fileSource);
    }
}

void Style::Impl::onSpriteLoaded(std::vector<std::unique_ptr<Image>>&& images_) {
    for (auto& image : images_) {
        addImage(std::move(image));
    }
    spriteLoaded = true;
    observer->onUpdate(); // For *-pattern properties.
}

void Style::Impl::onSpriteError(std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load sprite: %s", util::toString(error).c_str());
    observer->onResourceError(error);
    // Unblock rendering tiles (even though sprite request has failed).
    spriteLoaded = true;
    observer->onUpdate();
}

void Style::Impl::onLayerChanged(Layer& layer) {
    layers.update(layer);
    observer->onUpdate();
}

void Style::Impl::onLightChanged(const Light&) {
    observer->onUpdate();
}

void Style::Impl::dumpDebugLogs() const {
    Log::Info(Event::General, "styleURL: %s", url.c_str());
    for (const auto& source : sources) {
        source->dumpDebugLogs();
    }
}

const std::string& Style::Impl::getGlyphURL() const {
    return glyphURL;
}

Immutable<std::vector<Immutable<Image::Impl>>> Style::Impl::getImageImpls() const {
    return images.getImpls();
}

Immutable<std::vector<Immutable<Source::Impl>>> Style::Impl::getSourceImpls() const {
    return sources.getImpls();
}

Immutable<std::vector<Immutable<Layer::Impl>>> Style::Impl::getLayerImpls() const {
    return layers.getImpls();
}

} // namespace style
} // namespace mbgl
