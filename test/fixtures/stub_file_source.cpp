#include "stub_file_source.hpp"

namespace mbgl {

using namespace std::chrono_literals;

class StubFileRequest : public FileRequest {
public:
    StubFileRequest(StubFileSource& fileSource_)
        : fileSource(fileSource_) {
    }

    ~StubFileRequest() {
        fileSource.pending.erase(this);
    }

    StubFileSource& fileSource;
};

StubFileSource::StubFileSource() {
    timer.start(10ms, 10ms, [this] {
        // Explicit move to avoid iterator invalidation if ~StubFileRequest gets called within the loop.
        auto pending_ = std::move(pending);
        for (auto& pair : pending_) {
            if (pair.second.first) {
                pair.second.second(*pair.second.first);
            }
        }
    });
}

StubFileSource::~StubFileSource() = default;

std::unique_ptr<FileRequest> StubFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<StubFileRequest>(*this);
    pending.emplace(req.get(), std::make_pair(response(resource), callback));
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
