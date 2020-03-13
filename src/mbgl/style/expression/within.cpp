#include <mbgl/style/expression/within.hpp>

#include <mapbox/geojson.hpp>
#include <mapbox/geometry.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/util/string.hpp>

#include <rapidjson/document.h>
#include <mbgl/math/clamp.hpp>

namespace mbgl {
namespace {

Point<int64_t> latLonToTileCoodinates(const Point<double>& c, const mbgl::CanonicalTileID& canonical) {
    Point<int64_t> p;

    const double size = util::EXTENT * std::pow(2, canonical.z);

    auto x = (c.x + 180.0) * size / 360.0;
    p.x = (util::clamp<int64_t>(x, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()));

    auto y = (180 - (std::log(std::tan((c.y + 90) * M_PI / 360.0)) * 180 / M_PI)) * size / 360;
    p.y = (util::clamp<int64_t>(y, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()));

    return p;
};

Polygon<int64_t> getTilePolygon(const Polygon<double>& polygon,
                                const mbgl::CanonicalTileID& canonical,
                                WithinBBox& bbox) {
    Polygon<int64_t> result;
    result.reserve(polygon.size());
    for (const auto& ring : polygon) {
        LinearRing<int64_t> temp;
        temp.reserve(ring.size());
        for (const auto p : ring) {
            const auto coord = latLonToTileCoodinates(p, canonical);
            temp.push_back(coord);
            updateBBox(bbox, coord);
        }
        result.push_back(std::move(temp));
    }
    return result;
}

MultiPolygon<int64_t> getTilePolygons(const MultiPolygon<double>& polygons,
                                      const mbgl::CanonicalTileID& canonical,
                                      WithinBBox& bbox) {
    MultiPolygon<int64_t> result;
    result.reserve(polygons.size());
    for (const auto& pg : polygons) {
        result.push_back(getTilePolygon(pg, canonical, bbox));
    }
    return result;
}

MultiPoint<int64_t> getTilePoints(const GeometryCoordinates& points,
                                  const mbgl::CanonicalTileID& canonical,
                                  WithinBBox& bbox) {
    const int64_t x0 = util::EXTENT * canonical.x;
    const int64_t y0 = util::EXTENT * canonical.y;
    MultiPoint<int64_t> results;
    results.reserve(points.size());
    for (const auto& p : points) {
        auto point = Point<int64_t>(p.x + x0, p.y + y0);
        updateBBox(bbox, point);
        results.push_back(point);
    }
    return results;
}

MultiLineString<int64_t> getTileLines(const GeometryCollection& lines,
                                      const mbgl::CanonicalTileID& canonical,
                                      WithinBBox& bbox) {
    const int64_t x0 = util::EXTENT * canonical.x;
    const int64_t y0 = util::EXTENT * canonical.y;
    MultiLineString<int64_t> results;
    results.reserve(bbox.size());
    for (const auto& line : lines) {
        LineString<int64_t> lineString;
        lineString.reserve(line.size());
        for (const auto& p : line) {
            auto point = Point<int64_t>(p.x + x0, p.y + y0);
            updateBBox(bbox, point);
            lineString.push_back(point);
        }
        results.push_back(std::move(lineString));
    }
    return results;
}

bool featureWithinPolygons(const GeometryTileFeature& feature,
                           const CanonicalTileID& canonical,
                           const Feature::geometry_type& polygonGeoSet) {
    return polygonGeoSet.match(
        [&feature, &canonical](const mapbox::geometry::multi_polygon<double>& polys) -> bool {
            WithinBBox polyBBox = DefaultBBox;
            auto polygons = getTilePolygons(polys, canonical, polyBBox);

            const GeometryCollection& geometries = feature.getGeometries();
            switch (feature.getType()) {
                case FeatureType::Point: {
                    WithinBBox pointBBox = DefaultBBox;
                    MultiPoint<int64_t> points = getTilePoints(geometries.at(0), canonical, pointBBox);
                    if (!boxWithinBox(pointBBox, polyBBox)) return false;

                    return std::all_of(points.begin(), points.end(), [&polygons](const auto& p) {
                        return pointWithinPolygons(p, polygons);
                    });
                }
                case FeatureType::LineString: {
                    WithinBBox lineBBox = DefaultBBox;
                    MultiLineString<int64_t> multiLineString = getTileLines(geometries, canonical, lineBBox);

                    if (!boxWithinBox(lineBBox, polyBBox)) return false;

                    return std::all_of(multiLineString.begin(), multiLineString.end(), [&polygons](const auto& line) {
                        return lineStringWithinPolygons(line, polygons);
                    });
                }
                default:
                    return false;
            };
        },
        [&feature, &canonical](const mapbox::geometry::polygon<double>& poly) -> bool {
            WithinBBox polyBBox = DefaultBBox;
            const auto polygon = getTilePolygon(poly, canonical, polyBBox);

            const GeometryCollection& geometries = feature.getGeometries();
            switch (feature.getType()) {
                case FeatureType::Point: {
                    WithinBBox pointBBox = DefaultBBox;
                    MultiPoint<int64_t> points = getTilePoints(geometries.at(0), canonical, pointBBox);
                    if (!boxWithinBox(pointBBox, polyBBox)) return false;

                    return std::all_of(points.begin(), points.end(), [&polygon](const auto& p) {
                        return pointWithinPolygon(p, polygon);
                    });
                }
                case FeatureType::LineString: {
                    WithinBBox lineBBox = DefaultBBox;
                    MultiLineString<int64_t> multiLineString = getTileLines(geometries, canonical, lineBBox);
                    if (!boxWithinBox(lineBBox, polyBBox)) return false;

                    return std::all_of(multiLineString.begin(), multiLineString.end(), [&polygon](const auto& line) {
                        return lineStringWithinPolygon(line, polygon);
                    });
                }
                default:
                    return false;
            };
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

mbgl::optional<Feature::geometry_type> getPolygonInfo(const Feature& polyFeature,
                                                      mbgl::style::expression::ParsingContext& ctx) {
    const auto type = apply_visitor(ToFeatureType(), polyFeature.geometry);
    if (type == FeatureType::Polygon) {
        return polyFeature.geometry;
    }
    ctx.error("'within' expression requires valid geojson source that contains polygon geometry type.");
    return nullopt;
}
} // namespace

namespace style {
namespace expression {

Within::Within(GeoJSON geojson, Feature::geometry_type geometries_)
    : Expression(Kind::Within, type::Boolean), geoJSONSource(std::move(geojson)), geometries(std::move(geometries_)) {}

Within::~Within() = default;

using namespace mbgl::style::conversion;

EvaluationResult Within::evaluate(const EvaluationContext& params) const {
    if (!params.feature || !params.canonical) {
        return false;
    }
    auto geometryType = params.feature->getType();
    // Currently only support Point and LineString types in Polygon/Polygons
    if (geometryType == FeatureType::Point || geometryType == FeatureType::LineString) {
        return featureWithinPolygons(*params.feature, *params.canonical, geometries);
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
                if (auto ret = getPolygonInfo(mbgl::Feature(geometrySet), ctx)) {
                    return ParseResult(std::make_unique<Within>(*parsedValue, std::move(*ret)));
                }
                return ParseResult();
            },
            [&parsedValue, &ctx](const mapbox::feature::feature<double>& feature) {
                if (auto ret = getPolygonInfo(mbgl::Feature(feature), ctx)) {
                    return ParseResult(std::make_unique<Within>(*parsedValue, std::move(*ret)));
                }
                return ParseResult();
            },
            [&parsedValue, &ctx](const mapbox::feature::feature_collection<double>& features) {
                for (const auto& feature : features) {
                    if (auto ret = getPolygonInfo(mbgl::Feature(feature), ctx)) {
                        return ParseResult(std::make_unique<Within>(*parsedValue, std::move(*ret)));
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
    if (v.IsObject()) {
        std::unordered_map<std::string, Value> result;
        for (const auto& m : v.GetObject()) {
            result.emplace(m.name.GetString(), valueConverter(m.value));
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
        return geoJSONSource == rhs->geoJSONSource && geometries == rhs->geometries;
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
