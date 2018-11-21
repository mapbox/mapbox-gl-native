#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/hash.hpp>

namespace std {

size_t hash<mbgl::CanonicalTileID>::operator()(const mbgl::CanonicalTileID& id) const {
    std::size_t seed = 0;
    mbgl::util::hash_combine(seed, id.x);
    mbgl::util::hash_combine(seed, id.y);
    mbgl::util::hash_combine(seed, id.z);
    return seed;
}

size_t hash<mbgl::UnwrappedTileID>::operator()(const mbgl::UnwrappedTileID& id) const {
    std::size_t seed = 0;
    mbgl::util::hash_combine(seed, std::hash<mbgl::CanonicalTileID>{}(id.canonical));
    mbgl::util::hash_combine(seed, id.wrap);
    return seed;
}

size_t hash<mbgl::OverscaledTileID>::operator()(const mbgl::OverscaledTileID& id) const {
    std::size_t seed = 0;
    mbgl::util::hash_combine(seed, std::hash<mbgl::CanonicalTileID>{}(id.canonical));
    mbgl::util::hash_combine(seed, id.overscaledZ);
    return seed;
}

} // namespace std
