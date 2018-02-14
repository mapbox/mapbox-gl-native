#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/optional.hpp>


/**
 The RenderFrontend is passed to the Map to facilitate rendering in a platform
 dependent way.
 */
class MGLRenderFrontend : public mbgl::RendererFrontend
{
public:
    MGLRenderFrontend(std::unique_ptr<mbgl::Renderer> renderer_, MGLMapView* nativeView_, mbgl::RendererBackend& mbglBackend_, bool async = false)
        : renderer(std::move(renderer_))
        , nativeView(nativeView_)
        , mbglBackend(mbglBackend_) {
        if (async) {
            asyncInvalidate.emplace([&]() {
                [nativeView setNeedsGLDisplay];
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
            [nativeView setNeedsGLDisplay];
        }
    }
    
    void setObserver(mbgl::RendererObserver& observer) override {
        if (!renderer) return;
        renderer->setObserver(&observer);
    }
    
    void render() {
        if (!renderer || !updateParameters) return;
        
        mbgl::BackendScope guard { mbglBackend, mbgl::BackendScope::ScopeType::Implicit };
        
        renderer->render(*updateParameters);
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
    mbgl::RendererBackend& mbglBackend;
    std::shared_ptr<mbgl::UpdateParameters> updateParameters;
    mbgl::optional<mbgl::util::AsyncTask> asyncInvalidate;
};
