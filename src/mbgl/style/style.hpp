#pragma once

#include <mbgl/style/render_item.hpp>
#include <mbgl/style/zoom_history.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/property_transition.hpp>

#include <mbgl/source/source_observer.hpp>
#include <mbgl/text/glyph_store_observer.hpp>
#include <mbgl/sprite/sprite_store_observer.hpp>
#include <mbgl/map/mode.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/worker.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/feature.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <set>

namespace mbgl {

class FileSource;
class GlyphAtlas;
class GlyphStore;
class SpriteStore;
class SpriteAtlas;
class LineAtlas;
class StyleUpdateParameters;
class StyleQueryParameters;
class StyleObserver;

struct RenderData {
    Color backgroundColor = {{ 0, 0, 0, 0 }};
    std::set<Source*> sources;
    std::vector<RenderItem> order;
};

class Style : public GlyphStoreObserver,
              public SpriteStoreObserver,
              public SourceObserver,
              public util::noncopyable {
public:
    Style(FileSource&, float pixelRatio);
    ~Style();

    void setJSON(const std::string& data, const std::string& base);

    void setObserver(StyleObserver*);

    bool isLoaded() const;

    // Fetch the tiles needed by the current viewport and emit a signal when
    // a tile is ready so observers can render the tile.
    void update(const StyleUpdateParameters&);

    void cascade(const TimePoint&, MapMode);
    void recalculate(float z, const TimePoint&, MapMode);

    bool hasTransitions() const;

    std::exception_ptr getLastError() const {
        return lastError;
    }

    Source* getSource(const std::string& id) const;
    void addSource(std::unique_ptr<Source>);

    std::vector<std::unique_ptr<Layer>> getLayers() const;
    Layer* getLayer(const std::string& id) const;
    void addLayer(std::unique_ptr<Layer>,
                  optional<std::string> beforeLayerID = {});
    void removeLayer(const std::string& layerID);

    bool addClass(const std::string&, const PropertyTransition& = {});
    bool removeClass(const std::string&, const PropertyTransition& = {});
    bool hasClass(const std::string&) const;
    void setClasses(const std::vector<std::string>&, const PropertyTransition& = {});
    std::vector<std::string> getClasses() const;

    RenderData getRenderData() const;

    std::vector<Feature> queryRenderedFeatures(const StyleQueryParameters&) const;

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
    optional<PropertyTransition> transitionProperties;

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
    void onPlacementRedone() override;

    StyleObserver* observer = nullptr;

    std::exception_ptr lastError;

    ZoomHistory zoomHistory;
    bool hasPendingTransitions = false;

public:
    bool shouldReparsePartialTiles = false;
    bool loaded = false;
    Worker workers;
};

} // namespace mbgl
