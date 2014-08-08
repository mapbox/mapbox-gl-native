#include <mbgl/util/filesource.hpp>
#include <mbgl/platform/platform.hpp>

namespace mbgl {

FileSource::FileSource(const std::shared_ptr<uv::loop> &loop)
    : loop(loop) {}


void FileSource::setBase(const std::string &value) {
    base = value;
}

const std::string &FileSource::getBase() const {
    return base;
}

void FileSource::load(ResourceType type, const std::string &url, std::function<void(platform::Response *)> callback) {
    // convert relative URLs to absolute URLs

    // try to load from disk (not yet)

    // load from the internet
    platform::request_http(url, callback, loop);
}

}