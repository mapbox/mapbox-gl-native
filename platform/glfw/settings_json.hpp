#pragma once

#include <mbgl/map/mode.hpp>

namespace mbgl {

class Settings_JSON {
public:
    Settings_JSON();
    void load();
    void save();
    void clear();

public:
    double longitude = 0;
    double latitude = 0;
    double zoom = 0;
    double bearing = 0;
    double pitch = 0;
    bool axonometric = false;
    double xSkew = 0.0;
    double ySkew = 1.0;

    EnumType debug = 0;
    bool online = true;
};

} // namespace mbgl
