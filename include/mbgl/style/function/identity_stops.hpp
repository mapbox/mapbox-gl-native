#pragma once

#include <mbgl/util/feature.hpp>

namespace mbgl {
namespace style {

template <class T>
class IdentityStops {
public:
    optional<T> evaluate(const Value&) const;

    friend bool operator==(const IdentityStops&,
                           const IdentityStops&) {
        return true;
    }
};

} // namespace style
} // namespace mbgl
