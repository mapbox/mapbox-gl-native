// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_VALUES_HPP
#define TAOCPP_SEQUENCES_INCLUDE_VALUES_HPP

namespace tao
{
  namespace seq
  {
    template< typename T, T... Ns >
    struct values
    {
      static constexpr T data[] = { Ns... };
    };
  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_VALUES_HPP
