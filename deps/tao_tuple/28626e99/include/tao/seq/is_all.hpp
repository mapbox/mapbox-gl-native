// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_IS_ALL_HPP
#define TAOCPP_SEQUENCES_INCLUDE_IS_ALL_HPP

#include "config.hpp"

#ifndef TAOCPP_FOLD_EXPRESSIONS
#include "integer_sequence.hpp"
#endif

#include <type_traits>

namespace tao
{
  namespace seq
  {

#ifdef TAOCPP_FOLD_EXPRESSIONS

    template< bool... Bs >
    using is_all = std::integral_constant< bool, ( Bs && ... ) >;

#else

    template< bool... Bs >
    using is_all = std::integral_constant< bool, std::is_same< integer_sequence< bool, true, Bs... >, integer_sequence< bool, Bs..., true > >::value >;

#endif

  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_IS_ALL_HPP
