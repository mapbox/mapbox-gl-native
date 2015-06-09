#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/text/collision_tile.hpp>
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
      worker(style_.workers),
      tileWorker(id_,
                 style_,
                 source_.max_zoom,
                 state,
                 std::make_unique<CollisionTile>(id_.z, 4096,
                                    source_.tile_size * id.overscaling,
                                    angle, collisionDebug)),
      source(source_),
      lastAngle(angle),
      currentAngle(angle) {
}

VectorTileData::~VectorTileData() {
    cancel();
}

void VectorTileData::request(float pixelRatio, const std::function<void()>& callback) {
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

        reparse(callback);
    });
}

bool VectorTileData::reparse(std::function<void()> callback) {
    if (parsing || (state != State::loaded && state != State::partial)) {
        return false;
    }

    parsing = true;

    workRequest = worker.parseVectorTile(tileWorker, data, [this, callback] (TileParseResult result) {
        parsing = false;

        if (state == State::obsolete) {
            return;
        }

        if (result.is<State>()) {
            state = result.get<State>();
        } else {
            std::stringstream message;
            message <<  "Failed to parse [" << std::string(id) << "]: " << result.get<std::string>();
            error = message.str();
            state = State::obsolete;
        }

        callback();
    });

    return true;
}

Bucket* VectorTileData::getBucket(const StyleLayer& layer) {
    if (!isReady() || !layer.bucket) {
        return nullptr;
    }

    return tileWorker.getBucket(layer);
}

size_t VectorTileData::countBuckets() const {
    return tileWorker.countBuckets();
}

void VectorTileData::redoPlacement(float angle, bool collisionDebug) {
    if (angle == currentAngle && collisionDebug == currentCollisionDebug)
        return;

    lastAngle = angle;
    lastCollisionDebug = collisionDebug;

    if (state != State::parsed || redoingPlacement)
        return;

    redoingPlacement = true;
    currentAngle = angle;
    currentCollisionDebug = collisionDebug;

    workRequest = worker.redoPlacement(tileWorker, angle, collisionDebug, [this] {
        for (const auto& layer : tileWorker.style.layers) {
            auto bucket = getBucket(*layer);
            if (bucket) {
                bucket->swapRenderData();
            }
        }
        redoingPlacement = false;
        redoPlacement(lastAngle, lastCollisionDebug);
    });
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
