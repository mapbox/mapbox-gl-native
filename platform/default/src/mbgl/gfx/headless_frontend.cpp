#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_state.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/util/monotonic_timer.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mbgl {

HeadlessFrontend::HeadlessFrontend(float pixelRatio_,
                                   gfx::HeadlessBackend::SwapBehaviour swapBehavior,
                                   const gfx::ContextMode contextMode,
                                   const optional<std::string> localFontFamily)
    : HeadlessFrontend({256, 256}, pixelRatio_, swapBehavior, contextMode, localFontFamily) {}

HeadlessFrontend::HeadlessFrontend(Size size_,
                                   float pixelRatio_,
                                   gfx::HeadlessBackend::SwapBehaviour swapBehavior,
                                   const gfx::ContextMode contextMode,
                                   const optional<std::string> localFontFamily)
    : size(size_),
      pixelRatio(pixelRatio_),
      frameTime(0),
      backend(gfx::HeadlessBackend::Create(
          {static_cast<uint32_t>(size.width * pixelRatio), static_cast<uint32_t>(size.height * pixelRatio)},
          swapBehavior,
          contextMode)),
      asyncInvalidate([this] {
          if (renderer && updateParameters) {
              auto startTime = mbgl::util::MonotonicTimer::now();
              gfx::BackendScope guard{*getBackend()};

              // onStyleImageMissing might be called during a render. The user implemented method
              // could trigger a call to MGLRenderFrontend#update which overwrites `updateParameters`.
              // Copy the shared pointer here so that the parameters aren't destroyed while `render(...)` is
              // still using them.
              auto updateParameters_ = updateParameters;
              renderer->render(*updateParameters_);

              auto endTime = mbgl::util::MonotonicTimer::now();
              frameTime = (endTime - startTime).count();
          }
      }),
      renderer(std::make_unique<Renderer>(*getBackend(), pixelRatio, localFontFamily)) {}

HeadlessFrontend::~HeadlessFrontend() = default;

void HeadlessFrontend::reset() {
    assert(renderer);
    renderer.reset();
}

void HeadlessFrontend::update(std::shared_ptr<UpdateParameters> updateParameters_) {
    updateParameters = updateParameters_;
    asyncInvalidate.send();
}

void HeadlessFrontend::setObserver(RendererObserver& observer_) {
    assert(renderer);
    renderer->setObserver(&observer_);
}

double HeadlessFrontend::getFrameTime() const {
    return frameTime;
}

Size HeadlessFrontend::getSize() const {
    return size;
}

Renderer* HeadlessFrontend::getRenderer() {
    assert(renderer);
    return renderer.get();
}

gfx::RendererBackend* HeadlessFrontend::getBackend() {
    return backend->getRendererBackend();
}

CameraOptions HeadlessFrontend::getCameraOptions() {
    if (updateParameters)
        return RendererState::getCameraOptions(*updateParameters);

    static CameraOptions nullCamera;
    return nullCamera;
}

bool HeadlessFrontend::hasImage(const std::string& id) {
    if (updateParameters) {
        return RendererState::hasImage(*updateParameters, id);
    }

    return false;
}

bool HeadlessFrontend::hasLayer(const std::string& id) {
    if (updateParameters) {
        return RendererState::hasLayer(*updateParameters, id);
    }

    return false;
}

bool HeadlessFrontend::hasSource(const std::string& id) {
    if (updateParameters) {
        return RendererState::hasSource(*updateParameters, id);
    }

    return false;
}
ScreenCoordinate HeadlessFrontend::pixelForLatLng(const LatLng& coordinate) {
    if (updateParameters) {
        return RendererState::pixelForLatLng(*updateParameters, coordinate);
    }

    return ScreenCoordinate {};
}

LatLng HeadlessFrontend::latLngForPixel(const ScreenCoordinate& point) {
    if (updateParameters) {
        return RendererState::latLngForPixel(*updateParameters, point);
    }

    return LatLng {};
}

void HeadlessFrontend::setSize(Size size_) {
    if (size != size_) {
        size = size_;
        backend->setSize({ static_cast<uint32_t>(size_.width * pixelRatio),
                           static_cast<uint32_t>(size_.height * pixelRatio) });
    }
}

PremultipliedImage HeadlessFrontend::readStillImage() {
    return backend->readStillImage();
}

HeadlessFrontend::RenderResult HeadlessFrontend::render(Map& map) {
    HeadlessFrontend::RenderResult result;
    std::exception_ptr error;

    map.renderStill([&](std::exception_ptr e) {
        if (e) {
            error = e;
        } else {
            result.image = backend->readStillImage();
            result.stats = getBackend()->getContext().renderingStats();
        }
    });

    while (!result.image.valid() && !error) {
        util::RunLoop::Get()->runOnce();
    }

    if (error) {
        std::rethrow_exception(error);
    }

    return result;
}

void HeadlessFrontend::renderOnce(Map&) {
    util::RunLoop::Get()->runOnce();
}

optional<TransformState> HeadlessFrontend::getTransformState() const {
    if (updateParameters) {
        return updateParameters->transformState;
    } else {
        return {};
    }
}

} // namespace mbgl
