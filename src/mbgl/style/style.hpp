#ifndef MBGL_STYLE_STYLE
#define MBGL_STYLE_STYLE

#include <mbgl/style/property_transition.hpp>
#include <mbgl/style/zoom_history.hpp>

#include <mbgl/util/uv.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

namespace mbgl {

class StyleLayer;

class Style : public util::noncopyable {
public:
    Style();
    ~Style();

    void loadJSON(const uint8_t *const data);

    void updateProperties(float z, std::chrono::steady_clock::time_point now);

    void setDefaultTransitionDuration(std::chrono::steady_clock::duration);
    void cascadeClasses(const std::vector<std::string>&);

    bool hasTransitions() const;

    const std::string &getSpriteURL() const;

    std::vector<util::ptr<StyleLayer>> layers;
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
