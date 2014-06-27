#ifndef LLMR_STYLE_STYLE
#define LLMR_STYLE_STYLE

#include <cstdint>

#include <llmr/util/time.hpp>
#include <llmr/util/uv.hpp>

#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <memory>

namespace llmr {

class Sprite;
class Source;
class StyleLayer;
typedef std::vector<std::shared_ptr<StyleLayer>> StyleLayerGroup;

class Style {
public:
    struct exception : std::runtime_error { exception(const char *msg) : std::runtime_error(msg) {} };

public:
    Style();

    void reset();

    void loadJSON(const uint8_t *const data);

    size_t layerCount() const;
    void cascade(float z);

    bool needsTransition() const;
    void updateTransitions(timestamp now);
    void cancelTransitions();

    void setDefaultTransitionDuration(uint64_t duration_milliseconds = 0);

    void updateSources();
    const std::set<std::shared_ptr<Source>> getActiveSources() const;

    const std::vector<std::string> &getAppliedClasses() const;
    void toggleClass(const std::string &name);

public:
    std::shared_ptr<Sprite> sprite;
    std::shared_ptr<StyleLayerGroup> layers;
    std::shared_ptr<StyleLayer> background;
    std::vector<std::string> appliedClasses;


private:
    void updateSources(const std::shared_ptr<StyleLayerGroup> &layers);
    void cascade(const std::shared_ptr<StyleLayerGroup> &layers, float z);

private:
    std::set<std::shared_ptr<Source>> activeSources;
    uint64_t default_transition_duration = 0;
    bool initial_render_complete = false;

    mutable uv::rwlock mtx;
};

}

#endif
