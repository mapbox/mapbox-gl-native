#include <mbgl/util/grid_index.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/math/minmax.hpp>

#include <unordered_set>
#include <cmath>

namespace mbgl {


template <class T>
GridIndex<T>::GridIndex(const float width_, const float height_, const int16_t cellSize_) :
    width(width_),
    height(height_),
    xCellCount(std::ceil(width_ / cellSize_)),
    yCellCount(std::ceil(height_ / cellSize_)),
    xScale(xCellCount / width_),
    yScale(yCellCount / height_)
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

    int16_t x, y, cellIndex;
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

    int16_t x, y, cellIndex;
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
    query(queryBBox, [&](const T& t, const BBox&) -> bool {
        result.push_back(t);
        return false;
    });
    return result;
}

template <class T>
std::vector<std::pair<T, typename GridIndex<T>::BBox>> GridIndex<T>::queryWithBoxes(const BBox& queryBBox) const {
    std::vector<std::pair<T, BBox>> result;
    query(queryBBox, [&](const T& t, const BBox& bbox) -> bool {
        result.push_back(std::make_pair(t, bbox));
        return false;
    });
    return result;
}

template <class T>
bool GridIndex<T>::hitTest(const BBox& queryBBox) const {
    bool hit = false;
    query(queryBBox, [&](const T&, const BBox&) -> bool {
        hit = true;
        return true;
    });
    return hit;
}

template <class T>
bool GridIndex<T>::hitTest(const BCircle& queryBCircle) const {
    bool hit = false;
    query(queryBCircle, [&](const T&, const BBox&) -> bool {
        hit = true;
        return true;
    });
    return hit;
}

template <class T>
bool GridIndex<T>::noIntersection(const BBox& queryBBox) const {
    return queryBBox.max.x < 0 || queryBBox.min.x >= width || queryBBox.max.y < 0 || queryBBox.min.y >= height;
}

template <class T>
bool GridIndex<T>::completeIntersection(const BBox& queryBBox) const {
    return queryBBox.min.x <= 0 && queryBBox.min.y <= 0 && width <= queryBBox.max.x && height <= queryBBox.max.y;
}

template <class T>
typename GridIndex<T>::BBox GridIndex<T>::convertToBox(const BCircle& circle) const {
    return BBox{{circle.center.x - circle.radius, circle.center.y - circle.radius},
                {circle.center.x + circle.radius, circle.center.y + circle.radius}};
}

template <class T>
void GridIndex<T>::query(const BBox& queryBBox, std::function<bool (const T&, const BBox&)> resultFn) const {
    std::unordered_set<size_t> seenBoxes;
    std::unordered_set<size_t> seenCircles;
    
    if (noIntersection(queryBBox)) {
        return;
    } else if (completeIntersection(queryBBox)) {
        for (auto& element : boxElements) {
            if (resultFn(element.first, element.second)) {
                return;
            }
        }
        for (auto& element : circleElements) {
            if (resultFn(element.first, convertToBox(element.second))) {
                return;
            }
        }
        return;
    }

    auto cx1 = convertToXCellCoord(queryBBox.min.x);
    auto cy1 = convertToYCellCoord(queryBBox.min.y);
    auto cx2 = convertToXCellCoord(queryBBox.max.x);
    auto cy2 = convertToYCellCoord(queryBBox.max.y);

    int16_t x, y, cellIndex;
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
                        if (resultFn(pair.first, bbox)) {
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
                        if (resultFn(pair.first, convertToBox(bcircle))) {
                            return;
                        }
                    }
                }
            }
        }
    }
}

template <class T>
void GridIndex<T>::query(const BCircle& queryBCircle, std::function<bool (const T&, const BBox&)> resultFn) const {
    std::unordered_set<size_t> seenBoxes;
    std::unordered_set<size_t> seenCircles;

    BBox queryBBox = convertToBox(queryBCircle);
    if (noIntersection(queryBBox)) {
        return;
    } else if (completeIntersection(queryBBox)) {
        for (auto& element : boxElements) {
            if (resultFn(element.first, element.second)) {
                return;
            }
        }
        for (auto& element : circleElements) {
            if (resultFn(element.first, convertToBox(element.second))) {
                return;
            }
        }
    }

    auto cx1 = convertToXCellCoord(queryBCircle.center.x - queryBCircle.radius);
    auto cy1 = convertToYCellCoord(queryBCircle.center.y - queryBCircle.radius);
    auto cx2 = convertToXCellCoord(queryBCircle.center.x + queryBCircle.radius);
    auto cy2 = convertToYCellCoord(queryBCircle.center.y + queryBCircle.radius);

    int16_t x, y, cellIndex;
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
                        if (resultFn(pair.first, bbox)) {
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
                        if (resultFn(pair.first, convertToBox(bcircle))) {
                            return;
                        }
                    }
                }
            }
        }
    }
}

template <class T>
int16_t GridIndex<T>::convertToXCellCoord(const float x) const {
    return util::max(0.0, util::min(xCellCount - 1.0, std::floor(x * xScale)));
}

template <class T>
int16_t GridIndex<T>::convertToYCellCoord(const float y) const {
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
