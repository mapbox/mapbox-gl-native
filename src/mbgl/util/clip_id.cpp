#include <mbgl/util/clip_id.hpp>

#include <iostream>

namespace mbgl {

::std::ostream& operator<<(::std::ostream& os, const ClipID& rhs) {
    return os << "mask=" << rhs.mask << ",ref=" << rhs.reference;
}

} // namespace mbgl
