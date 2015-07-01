#ifndef MBGL_MAP_MAP_CONTEXT
#define MBGL_MAP_MAP_CONTEXT

#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/map/transform_state.hpp>
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
struct LatLng;
struct LatLngBounds;

class MapContext : public Style::Observer {
public:
    MapContext(View&, FileSource&, MapData&);
    ~MapContext();

    struct RenderResult {
        bool fullyLoaded;
        bool needsRerender;
    };

    void pause();

    void resize(uint16_t width, uint16_t height, float ratio);

    using StillImageCallback = std::function<void(std::exception_ptr, std::unique_ptr<const StillImage>)>;

    void triggerUpdate(const TransformState&, Update = Update::Nothing);
    void renderStill(const TransformState&, StillImageCallback callback);
    RenderResult renderSync(const TransformState&);

    void setStyleURL(const std::string&);
    void setStyleJSON(const std::string& json, const std::string& base);
    std::string getStyleURL() const { return styleURL; }
    std::string getStyleJSON() const { return styleJSON; }

    bool isLoaded() const;

    double getTopOffsetPixelsForAnnotationSymbol(const std::string& symbol);
    void updateAnnotationTiles(const std::unordered_set<TileID, TileID::Hash>&);

    void setSourceTileCacheSize(size_t size);
    void onLowMemory();

    void cleanup();

    // Style::Observer implementation.
    void onTileDataChanged() override;
    void onResourceLoadingFailed(std::exception_ptr error) override;

private:
    // Style-related updates.
    void cascadeClasses();

    // Update the state indicated by the accumulated Update flags, then render.
    void update();

    // Loads the actual JSON object an creates a new Style object.
    void loadStyleJSON(const std::string& json, const std::string& base);

    View& view;
    MapData& data;

    util::GLObjectStore glObjectStore;

    UpdateType updated { static_cast<UpdateType>(Update::Nothing) };
    std::unique_ptr<uv::async> asyncUpdate;

    std::unique_ptr<TexturePool> texturePool;
    std::unique_ptr<Painter> painter;
    std::unique_ptr<Style> style;

    std::string styleURL;
    std::string styleJSON;

    Request* styleRequest = nullptr;

    StillImageCallback callback;
    size_t sourceCacheSize;
    TransformState transformState;
};

}

#endif
