#include "merge_lines.hpp"
#include <sstream>

namespace mbgl {
namespace util {

unsigned int mergeFromRight(std::vector<SymbolFeature> &features,
                            std::map<std::string, unsigned int> &rightIndex,
                            std::map<std::string, unsigned int>::iterator left,
                            std::string &rightKey,
                            std::vector<std::vector<Coordinate>> &geom) {

    unsigned int index = left->second;
    rightIndex.erase(left);
    rightIndex[rightKey] = index;
    features[index].geometry[0].pop_back();
    features[index].geometry[0].insert(
        features[index].geometry[0].end(), geom[0].begin(), geom[0].end());
    geom[0].clear();
    return index;
}

unsigned int mergeFromLeft(std::vector<SymbolFeature> &features,
                           std::map<std::string, unsigned int> &leftIndex,
                           std::string &leftKey,
                           std::map<std::string, unsigned int>::iterator right,
                           std::vector<std::vector<Coordinate>> &geom) {

    unsigned int index = right->second;
    leftIndex.erase(right);
    leftIndex[leftKey] = index;
    geom[0].pop_back();
    geom[0].insert(
        geom[0].end(), features[index].geometry[0].begin(), features[index].geometry[0].end());
    features[index].geometry[0].clear();
    std::swap(features[index].geometry[0], geom[0]);
    return index;
}

std::string
getKey(const std::u32string &text, const std::vector<std::vector<Coordinate>> &geom, bool onRight) {
    const Coordinate &coord = onRight ? geom[0].back() : geom[0].front();
    std::ostringstream key;
    for (const auto c : text) {
        key << (char)c;
    }
    key << ":" << coord.x << ":" << coord.y;
    return key.str();
}

void mergeLines(std::vector<SymbolFeature> &features) {

    std::map<std::string, unsigned int> leftIndex;
    std::map<std::string, unsigned int> rightIndex;

    for (unsigned int k = 0; k < features.size(); k++) {
        SymbolFeature &feature = features[k];
        std::vector<std::vector<Coordinate>> &geometry = feature.geometry;

        if (!feature.label.length()) {
            continue;
        }

        std::string leftKey = getKey(feature.label, geometry, false);
        std::string rightKey = getKey(feature.label, geometry, true);

        auto left = rightIndex.find(leftKey);
        auto right = leftIndex.find(rightKey);

        if ((left != rightIndex.end()) && (right != leftIndex.end()) &&
            (left->second != right->second)) {
            // found lines with the same text adjacent to both ends of the current line, merge all
            // three
            unsigned int j = mergeFromLeft(features, leftIndex, leftKey, right, geometry);
            unsigned int i =
                mergeFromRight(features, rightIndex, left, rightKey, features[j].geometry);

            leftIndex.erase(leftKey);
            rightIndex.erase(rightKey);
            rightIndex[getKey(feature.label, features[i].geometry, true)] = i;

        } else if (left != rightIndex.end()) {
            // found mergeable line adjacent to the start of the current line, merge
            mergeFromRight(features, rightIndex, left, rightKey, geometry);

        } else if (right != leftIndex.end()) {
            // found mergeable line adjacent to the end of the current line, merge
            mergeFromLeft(features, leftIndex, leftKey, right, geometry);

        } else {
            // no adjacent lines, add as a new item
            leftIndex[leftKey] = k;
            rightIndex[rightKey] = k;
        }
    }
}

} // end namespace util
} // end namespace mbgl
