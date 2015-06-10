#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_task.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/map/live_tile.hpp>
#include <mbgl/util/pbf.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

#include <cassert>
#include <future>

namespace mbgl {

class Worker::Impl {
public:
    Impl(uv_loop_t*, FileSource* fs) {
        // FIXME: Workers should not access the FileSource but it
        // is currently needed because of GlyphsPBF. See #1664.
        util::ThreadContext::setFileSource(fs);
    }

    bool parseRasterTile(RasterBucket* bucket, std::string data) {
        return bucket->setImage(data);
    }

    TileParseResult parseVectorTile(TileWorker* worker, std::string data) {
        try {
            pbf tilePBF(reinterpret_cast<const unsigned char *>(data.data()), data.size());
            return worker->parse(VectorTile(tilePBF));
        } catch (const std::exception& ex) {
            return TileParseResult(ex.what());
        }
    }

    TileParseResult parseLiveTile(TileWorker* worker, const LiveTile* tile) {
        return worker->parse(*tile);
    }

    void redoPlacement(TileWorker* worker, float angle, bool collisionDebug) {
        worker->redoPlacement(angle, collisionDebug);
    }
};

Worker::Worker(std::size_t count) {
    util::ThreadContext context = {"Worker", util::ThreadType::Worker, util::ThreadPriority::Low};
    for (std::size_t i = 0; i < count; i++) {
        threads.emplace_back(std::make_unique<util::Thread<Impl>>(context, util::ThreadContext::getFileSource()));
    }
}

Worker::~Worker() = default;

std::unique_ptr<WorkRequest> Worker::parseRasterTile(RasterBucket& bucket, std::string data, std::function<void (bool)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithResult(&Worker::Impl::parseRasterTile, callback, &bucket, data);
}

std::unique_ptr<WorkRequest> Worker::parseVectorTile(TileWorker& worker, std::string data, std::function<void (TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithResult(&Worker::Impl::parseVectorTile, callback, &worker, data);
}

std::unique_ptr<WorkRequest> Worker::parseLiveTile(TileWorker& worker, const LiveTile& tile, std::function<void (TileParseResult)> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithResult(&Worker::Impl::parseLiveTile, callback, &worker, &tile);
}

std::unique_ptr<WorkRequest> Worker::redoPlacement(TileWorker& worker, float angle, bool collisionDebug, std::function<void ()> callback) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithResult(&Worker::Impl::redoPlacement, callback, &worker, angle, collisionDebug);
}

} // end namespace mbgl
