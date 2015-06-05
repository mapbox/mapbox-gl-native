#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/util/pbf.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/style/style.hpp>

using namespace mbgl;

VectorTileData::VectorTileData(const TileID& id_,
                               Style& style_,
                               const SourceInfo& source_,
                               float angle,
                               bool collisionDebug)
    : TileData(id_),
      source(source_),
      worker(style_.workers),
      workerData(id_,
                 style_,
                 source.max_zoom,
                 state,
                 std::make_unique<CollisionTile>(id_.z, 4096,
                                    source_.tile_size * id.overscaling,
                                    angle, collisionDebug)),
      lastAngle(angle),
      currentAngle(angle) {
}

VectorTileData::~VectorTileData() {
    cancel();
}

void VectorTileData::request(Worker&,
                       float pixelRatio,
                       const std::function<void()>& callback) {
    std::string url = source.tileURL(id, pixelRatio);
    state = State::loading;

    FileSource* fs = util::ThreadContext::getFileSource();
    req = fs->request({ Resource::Kind::Tile, url }, util::RunLoop::current.get()->get(), [url, callback, this](const Response &res) {
        req = nullptr;

        if (res.status != Response::Successful) {
            std::stringstream message;
            message <<  "Failed to load [" << url << "]: " << res.message;
            error = message.str();
            state = State::obsolete;
            callback();
            return;
        }

        state = State::loaded;
        data = res.data;

        reparse(worker, callback);
    });
}

bool VectorTileData::reparse(Worker&, std::function<void()> callback) {
    if (parsing.test_and_set(std::memory_order_acquire)) {
        return false;
    }

    workRequest = worker.send([this] {
        if (getState() != TileData::State::loaded && getState() != TileData::State::partial) {
            return;
        }

        TileParseResult result;

        try {
            VectorTile vectorTile(pbf((const uint8_t *)data.data(), data.size()));
            result = workerData.parse(vectorTile);
        } catch (const std::exception& ex) {
            std::stringstream message;
            message << "Failed to parse [" << int(id.sourceZ) << "/" << id.x << "/" << id.y << "]: " << ex.what();
            result = message.str();
        }

        if (getState() == TileData::State::obsolete) {
            return;
        } else if (result.is<State>()) {
            state = result.get<State>();
        } else {
            error = result.get<std::string>();
            state = State::obsolete;
        }

        parsing.clear(std::memory_order_release);
    }, callback);

    return true;
}

Bucket* VectorTileData::getBucket(const StyleLayer& layer) {
    if (!isReady() || !layer.bucket) {
        return nullptr;
    }

    return workerData.getBucket(layer);
}

size_t VectorTileData::countBuckets() const {
    return workerData.countBuckets();
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
        workRequest = worker.send([this, angle, collisionDebug] { workerData.redoPlacement(angle, collisionDebug); }, callback);
    }
}

void VectorTileData::endRedoPlacement() {
    for (const auto& layer_desc : workerData.style.layers) {
        auto bucket = getBucket(*layer_desc);
        if (bucket) {
            bucket->swapRenderData();
        }
    }
    redoingPlacement = false;
    redoPlacement();
}

void VectorTileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
    }
    if (req) {
        util::ThreadContext::getFileSource()->cancel(req);
        req = nullptr;
    }
    workRequest.reset();
}
