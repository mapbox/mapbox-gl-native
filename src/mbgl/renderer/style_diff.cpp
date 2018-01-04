#include <mbgl/renderer/style_diff.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/util/immutable.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/longest_common_subsequence.hpp>

namespace mbgl {

template <class T, class Eq>
StyleDifference<T> diff(const Immutable<std::vector<T>>& a,
                        const Immutable<std::vector<T>>& b,
                        const Eq& eq) {
    StyleDifference<T> result;

    if (a == b) {
        return result;
    }

    std::vector<T> lcs;

    longest_common_subsequence(a->begin(), a->end(), b->begin(), b->end(), std::back_inserter(lcs), eq);

    auto aIt = a->begin();
    auto bIt = b->begin();
    auto lIt = lcs.begin();

    while (aIt != a->end() || bIt != b->end()) {
        if (aIt != a->end() && (lIt == lcs.end() || !eq(*lIt, *aIt))) {
            result.removed.emplace((*aIt)->id, *aIt);
            aIt++;
        } else if (bIt != b->end() && (lIt == lcs.end() || !eq(*lIt, *bIt))) {
            result.added.emplace((*bIt)->id, *bIt);
            bIt++;
        } else {
            if (aIt->get() != bIt->get()) {
                result.changed.emplace((*bIt)->id, StyleChange<T> { *aIt, *bIt });
            }
            aIt++;
            bIt++;
            lIt++;
        }
    }

    return result;
}

ImageDifference diffImages(const Immutable<std::vector<ImmutableImage>>& a,
                           const Immutable<std::vector<ImmutableImage>>& b) {
    return diff(a, b, [] (const ImmutableImage& lhs, const ImmutableImage& rhs) {
        return lhs->id == rhs->id;
    });
}

SourceDifference diffSources(const Immutable<std::vector<ImmutableSource>>& a,
                             const Immutable<std::vector<ImmutableSource>>& b) {
    return diff(a, b, [] (const ImmutableSource& lhs, const ImmutableSource& rhs) {
        return std::tie(lhs->id, lhs->type)
            == std::tie(rhs->id, rhs->type);
    });
}

LayerDifference diffLayers(const Immutable<std::vector<ImmutableLayer>>& a,
                           const Immutable<std::vector<ImmutableLayer>>& b) {
    return diff(a, b, [] (const ImmutableLayer& lhs, const ImmutableLayer& rhs) {
        return std::tie(lhs->id, lhs->type)
            == std::tie(rhs->id, rhs->type);
    });
}

bool hasLayoutDifference(const LayerDifference& layerDiff, const std::string& layerID) {
    if (layerDiff.added.count(layerID))
        return true;
    const auto it = layerDiff.changed.find(layerID);
    if (it == layerDiff.changed.end())
        return false;
    return it->second.before->hasLayoutDifference(*it->second.after);
}

} // namespace mbgl
