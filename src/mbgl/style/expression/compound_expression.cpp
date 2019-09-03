#include <boost/algorithm/string/join.hpp>
#include <mbgl/style/expression/collator.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/check_subtype.hpp>
#include <mbgl/style/expression/util.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/variant.hpp>

#include <mapbox/eternal.hpp>

#include <cmath>

namespace mbgl {
namespace style {
namespace expression {

/*
    Represents the parameter list for an expression that takes an arbitrary
    number of arguments (of a specific type).
*/
struct VarargsType { type::Type type; };

bool operator==(const VarargsType& lhs, const VarargsType& rhs) {
    return lhs.type == rhs.type;
}

template <typename T>
struct Varargs : std::vector<T> {
    template <class... Args>
    Varargs(Args&&... args) : std::vector<T>(std::forward<Args>(args)...) {}
};

namespace detail {
// Base class for the Signature<Fn> structs that are used to determine
// each CompoundExpression definition's type::Type data from the type of its
// "evaluate" function.
struct SignatureBase {
    using Args = std::vector<std::unique_ptr<Expression>>;

    SignatureBase(type::Type result_, variant<std::vector<type::Type>, VarargsType> params_, std::string name_) :
        result(std::move(result_)),
        params(std::move(params_)),
        name(std::move(name_))
    {}
    virtual ~SignatureBase() = default;

    virtual EvaluationResult apply(const EvaluationContext&, const Args&) const = 0;

    type::Type result;
    variant<std::vector<type::Type>, VarargsType> params;
    std::string name;
};

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
    Signature(R (*evaluate_)(Params...), std::string name_) :
        SignatureBase(
            valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
            std::vector<type::Type> {valueTypeToExpressionType<std::decay_t<Params>>()...},
            std::move(name_)
        ),
        evaluate(evaluate_)    {}

    EvaluationResult apply(const EvaluationContext& evaluationParameters, const Args& args) const override {
        return applyImpl(evaluationParameters, args, std::index_sequence_for<Params...>{});
    }

    R (*evaluate)(Params...);

private:
    template <std::size_t ...I>
    EvaluationResult applyImpl(const EvaluationContext& evaluationParameters, const Args& args, std::index_sequence<I...>) const {
        std::array<Value, sizeof...(Params)> evaluated;
        for (std::size_t i = 0; i < sizeof...(Params); ++i) {
            const EvaluationResult evaluatedArg = args.at(i)->evaluate(evaluationParameters);
            if (!evaluatedArg) return evaluatedArg.error();
            evaluated[i] = std::move(*evaluatedArg);
        }
        const R value = evaluate(*fromExpressionValue<std::decay_t<Params>>(evaluated[I])...);
        if (!value) return value.error();
        return *value;
    }
};

// Varargs evaluate function (const Varargs<T>&) -> Result<U>
template <class R, typename T>
struct Signature<R (const Varargs<T>&)> : SignatureBase {
    Signature(R (*evaluate_)(const Varargs<T>&), std::string name_) :
        SignatureBase(
            valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
            VarargsType { valueTypeToExpressionType<T>() },
            std::move(name_)
        ),
        evaluate(evaluate_)
    {}

    EvaluationResult apply(const EvaluationContext& evaluationParameters, const Args& args) const override {
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
    Signature(R (*evaluate_)(const EvaluationContext&, Params...), std::string name_) :
        SignatureBase(
            valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
            std::vector<type::Type> {valueTypeToExpressionType<std::decay_t<Params>>()...},
            std::move(name_)
        ),
        evaluate(evaluate_)
    {}

    EvaluationResult apply(const EvaluationContext& evaluationParameters, const Args& args) const override {
        return applyImpl(evaluationParameters, args, std::index_sequence_for<Params...>{});
    }

private:
    template <std::size_t ...I>
    EvaluationResult applyImpl(const EvaluationContext& evaluationParameters, const Args& args, std::index_sequence<I...>) const {
        std::array<Value, sizeof...(Params)> evaluated;
        for (std::size_t i = 0; i < sizeof...(Params); ++i) {
            const EvaluationResult evaluatedArg = args.at(i)->evaluate(evaluationParameters);
            if (!evaluatedArg) return evaluatedArg.error();
            evaluated[i] = std::move(*evaluatedArg);
        }
        const R value = evaluate(evaluationParameters, *fromExpressionValue<std::decay_t<Params>>(evaluated[I])...);
        if (!value) return value.error();
        return *value;
    }

    R (*evaluate)(const EvaluationContext&, Params...);
};
    
// Evaluate function needing EvaluationContext and Varargs
// (const EvaluationContext&, const Varargs<T>&) -> Result<U>
template <class R, typename T>
struct Signature<R (const EvaluationContext&, const Varargs<T>&)> : SignatureBase {
    Signature(R (*evaluate_)(const EvaluationContext&, const Varargs<T>&), std::string name_) :
    SignatureBase(
                  valueTypeToExpressionType<std::decay_t<typename R::Value>>(),
                  VarargsType { valueTypeToExpressionType<T>() },
                  std::move(name_)
                  ),
    evaluate(evaluate_)
    {}

    EvaluationResult apply(const EvaluationContext& evaluationParameters, const Args& args) const override {
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

// Machinery to pull out function type from lambdas.
template <class, class = void>
struct SignatureType;

template <class T, class R, class... Params>
struct SignatureType<R (T::*)(Params...) const>
{ using Type = R (Params...);  };

template <class Lambda>
struct SignatureType<Lambda, std::enable_if_t<std::is_class<Lambda>::value>>
{ using Type = typename SignatureType<decltype(&Lambda::operator())>::Type; };

template <typename Fn>
static std::unique_ptr<detail::SignatureBase> makeSignature(std::string name, Fn evaluateFunction) {
    return std::make_unique<Signature<typename SignatureType<Fn>::Type>>(evaluateFunction, std::move(name));
}

} // namespace detail

Value featureIdAsExpressionValue(EvaluationContext params) {
    assert(params.feature);
    auto id = params.feature->getID();
    if (id.is<NullValue>()) return Null;
    return id.match([](const auto& idid) {
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
    return id.match(
        [](double value) { return value; },
        [](uint64_t value) { return optional<double>(static_cast<double>(value)); },
        [](int64_t value) { return optional<double>(static_cast<double>(value)); },
        [](auto) { return optional<double>(); }
    );
};

optional<std::string> featureIdAsString(EvaluationContext params) {
    assert(params.feature);
    auto id = params.feature->getID();
    return id.match(
        [](std::string value) { return value; },
        [](auto) { return optional<std::string>(); }
    );
};

const auto& eCompoundExpression() {
    static auto signature = detail::makeSignature("e", []() -> Result<double> { return 2.718281828459045; });
    return signature;
}

const auto& piCompoundExpression() {
    static auto signature = detail::makeSignature("pi", []() -> Result<double> { return 3.141592653589793; });
    return signature;
}

const auto& ln2CompoundExpression() {
    static auto signature = detail::makeSignature("ln2", []() -> Result<double> { return 0.6931471805599453; });
    return signature;
}

const auto& typeofCompoundExpression() {
    static auto signature = detail::makeSignature("typeof", [](const Value& v) -> Result<std::string> { return toString(typeOf(v)); });
    return signature;
}

const auto& toRgbaCompoundExpression() {
    static auto signature = detail::makeSignature("to-rgba", [](const Color& color) -> Result<std::array<double, 4>> {
        return color.toArray();
    });
    return signature;
}

const auto& rgbaCompoundExpression() {
    static auto signature = detail::makeSignature("rgba", [](double r, double g, double b, double a) { return rgba(r, g, b, a); });
    return signature;
}

const auto& rgbCompoundExpression() {
    static auto signature = detail::makeSignature("rgb", [](double r, double g, double b) { return rgba(r, g, b, 1.0f); });
    return signature;
}

const auto& zoomCompoundExpression() {
    static auto signature = detail::makeSignature("zoom", [](const EvaluationContext& params) -> Result<double> {
        if (!params.zoom) {
            return EvaluationError {
                "The 'zoom' expression is unavailable in the current evaluation context."
            };
        }
        return *(params.zoom);
    });
    return signature;
}

const auto& heatmapDensityCompoundExpression() {
    static auto signature = detail::makeSignature("heatmap-density", [](const EvaluationContext& params) -> Result<double> {
        if (!params.colorRampParameter) {
            return EvaluationError {
                "The 'heatmap-density' expression is unavailable in the current evaluation context."
            };
        }
        return *(params.colorRampParameter);
    });
    return signature;
}

const auto& lineProgressCompoundExpression() {
    static auto signature = detail::makeSignature("line-progress", [](const EvaluationContext& params) -> Result<double> {
        if (!params.colorRampParameter) {
            return EvaluationError {
                "The 'line-progress' expression is unavailable in the current evaluation context."
            };
        }
        return *(params.colorRampParameter);
    });
    return signature;
}

const auto& accumulatedCompoundExpression() {
    const static auto signature = detail::makeSignature("accumulated", [](const EvaluationContext& params) -> Result<Value> {
        if (!params.accumulated) {
            return EvaluationError {
                "The 'accumulated' expression is unavailable in the current evaluation context."
            };
        }
        return Value(toExpressionValue(*params.accumulated));
    });
    return signature;
}
    
const auto& hasContextCompoundExpression() {
    static auto signature = detail::makeSignature("has", [](const EvaluationContext& params, const std::string& key) -> Result<bool> {
        if (!params.feature) {
            return EvaluationError {
                "Feature data is unavailable in the current evaluation context."
            };
        }

        return params.feature->getValue(key) ? true : false;
    });
    return signature;
}

const auto& hasObjectCompoundExpression() {
    static auto signature = detail::makeSignature("has", [](const std::string& key, const std::unordered_map<std::string, Value>& object) -> Result<bool> {
        return object.find(key) != object.end();
    });
    return signature;
}

const auto& getContextCompoundExpression() {
    static auto signature = detail::makeSignature("get", [](const EvaluationContext& params, const std::string& key) -> Result<Value> {
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
    return signature;
}

const auto& getObjectCompoundExpression() {
    static auto signature = detail::makeSignature("get", [](const std::string& key, const std::unordered_map<std::string, Value>& object) -> Result<Value> {
        if (object.find(key) == object.end()) {
            return Null;
        }
        return object.at(key);
    });
    return signature;
}

const auto& propertiesCompoundExpression() {
    static auto signature = detail::makeSignature("properties", [](const EvaluationContext& params) -> Result<std::unordered_map<std::string, Value>> {
        if (!params.feature) {
            return EvaluationError {
                "Feature data is unavailable in the current evaluation context."
            };
        }
        std::unordered_map<std::string, Value> result;
        const PropertyMap& properties = params.feature->getProperties();
        result.reserve(properties.size());
        for (const auto& entry : properties) {
            result[entry.first] = toExpressionValue(entry.second);
        }
        return result;
    });
    return signature;
}

const auto& geometryTypeCompoundExpression() {
    static auto signature = detail::makeSignature("geometry-type", [](const EvaluationContext& params) -> Result<std::string> {
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
    return signature;
}

const auto& idCompoundExpression() {
    static auto signature = detail::makeSignature("id", [](const EvaluationContext& params) -> Result<Value> {
        if (!params.feature) {
            return EvaluationError {
                "Feature data is unavailable in the current evaluation context."
            };
        }

        auto id = params.feature->getID();
        return id.match(
            [](const auto& idValue) {
                return toExpressionValue(mbgl::Value(idValue));
            }
        );
    });
    return signature;
}

const auto& plusCompoundExpression() {
    static auto signature = detail::makeSignature("+", [](const Varargs<double>& args) -> Result<double> {
        double sum = 0.0f;
        for (auto arg : args) {
            sum += arg;
        }
        return sum;
    });
    return signature;
}

const auto& minusCompoundExpression() {
    static auto signature = detail::makeSignature("-", [](double a, double b) -> Result<double> { return a - b; });
    return signature;
}

const auto& negateCompoundExpression() {
    static auto signature = detail::makeSignature("-", [](double a) -> Result<double> { return -a; });
    return signature;
}

const auto& multiplyCompoundExpression() {
    static auto signature = detail::makeSignature("*", [](const Varargs<double>& args) -> Result<double> {
        double prod = 1.0f;
        for (auto arg : args) {
            prod *= arg;
        }
        return prod;
    });
    return signature;
}

const auto& divideCompoundExpression() {
    static auto signature = detail::makeSignature("/", [](double a, double b) -> Result<double> { return a / b; });
    return signature;
}

const auto& modCompoundExpression() {
    static auto signature = detail::makeSignature("%", [](double a, double b) -> Result<double> { return fmod(a, b); });
    return signature;
}

const auto& powCompoundExpression() {
    static auto signature = detail::makeSignature("^", [](double a, double b) -> Result<double> { return pow(a, b); });
    return signature;
}

const auto& sqrtCompoundExpression() {
    static auto signature = detail::makeSignature("sqrt", [](double x) -> Result<double> { return sqrt(x); });
    return signature;
}

const auto& log10CompoundExpression() {
    static auto signature = detail::makeSignature("log10", [](double x) -> Result<double> { return log10(x); });
    return signature;
}

const auto& lnCompoundExpression() {
    static auto signature = detail::makeSignature("ln", [](double x) -> Result<double> { return log(x); });
    return signature;
}

const auto& log2CompoundExpression() {
    static auto signature = detail::makeSignature("log2", [](double x) -> Result<double> { return util::log2(x); });
    return signature;
}

const auto& sinCompoundExpression() {
    static auto signature = detail::makeSignature("sin", [](double x) -> Result<double> { return sin(x); });
    return signature;
}

const auto& cosCompoundExpression() {
    static auto signature = detail::makeSignature("cos", [](double x) -> Result<double> { return cos(x); });
    return signature;
}

const auto& tanCompoundExpression() {
    static auto signature = detail::makeSignature("tan", [](double x) -> Result<double> { return tan(x); });
    return signature;
}

const auto& asinCompoundExpression() {
    static auto signature = detail::makeSignature("asin", [](double x) -> Result<double> { return asin(x); });
    return signature;
}

const auto& acosCompoundExpression() {
    static auto signature = detail::makeSignature("acos", [](double x) -> Result<double> { return acos(x); });
    return signature;
}

const auto& atanCompoundExpression() {
    static auto signature = detail::makeSignature("atan", [](double x) -> Result<double> { return atan(x); });
    return signature;
}

const auto& minCompoundExpression() {
    static auto signature = detail::makeSignature("min", [](const Varargs<double>& args) -> Result<double> {
        double result = std::numeric_limits<double>::infinity();
        for (double arg : args) {
            result = fmin(arg, result);
        }
        return result;
    });
    return signature;
}

const auto& maxCompoundExpression() {
    static auto signature = detail::makeSignature("max", [](const Varargs<double>& args) -> Result<double> {
        double result = -std::numeric_limits<double>::infinity();
        for (double arg : args) {
            result = fmax(arg, result);
        }
        return result;
    });
    return signature;
}

const auto& roundCompoundExpression() {
    static auto signature = detail::makeSignature("round", [](double x) -> Result<double> { return ::round(x); });
    return signature;
}

const auto& floorCompoundExpression() {
    static auto signature = detail::makeSignature("floor", [](double x) -> Result<double> { return std::floor(x); });
    return signature;
}

const auto& ceilCompoundExpression() {
    static auto signature = detail::makeSignature("ceil", [](double x) -> Result<double> { return std::ceil(x); });
    return signature;
}

const auto& absCompoundExpression() {
    static auto signature = detail::makeSignature("abs", [](double x) -> Result<double> { return std::abs(x); });
    return signature;
}

const auto& notCompoundExpression() {
    static auto signature = detail::makeSignature("!", [](bool e) -> Result<bool> { return !e; });
    return signature;
}

const auto& isSupportedScriptCompoundExpression() {
    static auto signature = detail::makeSignature("is-supported-script", [](const std::string& x) -> Result<bool> {
        return util::i18n::isStringInSupportedScript(x);
    });
    return signature;
}

const auto& upcaseCompoundExpression() {
    static auto signature = detail::makeSignature("upcase", [](const std::string& input) -> Result<std::string> {
        return platform::uppercase(input);
    });
    return signature;
}

const auto& downcaseCompoundExpression() {
    static auto signature = detail::makeSignature("downcase", [](const std::string& input) -> Result<std::string> {
        return platform::lowercase(input);
    });
    return signature;
}

const auto& concatCompoundExpression() {
    static auto signature = detail::makeSignature("concat", [](const Varargs<Value>& args) -> Result<std::string> {
        std::string s;
        for (const Value& arg : args) {
            s += toString(arg);
        }
        return s;
    });
    return signature;
}

const auto& resolvedLocaleCompoundExpression() {
    static auto signature = detail::makeSignature("resolved-locale", [](const Collator& collator) -> Result<std::string> {
        return collator.resolvedLocale();
    });
    return signature;
}

const auto& errorCompoundExpression() {
    static auto signature = detail::makeSignature("error", [](const std::string& input) -> Result<type::ErrorType> {
        return EvaluationError { input };
    });
    return signature;
}

// Legacy Filters
const auto& filterEqualsCompoundExpression() {
    static auto signature = detail::makeSignature("filter-==", [](const EvaluationContext& params, const std::string& key, const Value &lhs) -> Result<bool> {
        const auto rhs = featurePropertyAsExpressionValue(params, key);
        return rhs ? lhs == *rhs : false;
    });
    return signature;
}

const auto& filterIdEqualsCompoundExpression() {
    static auto signature = detail::makeSignature("filter-id-==", [](const EvaluationContext& params, const Value &lhs) -> Result<bool> {
        return lhs == featureIdAsExpressionValue(params);
    });
    return signature;
}

const auto& filterTypeEqualsCompoundExpression() {
    static auto signature = detail::makeSignature("filter-type-==", [](const EvaluationContext& params, const std::string &lhs) -> Result<bool> {
        if (!params.feature) return false;
        return featureTypeAsString(params.feature->getType()) == lhs;
    });
    return signature;
}

const auto& filterLessThanNumberCompoundExpression() {
    static auto signature = detail::makeSignature("filter-<", [](const EvaluationContext& params, const std::string& key, double lhs) -> Result<bool> {
        auto rhs = featurePropertyAsDouble(params, key);
        return rhs ? rhs < lhs : false;
    });
    return signature;
}

const auto& filterLessThanStringCompoundExpression() {
    static auto signature = detail::makeSignature("filter-<", [](const EvaluationContext& params, const std::string& key, std::string lhs) -> Result<bool> {
        auto rhs = featurePropertyAsString(params, key);
        return rhs ? rhs < lhs : false;
    });
    return signature;
}

const auto& filterIdLessThanNumberCompoundExpression() {
    static auto signature = detail::makeSignature("filter-id-<", [](const EvaluationContext& params, double lhs) -> Result<bool> {
        auto rhs = featureIdAsDouble(params);
        return rhs ? rhs < lhs : false;
    });
    return signature;
}

const auto& filterIdLessThanStringCompoundExpression() {
    static auto signature = detail::makeSignature("filter-id-<", [](const EvaluationContext& params, std::string lhs) -> Result<bool> {
        auto rhs = featureIdAsString(params);
        return rhs ? rhs < lhs : false;
    });
    return signature;
}

const auto& filterMoreThanNumberCompoundExpression() {
    static auto signature = detail::makeSignature("filter->", [](const EvaluationContext& params, const std::string& key, double lhs) -> Result<bool> {
        auto rhs = featurePropertyAsDouble(params, key);
        return rhs ? rhs > lhs : false;
    });
    return signature;
}

const auto& filterMoreThanStringCompoundExpression() {
    static auto signature = detail::makeSignature("filter->", [](const EvaluationContext& params, const std::string& key, std::string lhs) -> Result<bool> {
        auto rhs = featurePropertyAsString(params, key);
        return rhs ? rhs > lhs : false;
    });
    return signature;
}

const auto& filterIdMoreThanNumberCompoundExpression() {
    static auto signature = detail::makeSignature("filter-id->", [](const EvaluationContext& params, double lhs) -> Result<bool> {
        auto rhs = featureIdAsDouble(params);
        return rhs ? rhs > lhs : false;
    });
    return signature;
}

const auto& filterIdMoreThanStringCompoundExpression() {
    static auto signature = detail::makeSignature("filter-id->", [](const EvaluationContext& params, std::string lhs) -> Result<bool> {
        auto rhs = featureIdAsString(params);
        return rhs ? rhs > lhs : false;
    });
    return signature;
}

const auto& filterLessOrEqualThanNumberCompoundExpression() {
    static auto signature = detail::makeSignature("filter-<=", [](const EvaluationContext& params, const std::string& key, double lhs) -> Result<bool> {
        auto rhs = featurePropertyAsDouble(params, key);
        return rhs ? rhs <= lhs : false;
    });
    return signature;
}

const auto& filterLessOrEqualThanStringCompoundExpression() {
    static auto signature = detail::makeSignature("filter-<=", [](const EvaluationContext& params, const std::string& key, std::string lhs) -> Result<bool> {
        auto rhs = featurePropertyAsString(params, key);
        return rhs ? rhs <= lhs : false;
    });
    return signature;
}

const auto& filterIdLessOrEqualThanNumberCompoundExpression() {
    static auto signature = detail::makeSignature("filter-id-<=", [](const EvaluationContext& params, double lhs) -> Result<bool> {
        auto rhs = featureIdAsDouble(params);
        return rhs ? rhs <= lhs : false;
    });
    return signature;
}

const auto& filterIdLessOrEqualThanStringCompoundExpression() {
    static auto signature = detail::makeSignature("filter-id-<=", [](const EvaluationContext& params, std::string lhs) -> Result<bool> {
        auto rhs = featureIdAsString(params);
        return rhs ? rhs <= lhs : false;
    });
    return signature;
}

const auto& filterGreaterOrEqualThanNumberCompoundExpression() {
    static auto signature = detail::makeSignature("filter->=", [](const EvaluationContext& params, const std::string& key, double lhs) -> Result<bool> {
        auto rhs = featurePropertyAsDouble(params, key);
        return rhs ? rhs >= lhs : false;
    });
    return signature;
}

const auto& filterGreaterOrEqualThanStringCompoundExpression() {
    static auto signature = detail::makeSignature("filter->=", [](const EvaluationContext& params, const std::string& key, std::string lhs) -> Result<bool> {
        auto rhs = featurePropertyAsString(params, key);
        return rhs ? rhs >= lhs : false;
    });
    return signature;
}

const auto& filterIdGreaterOrEqualThanNumberCompoundExpression() {
    static auto signature = detail::makeSignature("filter-id->=", [](const EvaluationContext& params, double lhs) -> Result<bool> {
        auto rhs = featureIdAsDouble(params);
        return rhs ? rhs >= lhs : false;
    });
    return signature;
}

const auto& filterIdGreaterOrEqualThanStringCompoundExpression() {
    static auto signature = detail::makeSignature("filter-id->=", [](const EvaluationContext& params, std::string lhs) -> Result<bool> {
        auto rhs = featureIdAsString(params);
        return rhs ? rhs >= lhs : false;
    });
    return signature;
}

const auto& filterHasCompoundExpression() {
    static auto signature = detail::makeSignature("filter-has", [](const EvaluationContext& params, const std::string& key) -> Result<bool> {
        assert(params.feature);
        return bool(params.feature->getValue(key));
    });
    return signature;
}

const auto& filterHasIdCompoundExpression() {
    static auto signature = detail::makeSignature("filter-has-id", [](const EvaluationContext& params) -> Result<bool> {
        assert(params.feature);
        return !params.feature->getID().is<NullValue>();
    });
    return signature;
}

const auto& filterTypeInCompoundExpression() {
    static auto signature = detail::makeSignature("filter-type-in", [](const EvaluationContext& params, const Varargs<std::string>& types) -> Result<bool> {
        assert(params.feature);
        optional<std::string> type = featureTypeAsString(params.feature->getType());
        return std::find(types.begin(), types.end(), type) != types.end();
    });
    return signature;
}

const auto& filterIdInCompoundExpression() {
    static auto signature = detail::makeSignature("filter-id-in", [](const EvaluationContext& params, const Varargs<Value>& ids) -> Result<bool> {
        auto id = featureIdAsExpressionValue(params);
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    });
    return signature;
}


const auto& filterInCompoundExpression() {
    static auto signature = detail::makeSignature("filter-in", [](const EvaluationContext& params, const Varargs<Value>& varargs) -> Result<bool> {
        if (varargs.size() < 2) return false;
        assert(varargs[0].is<std::string>());
        auto value = featurePropertyAsExpressionValue(params, varargs[0].get<std::string>());
        return value ? std::find(varargs.begin() + 1, varargs.end(), *value) != varargs.end() : false;
    });
    return signature;
}

using ParseCompoundFunction = const std::unique_ptr<detail::SignatureBase>& (*)();
MAPBOX_ETERNAL_CONSTEXPR const auto compoundExpressionRegistry = mapbox::eternal::hash_map<mapbox::eternal::string, ParseCompoundFunction>({
    { "e", eCompoundExpression },
    { "pi", piCompoundExpression },
    { "ln2", ln2CompoundExpression },
    { "typeof", typeofCompoundExpression },
    { "to-rgba", toRgbaCompoundExpression },
    { "rgba", rgbaCompoundExpression },
    { "rgb", rgbCompoundExpression },
    { "zoom", zoomCompoundExpression },
    { "heatmap-density", heatmapDensityCompoundExpression },
    { "line-progress", lineProgressCompoundExpression },
    { "accumulated", accumulatedCompoundExpression },
    { "has", hasContextCompoundExpression },
    { "has", hasObjectCompoundExpression },
    { "get", getContextCompoundExpression },
    { "get", getObjectCompoundExpression },
    { "properties", propertiesCompoundExpression },
    { "geometry-type", geometryTypeCompoundExpression },
    { "id", idCompoundExpression },
    { "+", plusCompoundExpression },
    { "-", minusCompoundExpression },
    { "-", negateCompoundExpression },
    { "*", multiplyCompoundExpression },
    { "/", divideCompoundExpression },
    { "%", modCompoundExpression },
    { "^", powCompoundExpression },
    { "sqrt", sqrtCompoundExpression },
    { "log10", log10CompoundExpression },
    { "ln", lnCompoundExpression },
    { "log2", log2CompoundExpression },
    { "sin", sinCompoundExpression },
    { "cos", cosCompoundExpression },
    { "tan", tanCompoundExpression },
    { "asin", asinCompoundExpression },
    { "acos", acosCompoundExpression },
    { "atan", atanCompoundExpression },
    { "min", minCompoundExpression },
    { "max", maxCompoundExpression },
    { "round", roundCompoundExpression },
    { "floor", floorCompoundExpression },
    { "ceil", ceilCompoundExpression },
    { "abs", absCompoundExpression },
    { "!", notCompoundExpression },
    { "is-supported-script", isSupportedScriptCompoundExpression },
    { "upcase", upcaseCompoundExpression },
    { "downcase", downcaseCompoundExpression },
    { "concat", concatCompoundExpression },
    { "resolved-locale", resolvedLocaleCompoundExpression },
    { "error", errorCompoundExpression },
    // Legacy Filters
    { "filter-==", filterEqualsCompoundExpression },
    { "filter-id-==", filterIdEqualsCompoundExpression },
    { "filter-type-==", filterTypeEqualsCompoundExpression },
    { "filter-<", filterLessThanNumberCompoundExpression },
    { "filter-<", filterLessThanStringCompoundExpression },
    { "filter-id-<", filterIdLessThanNumberCompoundExpression },
    { "filter-id-<", filterIdLessThanStringCompoundExpression },
    { "filter->", filterMoreThanNumberCompoundExpression },
    { "filter->", filterMoreThanStringCompoundExpression },
    { "filter-id->", filterIdMoreThanNumberCompoundExpression },
    { "filter-id->", filterIdMoreThanStringCompoundExpression },
    { "filter-<=", filterLessOrEqualThanNumberCompoundExpression },
    { "filter-<=", filterLessOrEqualThanStringCompoundExpression },
    { "filter-id-<=", filterIdLessOrEqualThanNumberCompoundExpression },
    { "filter-id-<=", filterIdLessOrEqualThanStringCompoundExpression },
    { "filter->=", filterGreaterOrEqualThanNumberCompoundExpression },
    { "filter->=", filterGreaterOrEqualThanStringCompoundExpression },
    { "filter-id->=", filterIdGreaterOrEqualThanNumberCompoundExpression },
    { "filter-id->=", filterIdGreaterOrEqualThanStringCompoundExpression },
    { "filter-has", filterHasCompoundExpression },
    { "filter-has-id", filterHasIdCompoundExpression },
    { "filter-type-in", filterTypeInCompoundExpression },
    { "filter-id-in", filterIdInCompoundExpression },
    { "filter-in", filterInCompoundExpression },
});

using namespace mbgl::style::conversion;

using DefinitionIterator = decltype(compoundExpressionRegistry)::const_iterator;
using Definitions = std::pair<DefinitionIterator, DefinitionIterator>;

std::string expectedTypesError(const Definitions& definitions,
                               const std::vector<std::unique_ptr<Expression>>& args) {
    std::vector<std::string> availableOverloads; // Only used if there are no overloads with matching number of args
    std::vector<std::string> overloads;
    for (auto it = definitions.first; it != definitions.second; ++it) {
        const auto& signature = it->second();
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
    
    return "Expected arguments of type " + signatures + ", but found (" + actualTypes + ") instead.";
}

static ParseResult createCompoundExpression(const Definitions& definitions,
                                            std::vector<std::unique_ptr<Expression>> args,
                                            ParsingContext& ctx)
{
    ParsingContext signatureContext(ctx.getKey());

    for (auto it = definitions.first; it != definitions.second; ++it) {
        const auto& signature = it->second();
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
            return ParseResult(std::make_unique<CompoundExpression>(*signature, std::move(args)));
        }
    }

    if (definitions.second - definitions.first == 1) {
        ctx.appendErrors(std::move(signatureContext));
    } else {
        ctx.error(expectedTypesError(definitions, args));
    }

    return ParseResult();
}

ParseResult parseCompoundExpression(const std::string name, const Convertible& value, ParsingContext& ctx) {
    assert(isArray(value) && arrayLength(value) > 0);

    const auto definitions = compoundExpressionRegistry.equal_range(name.c_str());
    if (definitions.first == definitions.second) {
        ctx.error(
             R"(Unknown expression ")" + name + R"(". If you wanted a literal array, use ["literal", [...]].)",
            0
        );
        return ParseResult();
    }

    auto length = arrayLength(value);
    
    for (auto it = definitions.first; it != definitions.second; ++it) {
        const auto& signature = it->second();

        if (
            signature->params.is<VarargsType>() ||
            signature->params.get<std::vector<type::Type>>().size() == length - 1
            ) {
            // First parse all the args, potentially coercing to the
            // types expected by this overload.
            ctx.clearErrors();
            bool argParseFailed = false;
            std::vector<std::unique_ptr<Expression>> args;
            args.reserve(length - 1);

            for (std::size_t i = 1; i < length; i++) {
                optional<type::Type> expected = signature->params.match(
                    [](const VarargsType& varargs) { return varargs.type; },
                    [&](const std::vector<type::Type>& params_) { return params_[i - 1]; }
                );

                auto parsed = ctx.parse(arrayMember(value, i), i, expected);
                if (!parsed) {
                    argParseFailed = true;
                    break;
                }
                args.push_back(std::move(*parsed));
            }
            if (argParseFailed) {
                // Couldn't coerce args of this overload to expected type, move
                // on to next one.
                continue;
            } else {
                ParseResult parseWithArgs = createCompoundExpression(definitions, std::move(args), ctx);
                if (parseWithArgs) {
                    return parseWithArgs;
                }
            }
        }
    }
    // The args couldn't be coerced to any of the expected types.
    // Parse the arguments again without expected types just for the error message
    ctx.clearErrors();
    std::vector<std::unique_ptr<Expression>> args;
    args.reserve(length - 1);
    
    for (std::size_t i = 1; i < length; i++) {
        auto parsed = ctx.parse(arrayMember(value, i), i);
        if (!parsed) {
            return ParseResult();
        }
        args.push_back(std::move(*parsed));
    }

    ctx.error(expectedTypesError(definitions, args));

    return ParseResult();
}

ParseResult createCompoundExpression(const std::string& name,
                                     std::vector<std::unique_ptr<Expression>> args,
                                     ParsingContext& ctx)
{
    return createCompoundExpression(compoundExpressionRegistry.equal_range(name.c_str()), std::move(args), ctx);
}

CompoundExpression::CompoundExpression(const detail::SignatureBase& signature_, std::vector<std::unique_ptr<Expression>> args_) :
    Expression(Kind::CompoundExpression, signature_.result),
    signature(signature_),
    args(std::move(args_))
{}

std::string CompoundExpression::getOperator() const {
    return signature.name;
}

EvaluationResult CompoundExpression::evaluate(const EvaluationContext& evaluationParams) const {
    return signature.apply(evaluationParams, args);
}

optional<std::size_t> CompoundExpression::getParameterCount() const {
    return signature.params.match(
        [&](const VarargsType&) { return optional<std::size_t>(); },
        [&](const std::vector<type::Type>& p) -> optional<std::size_t> { return p.size(); }
    );
}

std::vector<optional<Value>> CompoundExpression::possibleOutputs() const {
    return { nullopt };
}

void CompoundExpression::eachChild(const std::function<void(const Expression&)>& visit) const {
    for (const std::unique_ptr<Expression>& e : args) {
        visit(*e);
    }
}

bool CompoundExpression::operator==(const Expression& e) const {
    if (e.getKind() == Kind::CompoundExpression) {
        auto rhs = static_cast<const CompoundExpression*>(&e);
        return signature.name == rhs->signature.name &&
            signature.result == rhs->signature.result &&
            signature.params == rhs->signature.params &&
            Expression::childrenEqual(args, rhs->args);
    }
    return false;
}

bool CompoundExpression::exists(const std::string& name) {
    return compoundExpressionRegistry.contains(name.c_str());
}

} // namespace expression
} // namespace style
} // namespace mbgl
