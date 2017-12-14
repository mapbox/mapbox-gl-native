#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/conversion.hpp>

#include <iterator>
#include <map>
#include <string>
#include <vector>
#include <memory>

namespace mbgl {
namespace style {
namespace expression {

class Expression;

struct ParsingError {
    std::string message;
    std::string key;
    bool operator==(const ParsingError& rhs) const { return message == rhs.message && key == rhs.key; }
};

using ParseResult = optional<std::unique_ptr<Expression>>;

namespace detail {

class Scope {
public:
    Scope(const std::map<std::string, std::shared_ptr<Expression>>& bindings_, std::shared_ptr<Scope> parent_ = nullptr) :
        bindings(bindings_),
        parent(std::move(parent_))
    {}

    const std::map<std::string, std::shared_ptr<Expression>>& bindings;
    std::shared_ptr<Scope> parent;
    
    optional<std::shared_ptr<Expression>> get(const std::string& name) {
        auto it = bindings.find(name);
        if (it != bindings.end()) {
            return {it->second};
        } else if (parent) {
            return parent->get(name);
        } else {
            return optional<std::shared_ptr<Expression>>();
        }
    }
};

} // namespace detail

class ParsingContext {
public:
    ParsingContext() : errors(std::make_shared<std::vector<ParsingError>>()) {}
    ParsingContext(std::string key_) : key(std::move(key_)), errors(std::make_shared<std::vector<ParsingError>>()) {}
    explicit ParsingContext(optional<type::Type> expected_)
        : expected(std::move(expected_)),
          errors(std::make_shared<std::vector<ParsingError>>())
    {}
    ParsingContext(ParsingContext&&) = default;
    
    ParsingContext(const ParsingContext&) = delete;
    ParsingContext& operator=(const ParsingContext&) = delete;

    std::string getKey() const { return key; }
    optional<type::Type> getExpected() const { return expected; }
    const std::vector<ParsingError>& getErrors() const { return *errors; }

    /*
        Parse the given style-spec JSON value into an Expression object.
        Specifically, this function is responsible for determining the expression
        type (either Literal, or the one named in value[0]) and dispatching to the
        appropriate ParseXxxx::parse(const V&, ParsingContext) method.
    */
    ParseResult parse(const mbgl::style::conversion::Convertible& value);
    
    /*
        Parse a child expression.
    */
    ParseResult parse(const mbgl::style::conversion::Convertible&,
                      std::size_t,
                      optional<type::Type> = {});
    
    /*
        Parse a child expression.
    */
    ParseResult parse(const mbgl::style::conversion::Convertible&,
                      std::size_t index,
                      optional<type::Type>,
                      const std::map<std::string, std::shared_ptr<Expression>>&);

    /*
        Check whether `t` is a subtype of `expected`, collecting an error if not.
     */
    optional<std::string> checkType(const type::Type& t);
    
    optional<std::shared_ptr<Expression>> getBinding(const std::string name) {
        if (!scope) return optional<std::shared_ptr<Expression>>();
        return scope->get(name);
    }

    void error(std::string message) {
        errors->push_back({message, key});
    }
    
    void error(std::string message, std::size_t child) {
        errors->push_back({message, key + "[" + util::toString(child) + "]"});
    }
    
    void error(std::string message, std::size_t child, std::size_t grandchild) {
        errors->push_back({message, key + "[" + util::toString(child) + "][" + util::toString(grandchild) + "]"});
    }
    
    void appendErrors(ParsingContext&& ctx) {
        errors->reserve(errors->size() + ctx.errors->size());
        std::move(ctx.errors->begin(), ctx.errors->end(), std::inserter(*errors, errors->end()));
        ctx.errors->clear();
    }
    
    void clearErrors() {
        errors->clear();
    }
    
private:
    ParsingContext(std::string key_,
                   std::shared_ptr<std::vector<ParsingError>> errors_,
                   optional<type::Type> expected_,
                   std::shared_ptr<detail::Scope> scope_)
        : key(std::move(key_)),
          expected(std::move(expected_)),
          scope(std::move(scope_)),
        errors(std::move(errors_))
    {}
    
    std::string key;
    optional<type::Type> expected;
    std::shared_ptr<detail::Scope> scope;
    std::shared_ptr<std::vector<ParsingError>> errors;
};

using ParseFunction = ParseResult (*)(const conversion::Convertible&, ParsingContext&);
using ExpressionRegistry = std::unordered_map<std::string, ParseFunction>;
const ExpressionRegistry& getExpressionRegistry();

} // namespace expression
} // namespace style
} // namespace mbgl
