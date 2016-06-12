#include <mbgl/util/grid_index.hpp>
#include <mbgl/geometry/feature_index.hpp>

#include <unordered_set>

namespace mbgl {


template <class T>
GridIndex<T>::GridIndex(int32_t extent_, int32_t n_, int32_t padding_) :
    extent(extent_),
    n(n_),
    padding(padding_),
    d(n + 2 * padding),
    scale(double(n) / double(extent)),
    min(-double(padding) / n * extent),
    max(extent + double(padding) / n * extent)
    {
        cells.resize(d * d);
    }

template <class T>
void GridIndex<T>::insert(T&& t, const BBox& bbox) {
    size_t uid = elements.size();

    auto cx1 = convertToCellCoord(bbox.min.x);
    auto cy1 = convertToCellCoord(bbox.min.y);
    auto cx2 = convertToCellCoord(bbox.max.x);
    auto cy2 = convertToCellCoord(bbox.max.y);

    for (int32_t x = cx1; x <= cx2; x++) {
        for (int32_t y = cy1; y <= cy2; y++) {
            auto cellIndex = d * y + x;
            cells[cellIndex].push_back(uid);
        }
    }

    elements.emplace_back(t, bbox);
}

template <class T>
std::vector<T> GridIndex<T>::query(const BBox& queryBBox) const {
    std::vector<T> result;
    std::unordered_set<size_t> seenUids;

    auto cx1 = convertToCellCoord(queryBBox.min.x);
    auto cy1 = convertToCellCoord(queryBBox.min.y);
    auto cx2 = convertToCellCoord(queryBBox.max.x);
    auto cy2 = convertToCellCoord(queryBBox.max.y);

    for (int32_t x = cx1; x <= cx2; x++) {
        for (int32_t y = cy1; y <= cy2; y++) {
            auto cellIndex = d * y + x;
            for (auto uid : cells[cellIndex]) {
                if (seenUids.count(uid) == 0) {
                    seenUids.insert(uid);

                    auto& pair = elements.at(uid);
                    auto& bbox = pair.second;
                    if (queryBBox.min.x <= bbox.max.x &&
                        queryBBox.min.y <= bbox.max.y &&
                        queryBBox.max.x >= bbox.min.x &&
                        queryBBox.max.y >= bbox.min.y) {

                        result.push_back(pair.first);
                    }
                }
            }
        }
    }

    return result;
}


template <class T>
int32_t GridIndex<T>::convertToCellCoord(int32_t x) const {
    return std::max(0.0, std::min(d - 1.0, std::floor(x * scale) + padding));
}

template class GridIndex<IndexedSubfeature>;
} // namespace mbgl
