#pragma once

#include <mbgl/util/noncopyable.hpp>

namespace mbgl {
namespace android {

class JsonObject : private mbgl::util::noncopyable {
public:
    static constexpr auto Name() { return "com/google/gson/JsonObject"; };

};


} // namespace android
} // namespace mbgl