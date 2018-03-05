// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_PARTIAL_SUM_HPP
#define TAOCPP_SEQUENCES_INCLUDE_PARTIAL_SUM_HPP

#include <cstddef>
#include <utility>

#include "make_integer_sequence.hpp"
#include "sum.hpp"

namespace tao
{
  namespace seq
  {
    namespace impl
    {
      template< std::size_t, typename S, typename = make_index_sequence< S::size() > >
      struct partial_sum;

      template< std::size_t I, typename T, T... Ns, std::size_t... Is >
      struct partial_sum< I, integer_sequence< T, Ns... >, index_sequence< Is... > >
        : seq::sum< T, ( ( Is < I ) ? Ns : 0 )... >
      {
        static_assert( I <= sizeof...( Is ), "tao::seq::partial_sum<I, S>: I is out of range" );
      };
    }

    template< std::size_t I, typename T, T... Ns >
    struct partial_sum
      : impl::partial_sum< I, integer_sequence< T, Ns... > >
    {};

    template< std::size_t I, typename T, T... Ns >
    struct partial_sum< I, integer_sequence< T, Ns... > >
      : impl::partial_sum< I, integer_sequence< T, Ns... > >
    {};
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_PARTIAL_SUM_HPP
