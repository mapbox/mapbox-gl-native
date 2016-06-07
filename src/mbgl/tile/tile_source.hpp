#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/storage/resource.hpp>

namespace mbgl {

class FileSource;
class AsyncRequest;
class Response;
class Tileset;

namespace style {
class UpdateParameters;
}

template <typename T>
class TileSource : private util::noncopyable {
public:
    // TileSources can have two states: optional or required.
    // - optional means that only low-cost actions should be taken to obtain the data
    //   (e.g. load from cache, but accept stale data)
    // - required means that every effort should be taken to obtain the data (e.g. load
    //   from internet and keep the data fresh if it expires)
    enum class Necessity : bool {
        Optional = false,
        Required = true,
    };

    TileSource(T&,
               const OverscaledTileID&,
               const style::UpdateParameters&,
               const Tileset&);

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
    bool isOptional() const {
        return necessity == Necessity::Optional;
    }

    bool isRequired() const {
        return necessity == Necessity::Required;
    }

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

    T& tileData;
    Necessity necessity;
    Resource resource;
    FileSource& fileSource;
    std::unique_ptr<AsyncRequest> request;
};

} // namespace mbgl
