#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/source.hpp>

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/platform/log.hpp>

using namespace mbgl;

TileData::TileData(const TileID& id_, const SourceInfo& source_)
    : id(id_),
      name(id),
      state(State::initial),
      source(source_),
      env(Environment::Get()),
      debugBucket(debugFontBuffer) {
    // Initialize tile debug coordinates
    debugFontBuffer.addText(name.c_str(), 50, 200, 5);
}

TileData::~TileData() {
    cancel();
}

const std::string TileData::toString() const {
    return std::string { "[tile " } + name + "]";
}

void TileData::request(Worker& worker, float pixelRatio, std::function<void()> callback) {
    std::string url = source.tileURL(id, pixelRatio);
    state = State::loading;

    req = env.request({ Resource::Kind::Tile, url }, [url, callback, &worker, this](const Response &res) {
        req = nullptr;

        if (res.status != Response::Successful) {
            Log::Error(Event::HttpRequest, "[%s] tile loading failed: %s", url.c_str(), res.message.c_str());
            return;
        }

        state = State::loaded;
        data = res.data;

        // Schedule tile parsing in another thread
        reparse(worker, callback);
    });
}

void TileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
    }
    if (req) {
        env.cancelRequest(req);
        req = nullptr;
    }
}

void TileData::reparse(Worker& worker, std::function<void()> callback) {
    util::ptr<TileData> tile = shared_from_this();
    worker.send(
        [this]() {
            EnvironmentScope scope(env, ThreadType::TileWorker, "TileWorker_" + name);
            parse();
        },
        [tile, callback]() {
             // `tile` is bound in this lambda to ensure that if it's the last owning pointer,
             // destruction happens on the map thread, not the worker thread. It is _not_ bound
             // in the above lambda, because we do not want the possibility to arise that the
             // after callback could execute and release the penultimate reference before the
             // work callback has been destructed.
            callback();
        });
}
