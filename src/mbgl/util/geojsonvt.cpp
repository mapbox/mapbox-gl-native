#include <mbgl/util/geojsonvt.hpp>

#include <queue>
#include <stack>
#include <cmath>

namespace mapbox { namespace util { namespace geojsonvt {

std::unordered_map<std::string, clock_t> Time::activities;

#pragma mark - Tile

Tile Tile::createTile(std::vector<ProjectedFeature> &features, uint32_t z2, uint32_t tx, uint32_t ty, double tolerance, uint16_t extent, bool noSimplify) {

    Tile tile;

    for (size_t i = 0; i < features.size(); ++i) {
        tile.numFeatures++;
        addFeature(tile, features[i], z2, tx, ty, tolerance, extent, noSimplify);
    }

    return std::move(tile);
}

void Tile::addFeature(Tile &tile, ProjectedFeature &feature, uint32_t z2, uint32_t tx, uint32_t ty, double tolerance, uint16_t extent, bool noSimplify) {

    ProjectedGeometryContainer *geom = &(feature.geometry.get<ProjectedGeometryContainer>());
    ProjectedFeatureType type = feature.type;
    std::vector<TileGeometry> transformed;
    double sqTolerance = tolerance * tolerance;
    ProjectedGeometryContainer ring;

    if (type == ProjectedFeatureType::Point) {
        for (size_t i = 0; i < geom->members.size(); ++i) {
            ProjectedPoint *p = &(geom->members[i].get<ProjectedPoint>());
            transformed.push_back(transformPoint(*p, z2, tx, ty, extent));
            tile.numPoints++;
            tile.numSimplified++;
        }
    } else {
        for (size_t i = 0; i < geom->members.size(); ++i) {
            ring = geom->members[i].get<ProjectedGeometryContainer>();

            if (!noSimplify && ((type == ProjectedFeatureType::LineString && ring.dist < tolerance) ||
                (type == ProjectedFeatureType::Polygon && ring.area < sqTolerance))) {
                tile.numPoints += ring.members.size();
                continue;
            }

            TileRing transformedRing;

            for (size_t j = 0; j < ring.members.size(); ++j) {
                ProjectedPoint *p = &(ring.members[j].get<ProjectedPoint>());
                if (noSimplify || p->z > sqTolerance) {
                    TilePoint transformedPoint = transformPoint(*p, z2, tx, ty, extent);
                    transformedRing.points.push_back(transformedPoint);
                    tile.numSimplified++;
                }
                tile.numPoints++;
            }

            transformed.push_back(transformedRing);
        }
    }

    if (transformed.size()) {
        tile.features.push_back(TileFeature(transformed, type, Tags(feature.tags)));
    }

}

TilePoint Tile::transformPoint(const ProjectedPoint &p, uint32_t z2, uint32_t tx, uint32_t ty, uint16_t extent) {

    uint16_t x = extent * (p.x * z2 - tx);
    uint16_t y = extent * (p.y * z2 - ty);

    return TilePoint(x, y);
}

#pragma mark - GeoJSONVT

std::vector<ProjectedFeature> GeoJSONVT::convertFeatures(const std::string &data, uint8_t baseZoom, double tolerance, bool debug) {

    if (debug) {
        Time::time("preprocess data");
    }

    uint32_t z2 = 1 << baseZoom;

    JSDocument deserializedData;
    deserializedData.Parse<0>(data.c_str());

    if (deserializedData.HasParseError()) {
        printf("invalid GeoJSON\n");
        return std::vector<ProjectedFeature>();
    }

    const uint16_t extent = 4096;

    std::vector<ProjectedFeature> features = Convert::convert(deserializedData, tolerance / (z2 * extent));

    if (debug) {
        Time::timeEnd("preprocess data");
    }

    return features;
}

GeoJSONVT::GeoJSONVT(const std::vector<ProjectedFeature>& features_, uint8_t baseZoom_, uint8_t maxZoom_, uint32_t maxPoints_, double tolerance_, bool debug_)
    : baseZoom(baseZoom_),
      maxZoom(maxZoom_),
      maxPoints(maxPoints_),
      tolerance(tolerance_),
      debug(debug_) {

    if (this->debug) {
        Time::time("generate tiles up to z" + std::to_string(maxZoom));
    }

    splitTile(features_, 0, 0, 0);

    if (this->debug) {
        printf("features: %i, points: %i\n", this->tiles[0].numFeatures, this->tiles[0].numPoints);
        Time::timeEnd("generate tiles up to z" + std::to_string(maxZoom));
        printf("tiles generated: %i {\n", this->total);
        for (const auto &pair : this->stats) {
            printf("    z%i: %i\n", pair.first, pair.second);
        }
        printf("}\n");
    }
}

void GeoJSONVT::splitTile(std::vector<ProjectedFeature> features_, uint8_t z_, uint32_t x_, uint32_t y_, int8_t cz, int32_t cx, int32_t cy) {

    std::queue<FeatureStackItem> stack;
    stack.emplace(features_, z_, x_, y_);

    while (stack.size()) {
        FeatureStackItem set = stack.front();
        stack.pop();
        std::vector<ProjectedFeature> features = std::move(set.features);
        uint8_t z = set.z;
        uint32_t x = set.x;
        uint32_t y = set.y;

        uint32_t z2 = 1 << z;
        const uint64_t id = toID(z, x, y);
        Tile* tile;
        double tileTolerance = (z == this->baseZoom ? 0 : this->tolerance / (z2 * this->extent));

        if (this->tiles.count(id)) {
            tile = &this->tiles[id];
        } else {
            if (this->debug) {
                Time::time("creation");
            }

            this->tiles[id] = std::move(Tile::createTile(features, z2, x, y, tileTolerance, extent, (z == this->baseZoom)));
            tile = &this->tiles[id];

            if (this->debug) {
                printf("tile z%i-%i-%i (features: %i, points: %i, simplified: %i\n", z, x, y,
                       tile->numFeatures, tile->numPoints, tile->numSimplified);
                Time::timeEnd("creation");

                uint8_t key = z;
                this->stats[key] = (this->stats.count(key) ? this->stats[key] + 1 : 1);
                this->total++;
            }
        }

        if ((cz < 0 && (z == this->maxZoom || this->tiles[id].numPoints <= this->maxPoints ||
            isClippedSquare(tile->features, this->extent, this->buffer))) || z == this->baseZoom || z == cz) {
            tile->source = std::vector<ProjectedFeature>(features);
            continue;
        }

        if (cz >= 0) {
            tile->source = std::vector<ProjectedFeature>(features);
        } else {
            tile->source = {};
        }

        if (this->debug) {
            Time::time("clipping");
        }

        double k1 = 0.5 * this->buffer / this->extent;
        double k2 = 0.5 - k1;
        double k3 = 0.5 + k1;
        double k4 = 1 + k1;

        std::vector<ProjectedFeature> tl;
        std::vector<ProjectedFeature> bl;
        std::vector<ProjectedFeature> tr;
        std::vector<ProjectedFeature> br;
        std::vector<ProjectedFeature> left;
        std::vector<ProjectedFeature> right;
        uint32_t m = 0;
        bool goLeft = false;
        bool goTop = false;

        if (cz >= 0) {
            m = 1 << (cz - z);
            goLeft = cx / m - x < 0.5;
            goTop  = cy / m - y < 0.5;
        }

        if (cz < 0 || goLeft) {
            left = Clip::clip(features, z2, x - k1, x + k3, 0, intersectX);
        }

        if (cz < 0 || !goLeft) {
            right = Clip::clip(features, z2, x + k2, x + k4, 0, intersectX);
        }

        if (left.size()) {
            if (cz < 0 || goTop) {
                tl = Clip::clip(left, z2, y - k1, y + k3, 1, intersectY);
            }

            if (cz < 0 || !goTop) {
                bl = Clip::clip(left, z2, y + k2, y + k4, 1, intersectY);
            }
        }

        if (right.size()) {
            if (cz < 0 || goTop) {
                tr = Clip::clip(right, z2, y - k1, y + k3, 1, intersectY);
            }

            if (cz < 0 || !goTop) {
                br = Clip::clip(right, z2, y + k2, y + k4, 1, intersectY);
            }
        }

        if (this->debug) {
            Time::timeEnd("clipping");
        }

        if (tl.size()) stack.emplace(std::move(tl), z + 1, x * 2,     y * 2);
        if (bl.size()) stack.emplace(std::move(bl), z + 1, x * 2,     y * 2 + 1);
        if (tr.size()) stack.emplace(std::move(tr), z + 1, x * 2 + 1, y * 2);
        if (br.size()) stack.emplace(std::move(br), z + 1, x * 2 + 1, y * 2 + 1);
    }
}

Tile& GeoJSONVT::getTile(uint8_t z, uint32_t x, uint32_t y) {

//    std::lock_guard<std::mutex> lock(mtx);

    const uint64_t id = toID(z, x, y);
    if (this->tiles.count(id)) {
        return this->tiles[id];
    }

    if (this->debug) {
        printf("drilling down to z%i-%i-%i\n", z, x, y);
    }

    uint8_t z0 = z;
    uint32_t x0 = x;
    uint32_t y0 = y;
    Tile *parent = nullptr;

    while (!parent && z0) {
        z0--;
        x0 = x0 / 2;
        y0 = y0 / 2;
        const uint64_t checkID = toID(z0, x0, y0);
        if (this->tiles.count(checkID)) {
            parent = &this->tiles[checkID];
        }
    }

    if (this->debug) {
        printf("found parent tile z%i-%i-%i\n", z0, x0, y0);
    }

    if (parent->source.size()) {
        if (isClippedSquare(parent->features, this->extent, this->buffer)) {
            return *parent;
        }

        if (this->debug) {
            Time::time("drilling down");
        }

        splitTile(parent->source, z0, x0, y0, z, x, y);

        if (this->debug) {
            Time::timeEnd("drilling down");
        }
    }

    return this->tiles[id];
}

bool GeoJSONVT::isClippedSquare(const std::vector<TileFeature> &features, uint16_t extent_, uint8_t buffer_) const {

    if (features.size() != 1) {
        return false;
    }

    const TileFeature feature = features.front();

    if (feature.type != TileFeatureType::Polygon || feature.geometry.size() > 1) {
        return false;
    }

    const TileRing *ring = &(feature.geometry.front().get<TileRing>());

    for (size_t i = 0; i < ring->points.size(); ++i) {
        const TilePoint *p = &ring->points[i];
        if ((p->x != -buffer_ && p->x != extent_ + buffer_) ||
            (p->y != -buffer_ && p->y != extent_ + buffer_)) {
            return false;
        }
    }

    return true;
}

uint64_t GeoJSONVT::toID(uint8_t z, uint32_t x, uint32_t y) {

    return (((1 << z) * y + x) * 32) + z;
}

ProjectedPoint GeoJSONVT::intersectX(const ProjectedPoint &a, const ProjectedPoint &b, double x) {

    double r1 = x;
    double r2 = (x - a.x) * (b.y - a.y) / (b.x - a.x) + a.y;
    double r3 = 1;

    return ProjectedPoint(r1, r2, r3);
}

ProjectedPoint GeoJSONVT::intersectY(const ProjectedPoint &a, const ProjectedPoint &b, double y) {

    double r1 = (y - a.y) * (b.x - a.x) / (b.y - a.y) + a.x;
    double r2 = y;
    double r3 = 1;

    return ProjectedPoint(r1, r2, r3);
}

#pragma mark - Convert

std::vector<ProjectedFeature> Convert::convert(const JSDocument &data, double tolerance) {

    std::vector<ProjectedFeature> features;
    const JSValue &rawType = data["type"];

    if (std::string(rawType.GetString()) == "FeatureCollection") {
        if (data.HasMember("features")) {
            const JSValue &rawFeatures = data["features"];
            if (rawFeatures.IsArray()) {
                printf("there are %i total features to convert\n", rawFeatures.Size());
                for (rapidjson::SizeType i = 0; i < rawFeatures.Size(); ++i) {
                    convertFeature(features, rawFeatures[i], tolerance);
                }
            }
        }
    } else if (std::string(data["type"].GetString()) == "Feature") {
        convertFeature(features, data, tolerance);
    } else {

        /* In this case, we want to pass the entire JSON document as the
         * value for key 'geometry' in a new JSON object, like so: 
         *
         * convertFeature(features, ["geometry": data], tolerance); (pseudo-code)
         *
         * Currently this fails due to lack of a proper copy constructor. 
         * Maybe use move semantics? */

//        JSValue feature;
//        feature.SetObject();
//        feature.AddMember("geometry", data, data.GetAllocator());
//        convertFeature(features, feature, tolerance);

    }

    return std::move(features);
}

void Convert::convertFeature(std::vector<ProjectedFeature> &features, const JSValue &feature, double tolerance) {

    const JSValue &geom = feature["geometry"];
    const JSValue &rawType = geom["type"];
    std::string type { rawType.GetString(), rawType.GetStringLength() };
    Tags tags;

    if (feature.HasMember("properties") && feature["properties"].IsObject()) {
        const JSValue &properties = feature["properties"];
        rapidjson::Value::ConstMemberIterator itr = properties.MemberBegin();
        for (; itr != properties.MemberEnd(); ++itr) {
            std::string key { itr->name.GetString(), itr->name.GetStringLength() };
            std::string val { itr->value.GetString(), itr->value.GetStringLength() };
            tags[key] = val;
        }
    }

    if (type == "Point") {

        std::array<double, 2> coordinates = {{ 0, 0 }};
        if (geom.HasMember("coordinates")) {
            const JSValue &rawCoordinates = geom["coordinates"];
            if (rawCoordinates.IsArray()) {
                coordinates[0] = rawCoordinates[(rapidjson::SizeType)0].GetDouble();
                coordinates[1] = rawCoordinates[(rapidjson::SizeType)1].GetDouble();
            }
        }
        ProjectedPoint point = projectPoint(LonLat(coordinates));

        std::vector<ProjectedGeometry> members = { point };
        ProjectedGeometryContainer geometry(members);

        features.push_back(create(tags, ProjectedFeatureType::Point, geometry));

    } else if (type == "MultiPoint") {

        std::vector<std::array<double, 2>> coordinatePairs;
        std::vector<LonLat> points;
        if (geom.HasMember("coordinates")) {
            const JSValue &rawCoordinatePairs = geom["coordinates"];
            if (rawCoordinatePairs.IsArray()) {
                for (rapidjson::SizeType i = 0; i < rawCoordinatePairs.Size(); ++i) {
                    std::array<double, 2> coordinates = {{ 0, 0 }};
                    const JSValue &rawCoordinates = rawCoordinatePairs[i];
                    if (rawCoordinates.IsArray()) {
                        coordinates[0] = rawCoordinates[(rapidjson::SizeType)0].GetDouble();
                        coordinates[1] = rawCoordinates[(rapidjson::SizeType)1].GetDouble();
                    }
                    points.push_back(LonLat(coordinates));
                }
            }
        }

        ProjectedGeometryContainer geometry = project(points);

        features.push_back(create(tags, ProjectedFeatureType::Point, geometry));

    } else if (type == "LineString") {

        std::vector<std::array<double, 2>> coordinatePairs;
        std::vector<LonLat> points;
        if (geom.HasMember("coordinates")) {
            const JSValue &rawCoordinatePairs = geom["coordinates"];
            if (rawCoordinatePairs.IsArray()) {
                for (rapidjson::SizeType i = 0; i < rawCoordinatePairs.Size(); ++i) {
                    std::array<double, 2> coordinates = {{ 0, 0 }};
                    const JSValue &rawCoordinates = rawCoordinatePairs[i];
                    if (rawCoordinates.IsArray()) {
                        coordinates[0] = rawCoordinates[(rapidjson::SizeType)0].GetDouble();
                        coordinates[1] = rawCoordinates[(rapidjson::SizeType)1].GetDouble();
                    }
                    points.push_back(LonLat(coordinates));
                }
            }
        }

        ProjectedGeometryContainer geometry({ project(points, tolerance) });

        features.push_back(create(tags, ProjectedFeatureType::LineString, geometry));

    } else if (type == "MultiLineString" || type == "Polygon") {

        ProjectedGeometryContainer rings;
        if (geom.HasMember("coordinates")) {
            const JSValue &rawLines = geom["coordinates"];
            if (rawLines.IsArray()) {
                for (rapidjson::SizeType i = 0; i < rawLines.Size(); ++i) {
                    const JSValue &rawCoordinatePairs = rawLines[i];
                    if (rawCoordinatePairs.IsArray()) {
                        std::vector<LonLat> points;
                        for (rapidjson::SizeType j = 0; j < rawCoordinatePairs.Size(); ++j) {
                            std::array<double, 2> coordinates = {{ 0, 0 }};
                            const JSValue &rawCoordinates = rawCoordinatePairs[j];
                            if (rawCoordinates.IsArray()) {
                                coordinates[0] = rawCoordinates[(rapidjson::SizeType)0].GetDouble();
                                coordinates[1] = rawCoordinates[(rapidjson::SizeType)1].GetDouble();
                            }
                            points.push_back(LonLat(coordinates));
                        }
                        ProjectedGeometryContainer ring = project(points, tolerance);
                        rings.members.push_back(ring);
                    }
                }
            }
        }

        ProjectedFeatureType projectedType = (type == "Polygon" ?
            ProjectedFeatureType::Polygon :
            ProjectedFeatureType::LineString);

        ProjectedGeometryContainer *geometry = &rings;

        features.push_back(create(tags, projectedType, *geometry));

        printf("features now has %lu items\n", features.size());
    }

    else if (type == "MultiPolygon") {

        ProjectedGeometryContainer rings;
        if (geom.HasMember("coordinates")) {
            const JSValue &rawPolygons = geom["coordinates"];
            if (rawPolygons.IsArray()) {
                for (rapidjson::SizeType i = 0; i < rawPolygons.Size(); ++i) {
                    std::vector<LonLat> points;
                    const JSValue &rawLines = rawPolygons[i];
                    for (rapidjson::SizeType j = 0; j < rawLines.Size(); ++j) {
                        std::array<double, 2> coordinates = {{ 0, 0 }};
                        const JSValue &rawCoordinatePairs = rawLines[i];
                        if (rawCoordinatePairs.IsArray()) {
                            coordinates[0] = rawCoordinatePairs[(rapidjson::SizeType)0].GetDouble();
                            coordinates[1] = rawCoordinatePairs[(rapidjson::SizeType)1].GetDouble();
                        }
                        points.push_back(LonLat(coordinates));
                    }
                    ProjectedGeometryContainer ring = project(points, tolerance);
                    rings.members.push_back(ring);
                }
            }
        }

        ProjectedGeometryContainer *geometry = &rings;

        features.push_back(create(tags, ProjectedFeatureType::Polygon, *geometry));

    } else if (type == "GeometryCollection") {

        if (geom.HasMember("geometries")) {
            const JSValue &rawGeometries = geom["geometries"];
            if (rawGeometries.IsArray()) {
                for (rapidjson::SizeType i = 0; i < rawGeometries.Size(); ++i) {
                    convertFeature(features, rawGeometries[i], tolerance);
                }
            }
        }

    } else {

        printf("unsupported GeoJSON type: %s\n", geom["type"].GetString());

    }
}

ProjectedFeature Convert::create(Tags tags, ProjectedFeatureType type, ProjectedGeometry geometry) {

    ProjectedFeature feature(geometry, type, tags);
    calcBBox(feature);

    return std::move(feature);
}

ProjectedGeometryContainer Convert::project(const std::vector<LonLat> &lonlats, double tolerance) {

    ProjectedGeometryContainer projected;
    for (size_t i = 0; i < lonlats.size(); ++i) {
        projected.members.push_back(projectPoint(lonlats[i]));
    }
    if (tolerance) {
        Simplify::simplify(projected, tolerance);
        calcSize(projected);
    }

    return std::move(projected);
}

ProjectedPoint Convert::projectPoint(const LonLat &p_) {

    double sine = std::sin(p_.lat * M_PI / 180);
    double x = p_.lon / 360 + 0.5;
    double y = 0.5 - 0.25 * std::log((1 + sine) / (1 - sine)) / M_PI;

    return ProjectedPoint(x, y, 0);
}

void Convert::calcSize(ProjectedGeometryContainer &geometryContainer) {

    double area = 0, dist = 0;
    ProjectedPoint a, b;

    for (size_t i = 0; i < geometryContainer.members.size() - 1; ++i) {
        a = (b.isValid() ? b : geometryContainer.members[i].get<ProjectedPoint>());
        b = geometryContainer.members[i + 1].get<ProjectedPoint>();

        area += a.x * b.y - b.x * a.y;
        dist += std::abs(b.x - a.x) + std::abs(b.y - a.y);
    }

    geometryContainer.area = std::abs(area / 2);
    geometryContainer.dist = dist;
}

void Convert::calcBBox(ProjectedFeature &feature) {

    ProjectedGeometryContainer *geometry = &(feature.geometry.get<ProjectedGeometryContainer>());
    ProjectedPoint *minPoint = &(feature.minPoint);
    ProjectedPoint *maxPoint = &(feature.maxPoint);

    if (feature.type == ProjectedFeatureType::Point) {
        calcRingBBox(*minPoint, *maxPoint, *geometry);
    } else {
        for (size_t i = 0; i < geometry->members.size(); ++i) {
            ProjectedGeometryContainer *featureGeometry = &(geometry->members[i].get<ProjectedGeometryContainer>());
            calcRingBBox(*minPoint, *maxPoint, *featureGeometry);
        }
    }
}

void Convert::calcRingBBox(ProjectedPoint &minPoint, ProjectedPoint &maxPoint, const ProjectedGeometryContainer &geometry) {

    for (size_t i = 0; i < geometry.members.size(); ++i) {
        const ProjectedPoint *p = &(geometry.members[i].get<ProjectedPoint>());
        minPoint.x = std::min(p->x, minPoint.x);
        maxPoint.x = std::max(p->x, maxPoint.x);
        minPoint.y = std::min(p->y, minPoint.y);
        maxPoint.y = std::max(p->y, maxPoint.y);
    }
}

#pragma mark - Simplify

void Simplify::simplify(ProjectedGeometryContainer &points, double tolerance) {

    const double sqTolerance = tolerance * tolerance;
    const size_t len = points.members.size();
    size_t first = 0;
    size_t last = len - 1;
    std::stack<size_t> stack;
    double maxSqDist = 0;
    double sqDist = 0;
    size_t index = 0;

    points.members[first].get<ProjectedPoint>().z = 1;
    points.members[last].get<ProjectedPoint>().z  = 1;

    while (last) {

        maxSqDist = 0;

        for (size_t i = (first + 1); i < last; ++i) {
            sqDist = getSqSegDist(points.members[i].get<ProjectedPoint>(),
                points.members[first].get<ProjectedPoint>(),
                points.members[last].get<ProjectedPoint>());

            if (sqDist > maxSqDist) {
                index = i;
                maxSqDist = sqDist;
            }
        }

        if (maxSqDist > sqTolerance) {
            points.members[index].get<ProjectedPoint>().z = maxSqDist;
            stack.push(first);
            stack.push(index);
            stack.push(index);
            stack.push(last);
        }

        if (stack.size()) {
            last = stack.top();
            stack.pop();
        } else {
            last = 0;
        }

        if (stack.size()) {
            first = stack.top();
            stack.pop();
        } else {
            first = 0;
        }
    }
}

double Simplify::getSqSegDist(const ProjectedPoint &p, const ProjectedPoint &a, const ProjectedPoint &b) {

    double x = a.x;
    double y = a.y;
    double dx = b.x - a.x;
    double dy = b.y - a.y;

    if (dx || dy) {

        const double t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / (dx * dx + dy * dy);

        if (t > 1) {
            x = b.x;
            y = b.y;
        } else if (t) {
            x += dx * t;
            y += dy * t;
        }
    }

    dx = p.x - x;
    dy = p.y - y;

    return dx * dx + dy * dy;
}

#pragma mark - Clip

std::vector<ProjectedFeature> Clip::clip(const std::vector<ProjectedFeature> features, uint32_t scale, double k1, double k2, uint8_t axis, ProjectedPoint (*intersect)(const ProjectedPoint&, const ProjectedPoint&, double)) {

    k1 /= scale;
    k2 /= scale;

    std::vector<ProjectedFeature> clipped;

    for (size_t i = 0; i < features.size(); ++i) {

        const ProjectedFeature feature = features[i];
        const ProjectedGeometry geometry = feature.geometry;
        const ProjectedFeatureType type = feature.type;
        double min = 0;
        double max = 0;

        if (feature.minPoint.isValid()) {
            min = (axis == 0 ? feature.minPoint.x : feature.minPoint.y);
            max = (axis == 0 ? feature.maxPoint.x : feature.maxPoint.y);

            if (min >= k1 && max <= k2) {
                clipped.push_back(feature);
                continue;
            } else if (min > k2 || max < k1) {
                continue;
            }
        }

        ProjectedGeometryContainer slices;

        if (type == ProjectedFeatureType::Point) {
            slices = clipPoints(geometry.get<ProjectedGeometryContainer>(), k1, k2, axis);
        } else {
            slices = clipGeometry(geometry.get<ProjectedGeometryContainer>(), k1, k2, axis, intersect, (type == ProjectedFeatureType::Polygon));
        }

        if (slices.members.size()) {
            clipped.push_back(ProjectedFeature(slices, type, features[i].tags));
        }
    }

    return std::move(clipped);
}

ProjectedGeometryContainer Clip::clipPoints(ProjectedGeometryContainer geometry, double k1, double k2, uint8_t axis) {

    ProjectedGeometryContainer slice;

    for (size_t i = 0; i < geometry.members.size(); ++i) {
        ProjectedPoint *a = &(geometry.members[i].get<ProjectedPoint>());
        double ak = (axis == 0 ? a->x : a->y);

        if (ak >= k1 && ak <= k2) {
            slice.members.push_back(*a);
        }
    }

    return std::move(slice);
}

ProjectedGeometryContainer Clip::clipGeometry(ProjectedGeometryContainer geometry, double k1, double k2, uint8_t axis, ProjectedPoint (*intersect)(const ProjectedPoint&, const ProjectedPoint&, double), bool closed) {

    ProjectedGeometryContainer slices;

    for (size_t i = 0; i < geometry.members.size(); ++i) {

        double ak = 0;
        double bk = 0;
        ProjectedPoint b;
        const ProjectedGeometryContainer *points = &(geometry.members[i].get<ProjectedGeometryContainer>());
        const double area = points->area;
        const double dist = points->dist;
        const size_t len = points->members.size();
        ProjectedPoint a;

        ProjectedGeometryContainer slice;

        for (size_t j = 0; j < (len - 1); ++j) {
            a = (b.isValid() ? b : points->members[j].get<ProjectedPoint>());
            b = points->members[j + 1].get<ProjectedPoint>();
            ak = (bk ? bk : (axis == 0 ? a.x : a.y));
            bk = (axis == 0 ? b.x : b.y);

            if (ak < k1) {
                if (bk > k2) {
                    slice.members.push_back(intersect(a, b, k1));
                    slice.members.push_back(intersect(a, b, k2));
                    if (!closed) {
                        slice = newSlice(slices, slice, area, dist);
                    }
                } else if (bk >= k1) {
                    slice.members.push_back(intersect(a, b, k1));
                }
            } else if (ak > k2) {
                if (bk < k1) {
                    slice.members.push_back(intersect(a, b, k2));
                    slice.members.push_back(intersect(a, b, k1));
                    if (!closed) {
                        slice = newSlice(slices, slice, area, dist);
                    }
                } else if (bk <= k2) {
                    slice.members.push_back(intersect(a, b, k2));
                }
            } else {
                slice.members.push_back(a);

                if (bk < k1) {
                    slice.members.push_back(intersect(a, b, k1));
                    if (!closed) {
                        slice = newSlice(slices, slice, area, dist);
                    }
                } else if (bk > k2) {
                    slice.members.push_back(intersect(a, b, k2));
                    if (!closed) {
                        slice = newSlice(slices, slice, area, dist);
                    }
                }
            }
        }

        a = points->members[len - 1].get<ProjectedPoint>();
        ak = (axis == 0 ? a.x : a.y);

        if (ak >= k1 && ak <= k2) {
            slice.members.push_back(a);
        }

        if (closed && slice.members.size()) {
            const ProjectedPoint *first = &(slice.members[0].get<ProjectedPoint>());
            const ProjectedPoint *last  = &(slice.members[slice.members.size() - 1].get<ProjectedPoint>());
            if (!first->isEqualToPoint(last)) {
                slice.members.push_back(ProjectedPoint(first->x, first->y, first->z));
            }
        }

        newSlice(slices, slice, area, dist);
    }

    return std::move(slices);
}

ProjectedGeometryContainer Clip::newSlice(ProjectedGeometryContainer &slices, ProjectedGeometryContainer &slice, double area, double dist) {

    if (slice.members.size()) {
        slice.area = area;
        slice.dist = dist;
        slices.members.push_back(slice);
    }

    return ProjectedGeometryContainer();
}

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */
