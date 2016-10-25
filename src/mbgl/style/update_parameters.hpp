#pragma once

#include <mbgl/map/mode.hpp>

namespace mbgl {

class TransformState;
class Scheduler;
class FileSource;
class AnnotationManager;

namespace style {

class Style;

class UpdateParameters {
public:
    UpdateParameters(float pixelRatio_,
                          MapDebugOptions debugOptions_,
                          const TransformState& transformState_,
                          Scheduler& workerScheduler_,
                          FileSource& fileSource_,
                          const MapMode mode_,
                          AnnotationManager& annotationManager_,
                          Style& style_)
        : pixelRatio(pixelRatio_),
          debugOptions(debugOptions_),
          transformState(transformState_),
          workerScheduler(workerScheduler_),
          fileSource(fileSource_),
          mode(mode_),
          annotationManager(annotationManager_),
          style(style_) {}

    float pixelRatio;
    MapDebugOptions debugOptions;
    const TransformState& transformState;
    Scheduler& workerScheduler;
    FileSource& fileSource;
    const MapMode mode;
    AnnotationManager& annotationManager;

    // TODO: remove
    Style& style;
};

} // namespace style
} // namespace mbgl
