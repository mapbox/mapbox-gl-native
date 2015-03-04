#include <mbgl/map/environment.hpp>
#include <mbgl/storage/file_source.hpp>

#include <uv.h>

#include <cassert>

namespace mbgl {

Environment::Environment(FileSource &fs) : fileSource(fs), loop(uv_loop_new()) {
}

void Environment::setup() {
    mapThread = std::this_thread::get_id();
}

bool Environment::inMapThread() const {
    return std::this_thread::get_id() == mapThread;
}

void Environment::requestAsync(const Resource &resource, std::function<void(const Response &)> callback) {
    fileSource.request(resource, *this, std::move(callback));
}

Request *Environment::request(const Resource &resource, std::function<void(const Response &)> callback) {
    assert(inMapThread());
    return fileSource.request(resource, loop, *this, std::move(callback));
}

void Environment::cancelRequest(Request *req) {
    assert(inMapThread());
    fileSource.cancel(req);
}

void Environment::terminate() {
    fileSource.abort(*this);
}

}
