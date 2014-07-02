#ifndef LLMR_STYLE_STYLE
#define LLMR_STYLE_STYLE

#include <llmr/style/property_transition.hpp>

#include <llmr/util/time.hpp>
#include <llmr/util/uv.hpp>

#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <memory>

namespace llmr {

class Sprite;
class Source;
class StyleLayer;
class StyleLayerGroup;

class Style {
public:
    struct exception : std::runtime_error { exception(const char *msg) : std::runtime_error(msg) {} };

public:
    Style();

    void loadJSON(const uint8_t *const data);

    size_t layerCount() const;
    void updateProperties(float z, timestamp t);

    void setDefaultTransitionDuration(uint16_t duration_milliseconds = 0);

    const std::set<std::shared_ptr<Source>> getActiveSources() const;

    const std::vector<std::string> &getAppliedClasses() const;
    void toggleClass(const std::string &name);

    // Updates the styling information to reflect the current array
    // of applied classes.
    void updateClasses();

    bool hasTransitions() const;

public:
    std::shared_ptr<Sprite> sprite;
    std::shared_ptr<StyleLayerGroup> layers;
    std::shared_ptr<StyleLayer> background;
    std::vector<std::string> appliedClasses;
    std::string sprite_url;

private:
    void updateSources();
    void updateSources(const std::shared_ptr<StyleLayerGroup> &group);


private:
    std::set<std::shared_ptr<Source>> activeSources;
    PropertyTransition defaultTransition;
    bool initial_render_complete = false;

    mutable uv::rwlock mtx;
};

}

#endif
