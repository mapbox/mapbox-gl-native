// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_SELECT_HPP
#define TAOCPP_SEQUENCES_INCLUDE_SELECT_HPP

#include <cstddef>
#include <type_traits>

#include "integer_sequence.hpp"
#include "values.hpp"

namespace tao
{
  namespace seq
  {
    template< std::size_t I, typename T, T... Ns >
    struct select
      : std::integral_constant< T, values< T, Ns... >::data[ I ] >
    {};

    template< std::size_t I, typename T, T... Ns >
    struct select< I, integer_sequence< T, Ns... > >
      : select< I, T, Ns... >
    {};
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_SELECT_HPP
