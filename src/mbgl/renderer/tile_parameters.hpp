#pragma once

#include <mbgl/map/mode.hpp>

#include <memory>

namespace mbgl {

class TransformState;
class FileSource;
class AnnotationManager;
class ImageManager;
class GlyphManager;

class TileParameters {
public:
    const float pixelRatio;
    const MapDebugOptions debugOptions;
    const TransformState& transformState;
    std::shared_ptr<FileSource> fileSource;
    const MapMode mode;
    AnnotationManager& annotationManager;
    ImageManager& imageManager;
    GlyphManager& glyphManager;
    const uint8_t prefetchZoomDelta;
};

} // namespace mbgl
