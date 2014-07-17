#ifndef MBGL_STYLE_STYLE
#define MBGL_STYLE_STYLE

#include <mbgl/style/property_transition.hpp>
#include <mbgl/style/style_source.hpp>

#include <mbgl/util/time.hpp>
#include <mbgl/util/uv.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <memory>

namespace mbgl {

class Sprite;
class StyleLayer;
class StyleLayerGroup;
struct BackgroundProperties;

class Style {
public:
    struct exception : std::runtime_error { exception(const char *msg) : std::runtime_error(msg) {} };

public:
    Style();

    void loadJSON(const uint8_t *const data);

    size_t layerCount() const;
    void updateProperties(float z, timestamp t);

    void setDefaultTransitionDuration(uint16_t duration_milliseconds = 0);

    void setAppliedClasses(const std::vector<std::string> &classes);
    const std::vector<std::string> &getAppliedClasses() const;
    void toggleClass(const std::string &name);

    // Updates the styling information to reflect the current array
    // of applied classes.
    void updateClasses();

    bool hasTransitions() const;

    const BackgroundProperties &getBackgroundProperties() const;

public:
    std::shared_ptr<Sprite> sprite;
    std::shared_ptr<StyleLayerGroup> layers;
    std::vector<std::string> appliedClasses;
    std::string sprite_url;
    std::string glyph_url;


private:
    PropertyTransition defaultTransition;
    bool initial_render_complete = false;

    mutable uv::rwlock mtx;
};

}

#endif
