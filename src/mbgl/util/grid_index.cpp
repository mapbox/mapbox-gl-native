#include <mbgl/util/grid_index.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/math/minmax.hpp>

#include <unordered_set>
#include <cmath>

namespace mbgl {


template <class T>
GridIndex<T>::GridIndex(int32_t width_, int32_t height_, int32_t cellSize_) :
    width(width_),
    height(height_),
    xCellCount(std::ceil(float(width_) / cellSize_)),
    yCellCount(std::ceil(float(height_) / cellSize_)),
    xScale(float(xCellCount) / width_),
    yScale(float(yCellCount) / height_)
    {
        boxCells.resize(xCellCount * yCellCount);
        circleCells.resize(xCellCount * yCellCount);
    }

template <class T>
void GridIndex<T>::insert(T&& t, const BBox& bbox) {
    size_t uid = boxElements.size();

    auto cx1 = convertToXCellCoord(bbox.min.x);
    auto cy1 = convertToYCellCoord(bbox.min.y);
    auto cx2 = convertToXCellCoord(bbox.max.x);
    auto cy2 = convertToYCellCoord(bbox.max.y);

    int32_t x, y, cellIndex;
    for (x = cx1; x <= cx2; ++x) {
        for (y = cy1; y <= cy2; ++y) {
            cellIndex = xCellCount * y + x;
            boxCells[cellIndex].push_back(uid);
        }
    }

    boxElements.emplace_back(t, bbox);
}

template <class T>
void GridIndex<T>::insert(T&& t, const BCircle& bcircle) {
    size_t uid = circleElements.size();

    auto cx1 = convertToXCellCoord(bcircle.center.x - bcircle.radius);
    auto cy1 = convertToYCellCoord(bcircle.center.y - bcircle.radius);
    auto cx2 = convertToXCellCoord(bcircle.center.x + bcircle.radius);
    auto cy2 = convertToYCellCoord(bcircle.center.y + bcircle.radius);

    int32_t x, y, cellIndex;
    for (x = cx1; x <= cx2; ++x) {
        for (y = cy1; y <= cy2; ++y) {
            cellIndex = xCellCount * y + x;
            circleCells[cellIndex].push_back(uid);
        }
    }

    circleElements.emplace_back(t, bcircle);
}

template <class T>
std::vector<T> GridIndex<T>::query(const BBox& queryBBox) const {
    std::vector<T> result;
    query(queryBBox, [&](const T& t) -> bool {
        result.push_back(t);
        return false;
    });
    return result;
}

// TODO: templatize this on geometry type
template <class T>
std::vector<T> GridIndex<T>::query(const BCircle& queryBCircle) const {
    std::vector<T> result;
    query(queryBCircle, [&](const T& t) -> bool {
        result.push_back(t);
        return false;
    });
    return result;
}

template <class T>
bool GridIndex<T>::hitTest(const BBox& queryBBox) const {
    bool hit = false;
    query(queryBBox, [&](const T&) -> bool {
        hit = true;
        return true;
    });
    return hit;
}

// TODO: templatize this on geometry type
template <class T>
bool GridIndex<T>::hitTest(const BCircle& queryBCircle) const {
    bool hit = false;
    query(queryBCircle, [&](const T&) -> bool {
        hit = true;
        return true;
    });
    return hit;
}

template <class T>
void GridIndex<T>::query(const BBox& queryBBox, std::function<bool (const T&)> resultFn) const {
    std::unordered_set<size_t> seenBoxes;
    std::unordered_set<size_t> seenCircles;

    auto cx1 = convertToXCellCoord(queryBBox.min.x);
    auto cy1 = convertToYCellCoord(queryBBox.min.y);
    auto cx2 = convertToXCellCoord(queryBBox.max.x);
    auto cy2 = convertToYCellCoord(queryBBox.max.y);

    int32_t x, y, cellIndex;
    for (x = cx1; x <= cx2; ++x) {
        for (y = cy1; y <= cy2; ++y) {
            cellIndex = xCellCount * y + x;
            // Look up other boxes
            for (auto uid : boxCells[cellIndex]) {
                if (seenBoxes.count(uid) == 0) {
                    seenBoxes.insert(uid);

                    auto& pair = boxElements.at(uid);
                    auto& bbox = pair.second;
                    if (boxesCollide(queryBBox, bbox)) {
                        if (resultFn(pair.first)) {
                            return;
                        }
                    }
                }
            }
            
            // Look up circles
            for (auto uid : circleCells[cellIndex]) {
                if (seenCircles.count(uid) == 0) {
                    seenCircles.insert(uid);

                    auto& pair = circleElements.at(uid);
                    auto& bcircle = pair.second;
                    if (circleAndBoxCollide(bcircle, queryBBox)) {
                        if (resultFn(pair.first)) {
                            return;
                        }
                    }
                }
            }
        }
    }
}

template <class T>
void GridIndex<T>::query(const BCircle& queryBCircle, std::function<bool (const T&)> resultFn) const {
    std::unordered_set<size_t> seenBoxes;
    std::unordered_set<size_t> seenCircles;

    auto cx1 = convertToXCellCoord(queryBCircle.center.x - queryBCircle.radius);
    auto cy1 = convertToYCellCoord(queryBCircle.center.y - queryBCircle.radius);
    auto cx2 = convertToXCellCoord(queryBCircle.center.x + queryBCircle.radius);
    auto cy2 = convertToYCellCoord(queryBCircle.center.y + queryBCircle.radius);

    int32_t x, y, cellIndex;
    for (x = cx1; x <= cx2; ++x) {
        for (y = cy1; y <= cy2; ++y) {
            cellIndex = xCellCount * y + x;
            // Look up boxes
            for (auto uid : boxCells[cellIndex]) {
                if (seenBoxes.count(uid) == 0) {
                    seenBoxes.insert(uid);

                    auto& pair = boxElements.at(uid);
                    auto& bbox = pair.second;
                    if (circleAndBoxCollide(queryBCircle, bbox)) {
                        if (resultFn(pair.first)) {
                            return;
                        }
                    }
                }
            }
            
            // Look up other circles
            for (auto uid : circleCells[cellIndex]) {
                if (seenCircles.count(uid) == 0) {
                    seenCircles.insert(uid);

                    auto& pair = circleElements.at(uid);
                    auto& bcircle = pair.second;
                    if (circlesCollide(queryBCircle, bcircle)) {
                        if (resultFn(pair.first)) {
                            return;
                        }
                    }
                }
            }
        }
    }
}

template <class T>
int32_t GridIndex<T>::convertToXCellCoord(int32_t x) const {
    return util::max(0.0, util::min(xCellCount - 1.0, std::floor(x * xScale)));
}

template <class T>
int32_t GridIndex<T>::convertToYCellCoord(int32_t y) const {
    return util::max(0.0, util::min(yCellCount - 1.0, std::floor(y * yScale)));
}

template <class T>
bool GridIndex<T>::boxesCollide(const BBox& first, const BBox& second) const {
	return first.min.x <= second.max.x &&
           first.min.y <= second.max.y &&
           first.max.x >= second.min.x &&
           first.max.y >= second.min.y;
}

template <class T>
bool GridIndex<T>::circlesCollide(const BCircle& first, const BCircle& second) const {
    auto dx = second.center.x - first.center.x;
    auto dy = second.center.y - first.center.y;
    auto bothRadii = first.radius + second.radius;
    return (bothRadii * bothRadii) > (dx * dx + dy * dy);
}

template <class T>
bool GridIndex<T>::circleAndBoxCollide(const BCircle& circle, const BBox& box) const {
    auto halfRectWidth = (box.max.x - box.min.x) / 2;
    auto distX = std::abs(circle.center.x - (box.min.x + halfRectWidth));
    if (distX > (halfRectWidth + circle.radius)) {
        return false;
    }

    auto halfRectHeight = (box.max.y - box.min.y) / 2;
    auto distY = std::abs(circle.center.y - (box.min.y + halfRectHeight));
    if (distY > (halfRectHeight + circle.radius)) {
        return false;
    }

    if (distX <= halfRectWidth || distY <= halfRectHeight) {
        return true;
    }

    auto dx = distX - halfRectWidth;
    auto dy = distY - halfRectHeight;
    return (dx * dx + dy * dy) <= (circle.radius * circle.radius);
}

template <class T>
bool GridIndex<T>::empty() const {
    return boxElements.empty() && circleElements.empty();
}


template class GridIndex<IndexedSubfeature>;

} // namespace mbgl
