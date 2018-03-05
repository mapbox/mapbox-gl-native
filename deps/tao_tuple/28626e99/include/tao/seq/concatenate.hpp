// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_CONCATENATE_HPP
#define TAOCPP_SEQUENCES_INCLUDE_CONCATENATE_HPP

#include <type_traits>

#include "integer_sequence.hpp"

namespace tao
{
  namespace seq
  {
    template< typename, typename >
    struct concatenate;

    template< typename TA, TA... As, typename TB, TB... Bs >
    struct concatenate< integer_sequence< TA, As... >, integer_sequence< TB, Bs... > >
    {
      using type = integer_sequence< typename std::common_type< TA, TB >::type, As..., Bs... >;
    };

    template< typename A, typename B >
    using concatenate_t = typename concatenate< A, B >::type;
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_CONCATENATE_HPP
