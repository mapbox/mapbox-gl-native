#ifndef MBGL_MAP_MAP_CONTEXT
#define MBGL_MAP_MAP_CONTEXT

#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/gl_object_store.hpp>
#include <mbgl/util/ptr.hpp>

#include <vector>

namespace uv {
class async;
}

namespace mbgl {

class View;
class MapData;
class TexturePool;
class Painter;
class Sprite;
class Worker;
class StillImage;
class SpriteImage;
struct LatLng;
struct LatLngBounds;


struct FrameData {
    std::array<uint16_t, 2> framebufferSize;
};

class MapContext : public Style::Observer {
public:
    MapContext(View&, FileSource&, MapData&);
    ~MapContext();

    void pause();

    void triggerUpdate(const TransformState&, Update = Update::Nothing);
    void renderStill(const TransformState&, const FrameData&, Map::StillImageCallback callback);

    // Triggers a synchronous render. Returns true if style has been fully loaded.
    bool renderSync(const TransformState&, const FrameData&);

    void setStyleURL(const std::string&);
    void setStyleJSON(const std::string& json, const std::string& base);
    std::string getStyleURL() const { return styleURL; }
    std::string getStyleJSON() const { return styleJSON; }

    bool isLoaded() const;

    double getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol);
    void updateAnnotations();

    void setSourceTileCacheSize(size_t size);
    void onLowMemory();

    void cleanup();

    void setSprite(const std::string&, std::shared_ptr<const SpriteImage>);

    // Style::Observer implementation.
    void onTileDataChanged() override;
    void onResourceLoadingFailed(std::exception_ptr error) override;

private:
    // Update the state indicated by the accumulated Update flags, then render.
    void update();

    // Loads the actual JSON object an creates a new Style object.
    void loadStyleJSON(const std::string& json, const std::string& base);

    View& view;
    MapData& data;

    util::GLObjectStore glObjectStore;

    Update updateFlags = Update::Nothing;
    std::unique_ptr<uv::async> asyncUpdate;
    std::unique_ptr<uv::async> asyncInvalidate;

    std::unique_ptr<TexturePool> texturePool;
    std::unique_ptr<Painter> painter;
    std::unique_ptr<Style> style;

    std::string styleURL;
    std::string styleJSON;

    Request* styleRequest = nullptr;

    Map::StillImageCallback callback;
    size_t sourceCacheSize;
    TransformState transformState;
    FrameData frameData;
};

}

#endif
