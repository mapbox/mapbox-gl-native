#ifndef MBGL_STYLE_STYLE
#define MBGL_STYLE_STYLE

#include <mbgl/style/property_transition.hpp>
#include <mbgl/style/zoom_history.hpp>

#include <mbgl/util/uv.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/worker.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace mbgl {

class Source;
class StyleLayer;

class Style : public util::noncopyable {
public:
    Style();
    ~Style();

    void loadJSON(const uint8_t *const data);
    bool isLoaded() const;

    void cascade(const std::vector<std::string>&);
    void recalculate(float z, TimePoint now);

    void setDefaultTransitionDuration(Duration);
    bool hasTransitions() const;

    const std::string &getSpriteURL() const;

    std::vector<util::ptr<Source>> sources;
    std::vector<util::ptr<StyleLayer>> layers;
    std::string glyph_url;
    std::string base;

private:
    bool loaded = false;
    std::string sprite_url;
    PropertyTransition defaultTransition;
    std::unique_ptr<uv::rwlock> mtx;
    ZoomHistory zoomHistory;

public:
    Worker workers;
};

}

#endif
