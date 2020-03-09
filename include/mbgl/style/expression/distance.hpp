#pragma once

#include <mapbox/cheap_ruler.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/util/geojson.hpp>

namespace mbgl {
namespace style {
namespace expression {

class Distance final : public Expression {
public:
    Distance(GeoJSON geoJSONSource_, Feature::geometry_type geometries_, mapbox::cheap_ruler::CheapRuler::Unit unit_);

    ~Distance() override;

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
    mapbox::cheap_ruler::CheapRuler::Unit unit;
};

} // namespace expression
} // namespace style
} // namespace mbgl
