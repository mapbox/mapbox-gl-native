#pragma once

#include <mbgl/util/constants.hpp>

#include <cstdint>
#include <array>
#include <tuple>
#include <forward_list>
#include <algorithm>
#include <iosfwd>
#include <cassert>

namespace mbgl {

class OverscaledTileID;
class CanonicalTileID;
class UnwrappedTileID;

// Has integer z/x/y coordinates
// All tiles must be derived from 0/0/0 (=no tiles outside of the main tile pyramid)
// Used for requesting data; represents data tiles that exist out there.
// z is never larger than the source's maxzoom
class CanonicalTileID {
public:
    CanonicalTileID(uint8_t z, uint32_t x, uint32_t y);
    bool operator==(const CanonicalTileID&) const;
    bool operator!=(const CanonicalTileID&) const;
    bool operator<(const CanonicalTileID&) const;
    bool isChildOf(const CanonicalTileID&) const;
    CanonicalTileID scaledTo(uint8_t z) const;
    std::array<CanonicalTileID, 4> children() const;

    uint8_t z;
    uint32_t x;
    uint32_t y;
};

::std::ostream& operator<<(::std::ostream& os, const CanonicalTileID& rhs);
namespace util {
std::string toString(const CanonicalTileID&);
} // namespace util

// Has integer z/x/y coordinates
// overscaledZ describes the zoom level this tile is intented to represent, e.g. when parsing data
// z is never larger than the source's maxzoom
// z/x/y describe the
class OverscaledTileID {
public:
    OverscaledTileID(uint8_t overscaledZ, int16_t wrap, CanonicalTileID);
    OverscaledTileID(uint8_t overscaledZ, int16_t wrap, uint8_t z, uint32_t x, uint32_t y);
    OverscaledTileID(uint8_t z, uint32_t x, uint32_t y);
    explicit OverscaledTileID(const CanonicalTileID&);
    explicit OverscaledTileID(CanonicalTileID&&);
    bool operator==(const OverscaledTileID&) const;
    bool operator!=(const OverscaledTileID&) const;
    bool operator<(const OverscaledTileID&) const;
    bool isChildOf(const OverscaledTileID&) const;
    uint32_t overscaleFactor() const;
    OverscaledTileID scaledTo(uint8_t z) const;
    UnwrappedTileID toUnwrapped() const;
    OverscaledTileID unwrapTo(int16_t wrap);

    uint8_t overscaledZ;
    int16_t wrap;
    CanonicalTileID canonical;
};

::std::ostream& operator<<(::std::ostream& os, const OverscaledTileID& rhs);
namespace util {
std::string toString(const OverscaledTileID&);
} // namespace util

// Has integer z/x/y coordinates
// wrap describes tiles that are left/right of the main tile pyramid, e.g. when wrapping the world
// Used for describing what position tiles are getting rendered at (= calc the matrix)
// z is never larger than the source's maxzoom
class UnwrappedTileID {
public:
    UnwrappedTileID(uint8_t z, int64_t x, int64_t y);
    UnwrappedTileID(int16_t wrap, CanonicalTileID);
    bool operator==(const UnwrappedTileID&) const;
    bool operator!=(const UnwrappedTileID&) const;
    bool operator<(const UnwrappedTileID&) const;
    bool isChildOf(const UnwrappedTileID&) const;
    std::array<UnwrappedTileID, 4> children() const;
    OverscaledTileID overscaleTo(uint8_t z) const;
    float pixelsToTileUnits(float pixelValue, float zoom) const;
    UnwrappedTileID unwrapTo(int16_t wrap);

    int16_t wrap;
    CanonicalTileID canonical;
};

::std::ostream& operator<<(::std::ostream& os, const UnwrappedTileID& rhs);
namespace util {
std::string toString(const UnwrappedTileID&);
} // namespace util

inline CanonicalTileID::CanonicalTileID(uint8_t z_, uint32_t x_, uint32_t y_) : z(z_), x(x_), y(y_) {
    assert(z <= 32);
    assert(x < (1ull << z));
    assert(y < (1ull << z));
}

inline bool CanonicalTileID::operator==(const CanonicalTileID& rhs) const {
    return z == rhs.z && x == rhs.x && y == rhs.y;
}

inline bool CanonicalTileID::operator!=(const CanonicalTileID& rhs) const {
    return z != rhs.z || x != rhs.x || y != rhs.y;
}

inline bool CanonicalTileID::operator<(const CanonicalTileID& rhs) const {
    return std::tie(z, x, y) < std::tie(rhs.z, rhs.x, rhs.y);
}

inline bool CanonicalTileID::isChildOf(const CanonicalTileID& parent) const {
    // We're first testing for z == 0, to avoid a 32 bit shift, which is undefined.
    return parent.z == 0 ||
           (parent.z < z && parent.x == (x >> (z - parent.z)) && parent.y == (y >> (z - parent.z)));
}

inline CanonicalTileID CanonicalTileID::scaledTo(uint8_t targetZ) const {
    if (targetZ <= z) {
        return { targetZ, x >> (z - targetZ), y >> (z - targetZ) }; // parent or same
    } else {
        return { targetZ, x << (targetZ - z), y << (targetZ - z) }; // child
    }
}

inline std::array<CanonicalTileID, 4> CanonicalTileID::children() const {
    const uint8_t childZ = z + 1;
    const uint32_t childX = x * 2;
    const uint32_t childY = y * 2;
    return { {
        { childZ, childX, childY },
        { childZ, childX, childY + 1 },
        { childZ, childX + 1, childY },
        { childZ, childX + 1, childY + 1 },
    } };
}

inline OverscaledTileID::OverscaledTileID(uint8_t overscaledZ_, int16_t wrap_, CanonicalTileID canonical_)
    : overscaledZ(overscaledZ_), wrap(wrap_), canonical(std::move(canonical_)) {
    assert(overscaledZ >= canonical.z);
}

inline OverscaledTileID::OverscaledTileID(uint8_t overscaledZ_, int16_t wrap_, uint8_t z, uint32_t x, uint32_t y)
    : overscaledZ(overscaledZ_), wrap(wrap_), canonical(z, x, y) {
    assert(overscaledZ >= canonical.z);
}

inline OverscaledTileID::OverscaledTileID(uint8_t z, uint32_t x, uint32_t y)
    : overscaledZ(z), wrap(0), canonical(z, x, y) {
}

inline OverscaledTileID::OverscaledTileID(const CanonicalTileID& canonical_)
    : overscaledZ(canonical_.z), wrap(0), canonical(canonical_) {
    assert(overscaledZ >= canonical.z);
}

inline OverscaledTileID::OverscaledTileID(CanonicalTileID&& canonical_)
    : overscaledZ(canonical_.z), wrap(0), canonical(std::forward<CanonicalTileID>(canonical_)) {
    assert(overscaledZ >= canonical.z);
}

inline bool OverscaledTileID::operator==(const OverscaledTileID& rhs) const {
    return overscaledZ == rhs.overscaledZ && wrap == rhs.wrap &&canonical == rhs.canonical;
}

inline bool OverscaledTileID::operator!=(const OverscaledTileID& rhs) const {
    return overscaledZ != rhs.overscaledZ || wrap != rhs.wrap || canonical != rhs.canonical;
}

inline bool OverscaledTileID::operator<(const OverscaledTileID& rhs) const {
    return std::tie(overscaledZ, wrap, canonical) < std::tie(rhs.overscaledZ, rhs.wrap, rhs.canonical);
}

inline uint32_t OverscaledTileID::overscaleFactor() const {
    return 1u << (overscaledZ - canonical.z);
}

inline bool OverscaledTileID::isChildOf(const OverscaledTileID& rhs) const {
    return overscaledZ > rhs.overscaledZ &&
           (canonical == rhs.canonical || canonical.isChildOf(rhs.canonical));
}

inline OverscaledTileID OverscaledTileID::scaledTo(uint8_t z) const {
    return { z, wrap, z >= canonical.z ? canonical : canonical.scaledTo(z) };
}

inline UnwrappedTileID OverscaledTileID::toUnwrapped() const {
    return { wrap, canonical };
}

inline OverscaledTileID OverscaledTileID::unwrapTo(int16_t newWrap) {
    return { overscaledZ, newWrap, canonical };
}

inline UnwrappedTileID::UnwrappedTileID(uint8_t z_, int64_t x_, int64_t y_)
    : wrap((x_ < 0 ? x_ - (1ll << z_) + 1 : x_) / (1ll << z_)),
      canonical(
          z_,
          static_cast<uint32_t>(x_ - wrap * (1ll << z_)),
          y_ < 0 ? 0 : std::min(static_cast<uint32_t>(y_), static_cast<uint32_t>(1ull << z_) - 1)) {
}

inline UnwrappedTileID::UnwrappedTileID(int16_t wrap_, CanonicalTileID canonical_)
    : wrap(wrap_), canonical(std::move(canonical_)) {
}

inline bool UnwrappedTileID::operator==(const UnwrappedTileID& rhs) const {
    return wrap == rhs.wrap && canonical == rhs.canonical;
}

inline bool UnwrappedTileID::operator!=(const UnwrappedTileID& rhs) const {
    return wrap != rhs.wrap || canonical != rhs.canonical;
}

inline bool UnwrappedTileID::operator<(const UnwrappedTileID& rhs) const {
    return std::tie(wrap, canonical) < std::tie(rhs.wrap, rhs.canonical);
}

inline UnwrappedTileID UnwrappedTileID::unwrapTo(int16_t newWrap) {
    return { newWrap, canonical };
}

inline bool UnwrappedTileID::isChildOf(const UnwrappedTileID& parent) const {
    return wrap == parent.wrap && canonical.isChildOf(parent.canonical);
}

inline std::array<UnwrappedTileID, 4> UnwrappedTileID::children() const {
    const uint8_t childZ = canonical.z + 1;
    const uint32_t childX = canonical.x * 2;
    const uint32_t childY = canonical.y * 2;
    return { {
        { wrap, { childZ, childX, childY } },
        { wrap, { childZ, childX, childY + 1 } },
        { wrap, { childZ, childX + 1, childY } },
        { wrap, { childZ, childX + 1, childY + 1 } },
    } };
}

inline OverscaledTileID UnwrappedTileID::overscaleTo(const uint8_t overscaledZ) const {
    assert(overscaledZ >= canonical.z);
    return { overscaledZ, wrap, canonical };
}

inline float UnwrappedTileID::pixelsToTileUnits(const float pixelValue, const float zoom) const {
    return pixelValue * (util::EXTENT / (util::tileSize * std::pow(2, zoom - canonical.z)));
}

} // namespace mbgl

namespace std {

template <>
struct hash<mbgl::CanonicalTileID> {
    size_t operator()(const mbgl::CanonicalTileID& id) const;
};

template <>
struct hash<mbgl::UnwrappedTileID> {
    size_t operator()(const mbgl::UnwrappedTileID& id) const;
};

template <>
struct hash<mbgl::OverscaledTileID> {
    size_t operator()(const mbgl::OverscaledTileID& id) const;
};

} // namespace std

