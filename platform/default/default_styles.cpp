#include "default_styles.hpp"

namespace mbgl {
namespace util {

const std::vector<std::pair<std::string, std::string>> defaultStyles = {
    { "asset://styles/mapbox-streets-v7.json", "Mapbox Streets" },
    { "asset://styles/emerald-v7.json", "Emerald" },
    { "asset://styles/light-v7.json", "Light" },
    { "asset://styles/dark-v7.json", "Dark" },
    { "asset://styles/satellite-v7.json", "Satellite" }
};

} // end namespace util
} // end namespace mbgl
