#ifndef LLMR_STYLE_STYLE
#define LLMR_STYLE_STYLE

#include <cstdint>

#include <llmr/util/pbf.hpp>
#include <llmr/style/bucket_description.hpp>
#include <llmr/style/layer_description.hpp>
#include <llmr/style/class_description.hpp>
#include <llmr/geometry/sprite_atlas.hpp>
#include <llmr/util/transition.hpp>
#include <llmr/util/uv.hpp>

#include <map>
#include <vector>
#include <set>
#include <memory>

namespace llmr {

class Sprite;

class Style {
public:
    struct exception : std::runtime_error { exception(const char *msg) : std::runtime_error(msg) {} };

public:
    Style();

    void reset();

    void loadJSON(const uint8_t *const data, size_t bytes);

    size_t layerCount() const;
    void cascade(float z);

    bool needsTransition() const;
    void updateTransitions(time now);
    void cancelTransitions();

    void setDefaultTransitionDuration(uint64_t duration = 0);

public:
    std::shared_ptr<Sprite> sprite;

    // This is static information parsed from the stylesheet.
    std::map<std::string, BucketDescription> buckets;
    std::vector<LayerDescription> layers;
    std::map<std::string, ClassDescription> classes;

    // Currently applied settings.
    std::set<std::string> appliedClasses;
    struct {
        BackgroundProperties background;
        std::map<std::string, FillProperties> fills;
        std::map<std::string, LineProperties> lines;
        std::map<std::string, IconProperties> icons;
        std::map<std::string, TextProperties> texts;
        std::map<std::string, RasterProperties> rasters;
        std::map<std::string, CompositeProperties> composites;
        std::map<std::string, std::map<TransitionablePropertyKey, std::string>> effective_classes;
    } computed;

private:
    bool transitionInProgress(std::string layer_name, TransitionablePropertyKey key);
    bool transitionExists(std::string layer_name, TransitionablePropertyKey key);
    bool inNeedOfTransition(std::string layer_name, TransitionablePropertyKey key);
    uint64_t transitionDuration(std::string layer_name, TransitionablePropertyKey key);
    uint64_t transitionDelay(std::string layer_name, TransitionablePropertyKey key);

private:
    // Last applied settings.
    struct {
        BackgroundProperties background;
        std::map<std::string, FillProperties> fills;
        std::map<std::string, LineProperties> lines;
        std::map<std::string, IconProperties> icons;
        std::map<std::string, TextProperties> texts;
        std::map<std::string, RasterProperties> rasters;
        std::map<std::string, CompositeProperties> composites;
        std::map<std::string, std::map<TransitionablePropertyKey, std::string>> effective_classes;
    } previous;

    // Settings values currently being transitioned.
    struct {
        BackgroundProperties background;
        std::map<std::string, FillProperties> fills;
        std::map<std::string, LineProperties> lines;
        std::map<std::string, IconProperties> icons;
        std::map<std::string, TextProperties> texts;
        std::map<std::string, RasterProperties> rasters;
        std::map<std::string, CompositeProperties> composites;
    } transitioning;

    std::map<std::string, std::map<TransitionablePropertyKey, PropertyTransition>> properties_to_transition;
    std::map<std::string, std::map<TransitionablePropertyKey, std::shared_ptr<util::transition>>> transitions;
    uint64_t default_transition_duration = 0;
    bool initial_render_complete = false;

    mutable uv::rwlock mtx;

};

}



#endif