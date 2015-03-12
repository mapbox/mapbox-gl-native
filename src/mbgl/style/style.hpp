#ifndef MBGL_STYLE_STYLE
#define MBGL_STYLE_STYLE

#include <mbgl/style/property_transition.hpp>
#include <mbgl/style/style_source.hpp>
#include <mbgl/style/zoom_history.hpp>

#include <mbgl/util/uv.hpp>
#include <mbgl/util/ptr.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <chrono>
#include <memory>

namespace mbgl {

class Map;
class Environment;
class GlyphAtlas;
class GlyphStore;
class SpriteAtlas;
class Sprite;
class LineAtlas;
class StyleLayer;
class StyleLayerGroup;
class TexturePool;

class Style : public util::noncopyable,
              public std::enable_shared_from_this<Style> {
public:
    struct exception : std::runtime_error { exception(const char *msg) : std::runtime_error(msg) {} };

    Style(Environment&);
    ~Style();

    void loadJSON(const uint8_t *const data);

    size_t layerCount() const;

    void updateSources(Map&,
                       Environment&,
                       uv::worker&,
                       TexturePool&,
                       std::function<void()> callback);

    void updateProperties(float z, std::chrono::steady_clock::time_point now);

    void setDefaultTransitionDuration(std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void cascadeClasses(const std::vector<std::string>&);

    bool hasTransitions() const;

    const std::unique_ptr<GlyphAtlas> glyphAtlas;
    const std::unique_ptr<SpriteAtlas> spriteAtlas;
    const std::unique_ptr<LineAtlas> lineAtlas;

    util::ptr<StyleLayerGroup> layers;
    std::set<util::ptr<StyleSource>> activeSources;
    std::string base;

private:
    std::string sprite_url;
    std::string glyph_url;
    PropertyTransition defaultTransition;
    bool initial_render_complete = false;
    std::unique_ptr<uv::rwlock> mtx;
    ZoomHistory zoomHistory;

    const std::unique_ptr<GlyphStore> glyphStore;
    util::ptr<Sprite> sprite;
};

}

#endif
