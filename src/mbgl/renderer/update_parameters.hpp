#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/style/light.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/layer.hpp>

namespace mbgl {

class Scheduler;
class FileSource;
class AnnotationManager;

class UpdateParameters {
public:
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
};

} // namespace mbgl
