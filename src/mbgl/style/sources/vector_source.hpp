#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {
namespace style {

class VectorSource : public Source {
public:
    VectorSource(std::string id,
                 std::string url,
                 std::unique_ptr<Tileset>&&);
};

} // namespace style
} // namespace mbgl
