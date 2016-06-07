#pragma once

#include <mbgl/tile/tile_source.hpp>
#include <mbgl/storage/resource.hpp>

namespace mbgl {

class FileSource;
class AsyncRequest;
class Response;

template <typename T>
class FileBasedTileSource : public TileSource {
public:
    FileBasedTileSource(T&, const Resource&, FileSource&);
    virtual ~FileBasedTileSource() = default;

protected:
    void makeRequired() override;
    void makeOptional() override;

private:
    void loadOptional();
    void loadedData(const Response&);
    void loadRequired();

private:
    T& tileData;
    Resource resource;
    FileSource& fileSource;
    std::unique_ptr<AsyncRequest> request;
};

} // namespace mbgl
