#pragma once

#include <mbgl/map/mode.hpp>

namespace mbgl {

class TransformState;
class Scheduler;
class FileSource;
class AnnotationManager;
class SpriteAtlas;
class GlyphAtlas;

class TileParameters {
public:
    float pixelRatio;
    MapDebugOptions debugOptions;
    const TransformState& transformState;
    Scheduler& workerScheduler;
    FileSource& fileSource;
    const MapMode mode;
    AnnotationManager& annotationManager;
    SpriteAtlas& spriteAtlas;
    GlyphAtlas& glyphAtlas;
};

} // namespace mbgl
