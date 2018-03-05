// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_MIN_HPP
#define TAOCPP_SEQUENCES_INCLUDE_MIN_HPP

#include "fold.hpp"

#include <type_traits>

namespace tao
{
  namespace seq
  {
    namespace impl
    {
      template< typename T, T A, T B >
      using min = std::integral_constant< T, ( ( A < B ) ? A : B ) >;
    }

    template< typename T, T... Ns >
    struct min
      : fold< impl::min, T, Ns... >
    {};

    template< typename T, T... Ns >
    struct min< integer_sequence< T, Ns... > >
      : min< T, Ns... >
    {};
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_MIN_HPP
