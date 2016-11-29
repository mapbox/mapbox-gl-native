#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <boost/functional/hash.hpp>

namespace std {

template <> struct hash<mbgl::CanonicalTileID> {
    size_t operator()(const mbgl::CanonicalTileID &id) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, id.x);
        boost::hash_combine(seed, id.y);
        boost::hash_combine(seed, id.z);
        return seed;
    }
};
    
template <> struct hash<mbgl::UnwrappedTileID> {
    size_t operator()(const mbgl::UnwrappedTileID &id) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, std::hash<mbgl::CanonicalTileID>{}(id.canonical));
        boost::hash_combine(seed, id.wrap);
        return seed;
    }
};
    
template <> struct hash<mbgl::OverscaledTileID> {
    size_t operator()(const mbgl::OverscaledTileID &id) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, std::hash<mbgl::CanonicalTileID>{}(id.canonical));
        boost::hash_combine(seed, id.overscaledZ);
        return seed;
    }
};

} // namespace std

