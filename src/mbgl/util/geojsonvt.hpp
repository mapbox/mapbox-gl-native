#ifndef MAPBOX_UTIL_GEOJSONVT
#define MAPBOX_UTIL_GEOJSONVT

#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <ctime>
#include <mutex>

#include <rapidjson/document.h>

#include <mbgl/util/variant.hpp>

namespace mapbox { namespace util { namespace geojsonvt {

#pragma mark -

class Time {
public:
    inline static void time(std::string activity) {
        Time::activities[activity] = clock();
    }

    inline static void timeEnd(std::string activity) {
        printf("%s: %fms\n", activity.c_str(), double(clock() - Time::activities[activity]) / (CLOCKS_PER_SEC / 1000));
    }

private:
    static std::unordered_map<std::string, clock_t> activities;
};

#pragma mark -

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
    ProjectedPoint minPoint = ProjectedPoint(1, 1, 0);
    ProjectedPoint maxPoint = ProjectedPoint(0, 0, 0);
};

#pragma mark -

class TilePoint;
class TileRing;

using TileGeometry = mapbox::util::variant<geojsonvt::TilePoint, geojsonvt::TileRing>;

#pragma mark -

class TilePoint {
public:
    TilePoint(uint16_t x_, uint16_t y_)
        : x(x_), y(y_) {}

public:
    const uint16_t x = 0;
    const uint16_t y = 0;
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

#pragma mark -

class Tile {
public:
    static Tile createTile(std::vector<ProjectedFeature> &features, uint32_t z2, uint32_t tx, uint32_t ty, double tolerance, uint16_t extent, bool noSimplify);

    static void addFeature(Tile &tile, ProjectedFeature &feature, uint32_t z2, uint32_t tx, uint32_t ty, double tolerance, uint16_t extent, bool noSimplify);

    inline operator bool() const { return this->numPoints > 0; }

private:
    static TilePoint transformPoint(const ProjectedPoint &p, uint32_t z2, uint32_t tx, uint32_t ty, uint16_t extent);

public:
    std::vector<TileFeature> features;
    uint32_t numPoints = 0;
    uint32_t numSimplified = 0;
    uint32_t numFeatures = 0;
    std::vector<ProjectedFeature> source;
};

#pragma mark -

class GeoJSONVT {
public:
    static std::vector<ProjectedFeature> convertFeatures(const std::string &data, uint8_t baseZoom = 14, double tolerance = 3, bool debug = true);

    GeoJSONVT(const std::vector<ProjectedFeature>& features_, uint8_t baseZoom = 14, uint8_t maxZoom = 4, uint32_t maxPoints = 100, double tolerance = 3, bool debug = true);

    Tile& getTile(uint8_t z, uint32_t x, uint32_t y);

private:
    void splitTile(std::vector<ProjectedFeature> features, uint8_t z, uint32_t x, uint32_t y, int8_t cz = -1, int32_t cx = -1, int32_t cy = -1);

    bool isClippedSquare(const std::vector<TileFeature> &features, uint16_t extent, uint8_t buffer) const;

    static uint64_t toID(uint8_t z, uint32_t x, uint32_t y);

    static ProjectedPoint intersectX(const ProjectedPoint &a, const ProjectedPoint &b, double x);

    static ProjectedPoint intersectY(const ProjectedPoint &a, const ProjectedPoint &b, double y);

    struct FeatureStackItem {
        std::vector<ProjectedFeature> features;
        uint8_t z;
        uint32_t x;
        uint32_t y;

        FeatureStackItem(std::vector<ProjectedFeature> features_, uint8_t z_, uint32_t x_, uint32_t y_)
            : features(features_), z(z_), x(x_), y(y_) {}
    };

private:
    std::mutex mtx;
    uint8_t baseZoom;
    uint8_t maxZoom;
    uint32_t maxPoints;
    double tolerance;
    bool debug;
    uint16_t extent = 4096;
    uint8_t buffer = 64;
    std::map<uint64_t, Tile> tiles;
    std::map<uint8_t, uint16_t> stats;
    uint16_t total = 0;
};

#pragma mark -

class Convert {
public:
    static std::vector<ProjectedFeature> convert(const JSDocument &data, double tolerance);

    static ProjectedFeature create(Tags tags, ProjectedFeatureType type, ProjectedGeometry geometry);

    static ProjectedGeometryContainer project(const std::vector<LonLat> &lonlats, double tolerance = 0);

private:
    static void convertFeature(std::vector<ProjectedFeature> &features, const JSValue &feature, double tolerance);

    static ProjectedPoint projectPoint(const LonLat &p);

    static void calcSize(ProjectedGeometryContainer &geometryContainer);

    static void calcBBox(ProjectedFeature &feature);

    static void calcRingBBox(ProjectedPoint &minPoint, ProjectedPoint &maxPoint, const ProjectedGeometryContainer &geometry);
};

#pragma mark -

class Simplify {
public:
    static void simplify(ProjectedGeometryContainer &points, double tolerance);

private:
    static double getSqSegDist(const ProjectedPoint &p, const ProjectedPoint &a, const ProjectedPoint &b);
};

#pragma mark -

class Clip {
public:
    static std::vector<ProjectedFeature> clip(std::vector<ProjectedFeature> features, uint32_t scale, double k1, double k2, uint8_t axis, ProjectedPoint (*intersect)(const ProjectedPoint&, const ProjectedPoint&, double));

private:
    static ProjectedGeometryContainer clipPoints(ProjectedGeometryContainer geometry, double k1, double k2, uint8_t axis);

    static ProjectedGeometryContainer clipGeometry(ProjectedGeometryContainer geometry, double k1, double k2, uint8_t axis, ProjectedPoint (*intersect)(const ProjectedPoint&, const ProjectedPoint&, double), bool closed);

    static ProjectedGeometryContainer newSlice(ProjectedGeometryContainer &slices, ProjectedGeometryContainer &slice, double area, double dist);
};

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */

#endif // MAPBOX_UTIL_GEOJSONVT
