#pragma once

#include <mbgl/actor/actor.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/run_loop.hpp>

#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <mbgl/util/geojson.hpp>

#include "map_renderer.hpp"

namespace mbgl {

class RenderedQueryOptions;
class SourceQueryOptions;

namespace util {

class AsyncTask;

} // namespace util

namespace android {

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
    AnnotationIDs queryShapeAnnotations(const ScreenBox& box) const;


    // Feature extension query
    FeatureExtensionValue
    queryFeatureExtensions(const std::string& sourceID,
                           const Feature& feature,
                           const std::string& extension,
                           const std::string& extensionField,
                           const optional<std::map<std::string, mbgl::Value>>& args) const;

    // Memory
    void reduceMemoryUse();

private:
    MapRenderer& mapRenderer;
    util::RunLoop* mapRunLoop;
    std::unique_ptr<util::AsyncTask> updateAsyncTask;
    std::shared_ptr<UpdateParameters> updateParams;
};

} // namespace android
} // namespace mbgl
