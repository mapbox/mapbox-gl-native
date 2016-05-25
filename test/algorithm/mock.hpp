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

struct MockRenderable {
    MockRenderable(mbgl::UnwrappedTileID id_, MockTileData& data_) : id(id_), data(data_) {}

    const mbgl::UnwrappedTileID id;
    MockTileData& data;

    bool operator==(const MockRenderable& rhs) const {
        return &data == &rhs.data;
    }
};

::std::ostream& operator<<(::std::ostream& os, const MockRenderable&) {
    return os << "Renderable{}";
}

struct MockSource {
    MockSourceInfo info;
    std::map<mbgl::OverscaledTileID, std::unique_ptr<MockTileData>> dataTiles;
    std::set<mbgl::UnwrappedTileID> idealTiles;
    std::map<mbgl::UnwrappedTileID, MockRenderable> renderables;

    // Test API
    inline MockTileData* createTileData(const mbgl::OverscaledTileID& tileID);
};

struct MockBucket {};


struct MockTileData {
    bool isRenderable() {
        return ready;
    }

    bool ready = false;
};

MockTileData* MockSource::createTileData(const mbgl::OverscaledTileID& tileID) {
    // Replace the existing MockTileData object, if any.
    return (dataTiles[tileID] = std::make_unique<MockTileData>()).get();
}
