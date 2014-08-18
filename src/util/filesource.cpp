#include <mbgl/util/filesource.hpp>
#include <mbgl/platform/platform.hpp>

#include <fstream>
#include <sstream>

namespace mbgl {

FileSource::FileSource() {}


void FileSource::setBase(const std::string &value) {
    base = value;
}

const std::string &FileSource::getBase() const {
    return base;
}

void FileSource::load(ResourceType /*type*/, const std::string &url, std::function<void(platform::Response *)> callback, const std::shared_ptr<uv::loop> loop) {
    // convert relative URLs to absolute URLs

    const std::string absoluteURL = [&]() -> std::string {
        const size_t separator = url.find("://");
        if (separator == std::string::npos) {
            // Relative URL.
            return base + url;
        } else {
            return url;
        }
    }();

    const size_t separator = absoluteURL.find("://");
    const std::string protocol = separator != std::string::npos ? absoluteURL.substr(0, separator) : "";

    if (protocol == "file") {
        // load from disk
        const std::string path = absoluteURL.substr(separator + 3);
        std::ifstream file(path);

        platform::Response response(callback);
        if (!file.good()) {
            response.error_message = "file not found (" + path + ")";
        } else {
            std::stringstream data;
            data << file.rdbuf();
            response.code = 200;
            response.body = data.str();
        }

        callback(&response);
    } else {
        // load from the internet
        platform::request_http(absoluteURL, callback, loop);
    }
}

}