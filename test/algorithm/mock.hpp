#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <set>
#include <map>

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/range.hpp>

struct MockTileData;

struct MockSource {
    mbgl::Range<uint8_t> zoomRange { 0, 16 };
    std::map<mbgl::OverscaledTileID, std::unique_ptr<MockTileData>> dataTiles;
    std::set<mbgl::UnwrappedTileID> idealTiles;

    // Test API
    inline MockTileData* createTileData(const mbgl::OverscaledTileID& tileID);
};

struct MockBucket {};


struct MockTileData {
    MockTileData(const mbgl::OverscaledTileID& tileID_) : tileID(tileID_) {}

    bool hasTriedCache() const {
        return triedOptional;
    }

    bool isRenderable() const {
        return renderable;
    }

    bool isLoaded() const {
        return loaded;
    }

    bool renderable = false;
    bool triedOptional = false;
    bool loaded = false;
    const mbgl::OverscaledTileID tileID;
};

MockTileData* MockSource::createTileData(const mbgl::OverscaledTileID& tileID) {
    // Replace the existing MockTileData object, if any.
    return (dataTiles[tileID] = std::make_unique<MockTileData>(tileID)).get();
}
