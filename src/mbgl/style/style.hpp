#pragma once

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/observer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/text/glyph_store_observer.hpp>
#include <mbgl/sprite/sprite_store_observer.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/map/zoom_history.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geo.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace mbgl {

class FileSource;
class GlyphAtlas;
class SpriteAtlas;
class LineAtlas;
class RenderData;

namespace style {

class UpdateParameters;
class QueryParameters;

class Style : public GlyphStoreObserver,
              public SpriteStoreObserver,
              public SourceObserver,
              public util::noncopyable {
public:
    Style(FileSource&, float pixelRatio);
    ~Style() override;

    void setJSON(const std::string&);

    void setObserver(Observer*);

    bool isLoaded() const;

    // Fetch the tiles needed by the current viewport and emit a signal when
    // a tile is ready so observers can render the tile.
    void update(const UpdateParameters&);

    void cascade(const TimePoint&, MapMode);
    void recalculate(float z, const TimePoint&, MapMode);

    bool hasTransitions() const;

    std::exception_ptr getLastError() const {
        return lastError;
    }

    Source* getSource(const std::string& id) const;
    void addSource(std::unique_ptr<Source>);
    void removeSource(const std::string& sourceID);

    std::vector<const Layer*> getLayers() const;
    Layer* getLayer(const std::string& id) const;
    Layer* addLayer(std::unique_ptr<Layer>,
                    optional<std::string> beforeLayerID = {});
    void removeLayer(const std::string& layerID);

    std::string getName() const;
    LatLng getDefaultLatLng() const;
    double getDefaultZoom() const;
    double getDefaultBearing() const;
    double getDefaultPitch() const;

    bool addClass(const std::string&, const TransitionOptions& = {});
    bool removeClass(const std::string&, const TransitionOptions& = {});
    bool hasClass(const std::string&) const;
    void setClasses(const std::vector<std::string>&, const TransitionOptions& = {});
    std::vector<std::string> getClasses() const;

    RenderData getRenderData(MapDebugOptions) const;

    std::vector<Feature> queryRenderedFeatures(const QueryParameters&) const;

    float getQueryRadius() const;

    void setSourceTileCacheSize(size_t);
    void onLowMemory();

    void dumpDebugLogs() const;

    FileSource& fileSource;
    std::unique_ptr<GlyphStore> glyphStore;
    std::unique_ptr<GlyphAtlas> glyphAtlas;
    std::unique_ptr<SpriteStore> spriteStore;
    std::unique_ptr<SpriteAtlas> spriteAtlas;
    std::unique_ptr<LineAtlas> lineAtlas;

private:
    std::vector<std::unique_ptr<Source>> sources;
    std::vector<std::unique_ptr<Layer>> layers;
    std::vector<std::string> classes;
    optional<TransitionOptions> transitionProperties;

    // Defaults
    std::string name;
    LatLng defaultLatLng;
    double defaultZoom;
    double defaultBearing;
    double defaultPitch;

    std::vector<std::unique_ptr<Layer>>::const_iterator findLayer(const std::string& layerID) const;

    // GlyphStoreObserver implementation.
    void onGlyphsLoaded(const FontStack&, const GlyphRange&) override;
    void onGlyphsError(const FontStack&, const GlyphRange&, std::exception_ptr) override;

    // SpriteStoreObserver implementation.
    void onSpriteLoaded() override;
    void onSpriteError(std::exception_ptr) override;

    // SourceObserver implementation.
    void onSourceLoaded(Source&) override;
    void onSourceError(Source&, std::exception_ptr) override;
    void onTileLoaded(Source&, const OverscaledTileID&, bool isNewTile) override;
    void onTileError(Source&, const OverscaledTileID&, std::exception_ptr) override;
    void onNeedsRepaint() override;

    Observer nullObserver;
    Observer* observer = &nullObserver;

    std::exception_ptr lastError;

    ZoomHistory zoomHistory;
    bool hasPendingTransitions = false;

public:
    bool shouldReparsePartialTiles = false;
    bool loaded = false;
    Worker workers;
};

} // namespace style
} // namespace mbgl
