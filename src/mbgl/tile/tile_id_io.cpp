#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/string.hpp>

#include <iostream>

namespace mbgl {

::std::ostream& operator<<(::std::ostream& os, const CanonicalTileID& rhs) {
    // Uncomment this to create code instead of shorthands.
    // return os << "CanonicalTileID{ " << uint32_t(rhs.z) << ", " << rhs.x << ", " << rhs.y << " }";
    return os << uint32_t(rhs.z) << "/" << rhs.x << "/" << rhs.y;
}

namespace util {
std::string toString(const CanonicalTileID& rhs) {
    return util::toString(rhs.z) + "/" + util::toString(rhs.x) + "/" + util::toString(rhs.y);
}
} // namespace util

::std::ostream& operator<<(::std::ostream& os, const OverscaledTileID& rhs) {
    return os << rhs.canonical << "=>" << uint32_t(rhs.overscaledZ);
}

namespace util {
std::string toString(const OverscaledTileID& rhs) {
    return util::toString(rhs.canonical) + "=>" + util::toString(rhs.overscaledZ);
}
} // namespace util

::std::ostream& operator<<(::std::ostream& os, const UnwrappedTileID& rhs) {
    // Uncomment this to create code instead of shorthands.
    // return os << "UnwrappedTileID{ " << uint32_t(rhs.wrap) << ", { " << uint32_t(rhs.canonical.z)
    //           << ", " << rhs.canonical.x << ", " << rhs.canonical.y << " } }";
    return os << rhs.canonical << (rhs.wrap >= 0 ? "+" : "") << rhs.wrap;
}

namespace util {
std::string toString(const UnwrappedTileID& rhs) {
    return util::toString(rhs.canonical) + (rhs.wrap >= 0 ? "+" : "") + util::toString(rhs.wrap);
}
} // namespace util

} // namespace mbgl
