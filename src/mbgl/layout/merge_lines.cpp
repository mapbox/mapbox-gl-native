#include <mbgl/layout/merge_lines.hpp>
#include <mbgl/layout/symbol_feature.hpp>

#include <boost/functional/hash.hpp>

namespace mbgl {
namespace util {

// Map of key -> index into features
using Index = std::unordered_map<size_t, size_t>;

size_t mergeFromRight(std::vector<SymbolFeature>& features,
                      Index& rightIndex,
                      Index::iterator left,
                      size_t rightKey,
                      GeometryCollection& geom) {

    const size_t index = left->second;
    rightIndex.erase(left);
    rightIndex[rightKey] = index;
    features[index].geometry[0].pop_back();
    features[index].geometry[0].insert(
        features[index].geometry[0].end(), geom[0].begin(), geom[0].end());
    geom[0].clear();
    return index;
}

size_t mergeFromLeft(std::vector<SymbolFeature>& features,
                     Index& leftIndex,
                     Index::iterator right,
                     size_t leftKey,
                     GeometryCollection& geom) {

    const size_t index = right->second;
    leftIndex.erase(right);
    leftIndex[leftKey] = index;
    geom[0].pop_back();
    geom[0].insert(
        geom[0].end(), features[index].geometry[0].begin(), features[index].geometry[0].end());
    features[index].geometry[0].clear();
    std::swap(features[index].geometry[0], geom[0]);
    return index;
}

size_t getKey(const std::u16string& text, const GeometryCoordinate& coord) {
    auto hash = std::hash<std::u16string>()(text);
    boost::hash_combine(hash, coord.x);
    boost::hash_combine(hash, coord.y);
    return hash;
}

void mergeLines(std::vector<SymbolFeature>& features) {
    Index leftIndex;
    Index rightIndex;

    for (size_t k = 0; k < features.size(); k++) {
        SymbolFeature& feature = features[k];
        GeometryCollection& geometry = feature.geometry;

        if (!feature.text || geometry.empty() || geometry[0].empty()) {
            continue;
        }

        const size_t leftKey = getKey(*feature.text, geometry[0].front());
        const size_t rightKey = getKey(*feature.text, geometry[0].back());

        const auto left = rightIndex.find(leftKey);
        const auto right = leftIndex.find(rightKey);

        if (left != rightIndex.end() && right != leftIndex.end() && left->second != right->second) {
            // found lines with the same text adjacent to both ends of the current line, merge all
            // three
            size_t j = mergeFromLeft(features, leftIndex, right, leftKey, geometry);
            size_t i = mergeFromRight(features, rightIndex, left, rightKey, features[j].geometry);

            leftIndex.erase(leftKey);
            rightIndex.erase(rightKey);
            rightIndex[getKey(*feature.text, features[i].geometry[0].back())] = i;

        } else if (left != rightIndex.end()) {
            // found mergeable line adjacent to the start of the current line, merge
            mergeFromRight(features, rightIndex, left, rightKey, geometry);

        } else if (right != leftIndex.end()) {
            // found mergeable line adjacent to the end of the current line, merge
            mergeFromLeft(features, leftIndex, right, leftKey, geometry);

        } else {
            // no adjacent lines, add as a new item
            leftIndex[leftKey] = k;
            rightIndex[rightKey] = k;
        }
    }
}

} // end namespace util
} // end namespace mbgl
