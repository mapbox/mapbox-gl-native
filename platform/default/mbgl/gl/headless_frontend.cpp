#include <mbgl/gl/headless_frontend.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mbgl {

HeadlessFrontend::HeadlessFrontend(float pixelRatio_, FileSource& fileSource, Scheduler& scheduler)
    : HeadlessFrontend({ 256, 256 }, pixelRatio_, fileSource, scheduler) {
}

HeadlessFrontend::HeadlessFrontend(Size size_, float pixelRatio_, FileSource& fileSource, Scheduler& scheduler)
    : size(size_),
    pixelRatio(pixelRatio_),
    backend({ static_cast<uint32_t>(size.width * pixelRatio),
              static_cast<uint32_t>(size.height * pixelRatio) }),
    asyncInvalidate([this] {
        if (renderer && updateParameters) {
            mbgl::BackendScope guard { backend };
            renderer->render(backend, *updateParameters);
        }
    }),
    renderer(std::make_unique<Renderer>(backend, pixelRatio, fileSource, scheduler)) {
}

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

Size HeadlessFrontend::getSize() const {
    return size;
}

Renderer* HeadlessFrontend::getRenderer() {
    assert(renderer);
    return renderer.get();
}

RendererBackend* HeadlessFrontend::getBackend() {
    return &backend;
}

void HeadlessFrontend::setSize(Size size_) {
    if (size != size_) {
        size = size_;
        backend.setSize({ static_cast<uint32_t>(size_.width * pixelRatio),
                          static_cast<uint32_t>(size_.height * pixelRatio) });
    }
}

PremultipliedImage HeadlessFrontend::readStillImage() {
    return backend.readStillImage();
}

PremultipliedImage HeadlessFrontend::render(Map& map) {
    PremultipliedImage result;

    map.renderStill([&](std::exception_ptr error) {
        if (error) {
            std::rethrow_exception(error);
        } else {
            result = backend.readStillImage();
        }
    });

    while (!result.valid()) {
        util::RunLoop::Get()->runOnce();
    }

    return result;
}

} // namespace mbgl
