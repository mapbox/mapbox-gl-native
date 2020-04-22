#pragma once

#include <mbgl/storage/resource.hpp>
#include <mbgl/tile/tile.hpp>

namespace mbgl {

class FileSource;
class AsyncRequest;
class Response;
class Tileset;
class TileParameters;

template <typename T>
class TileLoader {
public:
    TileLoader(const TileLoader&) = delete;
    TileLoader& operator=(const TileLoader&) = delete;
    TileLoader(T&,
               const OverscaledTileID&,
               const TileParameters&,
               const Tileset&);
    ~TileLoader();

    void setNecessity(TileNecessity newNecessity);
    void setUpdateParameters(const TileUpdateParameters&);

private:
    // called when the tile is one of the ideal tiles that we want to show definitely. the tile source
    // should try to make every effort (e.g. fetch from internet, or revalidate existing resources).
    void makeRequired();

    // called when the zoom level no longer corresponds to the displayed one, but
    // we're still interested in retaining the tile, e.g. for backfill.
    // subclassed TileSources should cancel actions they are taking to provide
    // an up-to-date version or load new data
    void makeOptional();

    void loadFromCache();
    void loadedData(const Response&);
    void loadFromNetwork();

    bool hasPendingNetworkRequest() const {
        return resource.loadingMethod == Resource::LoadingMethod::NetworkOnly && request;
    }

    T& tile;
    TileNecessity necessity;
    Resource resource;
    std::shared_ptr<FileSource> fileSource;
    std::unique_ptr<AsyncRequest> request;
    TileUpdateParameters updateParameters{Duration::zero(), false};
};

} // namespace mbgl
