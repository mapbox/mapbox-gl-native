#include <mbgl/platform/settings.hpp>

#include <mutex>

namespace mbgl {
namespace platform {

namespace {
mapbox::base::Value getValue(const std::string& key, const mapbox::base::ValueObject& object) {
    auto it = object.find(key);
    if (it != object.end()) {
        return it->second;
    }
    return {};
}
} // namespace

class Settings::Impl {
public:
    mapbox::base::ValueObject settings;
    std::mutex mutex;
};

Settings::Settings() : impl(std::make_unique<Impl>()) {}

Settings& Settings::getInstance() noexcept {
    static Settings instance;
    return instance;
}

void Settings::set(const std::string& key, mapbox::base::Value value) noexcept {
    std::lock_guard<std::mutex> lock(impl->mutex);
    impl->settings[key] = std::move(value);
}

void Settings::set(const mapbox::base::ValueObject& values) noexcept {
    std::lock_guard<std::mutex> lock(impl->mutex);
    for (const auto& pair : values) {
        impl->settings[pair.first] = pair.second;
    }
}

mapbox::base::ValueObject Settings::get(const std::vector<std::string>& keys) const noexcept {
    std::lock_guard<std::mutex> lock(impl->mutex);
    mapbox::base::ValueObject result;
    for (const auto& key : keys) {
        result[key] = getValue(key, impl->settings);
    }
    return result;
}

mapbox::base::Value Settings::get(const std::string& key) const noexcept {
    std::lock_guard<std::mutex> lock(impl->mutex);
    return getValue(key, impl->settings);
}

} // namespace platform
} // namespace mbgl
