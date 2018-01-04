#include <mbgl/style/style.hpp>
#include <mbgl/style/style_impl.hpp>
#include <mbgl/style/light.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/layer.hpp>

namespace mbgl {
namespace style {

Style::Style(Scheduler& scheduler, FileSource& fileSource, float pixelRatio)
    : impl(std::make_unique<Impl>(scheduler, fileSource, pixelRatio)) {
}

Style::~Style() = default;

void Style::loadJSON(const std::string& json) {
    impl->loadJSON(json);
}

void Style::loadURL(const std::string& url) {
    impl->loadURL(url);
}

std::string Style::getJSON() const {
    return impl->getJSON();
}

std::string Style::getURL() const {
    return impl->getURL();
}

std::string Style::getName() const {
    return impl->getName();
}

CameraOptions Style::getDefaultCamera() const {
    return impl->getDefaultCamera();
}

TransitionOptions Style::getTransitionOptions() const {
    return impl->getTransitionOptions();
}

void Style::setTransitionOptions(const TransitionOptions& options) {
    impl->mutated = true;
    impl->setTransitionOptions(options);
}

void Style::setLight(std::unique_ptr<Light> light) {
    impl->setLight(std::move(light));
}

Light* Style::getLight() {
    impl->mutated = true;
    return impl->getLight();
}

const Light* Style::getLight() const {
    return impl->getLight();
}

const Image* Style::getImage(const std::string& name) const {
    return impl->getImage(name);
}

void Style::addImage(std::unique_ptr<Image> image) {
    impl->mutated = true;
    impl->addImage(std::move(image));
}

void Style::removeImage(const std::string& name) {
    impl->mutated = true;
    impl->removeImage(name);
}

std::vector<Source*> Style::getSources() {
    impl->mutated = true;
    return impl->getSources();
}

std::vector<const Source*> Style::getSources() const {
    return const_cast<const Impl&>(*impl).getSources();
}

Source* Style::getSource(const std::string& id) {
    impl->mutated = true;
    return impl->getSource(id);
}

const Source* Style::getSource(const std::string& id) const {
    return impl->getSource(id);
}

void Style::addSource(std::unique_ptr<Source> source) {
    impl->mutated = true;
    impl->addSource(std::move(source));
}

std::unique_ptr<Source> Style::removeSource(const std::string& sourceID) {
    impl->mutated = true;
    return impl->removeSource(sourceID);
}

std::vector<Layer*> Style::getLayers() {
    impl->mutated = true;
    return impl->getLayers();
}

std::vector<const Layer*> Style::getLayers() const {
    return const_cast<const Impl&>(*impl).getLayers();
}

Layer* Style::getLayer(const std::string& layerID) {
    impl->mutated = true;
    return impl->getLayer(layerID);
}

const Layer* Style::getLayer(const std::string& layerID) const {
    return impl->getLayer(layerID);
}

void Style::addLayer(std::unique_ptr<Layer> layer, const optional<std::string>& before) {
    impl->mutated = true;
    impl->addLayer(std::move(layer), before);
}

std::unique_ptr<Layer> Style::removeLayer(const std::string& id) {
    impl->mutated = true;
    return impl->removeLayer(id);
}

} // namespace style
} // namespace mbgl
