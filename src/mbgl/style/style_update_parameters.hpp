#ifndef STYLE_UPDATE_PARAMETERS
#define STYLE_UPDATE_PARAMETERS

#include <mbgl/map/mode.hpp>

namespace mbgl {

class TransformState;
class Worker;
class TexturePool;
class MapData;
class Style;

class StyleUpdateParameters final {
public:
    StyleUpdateParameters(float pixelRatio_,
                          MapDebugOptions debugOptions_,
                          TimePoint animationTime_,
                          const TransformState& transformState_,
                          Worker& worker_,
                          TexturePool& texturePool_,
                          bool shouldReparsePartialTiles_,
                          MapData& data_,
                          Style& style_)
        : pixelRatio(pixelRatio_),
          debugOptions(debugOptions_),
          animationTime(animationTime_),
          transformState(transformState_),
          worker(worker_),
          texturePool(texturePool_),
          shouldReparsePartialTiles(shouldReparsePartialTiles_),
          data(data_),
          style(style_) {}

    float pixelRatio;
    MapDebugOptions debugOptions;
    TimePoint animationTime;
    const TransformState& transformState;
    Worker& worker;
    TexturePool& texturePool;
    bool shouldReparsePartialTiles;

    // TODO: remove
    MapData& data;
    Style& style;
};

} // namespace mbgl

#endif
