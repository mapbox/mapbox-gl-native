#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/ignore.hpp>

namespace mbgl {
namespace style {
namespace expression {


namespace detail {

/*
    The Signature<Fn> structs are wrappers around an "evaluate()" function whose
    purpose is to extract the necessary Type data from the evaluate function's
    type.  There are three key (partial) specializations:
    
    Signature<R (Params...)>:
    Wraps a simple evaluate function (const T0&, const T1&, ...) -> Result<U>
 
    Signature<R (const Varargs<T>&)>:
    Wraps an evaluate function that takes an arbitrary number of arguments (via
    a Varargs<T>, which is just an alias for std::vector).
 
    Signature<R (const EvaluationParameters&, Params...)>:
    Wraps an evaluate function that needs to access the expression evaluation
    parameters in addition to its subexpressions, i.e.,
    (const EvaluationParams& const T0&, const T1&, ...) -> Result<U>.  Needed
    for expressions like ["zoom"], ["get", key], etc.
    
    In each of the above evaluate signatures, T0, T1, etc. are the types of
    the successfully evaluated subexpressions.
*/
template <class, class Enable = void>
struct Signature;

// Simple evaluate function (const T0&, const T1&, ...) -> Result<U>
template <class R, class... Params>
struct Signature<R (Params...)> : SignatureBase {
    using Args = std::array<std::unique_ptr<Expression>, sizeof...(Params)>;
    
    Signature(R (*evaluate_)(Params...)) :
        SignatureBase(
            valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
            std::vector<type::Type> {valueTypeToExpressionType<std::decay_t<Params>>()...}
        ),
        evaluate(evaluate_)
    {}
    
    EvaluationResult apply(const EvaluationParameters& evaluationParameters, const Args& args) const {
        return applyImpl(evaluationParameters, args, std::index_sequence_for<Params...>{});
    }
    
    std::unique_ptr<Expression> makeExpression(const std::string& name,
                                               std::vector<std::unique_ptr<Expression>> args) const override {
        typename Signature::Args argsArray;
        std::copy_n(std::make_move_iterator(args.begin()), sizeof...(Params), argsArray.begin());
        return std::make_unique<CompoundExpression<Signature>>(name, *this, std::move(argsArray));
    }

    R (*evaluate)(Params...);
private:
    template <std::size_t ...I>
    EvaluationResult applyImpl(const EvaluationParameters& evaluationParameters, const Args& args, std::index_sequence<I...>) const {
        const std::array<EvaluationResult, sizeof...(I)> evaluated = {{std::get<I>(args)->evaluate(evaluationParameters)...}};
        for (const auto& arg : evaluated) {
            if(!arg) return arg.error();
        }
        const R value = evaluate(*fromExpressionValue<std::decay_t<Params>>(*(evaluated[I]))...);
        if (!value) return value.error();
        return *value;
    }
};

// Varargs evaluate function (const Varargs<T>&) -> Result<U>
template <class R, typename T>
struct Signature<R (const Varargs<T>&)> : SignatureBase {
    using Args = std::vector<std::unique_ptr<Expression>>;
    
    Signature(R (*evaluate_)(const Varargs<T>&)) :
        SignatureBase(
            valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
            VarargsType { valueTypeToExpressionType<T>() }
        ),
        evaluate(evaluate_)
    {}
    
    std::unique_ptr<Expression> makeExpression(const std::string& name,
                                               std::vector<std::unique_ptr<Expression>> args) const override  {
        return std::make_unique<CompoundExpression<Signature>>(name, *this, std::move(args));
    };
    
    EvaluationResult apply(const EvaluationParameters& evaluationParameters, const Args& args) const {
        Varargs<T> evaluated;
        for (const auto& arg : args) {
            const EvaluationResult evaluatedArg = arg->evaluate(evaluationParameters);
            if(!evaluatedArg) return evaluatedArg.error();
            evaluated.push_back(*fromExpressionValue<std::decay_t<T>>(*evaluatedArg));
        }
        const R value = evaluate(evaluated);
        if (!value) return value.error();
        return *value;
    }
    
    R (*evaluate)(const Varargs<T>&);
};

// Evaluate function needing parameter access,
// (const EvaluationParams&, const T0&, const T1&, ...) -> Result<U>
template <class R, class... Params>
struct Signature<R (const EvaluationParameters&, Params...)> : SignatureBase {
    using Args = std::array<std::unique_ptr<Expression>, sizeof...(Params)>;
    
    Signature(R (*evaluate_)(const EvaluationParameters&, Params...)) :
        SignatureBase(
            valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
            std::vector<type::Type> {valueTypeToExpressionType<std::decay_t<Params>>()...}
        ),
        evaluate(evaluate_)
    {}
    
    std::unique_ptr<Expression> makeExpression(const std::string& name,
                                               std::vector<std::unique_ptr<Expression>> args) const override {
        typename Signature::Args argsArray;
        std::copy_n(std::make_move_iterator(args.begin()), sizeof...(Params), argsArray.begin());
        return std::make_unique<CompoundExpression<Signature>>(name, *this, std::move(argsArray));
    }
    
    EvaluationResult apply(const EvaluationParameters& evaluationParameters, const Args& args) const {
        return applyImpl(evaluationParameters, args, std::index_sequence_for<Params...>{});
    }
    
private:
    template <std::size_t ...I>
    EvaluationResult applyImpl(const EvaluationParameters& evaluationParameters, const Args& args, std::index_sequence<I...>) const {
        const std::array<EvaluationResult, sizeof...(I)> evaluated = {{std::get<I>(args)->evaluate(evaluationParameters)...}};
        for (const auto& arg : evaluated) {
            if(!arg) return arg.error();
        }
        // TODO: assert correct runtime type of each arg value
        const R value = evaluate(evaluationParameters, *fromExpressionValue<std::decay_t<Params>>(*(evaluated[I]))...);
        if (!value) return value.error();
        return *value;
    }
    
    R (*evaluate)(const EvaluationParameters&, Params...);
};

// Machinery to pull out function types from class methods, lambdas, etc.
template <class R, class... Params>
struct Signature<R (*)(Params...)>
    : Signature<R (Params...)>
{ using Signature<R (Params...)>::Signature; };

template <class T, class R, class... Params>
struct Signature<R (T::*)(Params...) const>
    : Signature<R (Params...)>
{ using Signature<R (Params...)>::Signature;  };

template <class T, class R, class... Params>
struct Signature<R (T::*)(Params...)>
    : Signature<R (Params...)>
{ using Signature<R (Params...)>::Signature; };

template <class Lambda>
struct Signature<Lambda, std::enable_if_t<std::is_class<Lambda>::value>>
    : Signature<decltype(&Lambda::operator())>
{ using Signature<decltype(&Lambda::operator())>::Signature; };

} // namespace detail

using Definition = CompoundExpressionRegistry::Definition;

template <typename T>
Result<T> assertion(const Value& v) {
    if (!v.is<T>()) {
        return EvaluationError {
            "Expected value to be of type " + toString(valueTypeToExpressionType<T>()) +
            ", but found " + toString(typeOf(v)) + " instead."
        };
    }
    return v.get<T>();
}

std::string stringifyColor(double r, double g, double b, double a) {
    return stringify(r) + ", " +
        stringify(g) + ", " +
        stringify(b) + ", " +
        stringify(a);
}
Result<mbgl::Color> rgba(double r, double g, double b, double a) {
    if (
        r < 0 || r > 255 ||
        g < 0 || g > 255 ||
        b < 0 || b > 255
    ) {
        return EvaluationError {
            "Invalid rgba value [" + stringifyColor(r, g, b, a)  +
            "]: 'r', 'g', and 'b' must be between 0 and 255."
        };
    }
    if (a < 0 || a > 1) {
        return EvaluationError {
            "Invalid rgba value [" + stringifyColor(r, g, b, a)  +
            "]: 'a' must be between 0 and 1."
        };
    }
    return mbgl::Color(r / 255, g / 255, b / 255, a);
}

template <typename T>
Result<bool> equal(const T& lhs, const T& rhs) { return lhs == rhs; }

template <typename T>
Result<bool> notEqual(const T& lhs, const T& rhs) { return lhs != rhs; }

template <typename Fn>
static std::unique_ptr<detail::SignatureBase> makeSignature(Fn evaluateFunction) {
    return std::make_unique<detail::Signature<Fn>>(evaluateFunction);
}

std::unordered_map<std::string, CompoundExpressionRegistry::Definition> initializeDefinitions() {
    std::unordered_map<std::string, CompoundExpressionRegistry::Definition> definitions;
    auto define = [&](std::string name, auto fn) {
        definitions[name].push_back(makeSignature(fn));
    };
    
    define("e", []() -> Result<double> { return 2.718281828459045; });
    define("pi", []() -> Result<double> { return 3.141592653589793; });
    define("ln2", []() -> Result<double> { return 0.6931471805599453; });

    define("typeof", [](const Value& v) -> Result<std::string> { return toString(typeOf(v)); });
    define("number", assertion<double>);
    define("string", assertion<std::string>);
    define("boolean", assertion<bool>);
    define("object", assertion<std::unordered_map<std::string, Value>>);
    
    define("to-string", [](const Value& value) -> Result<std::string> {
        return value.match(
            [](const std::unordered_map<std::string, Value>&) -> Result<std::string> {
                return EvaluationError {
                    R"(Expected a primitive value in ["string", ...], but found Object instead.)"
                };
            },
            [](const std::vector<Value>& v) -> Result<std::string> {
                return EvaluationError {
                    R"(Expected a primitive value in ["string", ...], but found )" + toString(typeOf(v)) + " instead."
                };
            },
            [](const auto& v) -> Result<std::string> { return stringify(v); }
        );
    });
    define("to-number", [](const Value& v) -> Result<double> {
        optional<double> result = v.match(
            [](const double f) -> optional<double> { return f; },
            [](const std::string& s) -> optional<double> {
                try {
                    return std::stof(s);
                } catch(std::exception) {
                    return optional<double>();
                }
            },
            [](const auto&) { return optional<double>(); }
        );
        if (!result) {
            return EvaluationError {
                "Could not convert " + stringify(v) + " to number."
            };
        }
        return *result;
    });
    define("to-boolean", [](const Value& v) -> Result<bool> {
        return v.match(
            [&] (double f) { return (bool)f; },
            [&] (const std::string& s) { return s.length() > 0; },
            [&] (bool b) { return b; },
            [&] (const NullValue&) { return false; },
            [&] (const auto&) { return true; }
        );
    });
    define("to-rgba", [](const mbgl::Color& color) -> Result<std::array<double, 4>> {
        return std::array<double, 4> {{ color.r, color.g, color.b, color.a }};
    });
    
    define("to-color", [](const Value& colorValue) -> Result<mbgl::Color> {
        return colorValue.match(
            [&](const std::string& colorString) -> Result<mbgl::Color> {
                const optional<mbgl::Color> result = mbgl::Color::parse(colorString);
                if (result) {
                    return *result;
                } else {
                    return EvaluationError{
                        "Could not parse color from value '" + colorString + "'"
                    };
                }
            },
            [&](const std::vector<Value>& components) -> Result<mbgl::Color> {
                std::size_t len = components.size();
                bool isNumeric = std::all_of(components.begin(), components.end(), [](const Value& item) {
                    return item.template is<double>();
                });
                if ((len == 3 || len == 4) && isNumeric) {
                    return {rgba(
                        components[0].template get<double>(),
                        components[1].template get<double>(),
                        components[2].template get<double>(),
                        len == 4 ? components[3].template get<double>() : 1.0
                    )};
                } else {
                    return EvaluationError{
                        "Could not parse color from value '" + stringify(colorValue) + "'"
                    };
                }
            },
            [&](const auto&) -> Result<mbgl::Color> {
                return EvaluationError{
                    "Could not parse color from value '" + stringify(colorValue) + "'"
                };
            }
        );
    
    });
    
    define("rgba", rgba);
    define("rgb", [](double r, double g, double b) { return rgba(r, g, b, 1.0f); });
    
    define("zoom", [](const EvaluationParameters& params) -> Result<double> {
        if (!params.zoom) {
            return EvaluationError {
                "The 'zoom' expression is unavailable in the current evaluation context."
            };
        }
        return *(params.zoom);
    });
    
    define("has", [](const EvaluationParameters& params, const std::string& key) -> Result<bool> {
        if (!params.feature) {
            return EvaluationError {
                "Feature data is unavailable in the current evaluation context."
            };
        }
        
        return params.feature->getValue(key) ? true : false;
    });
    define("has", [](const std::string& key, const std::unordered_map<std::string, Value>& object) -> Result<bool> {
        return object.find(key) != object.end();
    });
    
    define("get", [](const EvaluationParameters& params, const std::string& key) -> Result<Value> {
        if (!params.feature) {
            return EvaluationError {
                "Feature data is unavailable in the current evaluation context."
            };
        }

        auto propertyValue = params.feature->getValue(key);
        if (!propertyValue) {
            return Null;
        }
        return Value(toExpressionValue(*propertyValue));
    });
    define("get", [](const std::string& key, const std::unordered_map<std::string, Value>& object) -> Result<Value> {
        if (object.find(key) == object.end()) {
            return Null;
        }
        return object.at(key);
    });
    
    define("length", [](const std::vector<Value>& arr) -> Result<double> {
        return arr.size();
    });
    define("length", [] (const std::string s) -> Result<double> {
        return s.size();
    });
    
    define("properties", [](const EvaluationParameters& params) -> Result<std::unordered_map<std::string, Value>> {
        if (!params.feature) {
            return EvaluationError {
                "Feature data is unavailable in the current evaluation context."
            };
        }
        std::unordered_map<std::string, Value> result;
        const PropertyMap properties = params.feature->getProperties();
        for (const auto& entry : properties) {
            result[entry.first] = toExpressionValue(entry.second);
        }
        return result;
    });
    
    define("geometry-type", [](const EvaluationParameters& params) -> Result<std::string> {
        if (!params.feature) {
            return EvaluationError {
                "Feature data is unavailable in the current evaluation context."
            };
        }
    
        auto type = params.feature->getType();
        if (type == FeatureType::Point) {
            return "Point";
        } else if (type == FeatureType::LineString) {
            return "LineString";
        } else if (type == FeatureType::Polygon) {
            return "Polygon";
        } else {
            return "Unknown";
        }
    });
    
    define("id", [](const EvaluationParameters& params) -> Result<Value> {
        if (!params.feature) {
            return EvaluationError {
                "Feature data is unavailable in the current evaluation context."
            };
        }
    
        auto id = params.feature->getID();
        if (!id) {
            return Null;
        }
        return id->match(
            [](const auto& idValue) {
                return toExpressionValue(mbgl::Value(idValue));
            }
        );
    });
    
    define("+", [](const Varargs<double>& args) -> Result<double> {
        double sum = 0.0f;
        for (auto arg : args) {
            sum += arg;
        }
        return sum;
    });
    define("-", [](double a, double b) -> Result<double> { return a - b; });
    define("-", [](double a) -> Result<double> { return -a; });
    define("*", [](const Varargs<double>& args) -> Result<double> {
        double prod = 1.0f;
        for (auto arg : args) {
            prod *= arg;
        }
        return prod;
    });
    define("/", [](double a, double b) -> Result<double> { return a / b; });
    define("%", [](double a, double b) -> Result<double> { return fmod(a, b); });
    define("^", [](double a, double b) -> Result<double> { return pow(a, b); });
    define("log10", [](double x) -> Result<double> { return log10(x); });
    define("ln", [](double x) -> Result<double> { return log(x); });
    define("log2", [](double x) -> Result<double> { return log2(x); });
    define("sin", [](double x) -> Result<double> { return sin(x); });
    define("cos", [](double x) -> Result<double> { return cos(x); });
    define("tan", [](double x) -> Result<double> { return tan(x); });
    define("asin", [](double x) -> Result<double> { return asin(x); });
    define("acos", [](double x) -> Result<double> { return acos(x); });
    define("atan", [](double x) -> Result<double> { return atan(x); });
    
    define("min", [](const Varargs<double>& args) -> Result<double> {
        double result = std::numeric_limits<double>::infinity();
        for (double arg : args) {
            result = fmin(arg, result);
        }
        return result;
    });
    define("max", [](const Varargs<double>& args) -> Result<double> {
        double result = -std::numeric_limits<double>::infinity();
        for (double arg : args) {
            result = fmax(arg, result);
        }
        return result;
    });
    
    define("==", equal<double>);
    define("==", equal<const std::string&>);
    define("==", equal<bool>);
    define("==", equal<NullValue>);

    define("==", notEqual<double>);
    define("==", notEqual<const std::string&>);
    define("==", notEqual<bool>);
    define("==", notEqual<NullValue>);

    define(">", [](double lhs, double rhs) -> Result<bool> { return lhs > rhs; });
    define(">", [](const std::string& lhs, const std::string& rhs) -> Result<bool> { return lhs > rhs; });
    define(">=", [](double lhs, double rhs) -> Result<bool> { return lhs >= rhs; });
    define(">=",[](const std::string& lhs, const std::string& rhs) -> Result<bool> { return lhs >= rhs; });
    define("<", [](double lhs, double rhs) -> Result<bool> { return lhs < rhs; });
    define("<", [](const std::string& lhs, const std::string& rhs) -> Result<bool> { return lhs < rhs; });
    define("<=", [](double lhs, double rhs) -> Result<bool> { return lhs <= rhs; });
    define("<=", [](const std::string& lhs, const std::string& rhs) -> Result<bool> { return lhs <= rhs; });
    
    define("any", [](const Varargs<bool>& args) -> Result<bool> {
        bool result = false;
        for (auto arg : args) result = result || arg;
        return result;
    });
    define("all", [](const Varargs<bool>& args) -> Result<bool> {
        bool result = true;
        for (bool arg : args) result = result && arg;
        return result;
    });
    define("!", [](bool e) -> Result<bool> { return !e; });
    
    define("upcase", [](const std::string& input) -> Result<std::string> {
        std::string s = input;
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c){ return std::toupper(c); });
        return s;
    });
    define("downcase", [](const std::string& input) -> Result<std::string> {
        std::string s = input;
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return s;
    });
    define("concat", [](const Varargs<std::string>& args) -> Result<std::string> {
        std::string s;
        for (const std::string& arg : args) {
            s += arg;
        }
        return s;
    });
    define("error", [](const std::string& input) -> Result<type::ErrorType> {
        return EvaluationError { input };
    });
    
    return definitions;
}

std::unordered_map<std::string, Definition> CompoundExpressionRegistry::definitions = initializeDefinitions();


ParseResult parseCompoundExpression(const std::string name, const mbgl::style::conversion::Convertible& value, ParsingContext ctx) {
    using namespace mbgl::style::conversion;
    assert(isArray(value) && arrayLength(value) > 0);

    auto it = CompoundExpressionRegistry::definitions.find(name);
    if (it == CompoundExpressionRegistry::definitions.end()) {
        ctx.error(
             R"(Unknown expression ")" + name + R"(". If you wanted a literal array, use ["literal", [...]].)",
            0
        );
        return ParseResult();
    }
    const CompoundExpressionRegistry::Definition& definition = it->second;
    
    std::vector<std::unique_ptr<Expression>> args;
    auto length = arrayLength(value);

    // Check if we have a single signature with the correct number of
    // parameters. If so, then use that signature's parameter types for parsing
    // (and inferring the types of) the arguments.
    optional<std::size_t> singleMatchingSignature;
    for (std::size_t j = 0; j < definition.size(); j++) {
        const std::unique_ptr<detail::SignatureBase>& signature = definition[j];
        if (
            signature->params.is<VarargsType>() ||
            signature->params.get<std::vector<type::Type>>().size() == length - 1
        ) {
            if (singleMatchingSignature) {
                singleMatchingSignature = {};
            } else {
                singleMatchingSignature = j;
            }
        }
    }

    // parse subexpressions first
    for (std::size_t i = 1; i < length; i++) {
        optional<type::Type> expected;
        
        if (singleMatchingSignature) {
            expected = definition[*singleMatchingSignature]->params.match(
                [](const VarargsType& varargs) { return varargs.type; },
                [&](const std::vector<type::Type>& params_) { return params_[i - 1]; }
            );
        }
    
        auto parsed = ctx.concat(i, expected).parse(arrayMember(value, i));
        if (!parsed) {
            return parsed;
        }
        args.push_back(std::move(*parsed));
    }
    return createCompoundExpression(name, definition, std::move(args), ctx);
}


ParseResult createCompoundExpression(const std::string& name,
                                     std::vector<std::unique_ptr<Expression>> args,
                                     ParsingContext ctx)
{
    return createCompoundExpression(name, CompoundExpressionRegistry::definitions.at(name), std::move(args), ctx);
}


ParseResult createCompoundExpression(const std::string& name,
                                     const Definition& definition,
                                     std::vector<std::unique_ptr<Expression>> args,
                                     ParsingContext ctx)
{
    std::vector<ParsingError> currentSignatureErrors;

    ParsingContext signatureContext(currentSignatureErrors);
    signatureContext.key = ctx.key;
    
    for (const std::unique_ptr<detail::SignatureBase>& signature : definition) {
        currentSignatureErrors.clear();
        
        
        if (signature->params.is<std::vector<type::Type>>()) {
            const std::vector<type::Type>& params = signature->params.get<std::vector<type::Type>>();
            if (params.size() != args.size()) {
                signatureContext.error(
                    "Expected " + std::to_string(params.size()) +
                    " arguments, but found " + std::to_string(args.size()) + " instead."
                );
                continue;
            }

            for (std::size_t i = 0; i < args.size(); i++) {
                const std::unique_ptr<Expression>& arg = args[i];
                signatureContext.concat(i + 1, params.at(i)).checkType(arg->getType());
            }
        } else if (signature->params.is<VarargsType>()) {
            const type::Type& paramType = signature->params.get<VarargsType>().type;
            for (std::size_t i = 0; i < args.size(); i++) {
                const std::unique_ptr<Expression>& arg = args[i];
                signatureContext.concat(i + 1, paramType).checkType(arg->getType());
            }
        }
        
        if (currentSignatureErrors.size() == 0) {
            return ParseResult(signature->makeExpression(name, std::move(args)));
        }
    }
    
    if (definition.size() == 1) {
        ctx.errors.insert(ctx.errors.end(), currentSignatureErrors.begin(), currentSignatureErrors.end());
    } else {
        std::string signatures;
        for (const auto& signature : definition) {
            signatures += (signatures.size() > 0 ? " | " : "");
            signature->params.match(
                [&](const VarargsType& varargs) {
                    signatures += "(" + toString(varargs.type) + ")";
                },
                [&](const std::vector<type::Type>& params) {
                    signatures += "(";
                    bool first = true;
                    for (const type::Type& param : params) {
                        if (!first) signatures += ", ";
                        signatures += toString(param);
                        first = false;
                    }
                    signatures += ")";
                }
            );
            
        }
        std::string actualTypes;
        for (const auto& arg : args) {
            if (actualTypes.size() > 0) {
                actualTypes += ", ";
            }
            actualTypes += toString(arg->getType());
        }
        ctx.error("Expected arguments of type " + signatures + ", but found (" + actualTypes + ") instead.");
    }
    
    return ParseResult();
}

} // namespace expression
} // namespace style
} // namespace mbgl
