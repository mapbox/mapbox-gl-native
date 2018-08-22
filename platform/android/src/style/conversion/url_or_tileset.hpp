#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/util/tileset.hpp>

#include <string>

#include "../value.hpp"

namespace mbgl {
namespace android {

variant<std::string, Tileset> convertURLOrTileset(mbgl::android::Value&& value);

}
}
