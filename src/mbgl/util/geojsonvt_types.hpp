#ifndef MAPBOX_UTIL_GEOJSONVT_TYPES
#define MAPBOX_UTIL_GEOJSONVT_TYPES

#include <mbgl/util/variant.hpp>

#include <rapidjson/document.h>

#include <array>
#include <map>
#include <string>
#include <vector>

namespace mapbox { namespace util { namespace geojsonvt {

struct LonLat {
    LonLat(std::array<double, 2> coordinates)
        : lon(coordinates[0]), lat(coordinates[1]) {}

    LonLat(double lon_, double lat_)
        : lon(lon_), lat(lat_) {}

    double lon;
    double lat;
};

#pragma mark -

class ProjectedPoint;
class ProjectedGeometryContainer;

using ProjectedGeometry = mapbox::util::variant<geojsonvt::ProjectedPoint, geojsonvt::ProjectedGeometryContainer>;

#pragma mark -

class ProjectedPoint {
public:
    ProjectedPoint(double x_ = -1, double y_ = -1, double z_ = -1)
        : x(x_), y(y_), z(z_) {}

    inline operator bool() const { return (x >= 0 && y >= 0 && z >= 0); }
    inline bool operator!= (const ProjectedPoint& rhs) const { return (x != rhs.x || y != rhs.y || z != rhs.z); }

public:
    double x = -1;
    double y = -1;
    double z = -1;
};

#pragma mark -

using JSDocument = rapidjson::Document;
using JSValue = rapidjson::Value;

#pragma mark -

class ProjectedGeometryContainer {
public:
    ProjectedGeometryContainer() {}
    ProjectedGeometryContainer(std::vector<ProjectedGeometry> members_)
        : members(members_) {}

public:
    std::vector<ProjectedGeometry> members;
    double area = 0;
    double dist = 0;
};

#pragma mark -

using Tags = std::map<std::string, std::string>;

#pragma mark -

enum class ProjectedFeatureType: uint8_t {
    Point = 1,
    LineString = 2,
    Polygon = 3
};

#pragma mark -

class ProjectedFeature {
public:
    ProjectedFeature(ProjectedGeometry geometry_, ProjectedFeatureType type_, Tags tags_)
        : geometry(geometry_), type(type_), tags(tags_) {}

public:
    ProjectedGeometry geometry;
    ProjectedFeatureType type;
    Tags tags;
    ProjectedPoint minPoint = ProjectedPoint();
    ProjectedPoint maxPoint = ProjectedPoint();
};

#pragma mark -

class TilePoint;
class TileRing;

using TileGeometry = mapbox::util::variant<geojsonvt::TilePoint, geojsonvt::TileRing>;

#pragma mark -

class TilePoint {
public:
    TilePoint(int16_t x_, int16_t y_)
        : x(x_), y(y_) {}

public:
    const int16_t x = 0;
    const int16_t y = 0;
};

#pragma mark -

class TileRing {
public:
    std::vector<TilePoint> points;
};

#pragma mark -

typedef ProjectedFeatureType TileFeatureType;

#pragma mark -

class TileFeature {
public:
    TileFeature(std::vector<TileGeometry> geometry_, TileFeatureType type_, Tags tags_)
        : geometry(geometry_), type(type_), tags(tags_) {}

public:
    std::vector<TileGeometry> geometry;
    TileFeatureType type;
    Tags tags;
};

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */

#endif // MAPBOX_UTIL_GEOJSONVT_TYPES
