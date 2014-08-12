#ifndef MBGL_UTIL_FILESOURCE
#define MBGL_UTIL_FILESOURCE

#include <mbgl/util/uv.hpp>

#include <string>
#include <memory>
#include <functional>

namespace mbgl {

namespace platform {
struct Response;
}

enum class ResourceType : uint8_t {
    Unknown,
    Tile,
    Glyphs,
    Image,
    JSON
};

class FileSource {
public:
    FileSource();

    void setBase(const std::string &value);
    const std::string &getBase() const;

    void load(ResourceType type, const std::string &url, std::function<void(platform::Response *)> callback, const std::shared_ptr<uv::loop> loop = nullptr);

private:
    // Stores a URL that is used as a base for loading resources with relative path.
    std::string base;

    // Stores the absolute path to the cache directory.
    const std::string cache;
};

}

#endif
