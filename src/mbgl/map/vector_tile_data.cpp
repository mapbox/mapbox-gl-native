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
                               float pitch,
                               bool collisionDebug)
    : TileData(id_),
      worker(style_.workers),
      tileWorker(id_,
                 source_.source_id,
                 style_,
                 style_.layers,
                 state,
                 std::make_unique<CollisionTile>(angle, pitch, collisionDebug)),
      source(source_),
      lastAngle(angle),
      currentAngle(angle),
      currentPitch(pitch),
      currentCollisionDebug(collisionDebug) {
}

VectorTileData::~VectorTileData() {
    cancel();
}

void VectorTileData::request(float pixelRatio, const std::function<void()>& callback) {
    std::string url = source.tileURL(id, pixelRatio);
    state = State::loading;

    FileSource* fs = util::ThreadContext::getFileSource();
    req = fs->request({ Resource::Kind::Tile, url }, util::RunLoop::getLoop(), [url, callback, this](const Response &res) {
        util::ThreadContext::getFileSource()->cancel(req);
        req = nullptr;

        if (res.status == Response::NotFound) {
            state = State::parsed;
            callback();
            return;
        }

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

void VectorTileData::redoPlacement(float angle, float pitch, bool collisionDebug) {
    if (angle == currentAngle &&
        pitch == currentPitch &&
        collisionDebug == currentCollisionDebug)
        return;

    lastAngle = angle;
    lastPitch = pitch;
    lastCollisionDebug = collisionDebug;

    if (state != State::parsed || redoingPlacement)
        return;

    redoingPlacement = true;
    currentAngle = angle;
    currentPitch = pitch;
    currentCollisionDebug = collisionDebug;

    workRequest = worker.redoPlacement(tileWorker, angle, pitch, collisionDebug, [this] {
        for (const auto& layer : tileWorker.layers) {
            auto bucket = getBucket(*layer);
            if (bucket) {
                bucket->swapRenderData();
            }
        }
        redoingPlacement = false;
        redoPlacement(lastAngle, lastPitch, lastCollisionDebug);
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
