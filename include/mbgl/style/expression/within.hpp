#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/util/geojson.hpp>

#include <memory>

namespace mbgl {
namespace style {
namespace expression {

class Within final : public Expression {
public:
    explicit Within(GeoJSON geojson);

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
};

} // namespace expression
} // namespace style
} // namespace mbgl
