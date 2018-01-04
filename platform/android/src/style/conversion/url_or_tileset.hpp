#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>

#include <mbgl/util/tileset.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/tileset.hpp>

#include <jni/jni.hpp>

#include <string>

namespace mbgl {
namespace android {

// This conversion is expected not to fail because it's used only in contexts where
// the value was originally a String or TileSet object on the Java side. If it fails
// to convert, it's a bug in our serialization or Java-side static typing.
inline variant<std::string, Tileset> convertURLOrTileset(mbgl::android::Value&& value) {
    using namespace mbgl::style::conversion;

    const Convertible convertible(std::move(value));
    if (isObject(convertible)) {
        Error error;
        optional<Tileset> tileset = convert<Tileset>(convertible, error);
        if (!tileset) {
            throw std::logic_error(error.message);
        }
        return { *tileset };
    } else {
        return { *toString(convertible) };
    }
}

}
}
