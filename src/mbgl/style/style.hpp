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

namespace mbgl {

class Sprite;
class StyleLayer;
class StyleLayerGroup;

class Style : public util::noncopyable {
public:
    struct exception : std::runtime_error { exception(const char *msg) : std::runtime_error(msg) {} };

    Style();
    ~Style();

    void loadJSON(const uint8_t *const data);

    size_t layerCount() const;
    void updateProperties(float z, std::chrono::steady_clock::time_point now);

    void setDefaultTransitionDuration(std::chrono::steady_clock::duration duration = std::chrono::steady_clock::duration::zero());
    void cascadeClasses(const std::vector<std::string>&);

    bool hasTransitions() const;

    const std::string &getSpriteURL() const;

    util::ptr<StyleLayerGroup> layers;
    std::vector<std::string> appliedClasses;
    std::string glyph_url;
    std::string base;

private:
    std::string sprite_url;
    PropertyTransition defaultTransition;
    bool initial_render_complete = false;
    std::unique_ptr<uv::rwlock> mtx;
    ZoomHistory zoomHistory;
};

}

#endif
