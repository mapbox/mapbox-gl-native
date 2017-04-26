#include <mbgl/renderer/style_diff.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/longest_common_subsequence.hpp>

namespace mbgl {

template <class T>
StyleDifference<T> diff(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> lcs;

    auto eq = [] (const T& lhs, const T& rhs) {
        return std::tie(lhs->id, lhs->type)
            == std::tie(rhs->id, rhs->type);
    };

    longest_common_subsequence(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(lcs), eq);

    auto aIt = a.begin();
    auto bIt = b.begin();
    auto lIt = lcs.begin();

    StyleDifference<T> result;

    while (aIt != a.end() || bIt != b.end()) {
        if (aIt != a.end() && (lIt == lcs.end() || !eq(*lIt, *aIt))) {
            result.removed.emplace((*aIt)->id, *aIt);
            aIt++;
        } else if (bIt != b.end() && (lIt == lcs.end() || !eq(*lIt, *bIt))) {
            result.added.emplace((*bIt)->id, *bIt);
            bIt++;
        } else {
            if (aIt->get() != bIt->get()) {
                result.changed.emplace((*bIt)->id, *bIt);
            }
            aIt++;
            bIt++;
            lIt++;
        }
    }

    return result;
}

SourceDifference diffSources(const std::vector<ImmutableSource>& a,
                             const std::vector<ImmutableSource>& b) {
    return diff(a, b);
}

LayerDifference diffLayers(const std::vector<ImmutableLayer>& a,
                           const std::vector<ImmutableLayer>& b) {
    return diff(a, b);
}

} // namespace mbgl
