#pragma once

// Polyfill needed by Windows because MSVC STL
// is not compatible with our IndexedTuple code
#if defined(_WINDOWS)

#include <tao/tuple/tuple.hpp>

#define get_polyfill tao::get
#define tuple_polyfill tao::tuple

#else

#include <tuple>

#define get_polyfill std::get
#define tuple_polyfill std::tuple

#endif
