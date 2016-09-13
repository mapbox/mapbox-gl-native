#pragma once

#include <utility>
#include <cstdint>
#include <boost/functional/hash.hpp>

namespace mbgl {

typedef std::pair<uint16_t, uint16_t> GlyphRange;

struct GlyphRangeHash {
    std::size_t operator()(const GlyphRange& glyphRange) const {
        return boost::hash_value(glyphRange);
    }
};
    
} // end namespace mbgl

