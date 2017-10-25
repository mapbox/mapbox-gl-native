#pragma once

#include <mapbox/geometry/point.hpp>
#include <mapbox/geometry/box.hpp>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <functional>

// TODO: Move into geometry.hpp project
namespace mapbox {
namespace geometry {

template <typename T>
struct circle
{
    using point_type = point<T>;

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
} // namespace mapbox

namespace mbgl {

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
    GridIndex(int32_t width_, int32_t height_, int32_t cellSize_);

    using BBox = mapbox::geometry::box<int16_t>;
    using BCircle = mapbox::geometry::circle<int16_t>;

    void insert(T&& t, const BBox&);
    void insert(T&& t, const BCircle&);
    
    std::vector<T> query(const BBox&) const;
    std::vector<T> query(const BCircle&) const;
    
    bool hitTest(const BBox&) const;
    bool hitTest(const BCircle&) const;
    
    bool empty() const;

private:

    void query(const BBox&, std::function<bool (const T&)>) const;
    void query(const BCircle&, std::function<bool (const T&)>) const;

    int32_t convertToXCellCoord(int32_t x) const;
    int32_t convertToYCellCoord(int32_t y) const;
    
    bool boxesCollide(const BBox&, const BBox&) const;
    bool circlesCollide(const BCircle&, const BCircle&) const;
    bool circleAndBoxCollide(const BCircle&, const BBox&) const;

    const int32_t width;
    const int32_t height;
    
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
