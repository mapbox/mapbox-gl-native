// This file is auto-generated, manual changes will be lost if the code is regenerated.

#pragma once

#include <vector>
#include <map>
#include <string>
#include <utility>

#ifdef _MSC_VER // msvc has trouble optimizing large inline std::vectors, so we disable the optimization
# define MAPBOX_MSVC_DISABLE_OPTIMIZATION() __pragma(optimize("", off))
# define MAPBOX_MSVC_ENABLE_OPTIMIZATION() __pragma(optimize("", on))
#else
# define MAPBOX_MSVC_DISABLE_OPTIMIZATION()
# define MAPBOX_MSVC_ENABLE_OPTIMIZATION()
#endif

namespace mapbox {
namespace fixtures {

template <typename T> using Polygon = std::vector<std::vector<T>>;
template <typename T> using Triangles = std::vector<T>;

using ShortPoint = std::pair<short, short>;
using ShortTriangles = Triangles<ShortPoint>;
using ShortPolygon = Polygon<ShortPoint>;

using IntegerPoint = std::pair<int, int>;
using IntegerTriangles = Triangles<IntegerPoint>;
using IntegerPolygon = Polygon<IntegerPoint>;

using DoublePoint = std::pair<double, double>;
using DoubleTriangles = Triangles<DoublePoint>;
using DoublePolygon = Polygon<DoublePoint>;


extern const ShortPolygon park;
extern const IntegerPolygon bad_diagonals;
extern const IntegerPolygon bad_hole;
extern const IntegerPolygon building;
extern const IntegerPolygon degenerate;
extern const DoublePolygon dude;
extern const IntegerPolygon eberly_3;
extern const DoublePolygon eberly_6;
extern const IntegerPolygon empty_square;
extern const IntegerPolygon hilbert;
extern const IntegerPolygon hole_touching_outer;
extern const DoublePolygon issue16;
extern const DoublePolygon issue17;
extern const DoublePolygon issue29;
extern const IntegerPolygon issue34;
extern const IntegerPolygon issue35;
extern const IntegerPolygon issue45;
extern const IntegerPolygon issue52;
extern const IntegerPolygon outside_ring;
extern const DoublePolygon self_touching;
extern const IntegerPolygon shared_points;
extern const IntegerPolygon simplified_us_border;
extern const IntegerPolygon steiner;
extern const IntegerPolygon touching_holes;
extern const IntegerPolygon water_huge;
extern const IntegerPolygon water_huge2;
extern const IntegerPolygon water;
extern const IntegerPolygon water2;
extern const IntegerPolygon water3;
extern const IntegerPolygon water3b;
extern const IntegerPolygon water4;

}
}
