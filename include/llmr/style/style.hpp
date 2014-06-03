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
#include <unordered_map>
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

    void loadJSON(const uint8_t *const data);

    size_t layerCount() const;
    void cascade(float z);

    bool needsTransition() const;
    void updateTransitions(time now);
    void cancelTransitions();

    void setDefaultTransitionDuration(uint64_t duration_milliseconds = 0);

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
        std::unordered_map<std::string, FillProperties> fills;
        std::unordered_map<std::string, LineProperties> lines;
        std::unordered_map<std::string, IconProperties> icons;
        std::unordered_map<std::string, TextProperties> texts;
        std::unordered_map<std::string, RasterProperties> rasters;
        std::unordered_map<std::string, CompositeProperties> composites;
        std::unordered_map<std::string, std::unordered_map<TransitionablePropertyKey, std::string>> effective_classes;
    } computed;

private:
    bool transitionInProgress(const std::string &layer_name, TransitionablePropertyKey key, time start);
    bool transitionExists(const std::string &layer_name, TransitionablePropertyKey key) const;
    bool inNeedOfTransition(const std::string &layer_name, TransitionablePropertyKey key) const;
    uint64_t transitionDuration(const std::string &layer_name, TransitionablePropertyKey key) const;
    uint64_t transitionDelay(const std::string &layer_name, TransitionablePropertyKey key) const;

    void cascadeProperties(GenericProperties &properties, const GenericClass& klass, const std::string& layer_name, const std::string& class_name, float z);

private:
    // Last applied settings.
    struct {
        BackgroundProperties background;
        std::unordered_map<std::string, FillProperties> fills;
        std::unordered_map<std::string, LineProperties> lines;
        std::unordered_map<std::string, IconProperties> icons;
        std::unordered_map<std::string, TextProperties> texts;
        std::unordered_map<std::string, RasterProperties> rasters;
        std::unordered_map<std::string, CompositeProperties> composites;
        std::unordered_map<std::string, std::unordered_map<TransitionablePropertyKey, std::string>> effective_classes;
    } previous;

    // Settings values currently being transitioned.
    struct {
        BackgroundProperties background;
        std::unordered_map<std::string, FillProperties> fills;
        std::unordered_map<std::string, LineProperties> lines;
        std::unordered_map<std::string, IconProperties> icons;
        std::unordered_map<std::string, TextProperties> texts;
        std::unordered_map<std::string, RasterProperties> rasters;
        std::unordered_map<std::string, CompositeProperties> composites;
    } transitioning;

    std::unordered_map<std::string, std::unordered_map<TransitionablePropertyKey, PropertyTransition>> properties_to_transition;
    std::unordered_map<std::string, std::unordered_map<TransitionablePropertyKey, std::shared_ptr<util::transition>>> transitions;
    uint64_t default_transition_duration = 0;
    bool initial_render_complete = false;

    mutable uv::rwlock mtx;

};

}



#endif