#pragma once

#include <mbgl/map/mode.hpp>

namespace mbgl {

class TransformState;
class Scheduler;
class FileSource;
class AnnotationManager;
class ImageManager;
class GlyphManager;

class TileParameters {
public:
    TileParameters(const float pixelRatio_,
                   const MapDebugOptions debugOptions_,
                   const TransformState& transformState_,
                   Scheduler& workerScheduler_,
                   FileSource& fileSource_,
                   const MapMode mode_,
                   AnnotationManager& annotationManager_,
                   ImageManager& imageManager_,
                   GlyphManager& glyphManager_,
                   const uint8_t prefetchZoomDelta_)
        : pixelRatio(pixelRatio_),
          debugOptions(debugOptions_),
          transformState(std::move(transformState_)),
          workerScheduler(workerScheduler_),
          fileSource(fileSource_),
          mode(mode_),
          annotationManager(annotationManager_),
          imageManager(imageManager_),
          glyphManager(glyphManager_),
          prefetchZoomDelta(prefetchZoomDelta_) {}

    const float pixelRatio;
    const MapDebugOptions debugOptions;
    const TransformState& transformState;
    Scheduler& workerScheduler;
    FileSource& fileSource;
    const MapMode mode;
    AnnotationManager& annotationManager;
    ImageManager& imageManager;
    GlyphManager& glyphManager;
    const uint8_t prefetchZoomDelta;
};

} // namespace mbgl
