#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {

class AnnotationSource : public style::Source {
public:
    AnnotationSource(std::string id,
                     std::string url,
                     uint16_t tileSize,
                     std::unique_ptr<Tileset>&&);
};

} // namespace mbgl
