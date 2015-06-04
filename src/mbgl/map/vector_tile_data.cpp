#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/map/tile_parser.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/util/pbf.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/style/style.hpp>

using namespace mbgl;

VectorTileData::VectorTileData(const TileID& id_,
                               Style& style_,
                               GlyphAtlas& glyphAtlas_,
                               GlyphStore& glyphStore_,
                               SpriteAtlas& spriteAtlas_,
                               util::ptr<Sprite> sprite_,
                               const SourceInfo& source_,
                               float angle,
                               bool collisionDebug)
    : TileData(id_, source_),
      glyphAtlas(glyphAtlas_),
      glyphStore(glyphStore_),
      spriteAtlas(spriteAtlas_),
      sprite(sprite_),
      style(style_),
      collision(std::make_unique<CollisionTile>(id_.z, 4096, source_.tile_size * id.overscaling, angle, collisionDebug)),
      lastAngle(angle),
      currentAngle(angle) {
}

VectorTileData::~VectorTileData() {
    // Cancel in most derived class destructor so that worker tasks are joined before
    // any member data goes away.
    cancel();
    glyphAtlas.removeGlyphs(reinterpret_cast<uintptr_t>(this));
}

void VectorTileData::parse() {
    if (getState() != State::loaded && getState() != State::partial) {
        return;
    }

    try {
        // Parsing creates state that is encapsulated in TileParser. While parsing,
        // the TileParser object writes results into this objects. All other state
        // is going to be discarded afterwards.
        VectorTile vectorTile(pbf((const uint8_t *)data.data(), data.size()));
        const VectorTile* vt = &vectorTile;
        TileParser parser(*vt, *this, style, glyphAtlas, glyphStore, spriteAtlas, sprite);
        parser.parse();

        if (getState() == State::obsolete) {
            return;
        }

        if (parser.isPartialParse()) {
            setState(State::partial);
        } else {
            setState(State::parsed);
        }
    } catch (const std::exception& ex) {
        std::stringstream message;
        message << "Failed to parse [" << int(id.sourceZ) << "/" << id.x << "/" << id.y << "]: " << ex.what();
        setError(message.str());
    }
}

Bucket* VectorTileData::getBucket(StyleLayer const& layer) {
    if (!isReady() || !layer.bucket) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(bucketsMutex);

    const auto it = buckets.find(layer.bucket->name);
    if (it == buckets.end()) {
        return nullptr;
    }

    assert(it->second);
    return it->second.get();
}

size_t VectorTileData::countBuckets() const {
    std::lock_guard<std::mutex> lock(bucketsMutex);

    return buckets.size();
}

void VectorTileData::setBucket(StyleLayer const& layer, std::unique_ptr<Bucket> bucket) {
    assert(layer.bucket);

    std::lock_guard<std::mutex> lock(bucketsMutex);

    if (buckets.find(layer.bucket->name) != buckets.end()) {
        return;
    }

    buckets[layer.bucket->name] = std::move(bucket);
}

void VectorTileData::setState(const State& state_) {
    TileData::setState(state_);

    if (isImmutable()) {
        collision->reset(0, 0);
    }
}

void VectorTileData::redoPlacement() {
    redoPlacement(lastAngle, lastCollisionDebug);
}

void VectorTileData::redoPlacement(float angle, bool collisionDebug) {
    if (angle != currentAngle || collisionDebug != currentCollisionDebug) {
        lastAngle = angle;
        lastCollisionDebug = collisionDebug;

        if (getState() != State::parsed || redoingPlacement) return;

        redoingPlacement = true;
        currentAngle = angle;
        currentCollisionDebug = collisionDebug;

        auto callback = std::bind(&VectorTileData::endRedoPlacement, this);
        workRequest = style.workers.send([this, angle, collisionDebug] { workerRedoPlacement(angle, collisionDebug); }, callback);

    }
}

void VectorTileData::workerRedoPlacement(float angle, bool collisionDebug) {
    collision->reset(angle, 0);
    collision->setDebug(collisionDebug);
    for (const auto& layer_desc : style.layers) {
        auto bucket = getBucket(*layer_desc);
        if (bucket) {
            bucket->placeFeatures();
        }
    }
}

void VectorTileData::endRedoPlacement() {
    for (const auto& layer_desc : style.layers) {
        auto bucket = getBucket(*layer_desc);
        if (bucket) {
            bucket->swapRenderData();
        }
    }
    redoingPlacement = false;
    redoPlacement();
}
