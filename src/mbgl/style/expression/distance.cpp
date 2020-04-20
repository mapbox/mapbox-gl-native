#include <mbgl/style/expression/distance.hpp>

#include <mapbox/cheap_ruler.hpp>
#include <mapbox/geojson.hpp>
#include <mapbox/geometry.hpp>

#include <mbgl/style/conversion/json.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/geometry_util.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/string.hpp>

#include <rapidjson/document.h>

#include <algorithm>
#include <deque>
#include <limits>
#include <queue>
#include <tuple>

namespace mbgl {
namespace {

const std::size_t MinPointsSize = 100;
const std::size_t MinLinePointsSize = 50;

// Inclusive index range for multipoint or linestring container
using IndexRange = std::pair<std::size_t, std::size_t>;

std::size_t getRangeSize(const IndexRange& range) {
    return range.second - range.first + 1;
}

using DistanceBBox = GeometryBBox<double>;

DistanceBBox getBBox(const mapbox::geometry::multi_point<double>& points, const IndexRange& range) {
    assert(range.second >= range.first && range.second < points.size());
    DistanceBBox bbox = DefaultDistanceBBox;
    for (std::size_t i = range.first; i <= range.second; ++i) {
        updateBBox(bbox, points[i]);
    }
    return bbox;
}

DistanceBBox getBBox(const mapbox::geometry::line_string<double>& line, const IndexRange& range) {
    assert(range.second >= range.first && range.second < line.size());
    DistanceBBox bbox = DefaultDistanceBBox;
    for (std::size_t i = range.first; i <= range.second; ++i) {
        updateBBox(bbox, line[i]);
    }
    return bbox;
}

// Calculate the distance between two bounding boxes.
// Calculate the delta in x and y direction, and use two fake points {0.0, 0.0} and {dx, dy} to calculate the distance.
// Distance will be 0.0 if bounding box are overlapping.
double bboxToBBoxDistance(const DistanceBBox& bbox1,
                          const DistanceBBox& bbox2,
                          mapbox::cheap_ruler::CheapRuler& ruler) {
    double dx = 0.0;
    double dy = 0.0;
    // bbox1 in left side
    if (bbox1[2] < bbox2[0]) {
        dx = bbox2[0] - bbox1[2];
    }
    // bbox1 in right side
    if (bbox1[0] > bbox2[2]) {
        dx = bbox1[0] - bbox2[2];
    }
    // bbox1 in above side
    if (bbox1[1] > bbox2[3]) {
        dy = bbox1[1] - bbox2[3];
    }
    // bbox1 in down side
    if (bbox1[3] < bbox2[1]) {
        dy = bbox2[1] - bbox1[3];
    }
    return ruler.distance(mapbox::geometry::point<double>{0.0, 0.0}, mapbox::geometry::point<double>{dx, dy});
}

double pointToLineDistance(const mapbox::geometry::point<double>& point,
                           const mapbox::geometry::line_string<double>& line,
                           mapbox::cheap_ruler::CheapRuler& ruler) {
    const auto nearestPoint = std::get<0>(ruler.pointOnLine(line, point));
    return ruler.distance(point, nearestPoint);
}

double lineToLineDistance(const mapbox::geometry::line_string<double>& line1,
                          IndexRange& range1,
                          const mapbox::geometry::line_string<double>& line2,
                          IndexRange& range2,
                          mapbox::cheap_ruler::CheapRuler& ruler) {
    assert(range1.second >= range1.first && range1.second < line1.size());
    assert(range2.second >= range2.first && range2.second < line2.size());
    double dist = std::numeric_limits<double>::infinity();
    for (std::size_t i = range1.first; i < range1.second; ++i) {
        const auto& p1 = line1[i];
        const auto& p2 = line1[i + 1];
        for (std::size_t j = range2.first; j < range2.second; ++j) {
            const auto& q1 = line2[j];
            const auto& q2 = line2[j + 1];
            if (segmentIntersectSegment(p1, p2, q1, q2)) return 0.0;
            auto dist1 = std::min(pointToLineDistance(p1, mapbox::geometry::line_string<double>{q1, q2}, ruler),
                                  pointToLineDistance(p2, mapbox::geometry::line_string<double>{q1, q2}, ruler));
            auto dist2 = std::min(pointToLineDistance(q1, mapbox::geometry::line_string<double>{p1, p2}, ruler),
                                  pointToLineDistance(q2, mapbox::geometry::line_string<double>{p1, p2}, ruler));
            dist = std::min(dist, std::min(dist1, dist2));
        }
    }
    return dist;
}

double pointsToPointsDistance(const mapbox::geometry::multi_point<double>& points1,
                              IndexRange& range1,
                              const mapbox::geometry::multi_point<double>& points2,
                              IndexRange& range2,
                              mapbox::cheap_ruler::CheapRuler& ruler) {
    assert(range1.second >= range1.first && range1.second < points1.size());
    assert(range2.second >= range2.first && range2.second < points2.size());
    double dist = std::numeric_limits<double>::infinity();
    for (std::size_t i = range1.first; i <= range1.second; ++i) {
        for (std::size_t j = range2.first; j <= range2.second; ++j) {
            dist = std::min(dist, ruler.distance(points1[i], points2[j]));
            if (dist == 0.0) return dist;
        }
    }
    return dist;
}

std::pair<mbgl::optional<IndexRange>, mbgl::optional<IndexRange>> splitRange(const IndexRange& range, bool isLine) {
    auto size = getRangeSize(range);
    if (isLine) {
        if (size == 2) {
            return std::make_pair(range, nullopt);
        }
        auto size1 = size / 2;
        IndexRange range1(range.first, range.first + size1);
        IndexRange range2(range.first + size1, range.second);
        return std::make_pair(std::move(range1), std::move(range2));
    } else {
        if (size == 1) {
            return std::make_pair(range, nullopt);
        }
        auto size1 = size / 2 - 1;
        IndexRange range1(range.first, range.first + size1);
        IndexRange range2(range.first + size1 + 1, range.second);
        return std::make_pair(std::move(range1), std::move(range2));
    }
}

// <distance, range1, range2>
using DistPair = std::tuple<double, IndexRange, IndexRange>;
struct Comparator {
    bool operator()(DistPair& left, DistPair& right) { return std::get<0>(left) < std::get<0>(right); }
};
// The priority queue will ensure the top element would always be the pair that has the biggest distance
using DistQueue = std::priority_queue<DistPair, std::deque<DistPair>, Comparator>;

// Divide and conqure, the time complexity is O(n*lgn), faster than Brute force O(n*n)
// Use index for in-place processing.
double lineToLineDistance(const mapbox::geometry::line_string<double>& line1,
                          const mapbox::geometry::multi_point<double>& line2,
                          mapbox::cheap_ruler::CheapRuler& ruler) {
    auto miniDist = ruler.distance(line1[0], line2[0]);
    DistQueue distQueue;
    distQueue.push(std::forward_as_tuple(0, IndexRange(0, line1.size() - 1), IndexRange(0, line2.size() - 1)));

    while (!distQueue.empty()) {
        auto distPair = distQueue.top();
        distQueue.pop();
        if (std::get<0>(distPair) > miniDist) continue;
        auto& rangeA = std::get<1>(distPair);
        auto& rangeB = std::get<2>(distPair);

        // In case the set size are relatively small, we could use brute-force directly
        if (getRangeSize(rangeA) <= MinLinePointsSize && getRangeSize(rangeB) <= MinLinePointsSize) {
            miniDist = std::min(miniDist, lineToLineDistance(line1, rangeA, line2, rangeB, ruler));
            if (miniDist == 0.0) return 0.0;
        } else {
            auto newRangesA = splitRange(rangeA, true /*isLine*/);
            auto newRangesB = splitRange(rangeB, true /*isLine*/);
            const auto updateQueue = [&distQueue, &miniDist, &ruler, &line1, &line2](
                                         mbgl::optional<IndexRange>& range1, mbgl::optional<IndexRange>& range2) {
                if (!range1 || !range2) return;
                auto tempDist = bboxToBBoxDistance(getBBox(line1, *range1), getBBox(line2, *range2), ruler);
                // Insert new pair to the queue if the bbox distance is less or equal to miniDist,
                // The pair with biggest distance will be at the top
                if (tempDist <= miniDist)
                    distQueue.push(std::make_tuple(tempDist, std::move(*range1), std::move(*range2)));
            };
            updateQueue(newRangesA.first, newRangesB.first);
            updateQueue(newRangesA.first, newRangesB.second);
            updateQueue(newRangesA.second, newRangesB.first);
            updateQueue(newRangesA.second, newRangesB.second);
        }
    }
    return miniDist;
}

// Divide and conqure, the time complexity is O(n*lgn), faster than Brute force O(n*n)
// Use index for in-place processing.
double pointsToPointsDistance(const mapbox::geometry::multi_point<double>& pointSet1,
                              const mapbox::geometry::multi_point<double>& pointSet2,
                              mapbox::cheap_ruler::CheapRuler& ruler) {
    auto miniDist = ruler.distance(pointSet1[0], pointSet2[0]);
    DistQueue distQueue;
    distQueue.push(std::forward_as_tuple(0, IndexRange(0, pointSet1.size() - 1), IndexRange(0, pointSet2.size() - 1)));

    while (!distQueue.empty()) {
        auto distPair = distQueue.top();
        distQueue.pop();
        if (std::get<0>(distPair) > miniDist) {
            continue;
        }
        auto& rangeA = std::get<1>(distPair);
        auto& rangeB = std::get<2>(distPair);

        // In case the set size are relatively small, we could use brute-force directly
        if (getRangeSize(rangeA) <= MinPointsSize && getRangeSize(rangeB) <= MinPointsSize) {
            miniDist = std::min(miniDist, pointsToPointsDistance(pointSet1, rangeA, pointSet2, rangeB, ruler));
            if (miniDist == 0.0) return 0.0;
        } else {
            auto newRangesA = splitRange(rangeA, false /*isLine*/);
            auto newRangesB = splitRange(rangeB, false /*isLine*/);
            const auto updateQueue = [&distQueue, &miniDist, &ruler, &pointSet1, &pointSet2](
                                         mbgl::optional<IndexRange>& range1, mbgl::optional<IndexRange>& range2) {
                if (!range1 || !range2) return;
                auto tempDist = bboxToBBoxDistance(getBBox(pointSet1, *range1), getBBox(pointSet2, *range2), ruler);
                // Insert new pair to the queue if the bbox distance is less or equal to miniDist,
                // The pair with biggest distance will be at the top
                if (tempDist <= miniDist)
                    distQueue.push(std::make_tuple(tempDist, std::move(*range1), std::move(*range2)));
            };
            updateQueue(newRangesA.first, newRangesB.first);
            updateQueue(newRangesA.first, newRangesB.second);
            updateQueue(newRangesA.second, newRangesB.first);
            updateQueue(newRangesA.second, newRangesB.second);
        }
    }
    return miniDist;
}

// Divide and conqure, the time complexity is O(n*lgn), faster than Brute force O(n*n)
// Most of the time, use index for in-place processing.
double pointsToLineDistance(const mapbox::geometry::multi_point<double>& points,
                            const mapbox::geometry::line_string<double>& line,
                            mapbox::cheap_ruler::CheapRuler& ruler) {
    auto miniDist = ruler.distance(points[0], line[0]);
    DistQueue distQueue;
    distQueue.push(std::forward_as_tuple(0, IndexRange(0, points.size() - 1), IndexRange(0, line.size() - 1)));

    while (!distQueue.empty()) {
        auto distPair = distQueue.top();
        distQueue.pop();
        if (std::get<0>(distPair) > miniDist) continue;
        auto& rangeA = std::get<1>(distPair);
        auto& rangeB = std::get<2>(distPair);

        // In case the set size are relatively small, we could use brute-force directly
        if (getRangeSize(rangeA) <= MinPointsSize && getRangeSize(rangeB) <= MinLinePointsSize) {
            assert(rangeA.second >= rangeA.first && rangeA.second < points.size());
            assert(rangeB.second >= rangeB.first && rangeB.second < line.size());
            auto subLine =
                mapbox::geometry::multi_point<double>(line.begin() + rangeB.first, line.begin() + rangeB.second + 1);
            for (std::size_t i = rangeA.first; i <= rangeA.second; ++i) {
                miniDist = std::min(miniDist, pointToLineDistance(points[i], subLine, ruler));
                if (miniDist == 0.0) return 0.0;
            }
        } else {
            auto newRangesA = splitRange(rangeA, false /*isLine*/);
            auto newRangesB = splitRange(rangeB, true /*isLine*/);
            const auto updateQueue = [&distQueue, &miniDist, &ruler, &points, &line](
                                         mbgl::optional<IndexRange>& range1, mbgl::optional<IndexRange>& range2) {
                if (!range1 || !range2) return;
                auto tempDist = bboxToBBoxDistance(getBBox(points, *range1), getBBox(line, *range2), ruler);
                // Insert new pair to the queue if the bbox distance is less or equal to miniDist,
                // The pair with biggest distance will be at the top
                if (tempDist <= miniDist)
                    distQueue.push(std::make_tuple(tempDist, std::move(*range1), std::move(*range2)));
            };
            updateQueue(newRangesA.first, newRangesB.first);
            updateQueue(newRangesA.first, newRangesB.second);
            updateQueue(newRangesA.second, newRangesB.first);
            updateQueue(newRangesA.second, newRangesB.second);
        }
    }
    return miniDist;
}

double pointsToLinesDistance(const mapbox::geometry::multi_point<double>& points,
                             const mapbox::geometry::multi_line_string<double>& lines,
                             mapbox::cheap_ruler::CheapRuler& ruler) {
    double dist = std::numeric_limits<double>::infinity();
    for (const auto& line : lines) {
        dist = std::min(dist, pointsToLineDistance(points, line, ruler));
        if (dist == 0.0) return 0.0;
    }
    return dist;
}

double lineToLinesDistance(const mapbox::geometry::line_string<double>& line,
                           const mapbox::geometry::multi_line_string<double>& lines,
                           mapbox::cheap_ruler::CheapRuler& ruler) {
    double dist = std::numeric_limits<double>::infinity();
    for (const auto& l : lines) {
        dist = std::min(dist, lineToLineDistance(line, l, ruler));
        if (dist == 0.0) return 0.0;
    }
    return dist;
}

double pointsToGeometryDistance(const mapbox::geometry::multi_point<double>& points,
                                const Feature::geometry_type& geoSet,
                                mapbox::cheap_ruler::CheapRuler::Unit unit) {
    mapbox::cheap_ruler::CheapRuler ruler(points.front().y, unit);
    return geoSet.match(
        [&points, &ruler](const mapbox::geometry::point<double>& p) {
            return pointsToPointsDistance(mapbox::geometry::multi_point<double>{p}, points, ruler);
        },
        [&points, &ruler](const mapbox::geometry::multi_point<double>& points1) {
            return pointsToPointsDistance(points, points1, ruler);
        },
        [&points, &ruler](const mapbox::geometry::line_string<double>& line) {
            return pointsToLineDistance(points, line, ruler);
        },
        [&points, &ruler](const mapbox::geometry::multi_line_string<double>& lines) {
            return pointsToLinesDistance(points, lines, ruler);
        },
        [](const auto&) { return std::numeric_limits<double>::quiet_NaN(); });
}

double lineToGeometryDistance(const mapbox::geometry::line_string<double>& line,
                              const Feature::geometry_type& geoSet,
                              mapbox::cheap_ruler::CheapRuler::Unit unit) {
    assert(!line.empty());
    mapbox::cheap_ruler::CheapRuler ruler(line.front().y, unit);
    return geoSet.match(
        [&line, &ruler](const mapbox::geometry::point<double>& p) {
            return pointsToLineDistance(mapbox::geometry::multi_point<double>{p}, line, ruler);
        },
        [&line, &ruler](const mapbox::geometry::multi_point<double>& points) {
            return pointsToLineDistance(points, line, ruler);
        },
        [&line, &ruler](const mapbox::geometry::line_string<double>& line1) {
            return lineToLineDistance(line, line1, ruler);
        },
        [&line, &ruler](const mapbox::geometry::multi_line_string<double>& lines) {
            return lineToLinesDistance(line, lines, ruler);
        },
        [](const auto&) { return std::numeric_limits<double>::quiet_NaN(); });
}

double calculateDistance(const GeometryTileFeature& feature,
                         const CanonicalTileID& canonical,
                         const Feature::geometry_type& geoSet,
                         mapbox::cheap_ruler::CheapRuler::Unit unit) {
    return convertGeometry(feature, canonical)
        .match(
            [&geoSet, &unit](const mapbox::geometry::point<double>& point) -> double {
                return pointsToGeometryDistance(mapbox::geometry::multi_point<double>{point}, geoSet, unit);
            },
            [&geoSet, &unit](const mapbox::geometry::multi_point<double>& points) -> double {
                return pointsToGeometryDistance(points, geoSet, unit);
            },
            [&geoSet, &unit](const mapbox::geometry::line_string<double>& line) -> double {
                return lineToGeometryDistance(line, geoSet, unit);
            },
            [&geoSet, &unit](const mapbox::geometry::multi_line_string<double>& lines) -> double {
                double dist = std::numeric_limits<double>::infinity();
                for (const auto& line : lines) {
                    dist = std::min(dist, lineToGeometryDistance(line, geoSet, unit));
                    if (dist == 0.0) return dist;
                }
                return dist;
            },
            [](const auto&) -> double { return std::numeric_limits<double>::quiet_NaN(); });
}

struct Arguments {
    Arguments(GeoJSON& geojson_, mapbox::cheap_ruler::CheapRuler::Unit unit_)
        : geojson(std::move(geojson_)), unit(unit_) {}

    GeoJSON geojson;
    mapbox::cheap_ruler::CheapRuler::Unit unit;
};

optional<Arguments> parseValue(const style::conversion::Convertible& value, style::expression::ParsingContext& ctx) {
    if (isArray(value)) {
        // object value, quoted with ["Distance", GeoJSONObj, "unit(optional)"]
        auto length = arrayLength(value);
        if (length != 2 && length != 3) {
            ctx.error("'distance' expression requires either one argument or two arguments, but found " +
                      util::toString(arrayLength(value) - 1) + " instead.");
            return nullopt;
        }

        // Parse Unit info for distance calculation, "Meters" by default
        mapbox::cheap_ruler::CheapRuler::Unit unit = mapbox::cheap_ruler::CheapRuler::Unit::Meters;
        if (length == 3) {
            auto input = toString(arrayMember(value, 2));
            if (input == nullopt) {
                ctx.error("Failed to parse unit argument from 'distance' expression");
                return nullopt;
            }
            if (*input == "meters" || *input == "metres") {
                unit = mapbox::cheap_ruler::CheapRuler::Unit::Meters;
            } else if (*input == "kilometers") {
                unit = mapbox::cheap_ruler::CheapRuler::Unit::Kilometers;
            } else if (*input == "miles") {
                unit = mapbox::cheap_ruler::CheapRuler::Unit::Miles;
            } else if (*input == "nauticalmiles") {
                unit = mapbox::cheap_ruler::CheapRuler::Unit::NauticalMiles;
            } else if (*input == "yards") {
                unit = mapbox::cheap_ruler::CheapRuler::Unit::Yards;
            } else if (*input == "feet") {
                unit = mapbox::cheap_ruler::CheapRuler::Unit::Feet;
            } else if (*input == "inches") {
                unit = mapbox::cheap_ruler::CheapRuler::Unit::Inches;
            } else {
                ctx.error(
                    "'distance' expression only accepts following units: kilometers, miles, nauticalmiles, "
                    "meters, metres, yards, feet, inches.");
                return nullopt;
            }
        }
        // Parse geometry info
        const auto& argument1 = arrayMember(value, 1);
        if (isObject(argument1)) {
            style::conversion::Error error;
            auto geojson = toGeoJSON(argument1, error);
            if (geojson && error.message.empty()) {
                return Arguments(*geojson, unit);
            }
            ctx.error(error.message);
        }
    }
    ctx.error(
        "'distance' expression needs to be an array with format [\"Distance\", GeoJSONObj, \"units\"(\"units\" is an "
        "optional argument, 'meters' will be used by default)].");
    return nullopt;
}

optional<Feature::geometry_type> getGeometry(const Feature& feature, mbgl::style::expression::ParsingContext& ctx) {
    const auto type = apply_visitor(ToFeatureType(), feature.geometry);
    if (type == FeatureType::Point || type == FeatureType::LineString) {
        return feature.geometry;
    }
    ctx.error("'distance' expression requires valid geojson object with valid geometry type: Point/LineString.");
    return nullopt;
}
} // namespace

namespace style {
namespace expression {

Distance::Distance(GeoJSON geojson, Feature::geometry_type geometries_, mapbox::cheap_ruler::CheapRuler::Unit unit_)
    : Expression(Kind::Distance, type::Number),
      geoJSONSource(std::move(geojson)),
      geometries(std::move(geometries_)),
      unit(unit_) {}

Distance::~Distance() = default;

using namespace mbgl::style::conversion;

EvaluationResult Distance::evaluate(const EvaluationContext& params) const {
    if (!params.feature || !params.canonical) {
        return EvaluationError{"distance expression requirs valid feature and canonical information."};
    }
    auto geometryType = params.feature->getType();
    if (geometryType == FeatureType::Point || geometryType == FeatureType::LineString) {
        auto distance = calculateDistance(*params.feature, *params.canonical, geometries, unit);
        if (!std::isnan(distance)) {
            assert(distance >= 0.0);
            return distance;
        }
    }
    return EvaluationError{"distance expression currently only evaluates Point/LineString geometries."};
}

ParseResult Distance::parse(const Convertible& value, ParsingContext& ctx) {
    auto parsedValue = parseValue(value, ctx);
    if (!parsedValue) {
        return ParseResult();
    }

    return parsedValue->geojson.match(
        [&parsedValue, &ctx](const mapbox::geometry::geometry<double>& geometrySet) {
            if (auto ret = getGeometry(mbgl::Feature(geometrySet), ctx)) {
                return ParseResult(
                    std::make_unique<Distance>(parsedValue->geojson, std::move(*ret), parsedValue->unit));
            }
            return ParseResult();
        },
        [&parsedValue, &ctx](const mapbox::feature::feature<double>& feature) {
            if (auto ret = getGeometry(mbgl::Feature(feature), ctx)) {
                return ParseResult(
                    std::make_unique<Distance>(parsedValue->geojson, std::move(*ret), parsedValue->unit));
            }
            return ParseResult();
        },
        [&parsedValue, &ctx](const mapbox::feature::feature_collection<double>& features) {
            for (const auto& feature : features) {
                if (auto ret = getGeometry(mbgl::Feature(feature), ctx)) {
                    return ParseResult(
                        std::make_unique<Distance>(parsedValue->geojson, std::move(*ret), parsedValue->unit));
                }
            }
            return ParseResult();
        },
        [&ctx](const auto&) {
            ctx.error("'distance' expression requires valid geojson that contains LineString/Point geometries.");
            return ParseResult();
        });

    return ParseResult();
}

Value convertValue(const mapbox::geojson::rapidjson_value& v) {
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
            result.push_back(convertValue(m));
        }
        return result;
    }
    if (v.IsObject()) {
        std::unordered_map<std::string, Value> result;
        for (const auto& m : v.GetObject()) {
            result.emplace(m.name.GetString(), convertValue(m.value));
        }
        return result;
    }
    // Ignore other types as valid geojson only contains above types.
    return Null;
}

std::string getUnits(const mapbox::cheap_ruler::CheapRuler::Unit& unit) {
    switch (unit) {
        case mapbox::cheap_ruler::CheapRuler::Kilometers:
            return "kilometers";
        case mapbox::cheap_ruler::CheapRuler::Miles:
            return "miles";
        case mapbox::cheap_ruler::CheapRuler::NauticalMiles:
            return "nauticalmiles";
        case mapbox::cheap_ruler::CheapRuler::Meters:
            return "meters";
        case mapbox::cheap_ruler::CheapRuler::Yards:
            return "yards";
        case mapbox::cheap_ruler::CheapRuler::Feet:
            return "feet";
        case mapbox::cheap_ruler::CheapRuler::Inches:
            return "inches";
        default:
            return "error";
    }
}

mbgl::Value Distance::serialize() const {
    std::unordered_map<std::string, Value> serialized;
    rapidjson::CrtAllocator allocator;
    const mapbox::geojson::rapidjson_value value = mapbox::geojson::convert(geoJSONSource, allocator);
    if (value.IsObject()) {
        for (const auto& m : value.GetObject()) {
            serialized.emplace(m.name.GetString(), convertValue(m.value));
        }
    } else {
        mbgl::Log::Error(mbgl::Event::General,
                         "Failed to serialize 'distance' expression, converted rapidJSON is not an object");
    }
    return std::vector<mbgl::Value>{{getOperator(), *fromExpressionValue<mbgl::Value>(serialized), getUnits(unit)}};
}

bool Distance::operator==(const Expression& e) const {
    if (e.getKind() == Kind::Distance) {
        auto rhs = static_cast<const Distance*>(&e);
        return geoJSONSource == rhs->geoJSONSource && geometries == rhs->geometries && unit == rhs->unit;
    }
    return false;
}

std::vector<optional<Value>> Distance::possibleOutputs() const {
    return {nullopt};
}

std::string Distance::getOperator() const {
    return "distance";
}

} // namespace expression
} // namespace style
} // namespace mbgl
