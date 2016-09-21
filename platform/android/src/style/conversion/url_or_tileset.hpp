#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>

#include <mbgl/util/tileset.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/tileset.hpp>

#include <jni/jni.hpp>

#include <string>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<variant<std::string, Tileset>> {

    template <class V>
    Result<variant<std::string, Tileset>> operator()(const V& value) const {
        if (isObject(value)) {
            Result<Tileset> tileset = convert<Tileset>(value);
            if (!tileset) {
                return tileset.error();
            }
            return *tileset;
        } else {
            return *toString(value);
        }
    }

};

}
}
}