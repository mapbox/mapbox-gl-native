#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/optional.hpp>


/**
 The RenderFrontend is passed to the Map to facilitate rendering in a platform
 dependent way.
 */
class MGLRenderFrontend : public mbgl::RendererFrontend
{
public:
    MGLRenderFrontend(std::unique_ptr<mbgl::Renderer> renderer_, MGLMapView* nativeView_, mbgl::gfx::RendererBackend& mbglBackend_, bool async = false)
        : renderer(std::move(renderer_))
        , nativeView(nativeView_)
        , mbglBackend(mbglBackend_) {
        if (async) {
            asyncInvalidate.emplace([&]() {
                [nativeView setNeedsRerender];
            });
        }
    }
    
    void reset() override {
        if (renderer) {
            renderer.reset();
        }
    }
    
    void update(std::shared_ptr<mbgl::UpdateParameters> updateParameters_) override {
        updateParameters = std::move(updateParameters_);
        if (asyncInvalidate) {
            asyncInvalidate->send();
        } else {
            [nativeView setNeedsRerender];
        }
    }
    
    void setObserver(mbgl::RendererObserver& observer) override {
        if (!renderer) return;
        renderer->setObserver(&observer);
    }
    
    void render() {
        if (!renderer || !updateParameters) return;
        
        mbgl::gfx::BackendScope guard { mbglBackend, mbgl::gfx::BackendScope::ScopeType::Implicit };
        
        // onStyleImageMissing might be called during a render. The user implemented method
        // could trigger a call to MGLRenderFrontend#update which overwrites `updateParameters`.
        // Copy the shared pointer here so that the parameters aren't destroyed while `render(...)` is
        // still using them.
        auto updateParameters_ = updateParameters;
        renderer->render(*updateParameters_);
    }
    
    mbgl::Renderer* getRenderer() {
        return renderer.get();
    }
    
    void reduceMemoryUse() {
        if (!renderer)  return;
        renderer->reduceMemoryUse();
    }
    
private:
    std::unique_ptr<mbgl::Renderer> renderer;
    __weak MGLMapView *nativeView = nullptr;
    mbgl::gfx::RendererBackend& mbglBackend;
    std::shared_ptr<mbgl::UpdateParameters> updateParameters;
    mbgl::optional<mbgl::util::AsyncTask> asyncInvalidate;
};
