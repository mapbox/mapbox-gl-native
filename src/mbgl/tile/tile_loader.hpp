#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/tile/tile.hpp>

namespace mbgl {

class FileSource;
class AsyncRequest;
class Response;
class Tileset;
class TileParameters;

template <typename T>
class TileLoader : private util::noncopyable {
public:
    TileLoader(T&,
               const OverscaledTileID&,
               const TileParameters&,
               const Tileset&);
    ~TileLoader();

    using Necessity = Resource::Necessity;

    void setNecessity(Necessity newNecessity) {
        if (newNecessity != necessity) {
            necessity = newNecessity;
            if (necessity == Necessity::Required) {
                makeRequired();
            } else {
                makeOptional();
            }
        }
    }

private:
    // called when the tile is one of the ideal tiles that we want to show definitely. the tile source
    // should try to make every effort (e.g. fetch from internet, or revalidate existing resources).
    void makeRequired();

    // called when the zoom level no longer corresponds to the displayed one, but
    // we're still interested in retaining the tile, e.g. for backfill.
    // subclassed TileSources should cancel actions they are taking to provide
    // an up-to-date version or load new data
    void makeOptional();

    void loadOptional();
    void loadedData(const Response&);
    void loadRequired();

    T& tile;
    Necessity necessity;
    Resource resource;
    FileSource& fileSource;
    std::unique_ptr<AsyncRequest> request;
};

} // namespace mbgl
