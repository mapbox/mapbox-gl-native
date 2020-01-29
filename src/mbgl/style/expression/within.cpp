#include <mapbox/geojson.hpp>
#include <mapbox/geometry.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/within.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/string.hpp>

namespace {

double isLeft(mbgl::Point<double> P0, mbgl::Point<double> P1, mbgl::Point<double> P2) {
    return ((P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y));
}

// winding number algorithm for checking if point inside a ploygon or not.
// http://geomalgorithms.com/a03-_inclusion.html#wn_PnPoly()
bool pointWithinPoly(mbgl::Point<double> point, const mapbox::geometry::polygon<double>& polys) {
    //  wn = the winding number (=0 only when point is outside)
    int wn = 0;
    for (auto poly : polys) {
        auto size = poly.size();
        auto i = size;
        // loop through every edge of the polygon
        for (i = 0; i < size - 1; ++i) {
            if (poly[i].y <= point.y) {
                if (poly[i + 1].y > point.y) { // upward horizontal crossing from point to edge E(poly[i], poly[i+1])
                    if (isLeft(poly[i], poly[i + 1], point) > 0) {
                        ++wn;
                    }
                }
            } else {
                if (poly[i + 1].y <= point.y) { // downward crossing
                    if (isLeft(poly[i], poly[i + 1], point) < 0) {
                        --wn;
                    }
                }
            }
        }
        if (wn != 0) {
            return true;
        }
    }
    return wn != 0;
}

bool pointsWithinPoly(const mbgl::GeometryTileFeature& feature,
                      const mbgl::CanonicalTileID& canonical,
                      const mbgl::GeoJSON& geoJson) {
    bool result = false;
    geoJson.match(
        [&](const mapbox::geometry::geometry<double>& geometrySet) {
            geometrySet.match(
                [&](const mapbox::geometry::polygon<double>& polygons) {
                    convertGeometry(feature, canonical)
                        .match(
                            [&](const mapbox::geometry::point<double>& point) {
                                result = pointWithinPoly(point, polygons);
                            },
                            [&](const mapbox::geometry::multi_point<double>& points) {
                                for (const auto& p : points) {
                                    result = pointWithinPoly(p, polygons);
                                    if (!result) {
                                        return;
                                    }
                                }
                            },
                            [&](const auto&) { return; });
                },
                [&](const auto&) { return; });
        },
        [&](const auto&) { return; });

    return result;
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
    return mbgl::optional<mbgl::GeoJSON>();
}

} // namespace

namespace mbgl {
namespace style {
namespace expression {

using namespace mbgl::style::conversion;

EvaluationResult Within::evaluate(const EvaluationContext& params) const {
    if (!params.feature || !params.canonical) {
        return false;
    }
    auto geometryType = params.feature->getType();
    // Currently only support Point/Points in polygon
    if (geometryType == FeatureType::Point) {
        return pointsWithinPoly(*params.feature, *params.canonical, geoJSONSource);
    }
    return false;
}

ParseResult Within::parse(const Convertible& value, ParsingContext& ctx) {
    if (isArray(value)) {
        // object value, quoted with ["Within", value]
        if (arrayLength(value) != 2) {
            ctx.error("'Within' expression requires exactly one argument, but found " +
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

mbgl::Value Within::serialize() const {
    return std::vector<mbgl::Value>{{getOperator()}, {mapbox::geojson::stringify(geoJSONSource)}};
}

} // namespace expression
} // namespace style
} // namespace mbgl
