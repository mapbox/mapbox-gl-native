#pragma once

#include <mapbox/geometry/point.hpp>
#include <mapbox/geometry/box.hpp>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <functional>

namespace mbgl {

namespace geometry {

template <typename T>
struct circle
{
    using point_type = mapbox::geometry::point<T>;

    constexpr circle(point_type const& center_, T const& radius_)
        : center(center_), radius(radius_)
    {}

    point_type center;
    T radius;
};

template <typename T>
constexpr bool operator==(circle<T> const& lhs, circle<T> const& rhs)
{
    return lhs.center == rhs.center && lhs.radius == rhs.radius;
}

template <typename T>
constexpr bool operator!=(circle<T> const& lhs, circle<T> const& rhs)
{
    return lhs.center != rhs.center || lhs.radius != rhs.radius;
}

} // namespace geometry


/*
 GridIndex is a data structure for testing the intersection of
 circles and rectangles in a 2d plane.
 It is optimized for rapid insertion and querying.
 GridIndex splits the plane into a set of "cells" and keeps track
 of which geometries intersect with each cell. At query time,
 full geometry comparisons are only done for items that share
 at least one cell. As long as the geometries are relatively
 uniformly distributed across the plane, this greatly reduces
 the number of comparisons necessary.
*/

template <class T>
class GridIndex {
public:

    GridIndex(const float width_, const float height_, const int16_t cellSize_);

    using BBox = mapbox::geometry::box<float>;
    using BCircle = geometry::circle<float>;

    void insert(T&& t, const BBox&);
    void insert(T&& t, const BCircle&);
    
    std::vector<T> query(const BBox&) const;
    std::vector<std::pair<T,BBox>> queryWithBoxes(const BBox&) const;
    
    bool hitTest(const BBox&) const;
    bool hitTest(const BCircle&) const;
    
    bool empty() const;

private:
    bool noIntersection(const BBox& queryBBox) const;
    bool completeIntersection(const BBox& queryBBox) const;
    BBox convertToBox(const BCircle& circle) const;

    void query(const BBox&, std::function<bool (const T&, const BBox&)>) const;
    void query(const BCircle&, std::function<bool (const T&, const BBox&)>) const;

    int16_t convertToXCellCoord(const float x) const;
    int16_t convertToYCellCoord(const float y) const;
    
    bool boxesCollide(const BBox&, const BBox&) const;
    bool circlesCollide(const BCircle&, const BCircle&) const;
    bool circleAndBoxCollide(const BCircle&, const BBox&) const;

    const float width;
    const float height;
    
    const int16_t xCellCount;
    const int16_t yCellCount;
    const double xScale;
    const double yScale;

    std::vector<std::pair<T, BBox>> boxElements;
    std::vector<std::pair<T, BCircle>> circleElements;
    
    std::vector<std::vector<size_t>> boxCells;
    std::vector<std::vector<size_t>> circleCells;

};

} // namespace mbgl
