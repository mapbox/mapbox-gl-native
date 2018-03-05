// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_MINUS_HPP
#define TAOCPP_SEQUENCES_INCLUDE_MINUS_HPP

#include <type_traits>

#include "zip.hpp"

namespace tao
{
  namespace seq
  {
    namespace impl
    {
      template< typename T, T A, T B >
      using minus = std::integral_constant< T, A - B >;
    }

    template< typename A, typename B >
    using minus = zip< impl::minus, A, B >;

    template< typename A, typename B >
    using minus_t = typename minus< A, B >::type;
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_MINUS_HPP
