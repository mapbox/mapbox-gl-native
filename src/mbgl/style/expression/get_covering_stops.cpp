#include <mbgl/style/expression/get_covering_stops.hpp>

namespace mbgl {
namespace style {
namespace expression {

Range<float> getCoveringStops(const std::map<double, std::unique_ptr<Expression>>& stops,
                              const double lower, const double upper) {
    assert(!stops.empty());
    auto minIt = stops.lower_bound(lower);
    auto maxIt = stops.lower_bound(upper);

    // lower_bound yields first element >= lowerZoom, but we want the *last*
    // element <= lowerZoom, so if we found a stop > lowerZoom, back up by one.
    if (minIt != stops.begin() && minIt != stops.end() && minIt->first > lower) {
        minIt--;
    }
    return Range<float> {
        static_cast<float>(minIt == stops.end() ? stops.rbegin()->first : minIt->first),
        static_cast<float>(maxIt == stops.end() ? stops.rbegin()->first : maxIt->first)
    };
}

} // namespace expression
} // namespace style
} // namespace mbgl
