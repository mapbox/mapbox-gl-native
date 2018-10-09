#pragma once

#include <mapbox/bindgen/jni/marshal.hpp>

#include "greeter.hpp"

namespace mapbox {
namespace bindgen {

struct Greeter : public Interface<Greeter, ::Greeter> {
    static constexpr auto Name() { return "Greeter"; }
};

} // namespace bindgen
} // namespace mapbox

