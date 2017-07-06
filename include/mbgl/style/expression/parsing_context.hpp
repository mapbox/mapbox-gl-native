#pragma once

#include <map>
#include <string>
#include <vector>
#include <mbgl/util/optional.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace expression {

class Expression;

struct ParsingError {
    std::string message;
    std::string key;
};

using ParseResult = optional<std::unique_ptr<Expression>>;

namespace detail {

struct Scope {
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
    ParsingContext(std::vector<ParsingError>& errors_, optional<type::Type> expected_ = {})
        : errors(errors_),
          expected(std::move(expected_))
    {}

    ParsingContext concat(std::size_t index_, optional<type::Type> expected_ = {}) {
        return ParsingContext(key + "[" + std::to_string(index_) + "]",
                              errors,
                              std::move(expected_),
                              scope);
    }
    
    ParsingContext concat(std::size_t index_,
                          optional<type::Type> expected_,
                          const std::map<std::string, std::shared_ptr<Expression>>& bindings) {
        return ParsingContext(key + "[" + std::to_string(index_) + "]",
                              errors,
                              std::move(expected_),
                              std::make_shared<detail::Scope>(bindings, scope));

    }
    
    /*
        Parse the given style-spec JSON value into an Expression object.
        Specifically, this function is responsible for determining the expression
        type (either Literal, or the one named in value[0]) and dispatching to the
        appropriate ParseXxxx::parse(const V&, ParsingContext) method.
    */
    ParseResult parse(const mbgl::style::conversion::Convertible& value);
    
    /*
        Check whether `t` is a subtype of `expected`, collecting an error if not.
     */
    optional<std::string> checkType(const type::Type& t);
    
    optional<std::shared_ptr<Expression>> getBinding(const std::string name) {
        if (!scope) return optional<std::shared_ptr<Expression>>();
        return scope->get(name);
    }

    void error(std::string message) {
        errors.push_back({message, key});
    }
    
    void  error(std::string message, std::size_t child) {
        errors.push_back({message, key + "[" + std::to_string(child) + "]"});
    }

    std::string key;
    std::vector<ParsingError>& errors;
    optional<type::Type> expected;
    std::shared_ptr<detail::Scope> scope;

private:
    ParsingContext(std::string key_,
                   std::vector<ParsingError>& errors_,
                   optional<type::Type> expected_,
                   std::shared_ptr<detail::Scope> scope_)
        : key(std::move(key_)),
          errors(errors_),
          expected(std::move(expected_)),
          scope(scope_)
    {}
};

} // namespace expression
} // namespace style
} // namespace mbgl
