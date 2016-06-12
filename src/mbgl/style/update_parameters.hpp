#pragma once

#include <mbgl/map/mode.hpp>

namespace mbgl {

class TransformState;
class Worker;
class FileSource;
namespace gl { class TexturePool; }
class AnnotationManager;

namespace style {

class Style;

class UpdateParameters {
public:
    UpdateParameters(float pixelRatio_,
                          MapDebugOptions debugOptions_,
                          TimePoint animationTime_,
                          const TransformState& transformState_,
                          Worker& worker_,
                          FileSource& fileSource_,
                          gl::TexturePool& texturePool_,
                          bool shouldReparsePartialTiles_,
                          const MapMode mode_,
                          AnnotationManager& annotationManager_,
                          Style& style_)
        : pixelRatio(pixelRatio_),
          debugOptions(debugOptions_),
          animationTime(std::move(animationTime_)),
          transformState(transformState_),
          worker(worker_),
          fileSource(fileSource_),
          texturePool(texturePool_),
          shouldReparsePartialTiles(shouldReparsePartialTiles_),
          mode(mode_),
          annotationManager(annotationManager_),
          style(style_) {}

    float pixelRatio;
    MapDebugOptions debugOptions;
    TimePoint animationTime;
    const TransformState& transformState;
    Worker& worker;
    FileSource& fileSource;
    gl::TexturePool& texturePool;
    bool shouldReparsePartialTiles;
    const MapMode mode;
    AnnotationManager& annotationManager;

    // TODO: remove
    Style& style;
};

} // namespace style
} // namespace mbgl
