#include <boost/algorithm/string/join.hpp>
#include <mbgl/style/expression/collator.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/check_subtype.hpp>
#include <mbgl/style/expression/util.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/platform.hpp>
#include <cmath>

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
 
    Signature<R (const EvaluationContext&, Params...)>:
    Wraps an evaluate function that needs to access the expression evaluation
    parameters in addition to its subexpressions, i.e.,
    (const EvaluationParams&, const T0&, const T1&, ...) -> Result<U>.  Needed
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

    Signature(R (*evaluate_)(Params...), std::string name_) :
        SignatureBase(
            valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
            std::vector<type::Type> {valueTypeToExpressionType<std::decay_t<Params>>()...},
            std::move(name_)
        ),
        evaluate(evaluate_)    {}
    
    EvaluationResult apply(const EvaluationContext& evaluationParameters, const Args& args) const {
        return applyImpl(evaluationParameters, args, std::index_sequence_for<Params...>{});
    }

    std::unique_ptr<Expression> makeExpression(std::vector<std::unique_ptr<Expression>> args) const override {
        typename Signature::Args argsArray;
        std::copy_n(std::make_move_iterator(args.begin()), sizeof...(Params), argsArray.begin());
        return std::make_unique<CompoundExpression<Signature>>(name, *this, std::move(argsArray));
    }

    R (*evaluate)(Params...);
private:
    template <std::size_t ...I>
    EvaluationResult applyImpl(const EvaluationContext& evaluationParameters, const Args& args, std::index_sequence<I...>) const {
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

    Signature(R (*evaluate_)(const Varargs<T>&), std::string name_) :
        SignatureBase(
            valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
            VarargsType { valueTypeToExpressionType<T>() },
            std::move(name_)
        ),
        evaluate(evaluate_)
    {}

    std::unique_ptr<Expression> makeExpression(std::vector<std::unique_ptr<Expression>> args) const override  {
        return std::make_unique<CompoundExpression<Signature>>(name, *this, std::move(args));
    };

    EvaluationResult apply(const EvaluationContext& evaluationParameters, const Args& args) const {
        Varargs<T> evaluated;
        evaluated.reserve(args.size());
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
struct Signature<R (const EvaluationContext&, Params...)> : SignatureBase {
    using Args = std::array<std::unique_ptr<Expression>, sizeof...(Params)>;

    Signature(R (*evaluate_)(const EvaluationContext&, Params...), std::string name_) :
        SignatureBase(
            valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
            std::vector<type::Type> {valueTypeToExpressionType<std::decay_t<Params>>()...},
            std::move(name_)
        ),
        evaluate(evaluate_)
    {}

    std::unique_ptr<Expression> makeExpression(std::vector<std::unique_ptr<Expression>> args) const override {
        typename Signature::Args argsArray;
        std::copy_n(std::make_move_iterator(args.begin()), sizeof...(Params), argsArray.begin());
        return std::make_unique<CompoundExpression<Signature>>(name, *this, std::move(argsArray));
    }

    EvaluationResult apply(const EvaluationContext& evaluationParameters, const Args& args) const {
        return applyImpl(evaluationParameters, args, std::index_sequence_for<Params...>{});
    }

private:
    template <std::size_t ...I>
    EvaluationResult applyImpl(const EvaluationContext& evaluationParameters, const Args& args, std::index_sequence<I...>) const {
        const std::array<EvaluationResult, sizeof...(I)> evaluated = {{std::get<I>(args)->evaluate(evaluationParameters)...}};
        for (const auto& arg : evaluated) {
            if(!arg) return arg.error();
        }
        // TODO: assert correct runtime type of each arg value
        const R value = evaluate(evaluationParameters, *fromExpressionValue<std::decay_t<Params>>(*(evaluated[I]))...);
        if (!value) return value.error();
        return *value;
    }
    
    R (*evaluate)(const EvaluationContext&, Params...);
};
    
// Evaluate function needing EvaluationContext and Varargs
// (const EvaluationContext&, const Varargs<T>&) -> Result<U>
template <class R, typename T>
struct Signature<R (const EvaluationContext&, const Varargs<T>&)> : SignatureBase {
    using Args = std::vector<std::unique_ptr<Expression>>;
    
    Signature(R (*evaluate_)(const EvaluationContext&, const Varargs<T>&), std::string name_) :
    SignatureBase(
                  valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
                  VarargsType { valueTypeToExpressionType<T>() },
                  std::move(name_)
                  ),
    evaluate(evaluate_)
    {}
    
    std::unique_ptr<Expression> makeExpression(std::vector<std::unique_ptr<Expression>> args) const override  {
        return std::make_unique<CompoundExpression<Signature>>(name, *this, std::move(args));
    };
    
    EvaluationResult apply(const EvaluationContext& evaluationParameters, const Args& args) const {
        Varargs<T> evaluated;
        evaluated.reserve(args.size());
        for (const auto& arg : args) {
            const EvaluationResult evaluatedArg = arg->evaluate(evaluationParameters);
            if(!evaluatedArg) return evaluatedArg.error();
            evaluated.push_back(*fromExpressionValue<std::decay_t<T>>(*evaluatedArg));
        }
        const R value = evaluate(evaluationParameters, evaluated);
        if (!value) return value.error();
        return *value;
    }
    
    R (*evaluate)(const EvaluationContext&, const Varargs<T>&);
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

template <typename Fn>
static std::unique_ptr<detail::SignatureBase> makeSignature(Fn evaluateFunction, std::string name) {
    return std::make_unique<detail::Signature<Fn>>(evaluateFunction, std::move(name));
}

Value featureIdAsExpressionValue(EvaluationContext params) {
    assert(params.feature);
    auto id = params.feature->getID();
    if (!id) return Null;
    return id->match([](const auto& idid) {
        return toExpressionValue(mbgl::Value(idid));
    });
};

optional<Value> featurePropertyAsExpressionValue(EvaluationContext params, const std::string& key) {
    assert(params.feature);
    auto property = params.feature->getValue(key);
    return property ? toExpressionValue(*property) : optional<Value>();
};

optional<std::string> featureTypeAsString(FeatureType type) {
    switch(type) {
    case FeatureType::Point:
        return optional<std::string>("Point");
    case FeatureType::LineString:
        return optional<std::string>("LineString");
    case FeatureType::Polygon:
        return optional<std::string>("Polygon");
    case FeatureType::Unknown:
        return optional<std::string>("Unknown");
    default:
        return {};
    }
};

optional<double> featurePropertyAsDouble(EvaluationContext params, const std::string& key) {
    assert(params.feature);
    auto property = params.feature->getValue(key);
    if (!property) return {};
    return property->match(
        [](double value) { return value; },
        [](uint64_t value) { return optional<double>(static_cast<double>(value)); },
        [](int64_t value) { return optional<double>(static_cast<double>(value)); },
        [](auto) { return optional<double>(); }
    );
};

optional<std::string> featurePropertyAsString(EvaluationContext params, const std::string& key) {
    assert(params.feature);
    auto property = params.feature->getValue(key);
    if (!property) return {};
    return property->match(
        [](std::string value) { return value; },
        [](auto) { return optional<std::string>(); }
    );
};

optional<double> featureIdAsDouble(EvaluationContext params) {
    assert(params.feature);
    auto id = params.feature->getID();
    if (!id) return optional<double>();
    return id->match(
        [](double value) { return value; },
        [](uint64_t value) { return optional<double>(static_cast<double>(value)); },
        [](int64_t value) { return optional<double>(static_cast<double>(value)); },
        [](auto) { return optional<double>(); }
    );
};

optional<std::string> featureIdAsString(EvaluationContext params) {
    assert(params.feature);
    auto id = params.feature->getID();
    if (!id) return optional<std::string>();
    return id->match(
        [](std::string value) { return value; },
        [](auto) { return optional<std::string>(); }
    );
};

std::unordered_map<std::string, CompoundExpressionRegistry::Definition> initializeDefinitions() {
    std::unordered_map<std::string, CompoundExpressionRegistry::Definition> definitions;
    auto define = [&](std::string name, auto fn) {
        definitions[name].push_back(makeSignature(fn, name));
    };

    define("e", []() -> Result<double> { return 2.718281828459045; });
    define("pi", []() -> Result<double> { return 3.141592653589793; });
    define("ln2", []() -> Result<double> { return 0.6931471805599453; });

    define("typeof", [](const Value& v) -> Result<std::string> { return toString(typeOf(v)); });

    define("to-string", [](const Value& value) -> Result<std::string> {
        return value.match(
            [](const NullValue&) -> Result<std::string> { return std::string(); },
            [](const Color& c) -> Result<std::string> { return c.stringify(); }, // avoid quoting
            [](const std::string& s) -> Result<std::string> { return s; }, // avoid quoting
            [](const auto& v) -> Result<std::string> { return stringify(v); }
        );
    });

    define("to-boolean", [](const Value& v) -> Result<bool> {
        return v.match(
            [&] (double f) { return static_cast<bool>(f); },
            [&] (const std::string& s) { return s.length() > 0; },
            [&] (bool b) { return b; },
            [&] (const NullValue&) { return false; },
            [&] (const auto&) { return true; }
        );
    });
    define("to-rgba", [](const Color& color) -> Result<std::array<double, 4>> {
        return color.toArray();
    });

    define("rgba", rgba);
    define("rgb", [](double r, double g, double b) { return rgba(r, g, b, 1.0f); });

    define("zoom", [](const EvaluationContext& params) -> Result<double> {
        if (!params.zoom) {
            return EvaluationError {
                "The 'zoom' expression is unavailable in the current evaluation context."
            };
        }
        return *(params.zoom);
    });

    define("heatmap-density", [](const EvaluationContext& params) -> Result<double> {
        if (!params.heatmapDensity) {
            return EvaluationError {
                "The 'heatmap-density' expression is unavailable in the current evaluation context."
            };
        }
        return *(params.heatmapDensity);
    });
    
    define("has", [](const EvaluationContext& params, const std::string& key) -> Result<bool> {
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
    
    define("get", [](const EvaluationContext& params, const std::string& key) -> Result<Value> {
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

    define("properties", [](const EvaluationContext& params) -> Result<std::unordered_map<std::string, Value>> {
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

    define("geometry-type", [](const EvaluationContext& params) -> Result<std::string> {
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

    define("id", [](const EvaluationContext& params) -> Result<Value> {
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
    define("sqrt", [](double x) -> Result<double> { return sqrt(x); });
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
    
    define("round", [](double x) -> Result<double> { return ::round(x); });
    define("floor", [](double x) -> Result<double> { return std::floor(x); });
    define("ceil", [](double x) -> Result<double> { return std::ceil(x); });
    define("abs", [](double x) -> Result<double> { return std::abs(x); });

    define(">", [](double lhs, double rhs) -> Result<bool> { return lhs > rhs; });
    define(">", [](const std::string& lhs, const std::string& rhs) -> Result<bool> { return lhs > rhs; });
    define(">", [](const std::string& lhs, const std::string& rhs, const Collator& c) -> Result<bool> { return c.compare(lhs, rhs) > 0; });
    define(">=", [](double lhs, double rhs) -> Result<bool> { return lhs >= rhs; });
    define(">=",[](const std::string& lhs, const std::string& rhs) -> Result<bool> { return lhs >= rhs; });
    define(">=", [](const std::string& lhs, const std::string& rhs, const Collator& c) -> Result<bool> { return c.compare(lhs, rhs) >= 0; });
    define("<", [](double lhs, double rhs) -> Result<bool> { return lhs < rhs; });
    define("<", [](const std::string& lhs, const std::string& rhs) -> Result<bool> { return lhs < rhs; });
    define("<", [](const std::string& lhs, const std::string& rhs, const Collator& c) -> Result<bool> { return c.compare(lhs, rhs) < 0; });
    define("<=", [](double lhs, double rhs) -> Result<bool> { return lhs <= rhs; });
    define("<=", [](const std::string& lhs, const std::string& rhs) -> Result<bool> { return lhs <= rhs; });
    define("<=", [](const std::string& lhs, const std::string& rhs, const Collator& c) -> Result<bool> { return c.compare(lhs, rhs) <= 0; });

    define("!", [](bool e) -> Result<bool> { return !e; });
    
    define("is-supported-script", [](const std::string& x) -> Result<bool> {
        return util::i18n::isStringInSupportedScript(x);
    });

    define("upcase", [](const std::string& input) -> Result<std::string> {
        return platform::uppercase(input);
    });
    define("downcase", [](const std::string& input) -> Result<std::string> {
        return platform::lowercase(input);
    });
    define("concat", [](const Varargs<std::string>& args) -> Result<std::string> {
        std::string s;
        for (const std::string& arg : args) {
            s += arg;
        }
        return s;
    });
    define("resolved-locale", [](const Collator& collator) -> Result<std::string> {
        return collator.resolvedLocale();
    });
    
    define("error", [](const std::string& input) -> Result<type::ErrorType> {
        return EvaluationError { input };
    });

    // Legacy Filters
    define("filter-==", [](const EvaluationContext& params, const std::string& key, const Value &lhs) -> Result<bool> {
        const auto rhs = featurePropertyAsExpressionValue(params, key);
        return rhs ? lhs == *rhs : false;
    });

    define("filter-id-==", [](const EvaluationContext& params, const Value &lhs) -> Result<bool> {
        return lhs == featureIdAsExpressionValue(params);
    });

    define("filter-type-==", [](const EvaluationContext& params, const std::string &lhs) -> Result<bool> {
        if (!params.feature) return false;
        return featureTypeAsString(params.feature->getType()) == lhs;
    });

    define("filter-<", [](const EvaluationContext& params, const std::string& key, double lhs) -> Result<bool> {
        auto rhs = featurePropertyAsDouble(params, key);
        return rhs ? rhs < lhs : false;
    });

    define("filter-<", [](const EvaluationContext& params, const std::string& key, std::string lhs) -> Result<bool> {
        auto rhs = featurePropertyAsString(params, key);
        return rhs ? rhs < lhs : false;
    });

    define("filter-id-<", [](const EvaluationContext& params, double lhs) -> Result<bool> {
        auto rhs = featureIdAsDouble(params);
        return rhs ? rhs < lhs : false;
    });

    define("filter-id-<", [](const EvaluationContext& params, std::string lhs) -> Result<bool> {
        auto rhs = featureIdAsString(params);
        return rhs ? rhs < lhs : false;
    });

    define("filter->", [](const EvaluationContext& params, const std::string& key, double lhs) -> Result<bool> {
        auto rhs = featurePropertyAsDouble(params, key);
        return rhs ? rhs > lhs : false;
    });

    define("filter->", [](const EvaluationContext& params, const std::string& key, std::string lhs) -> Result<bool> {
        auto rhs = featurePropertyAsString(params, key);
        return rhs ? rhs > lhs : false;
    });

    define("filter-id->", [](const EvaluationContext& params, double lhs) -> Result<bool> {
        auto rhs = featureIdAsDouble(params);
        return rhs ? rhs > lhs : false;
    });

    define("filter-id->", [](const EvaluationContext& params, std::string lhs) -> Result<bool> {
        auto rhs = featureIdAsString(params);
        return rhs ? rhs > lhs : false;
    });

    define("filter-<=", [](const EvaluationContext& params, const std::string& key, double lhs) -> Result<bool> {
        auto rhs = featurePropertyAsDouble(params, key);
        return rhs ? rhs <= lhs : false;
    });
    
    define("filter-<=", [](const EvaluationContext& params, const std::string& key, std::string lhs) -> Result<bool> {
        auto rhs = featurePropertyAsString(params, key);
        return rhs ? rhs <= lhs : false;
    });

    define("filter-id-<=", [](const EvaluationContext& params, double lhs) -> Result<bool> {
        auto rhs = featureIdAsDouble(params);
        return rhs ? rhs <= lhs : false;
    });
    
    define("filter-id-<=", [](const EvaluationContext& params, std::string lhs) -> Result<bool> {
        auto rhs = featureIdAsString(params);
        return rhs ? rhs <= lhs : false;
    });

    define("filter->=", [](const EvaluationContext& params, const std::string& key, double lhs) -> Result<bool> {
        auto rhs = featurePropertyAsDouble(params, key);
        return rhs ? rhs >= lhs : false;
    });
    
    define("filter->=", [](const EvaluationContext& params, const std::string& key, std::string lhs) -> Result<bool> {
        auto rhs = featurePropertyAsString(params, key);
        return rhs ? rhs >= lhs : false;
    });

    define("filter-id->=", [](const EvaluationContext& params, double lhs) -> Result<bool> {
        auto rhs = featureIdAsDouble(params);
        return rhs ? rhs >= lhs : false;
    });

    define("filter-id->=", [](const EvaluationContext& params, std::string lhs) -> Result<bool> {
        auto rhs = featureIdAsString(params);
        return rhs ? rhs >= lhs : false;
    });

    define("filter-has", [](const EvaluationContext& params, const std::string& key) -> Result<bool> {
        assert(params.feature);
        return bool(params.feature->getValue(key));
    });

    define("filter-has-id", [](const EvaluationContext& params) -> Result<bool> {
        assert(params.feature);
        return bool(params.feature->getID());
    });

    define("filter-type-in", [](const EvaluationContext& params, const Varargs<std::string>& types) -> Result<bool> {
        assert(params.feature);
        optional<std::string> type = featureTypeAsString(params.feature->getType());
        return std::find(types.begin(), types.end(), type) != types.end();
    });
    
    define("filter-id-in", [](const EvaluationContext& params, const Varargs<Value>& ids) -> Result<bool> {
        auto id = featureIdAsExpressionValue(params);
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    });

    define("filter-in", [](const EvaluationContext& params, const Varargs<Value>& varargs) -> Result<bool> {
        if (varargs.size() < 2) return false;
        assert(varargs[0].is<std::string>());
        auto value = featurePropertyAsExpressionValue(params, varargs[0].get<std::string>());
        return value ? std::find(varargs.begin() + 1, varargs.end(), *value) != varargs.end() : false;
    });

    return definitions;
}

std::unordered_map<std::string, Definition> CompoundExpressionRegistry::definitions = initializeDefinitions();

using namespace mbgl::style::conversion;

static ParseResult createCompoundExpression(const Definition& definition,
                                            std::vector<std::unique_ptr<Expression>> args,
                                            ParsingContext& ctx)
{
    ParsingContext signatureContext(ctx.getKey());

    for (const std::unique_ptr<detail::SignatureBase>& signature : definition) {
        signatureContext.clearErrors();

        if (signature->params.is<std::vector<type::Type>>()) {
            const std::vector<type::Type>& params = signature->params.get<std::vector<type::Type>>();
            if (params.size() != args.size()) {
                signatureContext.error(
                    "Expected " + util::toString(params.size()) +
                    " arguments, but found " + util::toString(args.size()) + " instead."
                );
                continue;
            }

            for (std::size_t i = 0; i < args.size(); i++) {
                const std::unique_ptr<Expression>& arg = args[i];
                optional<std::string> err = type::checkSubtype(params.at(i), arg->getType());
                if (err) {
                    signatureContext.error(*err, i + 1);
                }
            }
        } else if (signature->params.is<VarargsType>()) {
            const type::Type& paramType = signature->params.get<VarargsType>().type;
            for (std::size_t i = 0; i < args.size(); i++) {
                const std::unique_ptr<Expression>& arg = args[i];
                optional<std::string> err = type::checkSubtype(paramType, arg->getType());
                if (err) {
                    signatureContext.error(*err, i + 1);
                }
            }
        }

        if (signatureContext.getErrors().size() == 0) {
            return ParseResult(signature->makeExpression(std::move(args)));
        }
    }

    if (definition.size() == 1) {
        ctx.appendErrors(std::move(signatureContext));
    } else {
        std::vector<std::string> availableOverloads; // Only used if there are no overloads with matching number of args
        std::vector<std::string> overloads;
        for (const auto& signature : definition) {
            signature->params.match(
                [&](const VarargsType& varargs) {
                    std::string overload = "(" + toString(varargs.type) + ")";
                    overloads.push_back(overload);
                },
                [&](const std::vector<type::Type>& params) {
                    std::string overload = "(";
                    bool first = true;
                    for (const type::Type& param : params) {
                        if (!first) overload += ", ";
                        overload += toString(param);
                        first = false;
                    }
                    overload += ")";
                    if (params.size() == args.size()) {
                        overloads.push_back(overload);
                    } else {
                        availableOverloads.push_back(overload);
                    }
                }
            );

        }
        std::string signatures = overloads.empty() ?
            boost::algorithm::join(availableOverloads, " | ") :
            boost::algorithm::join(overloads, " | ");
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

ParseResult parseCompoundExpression(const std::string name, const Convertible& value, ParsingContext& ctx) {
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
    std::vector<std::unique_ptr<Expression>> args;
    args.reserve(length - 1);
    for (std::size_t i = 1; i < length; i++) {
        optional<type::Type> expected;

        if (singleMatchingSignature) {
            expected = definition[*singleMatchingSignature]->params.match(
                [](const VarargsType& varargs) { return varargs.type; },
                [&](const std::vector<type::Type>& params_) { return params_[i - 1]; }
            );
        }

        auto parsed = ctx.parse(arrayMember(value, i), i, expected);
        if (!parsed) {
            return parsed;
        }
        args.push_back(std::move(*parsed));
    }

    return createCompoundExpression(definition, std::move(args), ctx);
}

ParseResult createCompoundExpression(const std::string& name,
                                     std::vector<std::unique_ptr<Expression>> args,
                                     ParsingContext& ctx)
{
    return createCompoundExpression(CompoundExpressionRegistry::definitions.at(name), std::move(args), ctx);
}

} // namespace expression
} // namespace style
} // namespace mbgl
