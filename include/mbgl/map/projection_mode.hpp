#pragma once

#include <mbgl/util/optional.hpp>

#include <functional>

namespace mbgl {

/**
 * @brief Holds values for Axonometric rendering. All fields are
 * optional.
 */
struct ProjectionMode {
    ProjectionMode& withAxonometric(bool o) { axonometric = o; return *this; }
    ProjectionMode& withXSkew(double o) { xSkew = o; return *this; }
    ProjectionMode& withYSkew(double o) { ySkew = o; return *this; }

    /**
     * @brief Set to True to enable axonometric rendering, false otherwise.
     */
    optional<bool> axonometric;

    /**
     * @brief The X skew value represents how much to skew on the x-axis.
     */
    optional<double> xSkew;

    /**
     * @brief The Y skew value represents how much to skew on the y-axis.
     */
    optional<double> ySkew;
};

}  // namespace mbgl
