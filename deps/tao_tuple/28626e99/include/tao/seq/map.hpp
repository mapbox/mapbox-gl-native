// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_MAP_HPP
#define TAOCPP_SEQUENCES_INCLUDE_MAP_HPP

#include <cstddef>
#include <utility>

#include "integer_sequence.hpp"
#include "select.hpp"

namespace tao
{
  namespace seq
  {
    template< typename, typename >
    struct map;

    template< std::size_t... Ns, typename M >
    struct map< index_sequence< Ns... >, M >
    {
      using type = integer_sequence< typename M::value_type, select< Ns, M >::value... >;
    };

    template< typename S, typename M >
    using map_t = typename map< S, M >::type;
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_MAP_HPP
