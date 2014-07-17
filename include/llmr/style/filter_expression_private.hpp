#ifndef LLMR_STYLE_FILTER_EXPRESSION_PRIVATE
#define LLMR_STYLE_FILTER_EXPRESSION_PRIVATE

#include "filter_expression.hpp"
#include "filter_comparison_private.hpp"

namespace llmr {

template <typename Extractor>
bool FilterExpression::compare(const Extractor &extractor) const {
    if (type != GeometryType::Any && extractor.getType() != type && extractor.getType() != GeometryType::Any) {
        return false;
    }

    switch (op) {
    case Operator::And:
        for (const FilterComparison &comparison : comparisons) {
            if (!comparison.compare(extractor)) {
                return false;
            }
        }
        for (const FilterExpression &expression: expressions) {
            if (!expression.compare(extractor)) {
                return false;
            }
        }
        return true;
    case Operator::Or:
        for (const FilterComparison &comparison : comparisons) {
            if (comparison.compare(extractor)) {
                return true;
            }
        }
        for (const FilterExpression &expression: expressions) {
            if (expression.compare(extractor)) {
                return true;
            }
        }
        return false;
    case Operator::Xor: {
        int count = 0;
        for (const FilterComparison &comparison : comparisons) {
            count += comparison.compare(extractor);
            if (count > 1) {
                return false;
            }
        }
        for (const FilterExpression &expression: expressions) {
            count += expression.compare(extractor);
            if (count > 1) {
                return false;
            }
        }
        return count == 1;
    }
    case Operator::Nor:
        for (const FilterComparison &comparison : comparisons) {
            if (comparison.compare(extractor)) {
                return false;
            }
        }
        for (const FilterExpression &expression: expressions) {
            if (expression.compare(extractor)) {
                return false;
            }
        }
        return true;
    default:
        return true;
    }
}

}

#endif
