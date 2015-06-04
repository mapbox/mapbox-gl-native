#include <mbgl/text/collision_tile.hpp>
#include <mbgl/map/tile_worker.hpp>
#include <mbgl/map/tile_parser.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>

using namespace mbgl;

TileWorker::TileWorker(const TileID& id_,
                       Style& style_,
                       const uint16_t maxZoom_,
                       const std::atomic<TileData::State>& state_,
                       std::unique_ptr<CollisionTile> collision_)
    : id(id_),
      style(style_),
      maxZoom(maxZoom_),
      state(state_),
      collision(std::move(collision_)) {
}

TileWorker::~TileWorker() {
    style.glyphAtlas->removeGlyphs(reinterpret_cast<uintptr_t>(this));
}

void TileWorker::setBucket(const StyleLayer& layer, std::unique_ptr<Bucket> bucket) {
    assert(layer.bucket);

    std::lock_guard<std::mutex> lock(bucketsMutex);

    if (buckets.find(layer.bucket->name) != buckets.end()) {
        return;
    }

    buckets[layer.bucket->name] = std::move(bucket);
}

Bucket* TileWorker::getBucket(const StyleLayer& layer) const {
    std::lock_guard<std::mutex> lock(bucketsMutex);

    const auto it = buckets.find(layer.bucket->name);
    if (it == buckets.end()) {
        return nullptr;
    }

    assert(it->second);
    return it->second.get();
}

size_t TileWorker::countBuckets() const {
    std::lock_guard<std::mutex> lock(bucketsMutex);
    return buckets.size();
}

TileParseResult TileWorker::parse(const GeometryTile& geometryTile) {
    // Parsing creates state that is encapsulated in TileParser. While parsing,
    // the TileParser object writes results into this objects. All other state
    // is going to be discarded afterwards.
    TileParser parser(geometryTile, *this, style);
    parser.parse();

    if (parser.isPartialParse()) {
        return TileData::State::partial;
    } else {
        return TileData::State::parsed;
    }
}

void TileWorker::redoPlacement(float angle, bool collisionDebug) {
    collision->reset(angle, 0);
    collision->setDebug(collisionDebug);
    for (const auto& layer_desc : style.layers) {
        auto bucket = getBucket(*layer_desc);
        if (bucket) {
            bucket->placeFeatures();
        }
    }
}
