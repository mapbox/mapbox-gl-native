#include <mbgl/map/tile_data_deleter.hpp>

#include <mbgl/map/tile_data.hpp>

namespace mbgl {

TileDataDeleter::~TileDataDeleter() {
    // Force the canceling of all pending TileData
    // before destroying. We have assert()s on Debug
    // mode on TileData to make sure it doesn't block
    // at the destructor unless explicitly canceled.
    for (auto& entry : tileDataMap) {
        entry.first->tryCancel(true);
    }
}

void TileDataDeleter::add(TileData* data) {
    std::unique_ptr<TileData> tileData(data);

    // This custom deleter will try to cancel the task if it is
    // non-blocking, otherwise we keep it and subscribe to the
    // event when the work is completed. When the event triggers
    // we delete the TileData because this time it won't block.
    if (!tileData->tryCancel()) {
        data->setObserver(this);
        tileDataMap.emplace(data, std::move(tileData));
    }
}

void TileDataDeleter::onTileDataWorkCompleted(TileData* data) {
    tileDataMap.erase(data);
}

} // namespace mbgl
