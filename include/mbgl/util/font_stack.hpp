#pragma once

#include <string>
#include <vector>

namespace mbgl {

// An array of font names
using FontStack = std::vector<std::string>;

std::string fontStackToString(const FontStack&);

struct FontStackHash {
    std::size_t operator()(const FontStack&) const;
};

} // namespace mbgl
