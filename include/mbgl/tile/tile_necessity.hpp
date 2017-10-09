#pragma once

namespace mbgl {

// Tiles can have two states: optional or required.
// - optional means that only low-cost actions should be taken to obtain the data
//   (e.g. load from cache, but accept stale data)
// - required means that every effort should be taken to obtain the data (e.g. load
//   from internet and keep the data fresh if it expires)
enum class TileNecessity : bool {
    Optional = false,
    Required = true,
};

} // namespace mbgl
