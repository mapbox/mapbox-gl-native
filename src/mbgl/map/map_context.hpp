#ifndef MBGL_MAP_MAP_CONTEXT
#define MBGL_MAP_MAP_CONTEXT

#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/update.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/gl/gl_object_store.hpp>

#include <vector>

namespace mbgl {

class View;
class MapData;
class Painter;
class SpriteImage;
class FileRequest;
class PropertyTransition;

namespace gl { class TexturePool; }

struct FrameData {
    std::array<uint16_t, 2> framebufferSize;
    TimePoint timePoint;
};

class MapContext : public Style::Observer {
public:
    MapContext(View&, FileSource&, MapMode, GLContextMode, const float pixelRatio);
    ~MapContext();

    MapData& getData() { return data; }

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

    // Annotations
    void addAnnotationIcon(const std::string&, std::shared_ptr<const SpriteImage>);
    void removeAnnotationIcon(const std::string&);
    double getTopOffsetPixelsForAnnotationIcon(const std::string&);
    void updateAnnotations();

    // Style API
    void addLayer(std::unique_ptr<StyleLayer>,
                  const optional<std::string> before);
    void removeLayer(const std::string& id);

    void addClass(const std::string&, optional<PropertyTransition>);
    void removeClass(const std::string&, optional<PropertyTransition>);
    bool hasClass(const std::string&) const;
    void setClasses(const std::vector<std::string>&, optional<PropertyTransition>);
    std::vector<std::string> getClasses() const;

    void setSourceTileCacheSize(size_t size);
    void onLowMemory();

    void cleanup();
    void dumpDebugLogs() const;

private:
    void onResourceLoaded() override;
    void onResourceError(std::exception_ptr) override;

    // Update the state indicated by the accumulated Update flags, then render.
    void update();

    // Helper function for triggering asynchronous updates.
    void updateAsync(Update);

    // Loads the actual JSON object an creates a new Style object.
    void loadStyleJSON(const std::string& json, const std::string& base);

    View& view;
    FileSource& fileSource;
    std::unique_ptr<MapData> dataPtr;
    MapData& data;

    gl::GLObjectStore glObjectStore;

    Update updateFlags = Update::Nothing;
    util::AsyncTask asyncUpdate;
    util::AsyncTask asyncInvalidate;

    std::unique_ptr<gl::TexturePool> texturePool;
    std::unique_ptr<Painter> painter;
    std::unique_ptr<Style> style;

    std::string styleURL;
    std::string styleJSON;

    std::unique_ptr<FileRequest> styleRequest;

    Map::StillImageCallback callback;
    size_t sourceCacheSize;
    TransformState transformState;
    FrameData frameData;
};

} // namespace mbgl

#endif
