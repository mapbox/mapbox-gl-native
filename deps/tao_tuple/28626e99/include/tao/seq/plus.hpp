// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_PLUS_HPP
#define TAOCPP_SEQUENCES_INCLUDE_PLUS_HPP

#include <type_traits>

#include "zip.hpp"

namespace tao
{
  namespace seq
  {
    namespace impl
    {
      template< typename T, T A, T B >
      using plus = std::integral_constant< T, A + B >;
    }

    template< typename A, typename B >
    using plus = zip< impl::plus, A, B >;

    template< typename A, typename B >
    using plus_t = typename plus< A, B >::type;
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_PLUS_HPP
