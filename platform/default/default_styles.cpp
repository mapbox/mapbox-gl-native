#include "default_styles.hpp"

namespace mbgl {
namespace util {

const std::vector<std::pair<std::string, std::string>> defaultStyles = {
    { "asset://styles/streets-v8.json", "Mapbox Streets" },
    { "asset://styles/emerald-v8.json", "Emerald" },
    { "asset://styles/light-v8.json", "Light" },
    { "asset://styles/dark-v8.json", "Dark" },
    { "asset://styles/satellite-v8.json", "Satellite" },
    { "asset://styles/satellite-hybrid-v8.json", "Satellite Streets" }
};

} // end namespace util
} // end namespace mbgl
