#include <mbgl/renderer/style_diff.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/longest_common_subsequence.hpp>

namespace mbgl {

template <class T, class Eq>
StyleDifference<T> diff(const std::vector<T>& a, const std::vector<T>& b, const Eq& eq) {
    std::vector<T> lcs;

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

ImageDifference diffImages(const std::vector<ImmutableImage>& a,
                           const std::vector<ImmutableImage>& b) {
    return diff(a, b, [] (const ImmutableImage& lhs, const ImmutableImage& rhs) {
        return lhs->id == rhs->id;
    });
}

SourceDifference diffSources(const std::vector<ImmutableSource>& a,
                             const std::vector<ImmutableSource>& b) {
    return diff(a, b, [] (const ImmutableSource& lhs, const ImmutableSource& rhs) {
        return std::tie(lhs->id, lhs->type)
            == std::tie(rhs->id, rhs->type);
    });
}

LayerDifference diffLayers(const std::vector<ImmutableLayer>& a,
                           const std::vector<ImmutableLayer>& b) {
    return diff(a, b, [] (const ImmutableLayer& lhs, const ImmutableLayer& rhs) {
        return std::tie(lhs->id, lhs->type)
            == std::tie(rhs->id, rhs->type);
    });
}

} // namespace mbgl
