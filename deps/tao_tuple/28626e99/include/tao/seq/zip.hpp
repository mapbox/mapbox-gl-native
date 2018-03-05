// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_ZIP_HPP
#define TAOCPP_SEQUENCES_INCLUDE_ZIP_HPP

#include <type_traits>

#include "integer_sequence.hpp"

namespace tao
{
  namespace seq
  {
    template< template< typename U, U, U > class, typename, typename >
    struct zip;

    template< template< typename U, U, U > class OP, typename TA, TA... As, typename TB, TB... Bs >
    struct zip< OP, integer_sequence< TA, As... >, integer_sequence< TB, Bs... > >
    {
      using CT = typename std::common_type< TA, TB >::type;
      using type = integer_sequence< CT, OP< CT, As, Bs >::value... >;
    };

    template< template< typename U, U, U > class OP, typename A, typename B >
    using zip_t = typename zip< OP, A, B >::type;
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_ZIP_HPP
