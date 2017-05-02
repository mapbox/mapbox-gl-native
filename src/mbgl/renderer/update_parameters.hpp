#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/map/update.hpp>

namespace mbgl {

class TransformState;
class Scheduler;
class FileSource;
class AnnotationManager;

class UpdateParameters {
public:
    const MapMode mode;
    const Update updateFlags;
    const float pixelRatio;
    const MapDebugOptions debugOptions;
    const TimePoint timePoint;
    const TransformState& transformState;
    Scheduler& scheduler;
    FileSource& fileSource;
    AnnotationManager& annotationManager;
};

} // namespace mbgl
