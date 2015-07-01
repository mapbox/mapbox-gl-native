#ifndef MBGL_STYLE_STYLE
#define MBGL_STYLE_STYLE

#include <mbgl/style/property_transition.hpp>
#include <mbgl/style/zoom_history.hpp>

#include <mbgl/map/source.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/text/glyph_store.hpp>

#include <mbgl/util/uv.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/worker.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace mbgl {

class GlyphAtlas;
class GlyphStore;
class SpriteStore;
class SpriteAtlas;
class LineAtlas;
class StyleLayer;

class Style : public GlyphStore::Observer,
              public Source::Observer,
              public Sprite::Observer,
              public util::noncopyable {
public:
    Style(const std::string& data,
          const std::string& base,
          MapData&,
          uv_loop_t*);
    ~Style();

    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onTileDataChanged() = 0;
        virtual void onResourceLoadingFailed(std::exception_ptr error) = 0;
    };

    void setObserver(Observer*);

    bool isLoaded() const;

    // Fetch the tiles needed by the current viewport and emit a signal when
    // a tile is ready so observers can render the tile.
    void update(const TransformState&, TexturePool&);

    void cascade(const std::vector<std::string>&);
    void recalculate(float z, TimePoint now);

    void setDefaultTransitionDuration(Duration);
    bool hasTransitions() const;

    std::exception_ptr getLastError() const {
        return lastError;
    }

    Source* getSource(const std::string& id) const;

    MapData& data;
    std::unique_ptr<GlyphStore> glyphStore;
    std::unique_ptr<GlyphAtlas> glyphAtlas;
    util::ptr<Sprite> sprite;
    std::unique_ptr<SpriteStore> spriteStore;
    std::unique_ptr<SpriteAtlas> spriteAtlas;
    std::unique_ptr<LineAtlas> lineAtlas;

    std::vector<std::unique_ptr<Source>> sources;
    std::vector<util::ptr<StyleLayer>> layers;

private:
    // GlyphStore::Observer implementation.
    void onGlyphRangeLoaded() override;
    void onGlyphRangeLoadingFailed(std::exception_ptr error) override;

    // Source::Observer implementation.
    void onSourceLoaded() override;
    void onSourceLoadingFailed(std::exception_ptr error) override;
    void onTileLoaded(bool isNewTile) override;
    void onTileLoadingFailed(std::exception_ptr error) override;

    // Sprite::Observer implementation.
    void onSpriteLoaded() override;
    void onSpriteLoadingFailed(std::exception_ptr error) override;

    void emitTileDataChanged();
    void emitResourceLoadingFailed(std::exception_ptr error);

    bool shouldReparsePartialTiles = false;

    Observer* observer = nullptr;

    std::exception_ptr lastError;

    PropertyTransition defaultTransition;
    std::unique_ptr<uv::rwlock> mtx;
    ZoomHistory zoomHistory;

public:
    Worker workers;
};

}

#endif
