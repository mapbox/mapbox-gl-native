#include <mbgl/style/expression/within.hpp>

#include <mapbox/geojson.hpp>
#include <mapbox/geometry.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/string.hpp>

#include <rapidjson/document.h>
#include <mbgl/style/conversion/json.hpp>

namespace mbgl {
namespace {

class PolygonFeature : public GeometryTileFeature {
public:
    const Feature& feature;
    mutable optional<GeometryCollection> geometry;

    PolygonFeature(const Feature& feature_, const CanonicalTileID& canonical) : feature(feature_) {
        geometry = convertGeometry(feature.geometry, canonical);
        assert(geometry);
        geometry = fixupPolygons(*geometry);
    }

    FeatureType getType() const override { return FeatureType::Polygon; }
    const PropertyMap& getProperties() const override { return feature.properties; }
    FeatureIdentifier getID() const override { return feature.id; }
    optional<mbgl::Value> getValue(const std::string& /*key*/) const override { return optional<mbgl::Value>(); }
    const GeometryCollection& getGeometries() const override {
        assert(geometry);
        return *geometry;
    }
};

bool rayIntersect(const mbgl::Point<double>& p, const mbgl::Point<double>& p1, const mbgl::Point<double>& p2) {
    return ((p1.y > p.y) != (p2.y > p.y)) && (p.x < (p2.x - p1.x) * (p.y - p1.y) / (p2.y - p1.y) + p1.x);
}

// ray casting algorithm for detecting if point is in polygon
bool pointWithinPolygon(const mbgl::Point<double>& point, const mapbox::geometry::polygon<double>& polygon) {
    bool within = false;
    for (auto ring : polygon) {
        auto size = ring.size();
        // loop through every edge of the ring
        for (decltype(size) i = 0; i < size - 1; ++i) {
            if (rayIntersect(point, ring[i], ring[i + 1])) {
                within = !within;
            }
        }
    }
    return within;
}

bool pointWithinPolygons(const mbgl::Point<double>& point, const mapbox::geometry::multi_polygon<double>& polygons) {
    for (auto polygon : polygons) {
        auto within = pointWithinPolygon(point, polygon);
        if (within) return true;
    }
    return false;
}

bool pointsWithinPolygons(const mbgl::GeometryTileFeature& feature,
                          const mbgl::CanonicalTileID& canonical,
                          const mbgl::GeoJSON& geoJson) {
    return geoJson.match(
        [&feature, &canonical](const mapbox::geometry::geometry<double>& geometrySet) -> bool {
            mbgl::Feature f(geometrySet);
            PolygonFeature polyFeature(f, canonical);
            auto refinedGeoSet = convertGeometry(polyFeature, canonical);
            return refinedGeoSet.match(
                [&feature, &canonical](const mapbox::geometry::multi_polygon<double>& polygons) -> bool {
                    return convertGeometry(feature, canonical)
                        .match(
                            [&polygons](const mapbox::geometry::point<double>& point) -> bool {
                                return pointWithinPolygons(point, polygons);
                            },
                            [&polygons](const mapbox::geometry::multi_point<double>& points) -> bool {
                                auto result = false;
                                for (const auto& p : points) {
                                    result = pointWithinPolygons(p, polygons);
                                    if (!result) {
                                        return result;
                                    }
                                }
                                return result;
                            },
                            [](const auto&) -> bool { return false; });
                },
                [&feature, &canonical](const mapbox::geometry::polygon<double>& polygon) -> bool {
                    return convertGeometry(feature, canonical)
                        .match(
                            [&polygon](const mapbox::geometry::point<double>& point) -> bool {
                                return pointWithinPolygon(point, polygon);
                            },
                            [&polygon](const mapbox::geometry::multi_point<double>& points) -> bool {
                                auto result = false;
                                for (const auto& p : points) {
                                    result = pointWithinPolygon(p, polygon);
                                    if (!result) {
                                        return result;
                                    }
                                }
                                return result;
                            },
                            [](const auto&) -> bool { return false; });
                },
                [](const auto&) -> bool { return false; });
        },
        [](const auto&) -> bool { return false; });
}

mbgl::optional<mbgl::GeoJSON> parseValue(const mbgl::style::conversion::Convertible& value_,
                                         mbgl::style::expression::ParsingContext& ctx) {
    if (isObject(value_)) {
        const auto geometryType = objectMember(value_, "type");
        if (geometryType) {
            const auto type = toString(*geometryType);
            if (type && *type == "Polygon") {
                mbgl::style::conversion::Error error;
                auto geojson = toGeoJSON(value_, error);
                if (geojson && error.message.empty()) {
                    return geojson;
                }
                ctx.error(error.message);
            }
        }
    }
    ctx.error("'Within' expression requires valid geojson source that contains polygon geometry type.");
    return nullopt;
}

} // namespace

namespace style {
namespace expression {

Within::Within(GeoJSON geojson) : Expression(Kind::Within, type::Boolean), geoJSONSource(std::move(geojson)) {}

Within::~Within() {}

using namespace mbgl::style::conversion;

EvaluationResult Within::evaluate(const EvaluationContext& params) const {
    if (!params.feature || !params.canonical) {
        return false;
    }
    auto geometryType = params.feature->getType();
    // Currently only support Point/Points in Polygon/Polygons
    if (geometryType == FeatureType::Point) {
        return pointsWithinPolygons(*params.feature, *params.canonical, geoJSONSource);
    } else {
        mbgl::Log::Warning(mbgl::Event::General, "within expression currently only support 'Point' geometry type");
    }
    return false;
}

ParseResult Within::parse(const Convertible& value, ParsingContext& ctx) {
    if (isArray(value)) {
        // object value, quoted with ["Within", value]
        if (arrayLength(value) != 2) {
            ctx.error("'within' expression requires exactly one argument, but found " +
                      util::toString(arrayLength(value) - 1) + " instead.");
            return ParseResult();
        }

        auto parsedValue = parseValue(arrayMember(value, 1), ctx);
        if (!parsedValue) {
            return ParseResult();
        }
        return ParseResult(std::make_unique<Within>(*parsedValue));
    }
    return ParseResult();
}

Value valueConverter(const mapbox::geojson::rapidjson_value& v) {
    if (v.IsDouble()) {
        return v.GetDouble();
    }
    if (v.IsString()) {
        return std::string(v.GetString());
    }
    if (v.IsArray()) {
        std::vector<Value> result;
        for (const auto& m : v.GetArray()) {
            result.push_back(valueConverter(m));
        }
        return result;
    }
    // Ignore other types as valid geojson only contains above types.
    return Null;
}

mbgl::Value Within::serialize() const {
    std::unordered_map<std::string, Value> serialized;
    rapidjson::CrtAllocator allocator;
    const mapbox::geojson::rapidjson_value value = mapbox::geojson::convert(geoJSONSource, allocator);
    if (value.IsObject()) {
        for (const auto& m : value.GetObject()) {
            serialized.emplace(m.name.GetString(), valueConverter(m.value));
        }
    } else {
        mbgl::Log::Error(mbgl::Event::General,
                         "Failed to serialize 'within' expression, converted rapidJSON is not an object");
    }
    return std::vector<mbgl::Value>{{getOperator(), *fromExpressionValue<mbgl::Value>(serialized)}};
}

} // namespace expression
} // namespace style
} // namespace mbgl
