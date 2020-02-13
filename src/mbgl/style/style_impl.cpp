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

StyleImpl::StyleImpl(std::shared_ptr<FileSource> fileSource_, float pixelRatio)
    : annotationManager(*this),
      fileSource(std::move(fileSource_)),
      spriteLoader(std::make_unique<SpriteLoader>(pixelRatio)),
      light(std::make_unique<Light>()) {
    spriteLoader->setObserver(this);
    light->setObserver(this);
}

StyleImpl::~StyleImpl() = default;

void StyleImpl::loadJSON(const std::string& json_) {
    lastError = nullptr;
    for (auto &observer: observers)
        observer->onStyleLoading();

    url.clear();
    parse(json_);
}

void StyleImpl::loadURL(const std::string& url_) {
    if (!fileSource) {
        for (auto &observer: observers) {
            observer->onStyleError(
                std::make_exception_ptr(util::StyleLoadException("Unable to find resource provider for style url.")));
        }
        return;
    }

    lastError = nullptr;
    for (auto &observer: observers)
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
            for (auto &observer: observers) {
                observer->onStyleError(std::make_exception_ptr(util::StyleLoadException(message)));
                observer->onResourceError(std::make_exception_ptr(std::runtime_error(res.error->message)));
            }
        } else if (res.notModified || res.noContent) {
            return;
        } else {
            parse(*res.data);
        }
    });
}

void StyleImpl::parse(const std::string& json_) {
    Parser parser;

    if (auto error = parser.parse(json_)) {
        std::string message = "Failed to parse style: " + util::toString(error);
        Log::Error(Event::ParseStyle, message.c_str());
        for (auto &observer: observers) {
            observer->onStyleError(std::make_exception_ptr(util::StyleParseException(message)));
            observer->onResourceError(error);
        }
        return;
    }

    mutated = false;
    loaded = false;
    json = json_;

    sources.clear();
    layers.clear();
    images = makeMutable<ImageImpls>();

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

    annotationManager.onStyleLoaded();
    for (auto &observer: observers)
        observer->onStyleLoaded();
}

std::string StyleImpl::getJSON() const {
    return json;
}

std::string StyleImpl::getURL() const {
    return url;
}

void StyleImpl::setTransitionOptions(const TransitionOptions& options) {
    transitionOptions = options;
}

TransitionOptions StyleImpl::getTransitionOptions() const {
    return transitionOptions;
}

void StyleImpl::addSource(std::unique_ptr<Source> source) {
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

std::unique_ptr<Source> StyleImpl::removeSource(const std::string& id) {
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

std::vector<Layer*> StyleImpl::getLayers() {
    return layers.getWrappers();
}

std::vector<const Layer*> StyleImpl::getLayers() const {
    auto wrappers = layers.getWrappers();
    return std::vector<const Layer*>(wrappers.begin(), wrappers.end());
}

Layer* StyleImpl::getLayer(const std::string& id) const {
    return layers.get(id);
}

Layer* StyleImpl::addLayer(std::unique_ptr<Layer> layer, optional<std::string> before) {
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
    for (auto &observer: observers)
        observer->onUpdate();

    return result;
}

std::unique_ptr<Layer> StyleImpl::removeLayer(const std::string& id) {
    std::unique_ptr<Layer> layer = layers.remove(id);

    if (layer) {
        layer->setObserver(nullptr);
        for (auto &observer: observers)
            observer->onUpdate();
    }

    return layer;
}

void StyleImpl::setLight(std::unique_ptr<Light> light_) {
    light = std::move(light_);
    light->setObserver(this);
    onLightChanged(*light);
}

Light* StyleImpl::getLight() const {
    return light.get();
}

std::string StyleImpl::getName() const {
    return name;
}

CameraOptions StyleImpl::getDefaultCamera() const {
    return defaultCamera;
}

std::vector<Source*> StyleImpl::getSources() {
    return sources.getWrappers();
}

std::vector<const Source*> StyleImpl::getSources() const {
    auto wrappers = sources.getWrappers();
    return std::vector<const Source*>(wrappers.begin(), wrappers.end());
}

Source* StyleImpl::getSource(const std::string& id) const {
    return sources.get(id);
}

bool StyleImpl::isLoaded() const {
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

void StyleImpl::addImage(std::unique_ptr<style::Image> image) {
    auto newImages = makeMutable<ImageImpls>(*images);
    auto it =
        std::lower_bound(newImages->begin(), newImages->end(), image->getID(), [](const auto& a, const std::string& b) {
            return a->id < b;
        });
    if (it != newImages->end() && (*it)->id == image->getID()) {
        // We permit using addImage to update.
        *it = std::move(image->baseImpl);
    } else {
        newImages->insert(it, std::move(image->baseImpl));
    }
    images = std::move(newImages);
    for (auto &observer: observers)
        observer->onUpdate();
}

void StyleImpl::removeImage(const std::string& id) {
    auto newImages = makeMutable<ImageImpls>(*images);
    auto found =
        std::find_if(newImages->begin(), newImages->end(), [&id](const auto& image) { return image->id == id; });
    if (found == images->end()) {
        Log::Warning(Event::General, "Image '%s' is not present in style, cannot remove", id.c_str());
        return;
    }
    newImages->erase(found);
    images = std::move(newImages);
}

optional<Immutable<style::Image::Impl>> StyleImpl::getImage(const std::string& id) const {
    auto found = std::find_if(images->begin(), images->end(), [&id](const auto& image) { return image->id == id; });
    if (found == images->end()) return nullopt;
    return *found;
}

void StyleImpl::addObserver(style::Observer* observer) {
    observers.insert(observer);
}

void StyleImpl::removeObserver(Observer *observer)
{
    observers.erase(observer);
}

void StyleImpl::onSourceLoaded(Source& source) {
    sources.update(source);
    for (auto &observer: observers)
        observer->onSourceLoaded(source);
    for (auto &observer: observers)
        observer->onUpdate();
}

void StyleImpl::onSourceChanged(Source& source) {
    sources.update(source);
    for (auto &observer: observers)
        observer->onSourceChanged(source);
    for (auto &observer: observers)
        observer->onUpdate();
}

void StyleImpl::onSourceError(Source& source, std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load source %s: %s",
               source.getID().c_str(), util::toString(error).c_str());
    for (auto &observer: observers)
        observer->onSourceError(source, error);
    for (auto &observer: observers)
        observer->onResourceError(error);
}

void StyleImpl::onSourceDescriptionChanged(Source& source) {
    sources.update(source);
    for (auto &observer: observers)
        observer->onSourceDescriptionChanged(source);
    if (!source.loaded && fileSource) {
        source.loadDescription(*fileSource);
    }
}

void StyleImpl::onSpriteLoaded(std::vector<Immutable<style::Image::Impl>> images_) {
    auto newImages = makeMutable<ImageImpls>(*images);
    newImages->insert(
        newImages->end(), std::make_move_iterator(images_.begin()), std::make_move_iterator(images_.end()));
    std::sort(newImages->begin(), newImages->end(), [](const auto& a, const auto& b) { return a->id < b->id; });
    images = std::move(newImages);
    spriteLoaded = true;
    for (auto &observer: observers)
        observer->onUpdate(); // For *-pattern properties.
}

void StyleImpl::onSpriteError(std::exception_ptr error) {
    lastError = error;
    Log::Error(Event::Style, "Failed to load sprite: %s", util::toString(error).c_str());
    for (auto &observer: observers)
        observer->onResourceError(error);
    // Unblock rendering tiles (even though sprite request has failed).
    spriteLoaded = true;
    for (auto &observer: observers)
        observer->onUpdate();
}

void StyleImpl::onLayerChanged(Layer& layer) {
    layers.update(layer);
    for (auto &observer: observers)
        observer->onUpdate();
}

void StyleImpl::onLightChanged(const Light&) {
    for (auto &observer: observers)
        observer->onUpdate();
}

void StyleImpl::dumpDebugLogs() const {
    Log::Info(Event::General, "styleURL: %s", url.c_str());
    for (const auto& source : sources) {
        source->dumpDebugLogs();
    }
}

const std::string& StyleImpl::getGlyphURL() const {
    return glyphURL;
}

Immutable<std::vector<Immutable<Image::Impl>>> StyleImpl::getImageImpls() const {
    return images;
}

Immutable<std::vector<Immutable<Source::Impl>>> StyleImpl::getSourceImpls() const {
    return sources.getImpls();
}

Immutable<std::vector<Immutable<Layer::Impl>>> StyleImpl::getLayerImpls() const {
    return layers.getImpls();
}

} // namespace style
} // namespace mbgl
