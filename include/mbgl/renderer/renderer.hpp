#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geo.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class FileSource;
class RendererBackend;
class RendererObserver;
class RenderedQueryOptions;
class Scheduler;
class SourceQueryOptions;
class UpdateParameters;

class Renderer {
public:
    Renderer(RendererBackend&, float pixelRatio_, FileSource&, Scheduler&,
             GLContextMode = GLContextMode::Unique,
             const optional<std::string> programCacheDir = {});
    ~Renderer();

    void markContextLost();

    void setObserver(RendererObserver*);

    void render(const UpdateParameters&);

    // Feature queries
    std::vector<Feature> queryRenderedFeatures(const ScreenLineString&, const RenderedQueryOptions& options = {}) const;
    std::vector<Feature> queryRenderedFeatures(const ScreenCoordinate& point, const RenderedQueryOptions& options = {}) const;
    std::vector<Feature> queryRenderedFeatures(const ScreenBox& box, const RenderedQueryOptions& options = {}) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions& options = {}) const;
    AnnotationIDs queryPointAnnotations(const ScreenBox& box) const;

    // Debug
    void dumpDebugLogs();

    // Memory
    void onLowMemory();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
