#pragma once


#ifdef ORIGINAL

namespace mbgl {

template <typename T>
using optional = std::experimental::optional<T>;

using nullopt_t = std::experimental::nullopt_t;
constexpr nullopt_t nullopt = std::experimental::nullopt;

} // namespace mbgl

#else // use boost

#include <boost/optional.hpp>

namespace mbgl {

template <typename T>
using optional = boost::optional<T>;

const boost::none_t nullopt{boost::none_t::init_tag{}};


} // namespace mbgl

#endif
