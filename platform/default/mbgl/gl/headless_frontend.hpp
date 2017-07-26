#pragma once

#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/util/async_task.hpp>

#include <memory>

namespace mbgl {

class FileSource;
class Scheduler;
class Renderer;
class RendererBackend;
class Map;

class HeadlessFrontend : public RendererFrontend {
public:
    HeadlessFrontend(float pixelRatio_, FileSource&, Scheduler&);
    HeadlessFrontend(Size, float pixelRatio_, FileSource&, Scheduler&);
    ~HeadlessFrontend() override;

    void reset() override;
    void update(std::shared_ptr<UpdateParameters>) override;
    void setObserver(RendererObserver&) override;

    Size getSize() const;
    void setSize(Size);

    Renderer* getRenderer();
    RendererBackend* getBackend();

    PremultipliedImage readStillImage();
    PremultipliedImage render(Map&);

private:
    Size size;
    float pixelRatio;

    HeadlessBackend backend;
    util::AsyncTask asyncInvalidate;

    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<UpdateParameters> updateParameters;
};

} // namespace mbgl
