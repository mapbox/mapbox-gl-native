#pragma once

#include <mbgl/util/optional.hpp>

#include <mapbox/geometry/feature.hpp>

namespace mbgl {

using Value = mapbox::geometry::value;

class Feature : public mapbox::geometry::feature<double> {
public:
    Feature(geometry_type&& geometry_)
        : mapbox::geometry::feature<double> { std::move(geometry_) } {}

    optional<uint64_t> id;
};

} // namespace mbgl
