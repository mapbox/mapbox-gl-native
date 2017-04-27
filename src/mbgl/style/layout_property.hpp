#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>
#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/renderer/data_driven_property_evaluator.hpp>
#include <mbgl/util/indexed_tuple.hpp>

namespace mbgl {

class PropertyEvaluationParameters;

namespace style {

template <class T>
class LayoutProperty {
public:
    using UnevaluatedType = PropertyValue<T>;
    using EvaluatorType = PropertyEvaluator<T>;
    using PossiblyEvaluatedType = T;
    using Type = T;
};

template <class T>
class DataDrivenLayoutProperty {
public:
    using UnevaluatedType = DataDrivenPropertyValue<T>;
    using EvaluatorType = DataDrivenPropertyEvaluator<T>;
    using PossiblyEvaluatedType = PossiblyEvaluatedPropertyValue<T>;
    using Type = T;
};

template <class... Ps>
class LayoutProperties {
public:
    using Properties = TypeList<Ps...>;

    template <class TypeList>
    using Tuple = IndexedTuple<Properties, TypeList>;

    /*
        For layout properties we implement a two step evaluation process: if you have a zoom level,
        you can evaluate a set of unevaluated property values, producing a set of possibly evaluated
        values, where undefined, constant, or camera function values have been fully evaluated, and
        source or composite function values have not.

        Once you also have a particular feature, you can evaluate that set of possibly evaluated values
        fully, producing a set of fully evaluated values.

        This is in theory maximally efficient in terms of avoiding repeated evaluation of camera
        functions, though it's more of a historical accident than a purposeful optimization.
    */

    using       UnevaluatedTypes = TypeList<typename Ps::UnevaluatedType...>;
    using PossiblyEvaluatedTypes = TypeList<typename Ps::PossiblyEvaluatedType...>;
    using         EvaluatedTypes = TypeList<typename Ps::Type...>;

    class Evaluated : public Tuple<EvaluatedTypes> {
    public:
        using Tuple<EvaluatedTypes>::Tuple;
    };

    class PossiblyEvaluated : public Tuple<PossiblyEvaluatedTypes> {
    public:
        using Tuple<PossiblyEvaluatedTypes>::Tuple;

        template <class T>
        static T evaluate(float, const GeometryTileFeature&, const T& t, const T&) {
            return t;
        }

        template <class T>
        static T evaluate(float z, const GeometryTileFeature& feature,
                          const PossiblyEvaluatedPropertyValue<T>& v, const T& defaultValue) {
            return v.match(
                [&] (const T& t) {
                    return t;
                },
                [&] (const SourceFunction<T>& t) {
                    return t.evaluate(feature, defaultValue);
                },
                [&] (const CompositeFunction<T>& t) {
                    return t.evaluate(z, feature, defaultValue);
                });
        }

        template <class P>
        auto evaluate(float z, const GeometryTileFeature& feature) const {
            return evaluate(z, feature, this->template get<P>(), P::defaultValue());
        }

        Evaluated evaluate(float z, const GeometryTileFeature& feature) const {
            return Evaluated {
                evaluate<Ps>(z, feature)...
            };
        }
    };

    class Unevaluated : public Tuple<UnevaluatedTypes> {
    public:
        using Tuple<UnevaluatedTypes>::Tuple;
    };

    template <class P>
    auto evaluate(const PropertyEvaluationParameters& parameters) const {
        using Evaluator = typename P::EvaluatorType;
        return unevaluated.template get<P>()
            .evaluate(Evaluator(parameters, P::defaultValue()));
    }

    PossiblyEvaluated evaluate(const PropertyEvaluationParameters& parameters) const {
        return PossiblyEvaluated {
            evaluate<Ps>(parameters)...
        };
    }

    Unevaluated unevaluated;
};

} // namespace style
} // namespace mbgl
