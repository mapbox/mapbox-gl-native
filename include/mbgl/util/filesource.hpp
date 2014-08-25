#ifndef MBGL_UTIL_FILESOURCE
#define MBGL_UTIL_FILESOURCE

#include <mbgl/util/uv.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <string>
#include <memory>
#include <functional>

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

namespace mbgl {

namespace platform {
struct Response;
}

enum class ResourceType : uint8_t {
    Unknown = 0,
    Tile = 1,
    Glyphs = 2,
    Image = 3,
    JSON = 4
};

class FileSource : private util::noncopyable, public std::enable_shared_from_this<FileSource> {
public:
    FileSource();
    ~FileSource();

    void setBase(const std::string &value);
    const std::string &getBase() const;

    void load(ResourceType type, const std::string &url, std::function<void(platform::Response *)> callback, const std::shared_ptr<uv::loop> loop = nullptr);

private:
    void closeDatabase();
    void createSchema();
    bool loadFile(ResourceType type, const std::string &url, std::function<void(platform::Response *)> callback);
    void saveFile(ResourceType type, const std::string &url, platform::Response *res);

private:
    // Stores a URL that is used as a base for loading resources with relative path.
    std::string base;

    // Stores the absolute path to the cache directory.
    const std::string cache;

    sqlite3 *db = nullptr;
};

}

#endif
