#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/variant.hpp>

#include <unordered_map>

namespace mbgl {

template <class T>
class StyleDifference {
public:
    std::unordered_map<std::string, T> added;
    std::unordered_map<std::string, T> removed;
    std::unordered_map<std::string, T> changed;
};

using ImmutableSource = Immutable<style::Source::Impl>;
using SourceDifference = StyleDifference<ImmutableSource>;

SourceDifference diffSources(const std::vector<ImmutableSource>&,
                             const std::vector<ImmutableSource>&);

using ImmutableLayer = Immutable<style::Layer::Impl>;
using LayerDifference = StyleDifference<ImmutableLayer>;

LayerDifference diffLayers(const std::vector<ImmutableLayer>&,
                           const std::vector<ImmutableLayer>&);

} // namespace mbgl
