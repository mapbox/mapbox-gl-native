#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/expression/value.hpp>

#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>

#include <memory>
#include <vector>

namespace mbgl {
namespace style {
namespace expression {

/*
    CompoundExpression provides a mechanism for implementing an expression
    simply by providing a list of pure functions of the form
    (const T0& arg0, const T1& arg1, ...) -> Result<U> where T0, T1, ..., U are
    member types of mbgl::style::expression::Value.
 
    The majority of expressions specified in the style-spec are implemented in
    this fashion (see compound_expression.cpp).
*/


/*
    Represents the parameter list for an expression that takes an arbitrary
    number of arguments (of a specific type).
*/
struct VarargsType { type::Type type; };
template <typename T>
struct Varargs : std::vector<T> { using std::vector<T>::vector; };

namespace detail {
// Base class for the Signature<Fn> structs that are used to determine the
// each CompoundExpression definition's type::Type data from the type of its
// "evaluate" function.
struct SignatureBase {
    SignatureBase(type::Type result_, variant<std::vector<type::Type>, VarargsType> params_, std::string name_) :
        result(std::move(result_)),
        params(std::move(params_)),
        name(std::move(name_))
    {}
    virtual ~SignatureBase() = default;
    virtual std::unique_ptr<Expression> makeExpression(std::vector<std::unique_ptr<Expression>>) const = 0;
    type::Type result;
    variant<std::vector<type::Type>, VarargsType> params;
    std::string name;
};
} // namespace detail


/*
    Common base class for CompoundExpression<Signature> instances.  Used to
    allow downcasting (and access to things like name & parameter list) during
    an Expression tree traversal.
*/
class CompoundExpressionBase : public Expression {
public:
    CompoundExpressionBase(std::string name_, const detail::SignatureBase& signature) :
        Expression(signature.result),
        name(std::move(name_)),
        params(signature.params)
    {}

    std::string getName() const { return name; }
    optional<std::size_t> getParameterCount() const {
        return params.match(
            [&](const VarargsType&) { return optional<std::size_t>(); },
            [&](const std::vector<type::Type>& p) -> optional<std::size_t> { return p.size(); }
        );
    }

    std::vector<optional<Value>> possibleOutputs() const override {
        return { nullopt };
    }

private:
    std::string name;
    variant<std::vector<type::Type>, VarargsType> params;
};

template <typename Signature>
class CompoundExpression : public CompoundExpressionBase {
public:
    using Args = typename Signature::Args;
    
    CompoundExpression(const std::string& name_,
                       Signature signature_,
                       typename Signature::Args args_) :
        CompoundExpressionBase(name_, signature_),
        signature(signature_),
        args(std::move(args_))
    {}
    
    EvaluationResult evaluate(const EvaluationContext& evaluationParams) const override {
        return signature.apply(evaluationParams, args);
    }
    
    void eachChild(const std::function<void(const Expression&)>& visit) const override {
        for (const std::unique_ptr<Expression>& e : args) {
            visit(*e);
        }
    }

    bool operator==(const Expression& e) const override {
        if (auto rhs = dynamic_cast<const CompoundExpression*>(&e)) {
            return getName() == rhs->getName() && Expression::childrenEqual(args, rhs->args);
        }
        return false;
    }
    
    std::string getOperator() const override {
        return signature.name;
    }

private:
    Signature signature;
    typename Signature::Args args;
};

/*
    Holds the map of expression name => implementation (which is just one or
    more evaluation functions, each wrapped in a Signature struct).
*/
struct CompoundExpressionRegistry {
    using Definition = std::vector<std::unique_ptr<detail::SignatureBase>>;
    static std::unordered_map<std::string, Definition> definitions;
};

ParseResult parseCompoundExpression(const std::string name, const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

ParseResult createCompoundExpression(const CompoundExpressionRegistry::Definition& definition,
                                     std::vector<std::unique_ptr<Expression>> args,
                                     ParsingContext& ctx);
    
ParseResult createCompoundExpression(const std::string& name,
                                     std::vector<std::unique_ptr<Expression>> args,
                                     ParsingContext& ctx);

} // namespace expression
} // namespace style
} // namespace mbgl
