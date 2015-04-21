#include <mbgl/map/resource_loader.hpp>

#include <mbgl/map/environment.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/style/style.hpp>

#include <cassert>
#include <functional>

namespace mbgl {

ResourceLoader::ResourceLoader() {
    assert(Environment::currentlyOn(ThreadType::Map));
}

ResourceLoader::~ResourceLoader() {
    assert(Environment::currentlyOn(ThreadType::Map));
}

void ResourceLoader::setObserver(Observer* observer) {
    assert(Environment::currentlyOn(ThreadType::Map));
    assert(!observer_);

    observer_ = observer;
}

void ResourceLoader::setStyle(Style* style) {
    style_ = style;

    Environment& env = Environment::Get();
    for (const auto& source : style->sources) {
        source->load(accessToken_, env, std::bind(&ResourceLoader::emitTileDataChanged, this));
    }
}

void ResourceLoader::setAccessToken(const std::string& accessToken) {
    accessToken_ = accessToken;
}

void ResourceLoader::emitTileDataChanged() {
    assert(Environment::currentlyOn(ThreadType::Map));

    if (observer_) {
        observer_->onTileDataChanged();
    }
}

}
