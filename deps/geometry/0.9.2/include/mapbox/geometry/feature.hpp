#pragma once

#include <mapbox/geometry/geometry.hpp>

#include <mapbox/variant.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <mbgl/util/optional.hpp>

namespace mapbox {
namespace geometry {

struct value;

struct null_value_t
{
    constexpr null_value_t() {}
    constexpr null_value_t(std::nullptr_t) {}
    constexpr null_value_t(const null_value_t&) {}
    constexpr null_value_t(null_value_t&&) {}
};

constexpr bool operator==(const null_value_t&, const null_value_t&) { return true; }
constexpr bool operator!=(const null_value_t&, const null_value_t&) { return false; }
constexpr bool operator<(const null_value_t&, const null_value_t&) { return false; }

constexpr null_value_t null_value = null_value_t();

// Multiple numeric types (uint64_t, int64_t, double) are present in order to support
// the widest possible range of JSON numbers, which do not have a maximum range.
// Implementations that produce `value`s should use that order for type preference,
// using uint64_t for positive integers, int64_t for negative integers, and double
// for non-integers and integers outside the range of 64 bits.
/*
using value_base = mapbox::util::variant<null_value_t, bool, uint64_t, int64_t, double, std::string,
                                         mapbox::util::recursive_wrapper<std::vector<value>>,
                                         mapbox::util::recursive_wrapper<std::unordered_map<std::string, value>>>;
*/
typedef mapbox::util::variant<null_value_t, bool, uint64_t, int64_t, double, std::string,
                                         mapbox::util::recursive_wrapper<std::vector<value>>,
                                         mapbox::util::recursive_wrapper<std::unordered_map<std::string, value>>> value_base;
struct value : value_base
{
    using value_base::value_base;
    //typedef value_base base_t;
    //using base_t::base_t;

      value()
      : value_base()
      { }

      //pjmj
      value(null_value_t args)
      : value_base(std::move(args))
      { }
      value(bool&& args)
      : value_base(std::forward<bool>(args))
      { }
      value(uint64_t&& args)
      : value_base(std::forward<uint64_t>(args))
      { }
      value(int64_t&& args)
      : value_base(std::forward<int64_t>(args))
      { }
      value(double&& args)
      : value_base(std::forward<double>(args))
      { }
      value(std::string&& args)
      : value_base(std::move(args))
      { }
      value(std::vector<value>&& args)
      : value_base(mapbox::util::recursive_wrapper<std::vector<value>>(args))
      { }
      typedef mapbox::util::recursive_wrapper<std::unordered_map<std::string, value>> rec_map;
      value(std::unordered_map<std::string, value>&& args)
      : value_base(rec_map(args))
      { }

      value(const bool& args)
      : value_base(args)
      { }
      value(const uint64_t& args)
         : value_base(args)
      { }
      value(const int64_t& args)
         : value_base(args)
      { }
      value(const double& args)
         : value_base(args)
      { }
      value(const std::string& args)
         : value_base(args)
      { }
      value(const std::vector<value>& args)
         : value_base(mapbox::util::recursive_wrapper<std::vector<value>>(args))
      { }
      value(const std::unordered_map<std::string, value>& args)
         : value_base(rec_map(args))
      { }

};

using property_map = std::unordered_map<std::string, value>;

// The same considerations and requirement for numeric types apply as for `value_base`.
using identifier = mapbox::util::variant<uint64_t, int64_t, double, std::string>;

template <class T>
struct feature
{
    using coordinate_type = T;
    using geometry_type = mapbox::geometry::geometry<T>; // Fully qualified to avoid GCC -fpermissive error.

    geometry_type geometry;
    property_map properties {};
    mbgl::optional<identifier> id {};

    // GCC 4.9 does not support C++14 aggregates with non-static data member
    // initializers.
    feature(geometry_type geometry_,
            property_map properties_ = property_map {},
            mbgl::optional<identifier> id_ = {})
//    mbgl::optional<identifier> id_ = mbgl::optional<identifier> {})
        : geometry(std::move(geometry_)),
          properties(std::move(properties_)),
          id(std::move(id_)) {}
};

template <class T>
constexpr bool operator==(feature<T> const& lhs, feature<T> const& rhs)
{
    return lhs.id == rhs.id && lhs.geometry == rhs.geometry && lhs.properties == rhs.properties;
}

template <class T>
constexpr bool operator!=(feature<T> const& lhs, feature<T> const& rhs)
{
    return !(lhs == rhs);
}

template <class T, template <typename...> class Cont = std::vector>
struct feature_collection : Cont<feature<T>>
{
    using coordinate_type = T;
    using feature_type = feature<T>;
    using container_type = Cont<feature_type>;
    using container_type::container_type;
};

} // namespace geometry
} // namespace mapbox
