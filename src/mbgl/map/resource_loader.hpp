#ifndef MBGL_MAP_RESOURCE_LOADER
#define MBGL_MAP_RESOURCE_LOADER

#include <mbgl/map/source.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <string>

namespace mbgl {

class GlyphAtlas;
class GlyphStore;
class MapData;
class SpriteAtlas;
class Style;
class TexturePool;
class TransformState;

// ResourceLoader is responsible for loading and updating the Source(s) owned
// by the Style. The Source object currently owns all the tiles, thus this
// class will notify its observers of any change on these tiles which will
// ultimately cause a new rendering to be triggered.
class ResourceLoader : public GlyphStore::Observer,
                       public Source::Observer,
                       public Sprite::Observer,
                       private util::noncopyable {
public:
    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onTileDataChanged() = 0;
    };

    ResourceLoader();
    ~ResourceLoader();

    void setObserver(Observer* observer);

    // The style object currently owns all the sources. When setting
    // a new style we will go through all of them and try to load.
    void setStyle(Style* style);

    // TODO: Move GlyphStore to ResourceLoader. We cannot do it now
    // because we reset the ResourceLoader every time we change the
    // style.
    void setGlyphStore(GlyphStore* glyphStore);

    // Set the access token to be used for loading the tile data.
    void setAccessToken(const std::string& accessToken);

    // Fetch the tiles needed by the current viewport and emit a signal when
    // a tile is ready so observers can render the tile.
    void update(MapData&, const TransformState&, GlyphAtlas&, SpriteAtlas&, TexturePool&);

    // FIXME: There is probably a better place for this.
    inline util::ptr<Sprite> getSprite() const {
        return sprite_;
    }

    // GlyphStore::Observer implementation.
    void onGlyphRangeLoaded() override;

    // Source::Observer implementation.
    void onSourceLoaded() override;
    void onTileLoaded() override;

    // Sprite::Observer implementation.
    void onSpriteLoaded() override;

private:
    void emitTileDataChanged();

    bool hasNewResources_ = false;

    std::string accessToken_;
    util::ptr<Sprite> sprite_;

    GlyphStore* glyphStore_ = nullptr;
    Style* style_ = nullptr;

    Observer* observer_ = nullptr;
};

}

#endif
