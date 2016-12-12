#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>
#include <mapbox/geometry.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace mapbox::geojson;

geojson readGeoJSON(const std::string &path, bool use_convert) {
    std::ifstream t(path.c_str());
    std::stringstream buffer;
    buffer << t.rdbuf();
    if (use_convert) {
        rapidjson_document d;
        d.Parse<0>(buffer.str().c_str());
        return convert(d);
    } else {
        return parse(buffer.str());
    }
}

template <class T>
std::string writeGeoJSON(const T& t, bool use_convert) {
    if (use_convert) {
        rapidjson_allocator allocator;
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        convert(t, allocator).Accept(writer);
        return buffer.GetString();
    } else {
        return stringify(t);
    }
}

static void testPoint(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/point.json", use_convert);
    assert(data.is<geometry>());

    const auto &geom = data.get<geometry>();
    assert(geom.is<point>());

    const auto &p = geom.get<point>();
    assert(p.x == 30.5);
    assert(p.y == 50.5);

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

static void testMultiPoint(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/multi-point.json", use_convert);
    assert(data.is<geometry>());

    const auto &geom = data.get<geometry>();
    assert(geom.is<multi_point>());

    const auto &points = geom.get<multi_point>();
    assert(points.size() == 2);

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

static void testLineString(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/line-string.json", use_convert);
    assert(data.is<geometry>());

    const auto &geom = data.get<geometry>();
    assert(geom.is<line_string>());

    const auto &points = geom.get<line_string>();
    assert(points.size() == 2);

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

static void testMultiLineString(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/multi-line-string.json", use_convert);
    assert(data.is<geometry>());

    const auto &geom = data.get<geometry>();
    assert(geom.is<multi_line_string>());

    const auto &lines = geom.get<multi_line_string>();
    assert(lines.size() == 1);
    assert(lines[0].size() == 2);

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

static void testPolygon(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/polygon.json", use_convert);
    assert(data.is<geometry>());

    const auto &geom = data.get<geometry>();
    assert(geom.is<polygon>());

    const auto &rings = geom.get<polygon>();
    assert(rings.size() == 1);
    assert(rings[0].size() == 5);
    assert(rings[0][0] == rings[0][4]);

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

static void testMultiPolygon(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/multi-polygon.json", use_convert);
    assert(data.is<geometry>());

    const auto &geom = data.get<geometry>();
    assert(geom.is<multi_polygon>());

    const auto &polygons = geom.get<multi_polygon>();
    assert(polygons.size() == 1);
    assert(polygons[0].size() == 1);
    assert(polygons[0][0].size() == 5);
    assert(polygons[0][0][0] == polygons[0][0][4]);

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

static void testGeometryCollection(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/geometry-collection.json", use_convert);
    assert(data.is<geometry>());

    const auto &geom = data.get<geometry>();
    assert(geom.is<geometry_collection>());

    const auto &collection = geom.get<geometry_collection>();
    assert(collection[0].is<point>());
    assert(collection[1].is<line_string>());

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

static void testFeature(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/feature.json", use_convert);
    assert(data.is<feature>());

    const auto &f = data.get<feature>();
    assert(f.geometry.is<point>());

    assert(f.properties.at("bool").is<bool>());
    assert(f.properties.at("bool") == true);
    assert(f.properties.at("string").is<std::string>());
    assert(f.properties.at("string").get<std::string>() == "foo");
    assert(f.properties.at("double") == 2.5);
    assert(f.properties.at("double").is<double>());
    assert(f.properties.at("uint").get<std::uint64_t>() == 10);
    assert(f.properties.at("uint").is<std::uint64_t>());
    assert(f.properties.at("int").get<std::int64_t>() == -10);
    assert(f.properties.at("int").is<std::int64_t>());
    assert(f.properties.at("null").is<mapbox::geometry::null_value_t>());
    assert(f.properties.at("null") == mapbox::geometry::null_value_t{});

    using prop_map = std::unordered_map<std::string, value>;
    using values   = std::vector<value>;

    const auto &nested = f.properties.at("nested");

    // Explicit recursive_wrapper as a workaround for https://github.com/mapbox/variant/issues/102
    assert(nested.is<mapbox::util::recursive_wrapper<values>>());
    assert(nested.get<values>().at(0).is<std::uint64_t>());
    assert(nested.get<values>().at(0).get<std::uint64_t>() == 5);
    assert(nested.get<values>().at(1).is<mapbox::util::recursive_wrapper<prop_map>>());
    assert(nested.get<values>().at(1).get<prop_map>().at("foo").is<std::string>());
    assert(nested.get<values>().at(1).get<prop_map>().at("foo").get<std::string>() == "bar");

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

static void testFeatureNullProperties(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/feature-null-properties.json", use_convert);
    assert(data.is<feature>());

    const auto &f = data.get<feature>();
    assert(f.geometry.is<point>());
    assert(f.properties.size() == 0);

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

static void testFeatureCollection(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/feature-collection.json", use_convert);
    assert(data.is<feature_collection>());

    const auto &features = data.get<feature_collection>();
    assert(features.size() == 2);

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

static void testFeatureID(bool use_convert) {
    const auto &data = readGeoJSON("test/fixtures/feature-id.json", use_convert);
    assert(data.is<feature_collection>());

    const auto &features = data.get<feature_collection>();

    assert(features.at(0).id == identifier { uint64_t(1234) });
    assert(features.at(1).id == identifier { "abcd" });

    assert(parse(writeGeoJSON(data, use_convert)) == data);
}

void testAll(bool use_convert) {
    testPoint(use_convert);
    testMultiPoint(use_convert);
    testLineString(use_convert);
    testMultiLineString(use_convert);
    testPolygon(use_convert);
    testMultiPolygon(use_convert);
    testGeometryCollection(use_convert);
    testFeature(use_convert);
    testFeatureNullProperties(use_convert);
    testFeatureCollection(use_convert);
    testFeatureID(use_convert);
}

int main() {
    testAll(true);
    testAll(false);
    return 0;
}

