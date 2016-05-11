#ifndef MBGL_MAP_CHANGE
#define MBGL_MAP_CHANGE

#include <cstdint>

namespace mbgl {

enum MapChange : uint8_t {
    MapChangeRegionWillChange = 0,
    MapChangeRegionWillChangeAnimated = 1,
    MapChangeRegionIsChanging = 2,
    MapChangeRegionDidChange = 3,
    MapChangeRegionDidChangeAnimated = 4,
    MapChangeWillStartLoadingMap = 5,
    MapChangeDidFinishLoadingMap = 6,
    MapChangeDidFailLoadingMap = 7,
    MapChangeWillStartRenderingFrame = 8,
    MapChangeDidFinishRenderingFrame = 9,
    MapChangeDidFinishRenderingFrameFullyRendered = 10,
    MapChangeWillStartRenderingMap = 11,
    MapChangeDidFinishRenderingMap = 12,
    MapChangeDidFinishRenderingMapFullyRendered = 13,
};

} // namespace mbgl

#endif
