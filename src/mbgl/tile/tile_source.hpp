#pragma once

#include <mbgl/util/noncopyable.hpp>

namespace mbgl {

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

protected:
    TileSource() : necessity(Necessity::Optional) {
    }

public:
    virtual ~TileSource() = default;

    bool isOptional() const {
        return necessity == Necessity::Optional;
    }

    bool isRequired() const {
        return necessity == Necessity::Required;
    }

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

    bool isLoaded() const {
        // "loaded" is considered true once the initial load action completed, regardless of whether
        // the TileData is required or optional, and whether any actual TileData was loaded in the
        // case of optional TileData
        return loaded;
    }

protected:
    // called when the tile is one of the ideal tiles that we want to show definitely. the tile source
    // should try to make every effort (e.g. fetch from internet, or revalidate existing resources).
    virtual void makeRequired() {}

    // called when the zoom level no longer corresponds to the displayed one, but
    // we're still interested in retaining the tile, e.g. for backfill.
    // subclassed TileSources should cancel actions they are taking to provide
    // an up-to-date version or load new data
    virtual void makeOptional() {}

protected:
    Necessity necessity;
    bool loaded = false;
};

class GeometryTileData;
class RasterTileData;

class GeometryTileSource : public TileSource {
public:
    using data_type = GeometryTileData;

protected:
    GeometryTileSource(data_type& tileData_)
        : tileData(tileData_) {
    }

public:
    virtual ~GeometryTileSource() = default;

protected:
    data_type& tileData;
};

class RasterTileSource : public TileSource {
public:
    using data_type = RasterTileData;

protected:
    RasterTileSource(data_type& tileData_)
        : tileData(tileData_){};

public:
    virtual ~RasterTileSource() = default;

protected:
    data_type& tileData;
};

} // namespace mbgl
