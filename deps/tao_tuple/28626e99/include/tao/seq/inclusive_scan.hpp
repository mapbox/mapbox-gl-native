// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_INCLUSIVE_SCAN_HPP
#define TAOCPP_SEQUENCES_INCLUDE_INCLUSIVE_SCAN_HPP

#include <utility>

#include "plus.hpp"
#include "exclusive_scan.hpp"
#include "integer_sequence.hpp"

namespace tao
{
  namespace seq
  {
    template< typename T, T... Ns >
    struct inclusive_scan
      : plus< exclusive_scan_t< T, Ns... >, integer_sequence< T, Ns... > >
    {};

    template< typename T, T... Ns >
    struct inclusive_scan< integer_sequence< T, Ns... > >
      : plus< exclusive_scan_t< integer_sequence< T, Ns... > >, integer_sequence< T, Ns... > >
    {};

    template< typename T, T... Ns >
    using inclusive_scan_t = typename inclusive_scan< T, Ns... >::type;
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_INCLUSIVE_SCAN_HPP
