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
    const float pixelRatio;
    const MapDebugOptions debugOptions;
    const TimePoint timePoint;

    const std::string glyphURL;
    const bool spriteLoaded;
    const style::TransitionOptions transitionOptions;
    const Immutable<style::Light::Impl> light;
    const std::vector<Immutable<style::Image::Impl>> images;
    const std::vector<Immutable<style::Source::Impl>> sources;
    const std::vector<Immutable<style::Layer::Impl>> layers;

    Scheduler& scheduler;
    FileSource& fileSource;
    AnnotationManager& annotationManager;
    const TransformState& transformState;
};

} // namespace mbgl
