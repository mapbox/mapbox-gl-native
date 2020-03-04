#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/util/geojson.hpp>
#include <mbgl/util/geometry_within.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace expression {

class Within final : public Expression {
public:
    explicit Within(GeoJSON geojson, Feature::geometry_type geometries_, const WithinBBox& polygonBBox_);

    ~Within() override;

    EvaluationResult evaluate(const EvaluationContext&) const override;

    static ParseResult parse(const mbgl::style::conversion::Convertible&, ParsingContext&);

    void eachChild(const std::function<void(const Expression&)>&) const override {}

    bool operator==(const Expression& e) const override;

    std::vector<optional<Value>> possibleOutputs() const override;

    mbgl::Value serialize() const override;
    std::string getOperator() const override;

private:
    GeoJSON geoJSONSource;
    Feature::geometry_type geometries;
    WithinBBox polygonBBox;
};

} // namespace expression
} // namespace style
} // namespace mbgl
