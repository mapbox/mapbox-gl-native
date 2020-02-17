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

void printPolygon(const Polygon<double>& polygon) {
    mbgl::Log::Debug(mbgl::Event::General, "-------Print Polygon------");
    for (auto ring : polygon) {
        auto length = ring.size();
        mbgl::Log::Debug(mbgl::Event::General, "-------Print New ring with size: %d ------", length);
        // loop through every edge of the ring
        for (std::size_t i = 0; i < length - 1; ++i) {
            mbgl::Log::Debug(mbgl::Event::General, "point [%.15f, %.15f],", ring[i].x, ring[i].y);
        }
    }
}

void printLine(const LineString<double>& lineString) {
    mbgl::Log::Debug(mbgl::Event::General, "-------Print LineString------");
    for (std::size_t i = 0; i < lineString.size(); ++i) {
        mbgl::Log::Debug(mbgl::Event::General, "point [%.15f, %.15f],", lineString[i].x, lineString[i].y);
    }
}

bool rayIntersect(const mbgl::Point<double>& p, const mbgl::Point<double>& p1, const mbgl::Point<double>& p2) {
    return ((p1.y > p.y) != (p2.y > p.y)) && (p.x < (p2.x - p1.x) * (p.y - p1.y) / (p2.y - p1.y) + p1.x);
}

// ray casting algorithm for detecting if point is in polygon
bool pointWithinPolygon(const mbgl::Point<double>& point, const mapbox::geometry::polygon<double>& polygon) {
    bool within = false;
    for (auto ring : polygon) {
        auto length = ring.size();
        // loop through every edge of the ring
        for (std::size_t i = 0; i < length - 1; ++i) {
            if (rayIntersect(point, ring[i], ring[i + 1])) {
                within = !within;
            }
        }
    }
    return within;
}

bool pointWithinPolygons(const mbgl::Point<double>& point, const mapbox::geometry::multi_polygon<double>& polygons) {
    for (auto polygon : polygons) {
        if (pointWithinPolygon(point, polygon)) return true;
    }
    return false;
}

bool lineIntersectLine(const mbgl::Point<double>& p1,
                       const mbgl::Point<double>& p2,
                       const mbgl::Point<double>& q1,
                       const mbgl::Point<double>& q2) {
    // precondition is p1, p2 is inside polygon, so p1, p2 is not on line q1->q2
    const auto perp = [](const mbgl::Point<double>& v1, const mbgl::Point<double>& v2) {
        return (v1.x * v2.y - v1.y * v2.x);
    };

    // check if p1 and p2 are in different sides of line segment q1->q2
    const auto twoSided = [](const mbgl::Point<double>& p1,
                             const mbgl::Point<double>& p2,
                             const mbgl::Point<double>& q1,
                             const mbgl::Point<double>& q2) {
        double x1, y1, x2, y2, x3, y3;

        // q1->p1 (x1, y1), q1->p2 (x2, y2), q1->q2 (x3, y3)
        x1 = p1.x - q1.x;
        y1 = p1.y - q1.y;
        x2 = p2.x - q1.x;
        y2 = p2.y - q1.y;
        x3 = q2.x - q1.x;
        y3 = q2.y - q1.y;
        if ((x1 * y3 - x3 * y1) * (x2 * y3 - x3 * y2) < 0) return true;
        return false;
    };
    auto vectorP = mbgl::Point<double>(p2.x - p1.x, p2.y - p1.y);
    auto vectorQ = mbgl::Point<double>(q2.x - q1.x, q2.y - q1.y);

    // check if two segments are parallel or not
    if (perp(vectorQ, vectorP) == 0) return false;

    // check if there are intersecting with each other
    // p1 and p2 lie in separate side of segment q1->q2
    // q1 and q2 lie in separate side of segment p1->p2
    if (twoSided(p1, p2, q1, q2) && twoSided(q1, q2, p1, p2)) return true;
    return false;
}

bool lineIntersectPolygon(const mbgl::Point<double>& p1,
                          const mbgl::Point<double>& p2,
                          const mapbox::geometry::polygon<double>& polygon) {
    for (auto ring : polygon) {
        auto length = ring.size();
        // loop through every edge of the ring
        for (std::size_t i = 0; i < length - 1; ++i) {
            if (lineIntersectLine(p1, p2, ring[i], ring[i + 1])) {
                //               mbgl::Log::Debug(mbgl::Event::General,
                //                                       "intersecting with edge from %.15f to %.15f", i, i+ 1);
                return true;
            }
        }
    }
    return false;
}

bool lineStringWithinPolygon(const mbgl::LineString<double>& lineString,
                             const mapbox::geometry::polygon<double>& polygon) {
    // First, check if endpoints of line are all inside polygon
    for (std::size_t i = 0; i < lineString.size() - 1; ++i) {
        if (!pointWithinPolygon(lineString[i], polygon)) {
            mbgl::Log::Debug(
                mbgl::Event::General, "point %.15f, %.15f is not inside polygon", lineString[i].x, lineString[i].y);
            return false;
        }
    }

    // Second, check if there was line segment intersecting polygon edge
    for (std::size_t i = 0; i < lineString.size() - 1; ++i) {
        if (lineIntersectPolygon(lineString[i], lineString[i + 1], polygon)) {
            mbgl::Log::Debug(mbgl::Event::General,
                             "---------line [%.15f, %.15f] to [%.15f, %.15f] is not inside polygon",
                             lineString[i].x,
                             lineString[i].y,
                             lineString[i + 1].x,
                             lineString[i + 1].y);
            return false;
        }
    }

    mbgl::Log::Debug(mbgl::Event::General, "---------line is inside polygon, line point size: %d: ", lineString.size());
    return true;
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
                                return std::all_of(points.begin(), points.end(), [&polygons](const auto& p) {
                                    return pointWithinPolygons(p, polygons);
                                });
                            },
                            [](const auto&) -> bool {
                                return false;
                            });
                },
                [&feature, &canonical](const mapbox::geometry::polygon<double>& polygon) -> bool {
                    return convertGeometry(feature, canonical)
                        .match(
                            [&polygon](const mapbox::geometry::point<double>& point) -> bool {
                                return pointWithinPolygon(point, polygon);
                            },
                            [&polygon](const mapbox::geometry::multi_point<double>& points) -> bool {
                                return std::all_of(points.begin(), points.end(), [&polygon](const auto& p) {
                                    return pointWithinPolygon(p, polygon);
                                });
                            },
                            [](const auto&) -> bool { return false; });
                },
                [](const auto&) -> bool { return false; });
        },
        [](const auto&) -> bool { return false; });
}

bool lineStringWithinPolygons(const mbgl::LineString<double>& line, const MultiPolygon<double>& polygons) {
    for (auto polygon : polygons) {
        if (lineStringWithinPolygon(line, polygon)) return true;
    }
    return false;
}

bool linesWithinPolygons(const mbgl::GeometryTileFeature& feature,
                         const mbgl::CanonicalTileID& canonical,
                         const mbgl::GeoJSON& geoJson) {
    mbgl::Log::Debug(mbgl::Event::General, "------------------------Canonical ID is-------------------------------");
    mbgl::Log::Debug(
        mbgl::Event::General, "----- '%d' / '%d' / '%d'-------------", canonical.z, canonical.x, canonical.y);

    return geoJson.match(
        [&feature, &canonical](const mapbox::geometry::geometry<double>& geometrySet) -> bool {
            mbgl::Feature f(geometrySet);
            PolygonFeature polyFeature(f, CanonicalTileID(0, 0, 0));
            auto refinedGeoSet = convertGeometry(polyFeature, CanonicalTileID(0, 0, 0));
            return refinedGeoSet.match(
                [&feature, &canonical](const MultiPolygon<double>& polygons) -> bool {
                    for (auto polygon : polygons) {
                        printPolygon(polygon);
                    }
                    return convertGeometry(feature, canonical)
                        .match(
                            [&polygons](const LineString<double>& line) -> bool {
                                printLine(line);
                                return lineStringWithinPolygons(line, polygons);
                            },
                            [&polygons](const MultiLineString<double>& lines) -> bool {
                                return std::all_of(lines.begin(), lines.end(), [&polygons](const auto& line) {
                                    printLine(line);
                                    return lineStringWithinPolygons(line, polygons);
                                });
                            },
                            [](const auto&) -> bool {
                                return false;
                            });
                },
                [&feature, &canonical](const Polygon<double>& polygon) -> bool {
                    printPolygon(polygon);
                    return convertGeometry(feature, canonical)
                        .match(
                            [&polygon](const LineString<double>& line) -> bool {
                                printLine(line);
                                return lineStringWithinPolygon(line, polygon);
                            },
                            [&polygon](const MultiLineString<double>& lines) -> bool {
                                return std::all_of(lines.begin(), lines.end(), [&polygon](const auto& line) {
                                    printLine(line);
                                    return lineStringWithinPolygon(line, polygon);
                                });
                            },
                            [](const auto&) -> bool {
                                return false;
                            });
                },
                [](const auto&) -> bool {
                    return false;
                });
        },
        [](const auto&) -> bool {
            return false;
        });
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

Within::~Within() = default;

using namespace mbgl::style::conversion;

EvaluationResult Within::evaluate(const EvaluationContext& params) const {
    if (!params.feature || !params.canonical) {
        return false;
    }
    auto geometryType = params.feature->getType();
    // Currently only support Point/Points in Polygon/Polygons
    if (geometryType == FeatureType::Point) {
        pointsWithinPolygons(*params.feature, *params.canonical, geoJSONSource);
        return true;
    } else if (geometryType == FeatureType::LineString) {
        return linesWithinPolygons(*params.feature, *params.canonical, geoJSONSource);
    }
    mbgl::Log::Warning(mbgl::Event::General, "within expression currently only support 'Point' geometry type");

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
