#pragma once

#include <mbgl/util/noncopyable.hpp>

namespace mbgl {
namespace android {

class Geometry : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/mapbox/services/commons/geojson/Geometry"; };

};


} // namespace android
} // namespace mbgl