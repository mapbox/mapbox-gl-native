#pragma once

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/observer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/update_batch.hpp>
#include <mbgl/text/glyph_atlas_observer.hpp>
#include <mbgl/sprite/sprite_atlas_observer.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/map/zoom_history.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geo.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace mbgl {

class FileSource;
class GlyphAtlas;
class SpriteAtlas;
class LineAtlas;
class RenderData;

namespace style {

class Layer;
class UpdateParameters;
class QueryParameters;

class Style : public GlyphAtlasObserver,
              public SpriteAtlasObserver,
              public SourceObserver,
              public LayerObserver,
              public util::noncopyable {
public:
    Style(FileSource&, float pixelRatio);
    ~Style() override;

    void setJSON(const std::string&);

    void setObserver(Observer*);

    bool isLoaded() const;

    // Fetch the tiles needed by the current viewport and emit a signal when
    // a tile is ready so observers can render the tile.
    void updateTiles(const UpdateParameters&);

    void relayout();
    void cascade(const TimePoint&, MapMode);
    void recalculate(float z, const TimePoint&, MapMode);

    bool hasTransitions() const;

    std::exception_ptr getLastError() const {
        return lastError;
    }

    std::vector<const Source*> getSources() const;
    std::vector<Source*> getSources();
    Source* getSource(const std::string& id) const;
    void addSource(std::unique_ptr<Source>);
    std::unique_ptr<Source> removeSource(const std::string& sourceID);

    std::vector<const Layer*> getLayers() const;
    std::vector<Layer*> getLayers();
    Layer* getLayer(const std::string& id) const;
    Layer* addLayer(std::unique_ptr<Layer>,
                    optional<std::string> beforeLayerID = {});
    std::unique_ptr<Layer> removeLayer(const std::string& layerID);

    std::string getName() const;
    LatLng getDefaultLatLng() const;
    double getDefaultZoom() const;
    double getDefaultBearing() const;
    double getDefaultPitch() const;

    bool addClass(const std::string&);
    bool removeClass(const std::string&);
    void setClasses(const std::vector<std::string>&);

    TransitionOptions getTransitionOptions() const;
    void setTransitionOptions(const TransitionOptions&);

    bool hasClass(const std::string&) const;
    std::vector<std::string> getClasses() const;

    RenderData getRenderData(MapDebugOptions, float angle) const;

    std::vector<Feature> queryRenderedFeatures(const QueryParameters&) const;

    float getQueryRadius() const;

    void setSourceTileCacheSize(size_t);
    void onLowMemory();

    void dumpDebugLogs() const;

    FileSource& fileSource;
    std::unique_ptr<GlyphAtlas> glyphAtlas;
    std::unique_ptr<SpriteAtlas> spriteAtlas;
    std::unique_ptr<LineAtlas> lineAtlas;

private:
    std::vector<std::unique_ptr<Source>> sources;
    std::vector<std::unique_ptr<Layer>> layers;
    std::vector<std::string> classes;
    TransitionOptions transitionOptions;

    // Defaults
    std::string name;
    LatLng defaultLatLng;
    double defaultZoom = 0;
    double defaultBearing = 0;
    double defaultPitch = 0;

    std::vector<std::unique_ptr<Layer>>::const_iterator findLayer(const std::string& layerID) const;
    void reloadLayerSource(Layer&);
    void updateSymbolDependentTiles();

    // GlyphStoreObserver implementation.
    void onGlyphsLoaded(const FontStack&, const GlyphRange&) override;
    void onGlyphsError(const FontStack&, const GlyphRange&, std::exception_ptr) override;

    // SpriteStoreObserver implementation.
    void onSpriteLoaded() override;
    void onSpriteError(std::exception_ptr) override;

    // SourceObserver implementation.
    void onSourceLoaded(Source&) override;
    void onSourceAttributionChanged(Source&, const std::string&) override;
    void onSourceError(Source&, std::exception_ptr) override;
    void onSourceDescriptionChanged(Source&) override;
    void onTileChanged(Source&, const OverscaledTileID&) override;
    void onTileError(Source&, const OverscaledTileID&, std::exception_ptr) override;

    // LayerObserver implementation.
    void onLayerFilterChanged(Layer&) override;
    void onLayerVisibilityChanged(Layer&) override;
    void onLayerPaintPropertyChanged(Layer&) override;
    void onLayerLayoutPropertyChanged(Layer&, const char *) override;

    Observer nullObserver;
    Observer* observer = &nullObserver;

    std::exception_ptr lastError;

    UpdateBatch updateBatch;
    ZoomHistory zoomHistory;
    bool hasPendingTransitions = false;

public:
    bool loaded = false;
};

} // namespace style
} // namespace mbgl
