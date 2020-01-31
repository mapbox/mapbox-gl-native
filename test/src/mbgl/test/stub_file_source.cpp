#include <mbgl/test/stub_file_source.hpp>

namespace mbgl {

using namespace std::chrono_literals;

class StubFileRequest : public AsyncRequest {
public:
    StubFileRequest(StubFileSource& fileSource_)
        : fileSource(fileSource_) {
    }

    ~StubFileRequest() override {
        fileSource.remove(this);
    }

    StubFileSource& fileSource;
};

StubFileSource::StubFileSource(ResponseType type_)
        : type(type_) {
    if (type == ResponseType::Synchronous) {
        return;
    }

    timer.start(1ms, 1ms, [this] {
        // Explicit copy to avoid iterator invalidation if ~StubFileRequest gets called within the loop.
        auto pending_ = pending;
        for (auto& pair : pending_) {
            optional<Response> res = std::get<1>(pair.second)(std::get<0>(pair.second));
            if (res) {
                // This must be before calling the callback, because it's possible that the callback
                // could:
                //
                //   1. Deallocate the AsyncRequest itself, thus removing it from pending
                //   2. Allocate a new AsyncRequest at the same memory location
                //
                // If remove(pair.first) was called after both those things happened, it would
                // remove the newly allocated request rather than the intended request.
                if (!res->error) {
                    remove(pair.first);
                }

                std::get<2>(pair.second)(*res);
            }
        }
    });
}

StubFileSource::~StubFileSource() = default;

std::unique_ptr<AsyncRequest> StubFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<StubFileRequest>(*this);
    if (type == ResponseType::Synchronous) {
        optional<Response> res = response(resource);
        if (res) {
            callback(*res);
        }
    } else {
        pending.emplace(req.get(), std::make_tuple(resource, response, callback));
    }
    return std::move(req);
}

void StubFileSource::remove(AsyncRequest* req) {
    auto it = pending.find(req);
    if (it != pending.end()) {
        pending.erase(it);
    }
}

optional<Response> StubFileSource::defaultResponse(const Resource& resource) {
    switch (resource.kind) {
    case Resource::Kind::Style:
        if (!styleResponse) throw std::runtime_error("unexpected style request");
        return styleResponse(resource);
    case Resource::Kind::Source:
        if (!sourceResponse) throw std::runtime_error("unexpected source request");
        return sourceResponse(resource);
    case Resource::Kind::Tile:
        if (!tileResponse) throw std::runtime_error("unexpected tile request");
        return tileResponse(resource);
    case Resource::Kind::Glyphs:
        if (!glyphsResponse) throw std::runtime_error("unexpected glyphs request");
        return glyphsResponse(resource);
    case Resource::Kind::SpriteJSON:
        if (!spriteJSONResponse) throw std::runtime_error("unexpected sprite JSON request");
        return spriteJSONResponse(resource);
    case Resource::Kind::SpriteImage:
        if (!spriteImageResponse) throw std::runtime_error("unexpected sprite image request");
        return spriteImageResponse(resource);
    case Resource::Kind::Image:
        if (!imageResponse) throw std::runtime_error("unexpected image request");
        return imageResponse(resource);
    case Resource::Kind::Unknown:
        throw std::runtime_error("unknown resource type");
    }

    // The above switch is exhaustive, but placate GCC nonetheless:
    return Response();
}

} // namespace mbgl
