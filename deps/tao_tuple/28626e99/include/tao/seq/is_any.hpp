// The Art of C++ / Sequences
// Copyright (c) 2015 Daniel Frey

#ifndef TAOCPP_SEQUENCES_INCLUDE_IS_ANY_HPP
#define TAOCPP_SEQUENCES_INCLUDE_IS_ANY_HPP

#include "config.hpp"

#ifndef TAOCPP_FOLD_EXPRESSIONS
#include "is_all.hpp"
#endif

#include <type_traits>

namespace tao
{
  namespace seq
  {

#ifdef TAOCPP_FOLD_EXPRESSIONS

    template< bool... Bs >
    using is_any = std::integral_constant< bool, ( Bs || ... ) >;

#else

    template< bool... Bs >
    using is_any = std::integral_constant< bool, !is_all< !Bs... >::value >;

#endif

  }
}

#endif // TAOCPP_SEQUENCES_INCLUDE_IS_ANY_HPP
