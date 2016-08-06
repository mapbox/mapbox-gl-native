#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {
namespace style {

class VectorSource : public Source {
public:
    VectorSource(std::string id, variant<std::string, Tileset> urlOrTileset);

    // Private implementation

    class Impl;
};

} // namespace style
} // namespace mbgl
