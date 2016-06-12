#include <mbgl/test/stub_file_source.hpp>

namespace mbgl {

using namespace std::chrono_literals;

class StubFileRequest : public AsyncRequest {
public:
    StubFileRequest(StubFileSource& fileSource_)
        : fileSource(fileSource_) {
    }

    ~StubFileRequest() override {
        fileSource.pending.erase(this);
    }

    StubFileSource& fileSource;
};

StubFileSource::StubFileSource() {
    timer.start(10ms, 10ms, [this] {
        // Explicit copy to avoid iterator invalidation if ~StubFileRequest gets called within the loop.
        auto pending_ = pending;
        for (auto& pair : pending_) {
            optional<Response> res = std::get<1>(pair.second)(std::get<0>(pair.second));
            if (res) {
                std::get<2>(pair.second)(*res);
            }
        }
    });
}

StubFileSource::~StubFileSource() = default;

std::unique_ptr<AsyncRequest> StubFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<StubFileRequest>(*this);
    pending.emplace(req.get(), std::make_tuple(resource, response, callback));
    return std::move(req);
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
    case Resource::Kind::Unknown:
        throw std::runtime_error("unknown resource type");
    }

    // The above switch is exhaustive, but placate GCC nonetheless:
    return Response();
}

} // namespace mbgl
