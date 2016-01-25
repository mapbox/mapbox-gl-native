#ifndef MBGL_MAP_TILE_DATA_DELETER
#define MBGL_MAP_TILE_DATA_DELETER

#include <mbgl/map/tile_data.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <unordered_map>

namespace mbgl {

class TileData;

class TileDataDeleter : public TileData::Observer, private util::noncopyable {
public:
    virtual ~TileDataDeleter();

    void add(TileData*);

    // TileData::Observer implementation.
    void onTileDataWorkCompleted(TileData*) override;

private:
    std::unordered_map<TileData*, std::unique_ptr<TileData>> tileDataMap;
};

} // namespace mbgl

#endif
