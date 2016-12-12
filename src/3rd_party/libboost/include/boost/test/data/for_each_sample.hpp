//  (C) Copyright Gennadiy Rozental 2001.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
/// @file
/// Defines for_each_sample algorithm
// ***************************************************************************

#ifndef BOOST_TEST_DATA_FOR_EACH_SAMPLE_HPP_102211GER
#define BOOST_TEST_DATA_FOR_EACH_SAMPLE_HPP_102211GER

// Boost.Test
#include <boost/test/data/config.hpp>
#include <boost/test/data/traits.hpp>
#include <boost/test/data/size.hpp>
#include <boost/test/data/monomorphic/fwd.hpp>

#include <boost/test/detail/suppress_warnings.hpp>

// needed for std::min
#include <algorithm>

//____________________________________________________________________________//

namespace boost {
namespace unit_test {
namespace data {

// ************************************************************************** //
// **************                for_each_sample               ************** //
// ************************************************************************** //

template<typename DataSet, typename Action>
inline typename std::enable_if<monomorphic::is_dataset<DataSet>::value,void>::type
for_each_sample( DataSet const&     samples,
                 Action const&      act,
                 data::size_t       number_of_samples = BOOST_TEST_DS_INFINITE_SIZE )
{
    data::size_t size = (std::min)( samples.size(), number_of_samples );
    BOOST_TEST_DS_ASSERT( !size.is_inf(), "Dataset has infinite size. Please specify the number of samples" );

    auto it = samples.begin();

    while( size-- > 0 ) {
        data::traits<typename DataSet::sample>::invoke_action( *it, act );
        ++it;
    }
}

//____________________________________________________________________________//

template<typename DataSet, typename Action>
inline typename std::enable_if<!monomorphic::is_dataset<DataSet>::value,void>::type
for_each_sample( DataSet const&     samples,
                 Action const&      act,
                 data::size_t       number_of_samples = BOOST_TEST_DS_INFINITE_SIZE )
{
    data::for_each_sample( data::make( samples ), act, number_of_samples );
}

} // namespace data
} // namespace unit_test
} // namespace boost

#include <boost/test/detail/enable_warnings.hpp>

#endif // BOOST_TEST_DATA_FOR_EACH_SAMPLE_HPP_102211GER

