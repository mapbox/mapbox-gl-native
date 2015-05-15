#include "mbtiles_source_impl.hpp"
#include <mbgl/util/thread.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/compression.hpp>

#include "sqlite3.hpp"
#include <sqlite3.h>

namespace mbgl {
  using namespace mapbox::sqlite;

  MBTilesSource::MBTilesSource(const std::string& path_)
  : thread(util::make_unique<util::Thread<Impl>>("MBTiles Source", util::ThreadPriority::Low, path_)) {
  }

  MBTilesSource::~MBTilesSource() = default;

  MBTilesSource::Impl::Impl(uv_loop_t*, const std::string& path_)
  : path(path_) {
  }

  MBTilesSource::Impl::~Impl() {
    // Deleting these SQLite objects may result in exceptions, but we're in a destructor, so we can't throw anything.
    try {
      getStmt.reset();
      db.reset();
    } catch (mapbox::sqlite::Exception& ex) {
      Log::Error(Event::Database, ex.code, ex.what());
    }
  }

  void MBTilesSource::Impl::openDatabase() {
    Log::Info(Event::Database, "Opening sqlite: %s", path.c_str());
    db = util::make_unique<Database>(path.c_str(), ReadOnly);
  }

  void MBTilesSource::get(const Resource &resource, Callback callback) {
    // Can be called from any thread, but most likely from the file source thread.
    // Will try to load the tile from the SQLite database and call the callback when done.
    // Note that the callback is probably going to invoked from another thread, so the caller
    // must make sure that it can run in that thread.
    thread->invokeWithResult(&Impl::get, std::move(callback), resource);
  }

  std::unique_ptr<Response> MBTilesSource::Impl::get(const Resource &resource) {
    if (resource.kind == Resource::Kind::Tile) {
      const int64_t now = std::chrono::duration_cast<std::chrono::seconds>(SystemClock::now().time_since_epoch()).count();
      auto response = util::make_unique<Response>();
      response->status = Response::Status::Successful;
      response->expires = now + 100000;
      try {
        // This is called in the SQLite event loop.
        if (!db) {
          openDatabase();
        }

        if (!getStmt) {
          // Initialize the statement
          getStmt = util::make_unique<Statement>(db->prepare("SELECT tile_data FROM tiles WHERE zoom_level = ? AND tile_column = ? AND tile_row = ?"));
        } else {
          getStmt->reset();
        }
        int32_t y = (1 << resource.z) - resource.y - 1; // flip y for mbtiles

        getStmt->bind(1, (int)resource.z);
        getStmt->bind(2, resource.x);
        getStmt->bind(3, y);
        Log::Info(Event::Database, "Finding tile (%d, %d, %d [%d])", resource.z, resource.x, resource.y, y);
        if (getStmt->run()) {
          Log::Info(Event::Database, "==> tile (%d, %d, %d [%d])", resource.z, resource.x, resource.y, y);
          response->data = util::decompress(getStmt->get<std::string>(0));
          Log::Info(Event::Database, "Data: %d", response->data.length());
        }
      }  catch (mapbox::sqlite::Exception& ex) {
        Log::Error(Event::Database, ex.code, ex.what());
      }
      return std::move(response);
    }
    return nullptr;
  }

  void MBTilesSource::put(const Resource &resource, std::shared_ptr<const Response> response, Hint hint) {
#pragma unused(resource, response, hint)
  }

  void MBTilesSource::Impl::refresh(const Resource& resource, int64_t expires) {
#pragma unused(resource, expires)
  }
}