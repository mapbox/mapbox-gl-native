#pragma once

#include <mbgl/actor/actor.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>

#include <functional>
#include <memory>
#include <vector>
#include <string>

#include "map_renderer.hpp"

namespace mbgl {

class FileSource;
class Scheduler;
class RenderedQueryOptions;
class SourceQueryOptions;

namespace android {

class AndroidRendererBackend;

class AndroidRendererFrontend : public RendererFrontend {
public:

    AndroidRendererFrontend(MapRenderer&);
    ~AndroidRendererFrontend() override;

    void reset() override;
    void setObserver(RendererObserver&) override;

    void update(std::shared_ptr<UpdateParameters>) override;

    // Feature querying
    std::vector<Feature> queryRenderedFeatures(const ScreenCoordinate&, const RenderedQueryOptions&) const;
    std::vector<Feature> queryRenderedFeatures(const ScreenBox&, const RenderedQueryOptions&) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions&) const;
    AnnotationIDs queryPointAnnotations(const ScreenBox& box) const;

    // Memory
    void onLowMemory();

    // Snapshot - Callback will be called on calling thread through RunLoop
    using SnapshotCallback = std::function<void (PremultipliedImage)>;
    void requestSnapshot(SnapshotCallback);

private:
    std::unique_ptr<RendererObserver> rendererObserver;
    MapRenderer& mapRenderer;

    util::RunLoop* mapRunLoop;

    // TODO
    std::unique_ptr<SnapshotCallback> snapshotCallback;
};

} // namespace android
} // namespace mbgl
