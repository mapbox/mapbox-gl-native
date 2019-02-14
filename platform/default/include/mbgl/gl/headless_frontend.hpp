#pragma once

#include <mbgl/map/camera.hpp>
#include <mbgl/renderer/mode.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/optional.hpp>

#include <memory>

namespace mbgl {

class FileSource;
class Renderer;
class RendererBackend;
class Map;
class TransformState;

class HeadlessFrontend : public RendererFrontend {
public:
    HeadlessFrontend(float pixelRatio_, FileSource&, const optional<std::string> programCacheDir = {}, GLContextMode mode = GLContextMode::Unique, const optional<std::string> localFontFamily = {});
    HeadlessFrontend(Size, float pixelRatio_, FileSource&, const optional<std::string> programCacheDir = {}, GLContextMode mode = GLContextMode::Unique, const optional<std::string> localFontFamily = {});
    ~HeadlessFrontend() override;

    void reset() override;
    void update(std::shared_ptr<UpdateParameters>) override;
    void setObserver(RendererObserver&) override;

    Size getSize() const;
    void setSize(Size);

    Renderer* getRenderer();
    RendererBackend* getBackend();
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

    HeadlessBackend backend;
    util::AsyncTask asyncInvalidate;

    std::unique_ptr<Renderer> renderer;
    std::shared_ptr<UpdateParameters> updateParameters;
};

} // namespace mbgl
