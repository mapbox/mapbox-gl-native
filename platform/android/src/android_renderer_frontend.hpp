
#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/geo.hpp>

#include <functional>
#include <memory>
#include <vector>

namespace mbgl {

class View;
class Renderer;
class View;
class RenderedQueryOptions;
class SourceQueryOptions;

namespace android {

class AndroidRendererFrontend : public RendererFrontend {
public:
    using InvalidateCallback = std::function<void ()>;
    AndroidRendererFrontend(std::unique_ptr<Renderer>, InvalidateCallback);
    ~AndroidRendererFrontend() override;

    void reset() override;
    void setObserver(RendererObserver&) override;

    void update(std::shared_ptr<UpdateParameters>) override;
    void render(View& view);

    // Feature querying
    std::vector<Feature> queryRenderedFeatures(const ScreenCoordinate&, const RenderedQueryOptions&) const;
    std::vector<Feature> queryRenderedFeatures(const ScreenBox&, const RenderedQueryOptions&) const;
    std::vector<Feature> querySourceFeatures(const std::string& sourceID, const SourceQueryOptions&) const;
    AnnotationIDs queryPointAnnotations(const ScreenBox& box) const;

    // Memory
    void onLowMemory();

private:
    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<UpdateParameters> updateParameters;
    util::AsyncTask asyncInvalidate;
};

} // namespace android
} // namespace mbgl
