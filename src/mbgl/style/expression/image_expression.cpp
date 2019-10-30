#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/image.hpp>
#include <mbgl/style/expression/image_expression.hpp>

namespace mbgl {
namespace style {
namespace expression {

ImageExpression::ImageExpression(std::unique_ptr<Expression> imageID_)
    : Expression(Kind::ImageExpression, type::Image), imageID(std::move(imageID_)) {
    assert(imageID);
}

using namespace mbgl::style::conversion;

ParseResult ImageExpression::parse(const Convertible& value, ParsingContext& ctx) {
    std::size_t argsLength = arrayLength(value);
    if (argsLength < 2) {
        ctx.error("Invalid number of arguments for 'image' expression.");
        return ParseResult();
    }

    // Parse expression that evaluates to image id.
    auto imageIDArg = arrayMember(value, 1);
    ParseResult imageIDExpr = ctx.parse(imageIDArg, 1, {type::String});
    if (!imageIDExpr) {
        return ParseResult();
    }

    return ParseResult(std::make_unique<ImageExpression>(std::move(*imageIDExpr)));
}

void ImageExpression::eachChild(const std::function<void(const Expression&)>& fn) const {
    fn(*imageID);
}

bool ImageExpression::operator==(const Expression& e) const {
    if (e.getKind() == Kind::ImageExpression) {
        auto rhs = static_cast<const ImageExpression*>(&e);
        return *imageID == *rhs->imageID;
    }
    return false;
}

mbgl::Value ImageExpression::serialize() const {
    std::vector<mbgl::Value> serialized{{getOperator()}};
    serialized.push_back(imageID->serialize());
    return serialized;
}

EvaluationResult ImageExpression::evaluate(const EvaluationContext& ctx) const {
    auto imageIDResult = imageID->evaluate(ctx);
    if (!imageIDResult) {
        return imageIDResult.error();
    }

    optional<std::string> evaluatedImageID = toString(*imageIDResult);
    if (!evaluatedImageID) {
        return EvaluationError({"Could not evaluate ID for 'image' expression."});
    }

    bool available = ctx.availableImages && ctx.availableImages->count(*evaluatedImageID);
    return Image(*evaluatedImageID, available);
}

} // namespace expression
} // namespace style
} // namespace mbgl
