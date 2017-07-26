#include "settings_json.hpp"
#include <fstream>

namespace mbgl {

Settings_JSON::Settings_JSON() { load(); }

void Settings_JSON::load() {
    std::ifstream file("/tmp/mbgl-native.cfg");
    if (file) {
        file >> longitude;
        file >> latitude;
        file >> zoom;
        file >> bearing;
        file >> pitch;
        file >> debug;
        file >> online;
    }
}

void Settings_JSON::save() {
    std::ofstream file("/tmp/mbgl-native.cfg");
    if (file) {
        file << longitude << std::endl;
        file << latitude << std::endl;
        file << zoom << std::endl;
        file << bearing << std::endl;
        file << pitch << std::endl;
        file << debug << std::endl;
        file << online << std::endl;
    }
}

void Settings_JSON::clear() {
    longitude = 0;
    latitude = 0;
    zoom = 0;
    bearing = 0;
    pitch = 0;
    debug = 0;
    online = true;
}

} // namespace mbgl
