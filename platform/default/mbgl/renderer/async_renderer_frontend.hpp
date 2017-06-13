#pragma once

#include <mbgl/map/view.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/util/async_task.hpp>

#include <memory>
#include <vector>

namespace mbgl {
    
class Renderer;

// Default implementation for RendererFrontend
class AsyncRendererFrontend : public mbgl::RendererFrontend {
public:
    AsyncRendererFrontend(std::unique_ptr<Renderer>, View&);
    ~AsyncRendererFrontend() override;

    void reset() override;

    void update(std::shared_ptr<UpdateParameters> updateParameters_) override;
    
    void setObserver(RendererObserver& observer_) override;
    
    Renderer* getRenderer();
    
private:
    std::unique_ptr<Renderer> renderer;
    View& view;
    std::shared_ptr<UpdateParameters> updateParameters;
    util::AsyncTask asyncInvalidate;
};
    
} // namespace mbgl
