#include "geojsonvt_convert.hpp"
#include "geojsonvt_simplify.hpp"

#include <array>
#include <cmath>
#include <vector>

namespace mapbox { namespace util { namespace geojsonvt {

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
        a = (b ? b : geometryContainer.members[i].get<ProjectedPoint>());
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

} /* namespace geojsonvt */ } /* namespace util */ } /* namespace mapbox */
