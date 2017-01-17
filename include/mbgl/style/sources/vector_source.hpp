#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {
namespace style {

class VectorSource : public Source {
public:
    VectorSource(std::string id, variant<std::string, Tileset> urlOrTileset);

    optional<std::string> getURL() const;

    // Private implementation

    class Impl;
    Impl* const impl;
};

template <>
inline bool Source::is<VectorSource>() const {
    return type == SourceType::Vector;
}

} // namespace style
} // namespace mbgl
