#include <mbgl/storage/frontline_file_source.hpp>
#include <mbgl/storage/request.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/map/tile_id.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/platform/log.hpp>

#include <cassert>
#include <stdlib.h>

#include "sqlite3.hpp"
#include <sqlite3.h>

namespace mbgl {

using namespace mapbox::sqlite;

FrontlineFileSource::FrontlineFileSource(const std::string& path)
    : thread(std::make_unique<util::Thread<Impl>>(util::ThreadContext{ "FrontlineFileSource", util::ThreadType::Unknown, util::ThreadPriority::Low }, path)) {
}

FrontlineFileSource::~FrontlineFileSource() {
    MBGL_VERIFY_THREAD(tid);
}

class FrontlineFileSource::Impl {
public:
    explicit Impl(const std::string& path);
    ~Impl();

    bool handlesResource(const Resource&);
    void handleRequest(Request* req);
    void cancelRequest(Request* req);

private:
    void openDatabase();
    static TileID tileIDForResource(const Resource&);

private:
    const std::string path;
    std::unique_ptr<::mapbox::sqlite::Database> db;
};

FrontlineFileSource::Impl::Impl(const std::string& path_)
    : path(path_) {
}

FrontlineFileSource::Impl::~Impl() {
    try {
        db.reset();
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }
}

void FrontlineFileSource::Impl::openDatabase() {
    db = std::make_unique<Database>(path.c_str(), ReadOnly);
}

TileID FrontlineFileSource::Impl::tileIDForResource(const Resource& res) {
    const auto canonicalURL = util::mapbox::canonicalURL(res.url);
    auto parts = util::split(canonicalURL, "/");
    const int8_t  z = atoi(parts[parts.size() - 3].c_str());
    const int32_t x = atoi(parts[parts.size() - 2].c_str());
    const int32_t y = atoi(util::split(util::split(parts[parts.size() - 1], ".")[0], "@")[0].c_str());

    return TileID(z, x, (pow(2, z) - y - 1), z); // flip y for MBTiles
}

bool FrontlineFileSource::Impl::handlesResource(const Resource& res) {
    try {
        if (res.kind == Resource::Kind::Tile && res.url.find(".png") != std::string::npos) {
            if (!db) {
                openDatabase();
            }

            Statement getStmt = db->prepare("SELECT COUNT(`tile_data`) FROM `tiles` WHERE `zoom_level` = ? AND `tile_column` = ? AND `tile_row` = ?");

            TileID id = tileIDForResource(res);
            getStmt.bind(1, (int)id.z);
            getStmt.bind(2, (int)id.x);
            getStmt.bind(3, (int)id.y);

            if (getStmt.run() && getStmt.get<int>(0) > 0) {
                return true;
            }
        }
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    } catch (std::runtime_error& ex) {
        Log::Error(Event::Database, ex.what());
    }

    return false;
}

void FrontlineFileSource::Impl::handleRequest(Request* req) {
    if (!db) {
        openDatabase();
    }

    std::shared_ptr<Response> res = std::make_shared<Response>();

    try {
        Statement getStmt = db->prepare("SELECT `tile_data` FROM `tiles` WHERE `zoom_level` = ? AND `tile_column` = ? AND `tile_row` = ?");

        TileID id = tileIDForResource(req->resource);
        getStmt.bind(1, (int)id.z);
        getStmt.bind(2, (int)id.x);
        getStmt.bind(3, (int)id.y);

        if (getStmt.run()) {
            res->data = std::make_shared<std::string>(std::move(getStmt.get<std::string>(0)));
        } else {
            res->error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);
        }
    } catch (const std::exception& err) {
        res->error = std::make_unique<Response::Error>(Response::Error::Reason::Other, err.what());
    }

    req->notify(res);
}

void FrontlineFileSource::Impl::cancelRequest(Request* req) {
    // assume local/offline sources are too fast to be cancellable
    req->destruct();
}

bool FrontlineFileSource::handlesResource(const Resource& res) {
    return thread->invokeSync<bool>(&Impl::handlesResource, res);
}

Request* FrontlineFileSource::request(const Resource& resource, uv_loop_t* l, Callback callback) {
    assert(l);

    if (!callback) {
        throw util::MisuseException("FileSource callback can't be empty");
    }

    std::string url;

    switch (resource.kind) {
        case Resource::Kind::Tile:
            url = resource.url;
            break;

        default:
            url = "";
    }

    if (url.length()) {
        auto req = new Request({ resource.kind, url }, l, std::move(callback));
        thread->invoke(&Impl::handleRequest, req);
        return req;
    } else {
        return nullptr;
    }
}

void FrontlineFileSource::cancel(Request* req) {
    assert(req);
    req->cancel();
    thread->invoke(&Impl::cancelRequest, req);
}

} // namespace mbgl
