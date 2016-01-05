#include <mbgl/storage/offline_file_source.hpp>
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

class OfflineFileRequest : public FileRequest {
public:
    OfflineFileRequest(std::unique_ptr<WorkRequest> workRequest_)
        : workRequest(std::move(workRequest_)) {
    }

private:
    std::unique_ptr<WorkRequest> workRequest;
};

OfflineFileSource::OfflineFileSource(const std::string& path)
    : thread(std::make_unique<util::Thread<Impl>>(util::ThreadContext{ "OfflineFileSource", util::ThreadType::Unknown, util::ThreadPriority::Low }, path)) {
}

OfflineFileSource::~OfflineFileSource() = default;

class OfflineFileSource::Impl {
public:
    explicit Impl(const std::string& path);
    ~Impl();

    void handleRequest(Resource, Callback);

private:
    void respond(Statement&, Callback);

    const std::string path;
    std::unique_ptr<::mapbox::sqlite::Database> db;
};

OfflineFileSource::Impl::Impl(const std::string& path_)
    : path(path_) {
}

OfflineFileSource::Impl::~Impl() {
    try {
        db.reset();
    } catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
    }
}

void OfflineFileSource::Impl::handleRequest(Resource resource, Callback callback) {
    try {
        if (!db) {
            db = std::make_unique<Database>(path.c_str(), ReadOnly);
        }

        if (resource.kind == Resource::Kind::Tile) {
            const auto canonicalURL = util::mapbox::canonicalURL(resource.url);
            auto parts = util::split(canonicalURL, "/");
            const int8_t  z = atoi(parts[parts.size() - 3].c_str());
            const int32_t x = atoi(parts[parts.size() - 2].c_str());
            const int32_t y = atoi(util::split(util::split(parts[parts.size() - 1], ".")[0], "@")[0].c_str());

            const auto id = TileID(z, x, (pow(2, z) - y - 1), z); // flip y for MBTiles

            Statement getStmt = db->prepare("SELECT `tile_data` FROM `tiles` WHERE `zoom_level` = ? AND `tile_column` = ? AND `tile_row` = ?");

            getStmt.bind(1, (int)id.z);
            getStmt.bind(2, (int)id.x);
            getStmt.bind(3, (int)id.y);

            respond(getStmt, callback);

        } else if (resource.kind != Resource::Kind::Unknown) {
            std::string key = "";
            if (resource.kind == Resource::Kind::Glyphs) {
                key = "gl_glyph";
            } else if (resource.kind == Resource::Kind::Source) {
                key = "gl_source";
            } else if (resource.kind == Resource::Kind::SpriteImage) {
                key = "gl_sprite_image";
            } else if (resource.kind == Resource::Kind::SpriteJSON) {
                key = "gl_sprite_metadata";
            } else if (resource.kind == Resource::Kind::Style) {
                key = "gl_style";
            }
            assert(key.length());

            Statement getStmt = db->prepare("SELECT `value` FROM `metadata` WHERE `name` = ?");

            const auto name = key + "_" + util::mapbox::canonicalURL(resource.url);
            getStmt.bind(1, name.c_str());

            respond(getStmt, callback);
        }
    } catch (const std::exception& ex) {
        Log::Error(Event::Database, ex.what());

        Response response;
        response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);
        callback(response);
    }
}

void OfflineFileSource::Impl::respond(Statement& statement, Callback callback) {
    if (statement.run()) {
        Response response;
        response.data = std::make_shared<std::string>(statement.get<std::string>(0));
        callback(response);
    } else {
        Response response;
        response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound);
        callback(response);
    }
}

std::unique_ptr<FileRequest> OfflineFileSource::request(const Resource& resource, Callback callback) {
    return std::make_unique<OfflineFileRequest>(thread->invokeWithCallback(&Impl::handleRequest, callback, resource));
}

} // namespace mbgl
