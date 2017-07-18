#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/style/light.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/immutable.hpp>

#include <vector>

namespace mbgl {

class Scheduler;
class FileSource;
class AnnotationManager;

class UpdateParameters {
public:
    UpdateParameters(const bool styleLoaded_,
                     const MapMode mode_,
                     const float pixelRatio_,
                     const MapDebugOptions debugOptions_,
                     const TimePoint timePoint_,
                     const TransformState TransformState_,
                     const std::string glyphURL_,
                     const bool spriteLoaded_,
                     const style::TransitionOptions transitionOptions_,
                     const Immutable<style::Light::Impl> light_,
                     const Immutable<std::vector<Immutable<style::Image::Impl>>> images_,
                     const Immutable<std::vector<Immutable<style::Source::Impl>>> sources_,
                     const Immutable<std::vector<Immutable<style::Layer::Impl>>> layers_,
                     Scheduler& scheduler_,
                     FileSource& fileSource_,
                     AnnotationManager& annotationManager_,
                     const uint8_t prefetchZoomDelta_,
                     const bool stillImageRequest_)
        : styleLoaded(styleLoaded_),
          mode(mode_),
          pixelRatio(pixelRatio_),
          debugOptions(debugOptions_),
          timePoint(std::move(timePoint_)),
          transformState(std::move(TransformState_)),
          glyphURL(std::move(glyphURL_)),
          spriteLoaded(spriteLoaded_),
          transitionOptions(std::move(transitionOptions_)),
          light(std::move(light_)),
          images(std::move(images_)),
          sources(std::move(sources_)),
          layers(std::move(layers_)),
          scheduler(scheduler_),
          fileSource(fileSource_),
          annotationManager(annotationManager_),
          prefetchZoomDelta(prefetchZoomDelta_),
          stillImageRequest(stillImageRequest_) {}

    const bool styleLoaded;
    const MapMode mode;
    const float pixelRatio;
    const MapDebugOptions debugOptions;
    const TimePoint timePoint;
    const TransformState transformState;

    const std::string glyphURL;
    const bool spriteLoaded;
    const style::TransitionOptions transitionOptions;
    const Immutable<style::Light::Impl> light;
    const Immutable<std::vector<Immutable<style::Image::Impl>>> images;
    const Immutable<std::vector<Immutable<style::Source::Impl>>> sources;
    const Immutable<std::vector<Immutable<style::Layer::Impl>>> layers;

    Scheduler& scheduler;
    FileSource& fileSource;
    AnnotationManager& annotationManager;

    const uint8_t prefetchZoomDelta;
    
    // For still image requests, render requested
    const bool stillImageRequest;
};

} // namespace mbgl
