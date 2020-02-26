#pragma once

#include <mbgl/util/geo.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

/**
 * @brief Holds options to limit what parts of a map are visible. All fields are
 * optional.
 */
struct BoundOptions {
    /// Sets the latitude and longitude bounds to which the camera center are constrained
    BoundOptions& withLatLngBounds(LatLngBounds b) { bounds = b; return *this; }
    /// Sets the minimum zoom level
    BoundOptions& withMinZoom(double z) { minZoom = z; return *this; }
    /// Sets the maximum zoom level
    BoundOptions& withMaxZoom(double z) { maxZoom = z; return *this; }
    /// Sets the minimum pitch
    BoundOptions& withMinPitch(double p) {
        minPitch = p;
        return *this;
    }
    /// Sets the maximum pitch
    BoundOptions& withMaxPitch(double p) {
        maxPitch = p;
        return *this;
    }

    /// Constrain the center of the camera to be within these bounds.
    optional<LatLngBounds> bounds;

    /// Maximum zoom level allowed.
    optional<double> maxZoom;

    /// Minimum zoom level allowed.
    optional<double> minZoom;

    /// Maximum pitch allowed in degrees.
    optional<double> maxPitch;

    /// Minimum pitch allowed in degrees.
    optional<double> minPitch;
};

}  // namespace mbgl
