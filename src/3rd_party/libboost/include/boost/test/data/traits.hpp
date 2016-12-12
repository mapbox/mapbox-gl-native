//  (C) Copyright Gennadiy Rozental 2001.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
/// @file
/// Defines helper traits
// ***************************************************************************

#ifndef BOOST_TEST_DATA_TRAITS_HPP_102211GER
#define BOOST_TEST_DATA_TRAITS_HPP_102211GER

// Boost.Test
#include <boost/test/data/config.hpp>
#include <boost/test/data/monomorphic/fwd.hpp>

// STL
#include <tuple>

#include <boost/test/detail/suppress_warnings.hpp>

//____________________________________________________________________________//

namespace boost {
namespace unit_test {
namespace data {

// ************************************************************************** //
// **************                  data::traits                ************** //
// ************************************************************************** //

template<typename T>
struct traits {
    // type of the reference to sample returned by iterator
    typedef T const&    ref_type;

    template<typename Action>
    static void
    invoke_action( ref_type arg, Action const& action )
    {
        action( arg );
    }
};

//____________________________________________________________________________//

// !! ?? reimplement using variadics
template<typename T1, typename T2>
struct traits<std::tuple<T1,T2>> {
    // type of the reference to sample returned by iterator
    typedef std::tuple<T1 const&,T2 const&>     ref_type;

    template<typename Action>
    static void
    invoke_action( ref_type arg, Action const& action )
    {
        action( std::get<0>(arg), std::get<1>(arg) );
    }
};

//____________________________________________________________________________//

template<typename T1, typename T2, typename T3>
struct traits<std::tuple<T1,T2,T3>> {
    // type of the reference to sample returned by iterator
    typedef std::tuple<T1 const&,T2 const&,T3 const&>   ref_type;

    template<typename Action>
    static void
    invoke_action( ref_type arg, Action const& action )
    {
        action( std::get<0>(arg), std::get<1>(arg), std::get<2>(arg) );
    }
};

//____________________________________________________________________________//

} // namespace data
} // namespace unit_test
} // namespace boost

#include <boost/test/detail/enable_warnings.hpp>

#endif // BOOST_TEST_DATA_TRAITS_HPP_102211GER

