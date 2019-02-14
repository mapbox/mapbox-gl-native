#pragma once

#include <mbgl/map/camera.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/optional.hpp>

#include <memory>

namespace mbgl {

class Renderer;
class Map;
class TransformState;

namespace gfx {
class RendererBackend;
} // namespace gfx

class HeadlessFrontend : public RendererFrontend {
public:
    HeadlessFrontend(float pixelRatio_,
                     const optional<std::string> programCacheDir = {},
                     gfx::ContextMode mode = gfx::ContextMode::Unique,
                     const optional<std::string> localFontFamily = {});
    HeadlessFrontend(Size,
                     float pixelRatio_,
                     const optional<std::string> programCacheDir = {},
                     gfx::ContextMode mode = gfx::ContextMode::Unique,
                     const optional<std::string> localFontFamily = {});
    ~HeadlessFrontend() override;

    void reset() override;
    void update(std::shared_ptr<UpdateParameters>) override;
    void setObserver(RendererObserver&) override;

    Size getSize() const;
    void setSize(Size);

    Renderer* getRenderer();
    gfx::RendererBackend* getBackend();
    CameraOptions getCameraOptions();

    bool hasImage(const std::string&);
    bool hasLayer(const std::string&);
    bool hasSource(const std::string&);

    ScreenCoordinate pixelForLatLng(const LatLng&);
    LatLng latLngForPixel(const ScreenCoordinate&);

    PremultipliedImage readStillImage();
    PremultipliedImage render(Map&);

    optional<TransformState> getTransformState() const;

private:
    Size size;
    float pixelRatio;

    gl::HeadlessBackend backend;
    util::AsyncTask asyncInvalidate;

    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<UpdateParameters> updateParameters;
};

} // namespace mbgl
