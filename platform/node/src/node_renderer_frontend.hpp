#pragma once

#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geo.hpp>

#include <functional>
#include <memory>

namespace mbgl {
    class Renderer;
} //  namespace mbgl

namespace node_mbgl {

class NodeRendererFrontend : public mbgl::RendererFrontend {
public:
    using ViewAccessorFunction = std::function<mbgl::View* ()>;
    NodeRendererFrontend(std::unique_ptr<mbgl::Renderer>, ViewAccessorFunction);

    ~NodeRendererFrontend();
    
    void reset() override;
    
    void setObserver(mbgl::RendererObserver&) override;

    void update(std::shared_ptr<mbgl::UpdateParameters>) override;
    
    // Feature querying
    std::vector<mbgl::Feature> queryRenderedFeatures(const mbgl::ScreenCoordinate&,
                                                     const mbgl::RenderedQueryOptions& options = {}) const;
    std::vector<mbgl::Feature> queryRenderedFeatures(const mbgl::ScreenBox&,
                                                     const mbgl::RenderedQueryOptions& = {}) const;

    void dumpDebugLogs();

private:
    std::unique_ptr<mbgl::Renderer> renderer;
    std::shared_ptr<mbgl::UpdateParameters> updateParameters;
    mbgl::util::AsyncTask asyncInvalidate;
};

} // namespace node_mbgl
