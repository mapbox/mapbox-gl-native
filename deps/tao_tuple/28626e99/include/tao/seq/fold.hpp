// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_FOLD_HPP
#define TAOCPP_SEQUENCES_INCLUDE_FOLD_HPP

#include <type_traits>

#include "values.hpp"
#include "integer_sequence.hpp"
#include "make_integer_sequence.hpp"

namespace tao
{
  namespace seq
  {
    namespace impl
    {
      template< template< typename U, U, U > class, typename, bool, typename T, T... >
      struct folder;

      template< template< typename U, U, U > class OP, std::size_t... Is, typename T, T... Ns >
      struct folder< OP, index_sequence< Is... >, false, T, Ns... >
      {
        using values = seq::values< T, Ns... >;
        using type = integer_sequence< T, OP< T, values::data[ 2 * Is ], values::data[ 2 * Is + 1 ] >::value... >;
      };

      template< template< typename U, U, U > class OP, std::size_t... Is, typename T, T N, T... Ns >
      struct folder< OP, index_sequence< Is... >, true, T, N, Ns... >
      {
        using values = seq::values< T, Ns... >;
        using type = integer_sequence< T, N, OP< T, values::data[ 2 * Is ], values::data[ 2 * Is + 1 ] >::value... >;
      };
    }

    template< template< typename U, U, U > class, typename T, T... >
    struct fold;

    template< template< typename U, U, U > class OP, typename T, T N >
    struct fold< OP, T, N >
      : std::integral_constant< T, N >
    {};

    template< template< typename U, U, U > class OP, typename T, T... Ns >
    struct fold
      : fold< OP, typename impl::folder< OP, make_index_sequence< sizeof...( Ns ) / 2 >, sizeof...( Ns ) % 2 == 1, T, Ns... >::type >
    {};

    template< template< typename U, U, U > class OP, typename T, T... Ns >
    struct fold< OP, integer_sequence< T, Ns... > >
      : fold< OP, T, Ns... >
    {};
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_FOLD_HPP
