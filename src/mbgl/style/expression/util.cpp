#include <mbgl/style/expression/util.hpp>
#include <mbgl/style/expression/value.hpp>

namespace mbgl {
namespace style {
namespace expression {

std::string stringifyColor(double r, double g, double b, double a) {
    return stringify(r) + ", " +
        stringify(g) + ", " +
        stringify(b) + ", " +
        stringify(a);
}

Result<Color> rgba(double r, double g, double b, double a) {
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
    return Color(r / 255 * a, g / 255 * a, b / 255 * a, a);
}

} // namespace expression
} // namespace style
} // namespace mbgl
