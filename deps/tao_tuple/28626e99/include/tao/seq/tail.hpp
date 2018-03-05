// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_TAIL_HPP
#define TAOCPP_SEQUENCES_INCLUDE_TAIL_HPP

#include <type_traits>

#include "integer_sequence.hpp"

namespace tao
{
  namespace seq
  {
    template< typename T, T... Ns >
    struct tail;

    template< typename T, T N, T... Ns >
    struct tail< T, N, Ns... >
    {
      using type = integer_sequence< T, Ns... >;
    };

    template< typename T, T... Ns >
    struct tail< integer_sequence< T, Ns... > >
      : tail< T, Ns... >
    {};

    template< typename T, T... Ns >
    using tail_t = typename tail< T, Ns... >::type;
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_TAIL_HPP
