#pragma once

#include <mapbox/compatibility/value.hpp>

#include <memory>

namespace mbgl {
namespace platform {

#define DECLARE_MAPBOX_SETTING(name, value) constexpr const char* name = "mapbox_" #value

// The value for EXPERIMENTAL_THREAD_PRIORITY_* keys, must be a double.
DECLARE_MAPBOX_SETTING(EXPERIMENTAL_THREAD_PRIORITY_WORKER, thread_priority_worker);
DECLARE_MAPBOX_SETTING(EXPERIMENTAL_THREAD_PRIORITY_FILE, thread_priority_file);
DECLARE_MAPBOX_SETTING(EXPERIMENTAL_THREAD_PRIORITY_NETWORK, thread_priority_network);
DECLARE_MAPBOX_SETTING(EXPERIMENTAL_THREAD_PRIORITY_DATABASE, thread_priority_database);

// Settings class provides non-persistent, in-process key-value storage.
class Settings final {
public:
    // Returns singleton instance
    static Settings& getInstance() noexcept;

    // Sets setting value for a specified key.
    void set(const std::string& key, mapbox::base::Value value) noexcept;

    // Sets multiple setting values by merging current Settings object
    // with provided `values` object.
    void set(const mapbox::base::ValueObject& values) noexcept;

    // Returns setting value for a specified key or NullValue if element
    // for specified key is missing.
    mapbox::base::Value get(const std::string& key) const noexcept;

    // Returns values for settings whose keys are equal to the ones provided in `keys` argument.
    // Null values would be provided for keys whose elements are missing in Settings.
    mapbox::base::ValueObject get(const std::vector<std::string>& keys) const noexcept;

private:
    Settings();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace platform
} // namespace mbgl
