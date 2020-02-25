#include <mbgl/style/expression/within.hpp>

#include <mapbox/geojson.hpp>
#include <mapbox/geometry.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/util/string.hpp>

#include <rapidjson/document.h>

#include <utility>

namespace mbgl {
namespace {

class PolygonFeature : public GeometryTileFeature {
public:
    const Feature& feature;
    mutable optional<GeometryCollection> geometry;
    FeatureType type;

    PolygonFeature(const Feature& feature_, const CanonicalTileID& canonical) : feature(feature_) {
        type = apply_visitor(ToFeatureType(), feature.geometry);
        if (type == FeatureType::Polygon) {
            geometry = convertGeometry(feature.geometry, canonical);
            assert(geometry);
            geometry = fixupPolygons(*geometry);
        } else {
            mbgl::Log::Warning(mbgl::Event::General, "Provided feature does not contain polygon geometries.");
        }
    }

    FeatureType getType() const override { return type; }
    const PropertyMap& getProperties() const override { return feature.properties; }
    FeatureIdentifier getID() const override { return feature.id; }
    optional<mbgl::Value> getValue(const std::string& /*key*/) const override { return optional<mbgl::Value>(); }
    const GeometryCollection& getGeometries() const override {
        assert(geometry);
        return *geometry;
    }
};

bool pointsWithinPolygons(const mbgl::GeometryTileFeature& feature,
                          const mbgl::CanonicalTileID& canonical,
                          const Feature::geometry_type& polygonGeoSet,
                          const WithinBBox& polyBBox) {
    return polygonGeoSet.match(
        [&feature, &canonical, &polyBBox](const mapbox::geometry::multi_polygon<double>& polygons) -> bool {
            return convertGeometry(feature, canonical)
                .match(
                    [&polygons, &polyBBox](const mapbox::geometry::point<double>& point) -> bool {
                        return boxWithinBox(calculateBBox(point), polyBBox) && pointWithinPolygons(point, polygons);
                    },
                    [&polygons, &polyBBox](const mapbox::geometry::multi_point<double>& points) -> bool {
                        return boxWithinBox(calculateBBox(points), polyBBox) &&
                               std::all_of(points.begin(), points.end(), [&polygons](const auto& p) {
                                   return pointWithinPolygons(p, polygons);
                               });
                    },
                    [](const auto&) -> bool { return false; });
        },
        [&feature, &canonical, &polyBBox](const mapbox::geometry::polygon<double>& polygon) -> bool {
            return convertGeometry(feature, canonical)
                .match(
                    [&polygon, &polyBBox](const mapbox::geometry::point<double>& point) -> bool {
                        return boxWithinBox(calculateBBox(point), polyBBox) && pointWithinPolygon(point, polygon);
                    },
                    [&polygon, &polyBBox](const mapbox::geometry::multi_point<double>& points) -> bool {
                        return boxWithinBox(calculateBBox(points), polyBBox) &&
                               std::all_of(points.begin(), points.end(), [&polygon](const auto& p) {
                                   return pointWithinPolygon(p, polygon);
                               });
                    },
                    [](const auto&) -> bool { return false; });
        },
        [](const auto&) -> bool { return false; });
}

bool linesWithinPolygons(const mbgl::GeometryTileFeature& feature,
                         const mbgl::CanonicalTileID& canonical,
                         const Feature::geometry_type& polygonGeoSet,
                         const WithinBBox& polyBBox) {
    return polygonGeoSet.match(
        [&feature, &canonical, &polyBBox](const MultiPolygon<double>& polygons) -> bool {
            return convertGeometry(feature, canonical)
                .match(
                    [&polygons, &polyBBox](const LineString<double>& line) -> bool {
                        return boxWithinBox(calculateBBox(line), polyBBox) && lineStringWithinPolygons(line, polygons);
                    },
                    [&polygons, &polyBBox](const MultiLineString<double>& lines) -> bool {
                        return boxWithinBox(calculateBBox(lines), polyBBox) &&
                               std::all_of(lines.begin(), lines.end(), [&polygons](const auto& line) {
                                   return lineStringWithinPolygons(line, polygons);
                               });
                    },
                    [](const auto&) -> bool { return false; });
        },
        [&feature, &canonical, &polyBBox](const Polygon<double>& polygon) -> bool {
            return convertGeometry(feature, canonical)
                .match(
                    [&polygon, &polyBBox](const LineString<double>& line) -> bool {
                        return boxWithinBox(calculateBBox(line), polyBBox) && lineStringWithinPolygon(line, polygon);
                    },
                    [&polygon, &polyBBox](const MultiLineString<double>& lines) -> bool {
                        return boxWithinBox(calculateBBox(lines), polyBBox) &&
                               std::all_of(lines.begin(), lines.end(), [&polygon](const auto& line) {
                                   return lineStringWithinPolygon(line, polygon);
                               });
                    },
                    [](const auto&) -> bool { return false; });
        },
        [](const auto&) -> bool { return false; });
}

mbgl::optional<mbgl::GeoJSON> parseValue(const mbgl::style::conversion::Convertible& value_,
                                         mbgl::style::expression::ParsingContext& ctx) {
    if (isObject(value_)) {
        mbgl::style::conversion::Error error;
        auto geojson = toGeoJSON(value_, error);
        if (geojson && error.message.empty()) {
            return geojson;
        }
        ctx.error(error.message);
    }

    ctx.error("'within' expression requires valid geojson source that contains polygon geometry type.");
    return nullopt;
}

mbgl::optional<std::pair<Feature::geometry_type, WithinBBox>> generateResult(
    const PolygonFeature& polyFeature, mbgl::style::expression::ParsingContext& ctx) {
    if (polyFeature.getType() == FeatureType::Polygon) {
        auto refinedGeoSet = convertGeometry(polyFeature, CanonicalTileID(0, 0, 0));
        auto bbox = calculateBBox(refinedGeoSet);
        return std::make_pair(std::move(refinedGeoSet), bbox);
    }
    ctx.error("'within' expression requires valid geojson source that contains polygon geometry type.");
    return nullopt;
}
} // namespace

namespace style {
namespace expression {

Within::Within(GeoJSON geojson, Feature::geometry_type geometries_, WithinBBox polygonBBox_)
    : Expression(Kind::Within, type::Boolean),
      geoJSONSource(std::move(geojson)),
      geometries(std::move(geometries_)),
      polygonBBox(polygonBBox_) {}

Within::~Within() = default;

using namespace mbgl::style::conversion;

EvaluationResult Within::evaluate(const EvaluationContext& params) const {
    if (!params.feature || !params.canonical) {
        return false;
    }
    auto geometryType = params.feature->getType();
    // Currently only support Point/Points in Polygon/Polygons
    if (geometryType == FeatureType::Point) {
        return pointsWithinPolygons(*params.feature, *params.canonical, geometries, polygonBBox);
    } else if (geometryType == FeatureType::LineString) {
        return linesWithinPolygons(*params.feature, *params.canonical, geometries, polygonBBox);
    }
    mbgl::Log::Warning(mbgl::Event::General,
                       "within expression currently only support Point/LineString geometry type.");

    return false;
}

ParseResult Within::parse(const Convertible& value, ParsingContext& ctx) {
    if (isArray(value)) {
        // object value, quoted with ["within", value]
        if (arrayLength(value) != 2) {
            ctx.error("'within' expression requires exactly one argument, but found " +
                      util::toString(arrayLength(value) - 1) + " instead.");
            return ParseResult();
        }

        auto parsedValue = parseValue(arrayMember(value, 1), ctx);
        if (!parsedValue) {
            return ParseResult();
        }

        return parsedValue->match(
            [&parsedValue, &ctx](const mapbox::geometry::geometry<double>& geometrySet) {
                PolygonFeature polyFeature(mbgl::Feature(geometrySet), CanonicalTileID(0, 0, 0));
                if (auto ret = generateResult(polyFeature, ctx)) {
                    return ParseResult(std::make_unique<Within>(*parsedValue, std::move(ret->first), ret->second));
                }
                return ParseResult();
            },
            [&parsedValue, &ctx](const mapbox::feature::feature<double>& feature) {
                PolygonFeature polyFeature(mbgl::Feature(feature), CanonicalTileID(0, 0, 0));
                if (auto ret = generateResult(polyFeature, ctx)) {
                    return ParseResult(std::make_unique<Within>(*parsedValue, std::move(ret->first), ret->second));
                }
                return ParseResult();
            },
            [&parsedValue, &ctx](const mapbox::feature::feature_collection<double>& features) {
                for (const auto& feature : features) {
                    PolygonFeature polyFeature(mbgl::Feature(feature), CanonicalTileID(0, 0, 0));
                    if (auto ret = generateResult(polyFeature, ctx)) {
                        return ParseResult(std::make_unique<Within>(*parsedValue, std::move(ret->first), ret->second));
                    }
                }
                return ParseResult();
            },
            [&ctx](const auto&) {
                ctx.error("'within' expression requires valid geojson source that contains polygon geometry type.");
                return ParseResult();
            });
    }
    ctx.error("'within' expression needs to be an array with exactly one argument.");
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
        result.reserve(v.Size());
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

bool Within::operator==(const Expression& e) const {
    if (e.getKind() == Kind::Within) {
        auto rhs = static_cast<const Within*>(&e);
        return geoJSONSource == rhs->geoJSONSource;
    }
    return false;
}

std::vector<optional<Value>> Within::possibleOutputs() const {
    return {{true}, {false}};
}

std::string Within::getOperator() const {
    return "within";
}

} // namespace expression
} // namespace style
} // namespace mbgl
