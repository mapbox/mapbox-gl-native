#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <set>
#include <map>

#include <mbgl/tile/tile_id.hpp>

struct MockSourceInfo {
    uint8_t maxZoom = 16;
    uint8_t minZoom = 0;
};

struct MockTileData;

struct MockSource {
    MockSourceInfo info;
    std::map<mbgl::OverscaledTileID, std::unique_ptr<MockTileData>> dataTiles;
    std::set<mbgl::UnwrappedTileID> idealTiles;

    // Test API
    inline MockTileData* createTileData(const mbgl::OverscaledTileID& tileID);
};

struct MockBucket {};


struct MockTileData {
    MockTileData(const mbgl::OverscaledTileID& tileID_) : tileID(tileID_) {}

    bool hasTriedOptional() const {
        return triedOptional;
    }

    bool isRenderable() const {
        return renderable;
    }

    bool renderable = false;
    bool triedOptional = false;
    const mbgl::OverscaledTileID tileID;
};

MockTileData* MockSource::createTileData(const mbgl::OverscaledTileID& tileID) {
    // Replace the existing MockTileData object, if any.
    return (dataTiles[tileID] = std::make_unique<MockTileData>(tileID)).get();
}
