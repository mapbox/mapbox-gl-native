#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

#include <array>
#include <vector>
#include <memory>

namespace mbgl {

class GeometryTileFeature;

namespace style {
namespace expression {

class EvaluationError {
public:
    std::string message;
};

class EvaluationContext {
public:
    EvaluationContext(float zoom_) : zoom(zoom_), feature(nullptr) {}
    EvaluationContext(GeometryTileFeature const * feature_) : zoom(optional<float>()), feature(feature_) {}
    EvaluationContext(float zoom_, GeometryTileFeature const * feature_) :
        zoom(zoom_), feature(feature_)
    {}
    EvaluationContext(optional<float> zoom_, GeometryTileFeature const * feature_, optional<double> heatmapDensity_) :
        zoom(std::move(zoom_)), feature(feature_), heatmapDensity(std::move(heatmapDensity_))
    {}
    
    optional<float> zoom;
    GeometryTileFeature const * feature;
    optional<double> heatmapDensity;
};

template <typename T>
class Result : private variant<EvaluationError, T> {
public:
    using variant<EvaluationError, T>::variant;
    using Value = T;
    
    Result() = default;

    explicit operator bool () const {
        return this->template is<T>();
    }
    
    // optional does some type trait magic for this one, so this might
    // be problematic as is.
    const T* operator->() const {
        assert(this->template is<T>());
        return std::addressof(this->template get<T>());
    }
    
    T* operator->() {
        assert(this->template is<T>());
        return std::addressof(this->template get<T>());
    }
    
    T& operator*() {
        assert(this->template is<T>());
        return this->template get<T>();
    }
    
    const T& operator*() const {
        assert(this->template is<T>());
        return this->template get<T>();
    }
    
    const EvaluationError& error() const {
        assert(this->template is<EvaluationError>());
        return this->template get<EvaluationError>();
    }
};

class EvaluationResult : public Result<Value> {
public:
    using Result::Result; // NOLINT
    
    EvaluationResult() = default;

    EvaluationResult(const std::array<double, 4>& arr) :
        Result(toExpressionValue(arr))
    {}
    
    // used only for the special (private) "error" expression
    EvaluationResult(const type::ErrorType&) {
        assert(false);
    }
};

/*
    Expression is an abstract class that serves as an interface and base class
    for particular expression implementations.

    CompoundExpression implements the majority of expressions in the spec by
    inferring the argument and output from a simple function (const T0& arg0,
    const T1& arg1, ...) -> Result<U> where T0, T1, ..., U are member types of
    mbgl::style::expression::Value.
    
    The other Expression subclasses (Let, Curve, Match, etc.) exist in order to
    implement expressions that need specialized parsing, type checking, or
    evaluation logic that can't be handled by CompoundExpression's inference
    mechanism.
    
    Each Expression subclass also provides a static
    ParseResult ExpressionClass::parse(const V&, ParsingContext),
    which handles parsing a style-spec JSON representation of the expression.
*/

enum class Kind : int32_t {
    Coalesce,
    CompoundExpression,
    Literal,
    ArrayAssertion,
    At,
    Interpolate,
    Assertion,
    Length,
    Step,
    Let,
    Var,
    CollatorExpression,
    Coercion,
    Match,
    Error,
    Case,
    Any,
    All,
    Equals,
};

class Expression {
public:
    Expression(Kind kind_, type::Type type_) : kind(kind_), type(std::move(type_)) {}
    virtual ~Expression() = default;
    
    virtual EvaluationResult evaluate(const EvaluationContext& params) const = 0;
    virtual void eachChild(const std::function<void(const Expression&)>&) const = 0;
    virtual bool operator==(const Expression&) const = 0;
    bool operator!=(const Expression& rhs) const {
        return !operator==(rhs);
    }

    Kind getKind() const { return kind; };
    type::Type getType() const { return type; };
    
    EvaluationResult evaluate(optional<float> zoom, const Feature& feature, optional<double> heatmapDensity) const;

    /**
     * Statically analyze the expression, attempting to enumerate possible outputs. Returns
     * an array of values plus the sentinel null optional value, used to indicate that the
     * complete set of outputs is statically undecidable.
     */
    virtual std::vector<optional<Value>> possibleOutputs() const = 0;
    
    virtual mbgl::Value serialize() const {
        std::vector<mbgl::Value> serialized;
        serialized.emplace_back(getOperator());
        eachChild([&](const Expression &child) {
            serialized.emplace_back(child.serialize());
        });
        return serialized;
    };
    
    virtual std::string getOperator() const = 0;

protected:
    template <typename T>
    static bool childrenEqual(const T& lhs, const T& rhs) {
        if (lhs.size() != rhs.size()) return false;
        for (auto leftChild = lhs.begin(), rightChild = rhs.begin();
             leftChild != lhs.end();
             leftChild++, rightChild++)
         {
             if (!Expression::childEqual(*leftChild, *rightChild)) return false;
         }
         return true;
    }
    
    static bool childEqual(const std::unique_ptr<Expression>& lhs, const std::unique_ptr<Expression>& rhs) {
        return *lhs == *rhs;
    }
    
    template <typename T>
    static bool childEqual(const std::pair<T, std::unique_ptr<Expression>>& lhs,
                           const std::pair<T, std::unique_ptr<Expression>>& rhs) {
        return lhs.first == rhs.first && *(lhs.second) == *(rhs.second);
    }
    
    template <typename T>
    static bool childEqual(const std::pair<T, std::shared_ptr<Expression>>& lhs,
                           const std::pair<T, std::shared_ptr<Expression>>& rhs) {
        return lhs.first == rhs.first && *(lhs.second) == *(rhs.second);
    }
    
    static bool childEqual(const std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>& lhs,
                           const std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>& rhs) {
        return *(lhs.first) == *(rhs.first) && *(lhs.second) == *(rhs.second);
    }

private:
    Kind kind;
    type::Type type;
};

} // namespace expression
} // namespace style
} // namespace mbgl
