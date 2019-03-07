#pragma once

#include <mbgl/map/mode.hpp>

#include <memory>

namespace mbgl {

class FileSource;
class TransformState;
class AnnotationManager;
class ImageManager;
class GlyphManager;

class TileParameters {
public:
    const float pixelRatio;
    std::shared_ptr<FileSource> fileSource;
    const MapDebugOptions debugOptions;
    const TransformState& transformState;
    const MapMode mode;
    AnnotationManager& annotationManager;
    ImageManager& imageManager;
    GlyphManager& glyphManager;
    const uint8_t prefetchZoomDelta;
};

} // namespace mbgl
