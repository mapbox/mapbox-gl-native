// This version targets C++11 and later.
//
// Copyright (C) 2016-2018 Martin Moene.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// expected lite is based on:
//   A proposal to add a utility class to represent expected monad
//   by Vicente J. Botet Escriba and Pierre Talbot. http:://wg21.link/p0323

#ifndef NONSTD_EXPECTED_LITE_HPP
#define NONSTD_EXPECTED_LITE_HPP

#define expected_lite_MAJOR  0
#define expected_lite_MINOR  1
#define expected_lite_PATCH  0

#define expected_lite_VERSION  expected_STRINGIFY(expected_lite_MAJOR) "." expected_STRINGIFY(expected_lite_MINOR) "." expected_STRINGIFY(expected_lite_PATCH)

#define expected_STRINGIFY(  x )  expected_STRINGIFY_( x )
#define expected_STRINGIFY_( x )  #x

// expected-lite configuration:

#define nsel_EXPECTED_DEFAULT  0
#define nsel_EXPECTED_NONSTD   1
#define nsel_EXPECTED_STD      2

#if !defined( nsel_CONFIG_SELECT_EXPECTED )
# define nsel_CONFIG_SELECT_EXPECTED  ( nsel_HAVE_STD_EXPECTED ? nsel_EXPECTED_STD : nsel_EXPECTED_NONSTD )
#endif

// Proposal revisions:
//
// DXXXXR0: --
// N4015  : -2 (2014-05-26)
// N4109  : -1 (2014-06-29)
// P0323R0:  0 (2016-05-28)
// P0323R1:  1 (2016-10-12)
// -------:
// P0323R2:  2 (2017-06-15)
// P0323R3:  3 (2017-10-15)
// P0323R4:  4 (2017-11-26)
// P0323R5:  5 (2018-02-08)
//
// expected-lite uses 2 and higher

#ifndef  nsel_P0323R
# define nsel_P0323R  5
#endif

// C++ language version detection (C++20 is speculative):
// Note: VC14.0/1900 (VS2015) lacks too much from C++14.

#ifndef   nsel_CPLUSPLUS
# ifdef  _MSVC_LANG
#  define nsel_CPLUSPLUS  (_MSC_VER == 1900 ? 201103L : _MSVC_LANG )
# else
#  define nsel_CPLUSPLUS  __cplusplus
# endif
#endif

#define nsel_CPP98_OR_GREATER  ( nsel_CPLUSPLUS >= 199711L )
#define nsel_CPP11_OR_GREATER  ( nsel_CPLUSPLUS >= 201103L )
#define nsel_CPP14_OR_GREATER  ( nsel_CPLUSPLUS >= 201402L )
#define nsel_CPP17_OR_GREATER  ( nsel_CPLUSPLUS >= 201703L )
#define nsel_CPP20_OR_GREATER  ( nsel_CPLUSPLUS >= 202000L )

// Use C++20 std::expected if available and requested:

#if nsel_CPP20_OR_GREATER && defined(__has_include )
# if __has_include( <expected> )
#  define nsel_HAVE_STD_EXPECTED  1
# else
#  define nsel_HAVE_STD_EXPECTED  0
# endif
#else
# define  nsel_HAVE_STD_EXPECTED  0
#endif

#define  nsel_USES_STD_EXPECTED  ( (nsel_CONFIG_SELECT_EXPECTED == nsel_EXPECTED_STD) || ((nsel_CONFIG_SELECT_EXPECTED == nsel_EXPECTED_DEFAULT) && nsel_HAVE_STD_EXPECTED) )

// Using std::expected:

#if nsel_USES_STD_EXPECTED

#include <expected>

namespace nonstd {

    using std::expected;
//  ...

    using std::in_place;
    using std::in_place_t;
}

#else // nsel_USES_STD_EXPECTED

#include <cassert>
#include <exception>
#include <functional>
#include <initializer_list>
#include <new>
#include <stdexcept>
#include <system_error>
#include <type_traits>
#include <utility>

#if nsel_CPP11_OR_GREATER
# define nsel_constexpr  constexpr
#else
# define nsel_constexpr  /*constexpr*/
#endif

#if nsel_CPP14_OR_GREATER
# define nsel_constexpr14 constexpr
#else
# define nsel_constexpr14 /*constexpr*/
#endif

#if nsel_CPP17_OR_GREATER
# define nsel_inline17 inline
#else
# define nsel_inline17 /*inline*/
#endif

// Method enabling

#define nsel_REQUIRES_A(...) \
    , typename std::enable_if<__VA_ARGS__, void*>::type = nullptr

#define nsel_REQUIRES_0(...) \
    template< bool B = (__VA_ARGS__), typename std::enable_if<B, int>::type = 0 >

#define nsel_REQUIRES_R(R, ...) \
    typename std::enable_if<__VA_ARGS__, R>::type

#define nsel_REQUIRES_T(...) \
    , typename = typename std::enable_if< (__VA_ARGS__), nonstd::expected_lite::detail::enabler >::type

// Clang, GNUC, MSVC warning suppression macros:

#if defined(_MSC_VER) && !defined(__clang__)
# define nsel_COMPILER_MSVC_VERSION   (_MSC_VER / 10 - 10 * ( 5 + (_MSC_VER < 1900)) )
#else
# define nsel_COMPILER_MSVC_VERSION   0
#endif

#ifdef __clang__
# pragma clang diagnostic push
#elif defined  __GNUC__
# pragma  GCC  diagnostic push
#endif // __clang__

#if nsel_COMPILER_MSVC_VERSION >= 140
# pragma warning( push )
# define nsel_DISABLE_MSVC_WARNINGS(codes)  __pragma( warning(disable: codes) )
#else
# define nsel_DISABLE_MSVC_WARNINGS(codes)
#endif

#ifdef __clang__
# define nsel_RESTORE_WARNINGS()  _Pragma("clang diagnostic pop")
#elif defined __GNUC__
# define nsel_RESTORE_WARNINGS()  _Pragma("GCC diagnostic pop")
#elif nsel_COMPILER_MSVC_VERSION >= 140
# define nsel_RESTORE_WARNINGS()  __pragma( warning( pop ) )
#else
# define nsel_RESTORE_WARNINGS()
#endif

// Suppress the following MSVC (GSL) warnings:
// - C26409: Avoid calling new and delete explicitly, use std::make_unique<T> instead (r.11)

nsel_DISABLE_MSVC_WARNINGS( 26409 )

//
// in_place: code duplicated in any-lite, expected-lite, optional-lite, variant-lite:
//

#ifndef nonstd_lite_HAVE_IN_PLACE_TYPES
#define nonstd_lite_HAVE_IN_PLACE_TYPES  1

// C++17 std::in_place in <utility>:

#if nsel_CPP17_OR_GREATER

namespace nonstd {

using std::in_place;
using std::in_place_type;
using std::in_place_index;
using std::in_place_t;
using std::in_place_type_t;
using std::in_place_index_t;

#define nonstd_lite_in_place_t(      T)  std::in_place_t
#define nonstd_lite_in_place_type_t( T)  std::in_place_type_t<T>
#define nonstd_lite_in_place_index_t(K)  std::in_place_index_t<K>

#define nonstd_lite_in_place(      T)    std::in_place_t{}
#define nonstd_lite_in_place_type( T)    std::in_place_type_t<T>{}
#define nonstd_lite_in_place_index(K)    std::in_place_index_t<K>{}

} // namespace nonstd

#else // nsel_CPP17_OR_GREATER

namespace nonstd {
namespace detail {

template< class T >
struct in_place_type_tag {};

template< std::size_t K >
struct in_place_index_tag {};

} // namespace detail

struct in_place_t {};

template< class T >
inline in_place_t in_place( detail::in_place_type_tag<T> = detail::in_place_type_tag<T>() )
{
    return in_place_t();
}

template< std::size_t K >
inline in_place_t in_place( detail::in_place_index_tag<K> = detail::in_place_index_tag<K>() )
{
    return in_place_t();
}

template< class T >
inline in_place_t in_place_type( detail::in_place_type_tag<T> = detail::in_place_type_tag<T>() )
{
    return in_place_t();
}

template< std::size_t K >
inline in_place_t in_place_index( detail::in_place_index_tag<K> = detail::in_place_index_tag<K>() )
{
    return in_place_t();
}

// mimic templated typedef:

#define nonstd_lite_in_place_t(      T)  nonstd::in_place_t(&)( nonstd::detail::in_place_type_tag<T>  )
#define nonstd_lite_in_place_type_t( T)  nonstd::in_place_t(&)( nonstd::detail::in_place_type_tag<T>  )
#define nonstd_lite_in_place_index_t(K)  nonstd::in_place_t(&)( nonstd::detail::in_place_index_tag<K> )

#define nonstd_lite_in_place(      T)    nonstd::in_place_type<T>
#define nonstd_lite_in_place_type( T)    nonstd::in_place_type<T>
#define nonstd_lite_in_place_index(K)    nonstd::in_place_index<K>

} // namespace nonstd

#endif // nsel_CPP17_OR_GREATER
#endif // nonstd_lite_HAVE_IN_PLACE_TYPES

//
// expected:
//

namespace nonstd { namespace expected_lite {

namespace std20 {

// type traits C++20:

template< typename T >
struct remove_cvref
{
    typedef typename std::remove_cv< typename std::remove_reference<T>::type >::type type;
};

} // namespace std20

// forward declaration:

template< typename T, typename E >
class expected;

namespace detail {

/// for nsel_REQUIRES_T

enum class enabler{};

/// discriminated union to hold value or 'error'.

template< typename T, typename E >
union storage_t
{
    friend class expected<T,E>;

private:
    using value_type = T;
    using error_type = E;

    // no-op construction
    storage_t() {}
    ~storage_t() {}

    void construct_value( value_type const & v )
    {
        new( &m_value ) value_type( v );
    }

    void construct_value( value_type && v )
    {
        new( &m_value ) value_type( std::move( v ) );
    }

    void destruct_value()
    {
        m_value.~value_type();
    }

    void construct_error( error_type const & e )
    {
        new( &m_error ) error_type( e );
    }

    void construct_error( error_type && e )
    {
        new( &m_error ) error_type( std::move( e ) );
    }

    void destruct_error()
    {
        m_error.~error_type();
    }

    constexpr value_type const & value() const &
    {
        return m_value;
    }

    value_type & value() &
    {
        return m_value;
    }

    constexpr value_type const && value() const &&
    {
        return std::move( m_value );
    }

    nsel_constexpr14 value_type && value() &&
    {
        return std::move( m_value );
    }

    value_type const * value_ptr() const
    {
        return &m_value;
    }

    value_type * value_ptr()
    {
        return &m_value;
    }

    error_type const & error() const
    {
        return m_error;
    }

    error_type & error()
    {
        return m_error;
    }

private:
    value_type m_value;
    error_type m_error;
};

/// discriminated union to hold only 'error'.

template< typename E >
union storage_t<void, E>
{
    friend class expected<void,E>;

private:
    using value_type = void;
    using error_type = E;

    // no-op construction
    storage_t() {}
    ~storage_t() {}

    void construct_error( error_type const & e )
    {
        new( &m_error ) error_type( e );
    }

    void construct_error( error_type && e )
    {
        new( &m_error ) error_type( std::move( e ) );
    }

    void destruct_error()
    {
        m_error.~error_type();
    }

    error_type const & error() const
    {
        return m_error;
    }

    error_type & error()
    {
        return m_error;
    }

private:
    error_type m_error;
};

} // namespace detail

/// // x.x.3 Unexpected object type; unexpected_type; C++17 and later can also use aliased type unexpected.

#if nsel_P0323R <= 2
template< typename E = std::exception_ptr >
class unexpected_type
#else
template< typename E >
class unexpected_type
#endif // nsel_P0323R
{
public:
    using error_type = E;

    unexpected_type() = delete;
    constexpr unexpected_type( unexpected_type const &) = default;
    constexpr unexpected_type( unexpected_type &&) = default;
    nsel_constexpr14 unexpected_type& operator=( unexpected_type const &) = default;
    nsel_constexpr14 unexpected_type& operator=( unexpected_type &&) = default;

    template< typename E2
        nsel_REQUIRES_T(
            std::is_constructible<E,E2&&>::value
        )
    >
    constexpr explicit unexpected_type( E2 && error )
    : m_error( std::forward<E2>( error ) )
    {}

    template< typename E2 >
    constexpr explicit unexpected_type( unexpected_type<E2> const & error
        nsel_REQUIRES_A(
            std::is_constructible<E,E2 const &>::value
            && !std::is_convertible<E2 const &, E>::value /*=> explicit */ )
        )
    : m_error( error )
    {}

    template< typename E2 >
    constexpr /*non-explicit*/ unexpected_type( unexpected_type<E2> const & error
        nsel_REQUIRES_A(
            std::is_constructible<E,E2 const &>::value
            && std::is_convertible<E2 const &, E>::value /*=> non-explicit */ )
        )
    : m_error( error )
    {}

    template< typename E2 >
    constexpr explicit unexpected_type( unexpected_type<E2> && error
        nsel_REQUIRES_A(
            std::is_constructible<E,E2&&>::value
            && !std::is_convertible<E2&&, E>::value /*=> explicit */ )
        )
    : m_error( error )
    {}

    template< typename E2 >
    constexpr /*non-explicit*/ unexpected_type( unexpected_type<E2> && error
        nsel_REQUIRES_A(
            std::is_constructible<E,E2&&>::value
            && std::is_convertible<E2&&, E>::value /*=> non-explicit */ )
        )
    : m_error( error )
    {}

    nsel_constexpr14 E & value() & noexcept
    {
        return m_error;
    }

    constexpr E const & value() const & noexcept
    {
        return m_error;
    }

    nsel_constexpr14 E && value() && noexcept
    {
        return std::move( m_error );
    }

    constexpr E const && value() const && noexcept
    {
        return std::move( m_error );
    }

//      nsel_REQUIRES_A(
//        std::is_move_constructible<E>::value
//        && std::is_swappable<E>::value
//      )

    void swap( unexpected_type & other ) noexcept (
#if nsel_CPP17_OR_GREATER
        std::is_nothrow_move_constructible<E>::value
        && std::is_nothrow_swappable<E&>::value
#else
        std::is_nothrow_move_constructible<E>::value
        && noexcept ( std::swap( std::declval<E&>(), std::declval<E&>() ) )
#endif
    )
    {
        using std::swap;
        swap( m_error, other.m_error );
    }

private:
    error_type m_error;
};

/// class unexpected_type, std::exception_ptr specialization (P0323R2)

#if nsel_P0323R <= 2

template<>
class unexpected_type< std::exception_ptr >
{
public:
    using error_type = std::exception_ptr;

    unexpected_type() = delete;

    ~unexpected_type(){}

    explicit unexpected_type( std::exception_ptr const & error )
    : m_error( error )
    {}

    explicit unexpected_type(std::exception_ptr && error )
    : m_error( std::move( error ) )
    {}

    template< typename E >
    explicit unexpected_type( E error )
    : m_error( std::make_exception_ptr( error ) )
    {}

    std::exception_ptr const & value() const
    {
        return m_error;
    }

    std::exception_ptr & value()
    {
        return m_error;
    }

private:
    std::exception_ptr m_error;
};

#endif // nsel_P0323R

/// x.x.4, Unexpected equality operators

template< typename E >
constexpr bool operator==( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return x.value() == y.value();
}

template< typename E >
constexpr bool operator!=( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return ! ( x == y );
}

#if nsel_P0323R <= 2

template< typename E >
constexpr bool operator<( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return x.value() < y.value();
}

template< typename E >
constexpr bool operator>( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return ( y < x );
}

template< typename E >
constexpr bool operator<=( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return ! ( y < x  );
}

template< typename E >
constexpr bool operator>=( unexpected_type<E> const & x, unexpected_type<E> const & y )
{
    return ! ( x < y );
}

/// x.x.5 Specialized algorithms

template< typename E >
void swap( unexpected_type<E> & x, unexpected_type<E> & y) noexcept ( noexcept ( x.swap(y) ) )
{
    x.swap( y );
}

// unexpected: relational operators for std::exception_ptr:

inline constexpr bool operator<( unexpected_type<std::exception_ptr> const & /*x*/, unexpected_type<std::exception_ptr> const & /*y*/ )
{
    return false;
}

inline constexpr bool operator>( unexpected_type<std::exception_ptr> const & /*x*/, unexpected_type<std::exception_ptr> const & /*y*/ )
{
    return false;
}

inline constexpr bool operator<=( unexpected_type<std::exception_ptr> const & x, unexpected_type<std::exception_ptr> const & y )
{
    return ( x == y );
}

inline constexpr bool operator>=( unexpected_type<std::exception_ptr> const & x, unexpected_type<std::exception_ptr> const & y )
{
    return ( x == y );
}

#endif // nsel_P0323R

// unexpected: traits

#if nsel_P0323R <= 3

template< typename E>
struct is_unexpected : std::false_type {};

template< typename E>
struct is_unexpected< unexpected_type<E> > : std::true_type {};

#endif // nsel_P0323R

// unexpected: factory

// keep make_unexpected() removed in p0323r2 for pre-C++17:

template< typename E>
nsel_constexpr14 auto
make_unexpected( E && value ) -> unexpected_type< typename std::decay<E>::type >
{
    return unexpected_type< typename std::decay<E>::type >( value );
}

#if nsel_P0323R <= 3

/*nsel_constexpr14*/ auto inline
make_unexpected_from_current_exception() -> unexpected_type< std::exception_ptr >
{
    return unexpected_type< std::exception_ptr >( std::current_exception() );
}

#endif // nsel_P0323R

/// unexpect tag, in_place_unexpected tag: construct an error

struct unexpect_t{};
using in_place_unexpected_t = unexpect_t;

nsel_inline17 constexpr unexpect_t unexpect{};
nsel_inline17 constexpr unexpect_t in_place_unexpected{};

/// expected access error

template< typename E >
class bad_expected_access;

template <>
class bad_expected_access< void > : public std::exception
{
public:
    explicit bad_expected_access()
    : std::exception()
    {}
};

template< typename E >
class bad_expected_access : public bad_expected_access< void >
{
public:
    using error_type = E;

    explicit bad_expected_access( error_type error )
    : m_error( error )
    {}

    virtual char const * what() const noexcept override
    {
        return "bad_expected_access";
    }

    nsel_constexpr14 error_type & error() &
    {
        return m_error;
    }

    constexpr error_type const & error() const &
    {
        return m_error;
    }

    nsel_constexpr14 error_type && error() &&
    {
        return std::move( m_error );
    }

    constexpr error_type const && error() const &&
    {
        return std::move( m_error );
    }

private:
    error_type m_error;
};

/// class error_traits

template< typename Error >
struct error_traits
{
    static void rethrow( Error const & e )
    {
        throw bad_expected_access<Error>{ e };
    }
};

template<>
struct error_traits< std::exception_ptr >
{
    static void rethrow( std::exception_ptr const & e )
    {
        std::rethrow_exception( e );
    }
};

template<>
struct error_traits< std::error_code >
{
    static void rethrow( std::error_code const & e )
    {
        throw std::system_error( e );
    }
};

} // namespace expected_lite

// provide nonstd::unexpected_type:

using expected_lite::unexpected_type;

namespace expected_lite {

/// class expected

#if nsel_P0323R <= 2
template< typename T, typename E = std::exception_ptr >
class expected
#else
template< typename T, typename E >
class expected
#endif // nsel_P0323R
{
public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = nonstd::unexpected_type<E>;

    template< typename U >
    struct rebind
    {
        using type = expected<U, error_type>;
    };

    // x.x.4.1 constructors

    nsel_REQUIRES_0(
        std::is_default_constructible<T>::value
    )
    nsel_constexpr14 expected() noexcept
    (
        std::is_nothrow_default_constructible<T>::value
    )
    : has_value_( true )
    {
        contained.construct_value( value_type() );
    }

    nsel_constexpr14 expected( expected const & other
//      nsel_REQUIRES_A(
//          std::is_copy_constructible<T>::value
//          && std::is_copy_constructible<E>::value
//      )
    )
    : has_value_( other.has_value_ )
    {
        if ( has_value() ) contained.construct_value( other.contained.value() );
        else               contained.construct_error( other.contained.error() );
    }

    nsel_constexpr14 expected( expected && other
//      nsel_REQUIRES_A(
//          std::is_move_constructible<T>::value
//          && std::is_move_constructible<E>::value
//      )
    ) noexcept (
        std::is_nothrow_move_constructible<T>::value
        && std::is_nothrow_move_constructible<E>::value
    )
    : has_value_( other.has_value_ )
    {
        if ( has_value() ) contained.construct_value( std::move( other.contained.value() ) );
        else               contained.construct_error( std::move( other.contained.error() ) );
    }

    template< typename U, typename G >
    nsel_constexpr14 explicit expected( expected<U, G> const & other
        nsel_REQUIRES_A(
            std::is_constructible<T, const U&>::value
            && std::is_constructible<E, const G&>::value
            && !std::is_constructible<T, expected<U, G>&>::value
            && !std::is_constructible<T, expected<U, G>&&>::value
            && !std::is_constructible<T, const expected<U, G>&>::value
            && !std::is_constructible<T, const expected<U, G>&&>::value
            && !std::is_convertible<expected<U, G>&, T>::value
            && !std::is_convertible<expected<U, G>&&, T>::value
            && !std::is_convertible<const expected<U, G>&, T>::value
            && !std::is_convertible<const expected<U, G>&&, T>::value
            && (!std::is_convertible<U const&, T>::value || !std::is_convertible<const G&, E>::value ) /*=> explicit */ )
        )
    : has_value_( other.has_value_ )
    {
        if ( has_value() ) contained.construct_value( other.contained.value() );
        else               contained.construct_error( other.contained.error() );
    }

    template< typename U, typename G >
    nsel_constexpr14 /*non-explicit*/ expected( expected<U, G> const & other
        nsel_REQUIRES_A(
            std::is_constructible<T, const U&>::value
            && std::is_constructible<E, const G&>::value
            && !std::is_constructible<T, expected<U, G>&>::value
            && !std::is_constructible<T, expected<U, G>&&>::value
            && !std::is_constructible<T, const expected<U, G>&>::value
            && !std::is_constructible<T, const expected<U, G>&&>::value
            && !std::is_convertible<expected<U, G>&, T>::value
            && !std::is_convertible<expected<U, G>&&, T>::value
            && !std::is_convertible<const expected<U, G>&, T>::value
            && !std::is_convertible<const expected<U, G>&&, T>::value
            && !(!std::is_convertible<U const&, T>::value || !std::is_convertible<const G&, E>::value ) /*=> explicit */ )
        )
    : has_value_( other.has_value_ )
    {
        if ( has_value() ) contained.construct_value( other.contained.value() );
        else               contained.construct_error( other.contained.error() );
    }

    template< typename U, typename G >
    nsel_constexpr14 explicit expected( expected<U, G> && other
        nsel_REQUIRES_A(
            std::is_constructible<T, U>::value
            && std::is_constructible<E, G>::value
            && !std::is_constructible<T, expected<U, G>&>::value
            && !std::is_constructible<T, expected<U, G>&&>::value
            && !std::is_constructible<T, const expected<U, G>&>::value
            && !std::is_constructible<T, const expected<U, G>&&>::value
            && !std::is_convertible<expected<U, G>&, T>::value
            && !std::is_convertible<expected<U, G>&&, T>::value
            && !std::is_convertible<const expected<U, G>&, T>::value
            && !std::is_convertible<const expected<U, G>&&, T>::value
            && (!std::is_convertible<U, T>::value || !std::is_convertible<G, E>::value ) /*=> explicit */ )
        )
    : has_value_( other.has_value_ )
    {
        if ( has_value() ) contained.construct_value( std::move( other.contained.value() ) );
        else               contained.construct_error( std::move( other.contained.error() ) );
    }

    template< typename U, typename G >
    nsel_constexpr14 /*non-explicit*/ expected( expected<U, G> && other
        nsel_REQUIRES_A(
            std::is_constructible<T, U>::value
            && std::is_constructible<E, G>::value
            && !std::is_constructible<T, expected<U, G>&>::value
            && !std::is_constructible<T, expected<U, G>&&>::value
            && !std::is_constructible<T, const expected<U, G>&>::value
            && !std::is_constructible<T, const expected<U, G>&&>::value
            && !std::is_convertible<expected<U, G>&, T>::value
            && !std::is_convertible<expected<U, G>&&, T>::value
            && !std::is_convertible<const expected<U, G>&, T>::value
            && !std::is_convertible<const expected<U, G>&&, T>::value
            && !(!std::is_convertible<U, T>::value || !std::is_convertible<G, E>::value ) /*=> non-explicit */ )
        )
    : has_value_( other.has_value_ )
    {
        if ( has_value() ) contained.construct_value( std::move( other.contained.value() ) );
        else               contained.construct_error( std::move( other.contained.error() ) );
    }

    nsel_constexpr14 expected( value_type const & value
//        nsel_REQUIRES_A(
//            std::is_copy_constructible<T>::value )
    )
    : has_value_( true )
    {
        contained.construct_value( value );
    }

    template< typename U = T >
    nsel_constexpr14 explicit expected( U && value
        nsel_REQUIRES_A(
            std::is_constructible<T,U&&>::value
            && !std::is_same<typename std20::remove_cvref<U>::type, nonstd_lite_in_place_t(U)>::value
            && !std::is_same<expected<T,E>, typename std20::remove_cvref<U>::type>::value
            && !std::is_same<nonstd::unexpected_type<E>, typename std20::remove_cvref<U>::type>::value
            && !std::is_convertible<U&&,T>::value /*=> explicit */
        )
    ) noexcept
    (
        std::is_nothrow_move_constructible<U>::value &&
        std::is_nothrow_move_constructible<E>::value
    )
    : has_value_( true )
    {
        contained.construct_value( std::forward<U>( value ) );
    }

    template< typename U = T >
    nsel_constexpr14 expected( U && value
        nsel_REQUIRES_A(
            std::is_constructible<T,U&&>::value
            && !std::is_same<typename std20::remove_cvref<U>::type, nonstd_lite_in_place_t(U)>::value
            && !std::is_same<expected<T,E>, typename std20::remove_cvref<U>::type>::value
            && !std::is_same<nonstd::unexpected_type<E>, typename std20::remove_cvref<U>::type>::value
            &&  std::is_convertible<U&&,T>::value /*=> non-explicit */
        )
    ) noexcept
    (
        std::is_nothrow_move_constructible<U>::value &&
        std::is_nothrow_move_constructible<E>::value
    )
    : has_value_( true )
    {
        contained.construct_value( std::forward<U>( value ) );
    }

    template< typename... Args
        nsel_REQUIRES_T(
            std::is_constructible<T, Args&&...>::value
        )
    >
    nsel_constexpr14 explicit expected( nonstd_lite_in_place_t(T), Args&&... args )
    : has_value_( true )
    {
        contained.construct_value( std::forward<Args>( args )... );
    }

    template< typename U, typename... Args
        nsel_REQUIRES_T(
            std::is_constructible<T, std::initializer_list<U>, Args&&...>::value
        )
    >
    nsel_constexpr14 explicit expected( nonstd_lite_in_place_t(T), std::initializer_list<U> il, Args&&... args )
    : has_value_( true )
    {
        contained.construct_value( il, std::forward<Args>( args )... );
    }

    template< typename G = E >
    nsel_constexpr14 explicit expected( nonstd::unexpected_type<G> const & error
        nsel_REQUIRES_A(
            !std::is_convertible<const G&, E>::value /*=> explicit */ )
        )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

    template< typename G = E >
    nsel_constexpr14 /*non-explicit*/ expected( nonstd::unexpected_type<G> const & error
        nsel_REQUIRES_A(
            std::is_convertible<const G&, E>::value /*=> non-explicit */ )
        )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

    template< typename G = E >
    nsel_constexpr14 explicit expected( nonstd::unexpected_type<G> && error
        nsel_REQUIRES_A(
            !std::is_convertible<G&&, E>::value /*=> explicit */ )
        )
    : has_value_( false )
    {
        contained.construct_error( std::move( error.value() ) );
    }

    template< typename G = E >
    nsel_constexpr14 /*non-explicit*/ expected( nonstd::unexpected_type<G> && error
        nsel_REQUIRES_A(
            std::is_convertible<G&&, E>::value /*=> non-explicit */ )
        )
    : has_value_( false )
    {
        contained.construct_error( std::move( error.value() ) );
    }

    template< typename... Args
        nsel_REQUIRES_T(
            std::is_constructible<E, Args&&...>::value
        )
    >
    nsel_constexpr14 explicit expected( unexpect_t, Args&&... args )
    : has_value_( false )
    {
        contained.construct_error( std::forward<Args>( args )... );
    }

    template< typename U, typename... Args
        nsel_REQUIRES_T(
            std::is_constructible<T, std::initializer_list<U>, Args&&...>::value
        )
    >
    nsel_constexpr14 explicit expected( unexpect_t, std::initializer_list<U> il, Args&&... args )
    : has_value_( false )
    {
        contained.construct_error( il, std::forward<Args>( args )... );
    }

    // x.x.4.2 destructor

    ~expected()
    {
        if ( has_value() ) contained.destruct_value();
        else               contained.destruct_error();
    }

    // x.x.4.3 assignment

//    nsel_REQUIRES_A(
//        std::is_copy_constructible<T>::value &&
//        std::is_copy_assignable<T>::value &&
//        std::is_copy_constructible<E>::value &&
//        std::is_copy_assignable<E>::value )

    expected operator=( expected const & other )
    {
        expected( other ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES_A(
//        std::is_move_constructible<T>::value &&
//        std::is_move_assignable<T>::value &&
//        std::is_move_constructible<E>::value &&
//        std::is_move_assignable<E>::value )

    expected & operator=( expected && other ) noexcept
    (
        std::is_nothrow_move_assignable<T>::value &&
        std::is_nothrow_move_constructible<T>::value&&
        std::is_nothrow_move_assignable<E>::value &&
        std::is_nothrow_move_constructible<E>::value )
    {
        expected( std::move( other ) ).swap( *this );
        return *this;
    }

    template< typename U
        nsel_REQUIRES_T(
            std::is_constructible<T,U>::value &&
            std::is_assignable<T&, U>::value
        )
    >
    expected & operator=( U && value )
    {
        expected( std::forward<U>( value ) ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES_A(
//        std::is_copy_constructible<E>::value &&
//        std::is_assignable<E&, E>::value )

    expected & operator=( unexpected_type const & uvalue )
    {
        expected( std::move( uvalue ) ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES_A(
//        std::is_copy_constructible<E>::value &&
//        std::is_assignable<E&, E>::value )

    expected & operator=( unexpected_type && uvalue )
    {
        expected( std::move( uvalue ) ).swap( *this );
        return *this;
    }

    template< typename... Args
        nsel_REQUIRES_T(
            std::is_constructible<T, Args&&...>::value
        )
    >
    void emplace( Args &&... args )
    {
        expected( nonstd_lite_in_place(T), std::forward<Args>(args)... ).swap( *this );
    }

    template< typename U, typename... Args
        nsel_REQUIRES_T(
            std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value
        )
    >
    void emplace( std::initializer_list<U> il, Args &&... args )
    {
        expected( nonstd_lite_in_place(T), il, std::forward<Args>(args)... ).swap( *this );
    }

    // x.x.4.4 swap

//    nsel_REQUIRES_A(
//        std::is_move_constructible<T>::value &&
//        std::is_move_constructible<E>::value )

    void swap( expected & other ) noexcept
    (
#if nsel_CPP17_OR_GREATER
        std::is_nothrow_move_constructible<T>::value && std::is_nothrow_swappable<T&>::value &&
        std::is_nothrow_move_constructible<E>::value && std::is_nothrow_swappable<E&>::value
#else
        std::is_nothrow_move_constructible<T>::value && noexcept ( std::swap( std::declval<T&>(), std::declval<T&>() ) ) &&
        std::is_nothrow_move_constructible<E>::value && noexcept ( std::swap( std::declval<E&>(), std::declval<E&>() ) )
#endif
    )
    {
        using std::swap;

        if      (   bool(*this) &&   bool(other) ) { swap( contained.value(), other.contained.value() ); }
        else if ( ! bool(*this) && ! bool(other) ) { swap( contained.error(), other.contained.error() ); }
        else if (   bool(*this) && ! bool(other) ) { error_type t( std::move( other.error() ) );
                                                   other.contained.destruct_error();
                                                   other.contained.construct_value( std::move( contained.value() ) );
                                                   contained.destruct_value();
                                                   contained.construct_error( std::move( t ) );
                                                   swap( has_value_, other.has_value_ ); }
        else if ( ! bool(*this) &&   bool(other) ) { other.swap( *this ); }
    }

    // x.x.4.5 observers

    constexpr value_type const * operator ->() const
    {
        return assert( has_value() ), contained.value_ptr();
    }

    value_type * operator ->()
    {
        return assert( has_value() ), contained.value_ptr();
    }

    constexpr value_type const & operator *() const &
    {
        return assert( has_value() ), contained.value();
    }

    value_type & operator *() &
    {
        return assert( has_value() ), contained.value();
    }

    constexpr value_type const && operator *() const &&
    {
        return assert( has_value() ), std::move( contained.value() );
    }

    nsel_constexpr14 value_type && operator *() &&
    {
        return assert( has_value() ), std::move( contained.value() );
    }

    constexpr explicit operator bool() const noexcept
    {
        return has_value();
    }

    constexpr bool has_value() const noexcept
    {
        return has_value_;
    }

    constexpr value_type const & value() const &
    {
        return has_value()
            ? ( contained.value() )
            : ( error_traits<error_type>::rethrow( contained.error() ), contained.value() );
    }

    value_type & value() &
    {
        return has_value()
            ? ( contained.value() )
            : ( error_traits<error_type>::rethrow( contained.error() ), contained.value() );
    }

    constexpr value_type const && value() const &&
    {
        return std::move( has_value()
            ? ( contained.value() )
            : ( error_traits<error_type>::rethrow( contained.error() ), contained.value() ) );
    }

    nsel_constexpr14 value_type && value() &&
    {
        return std::move( has_value()
            ? ( contained.value() )
            : ( error_traits<error_type>::rethrow( contained.error() ), contained.value() ) );
    }

    constexpr error_type const & error() const &
    {
        return assert( ! has_value() ), contained.error();
    }

    error_type & error() &
    {
        return assert( ! has_value() ), contained.error();
    }

    constexpr error_type const && error() const &&
    {
        return assert( ! has_value() ), std::move( contained.error() );
    }

    error_type && error() &&
    {
        return assert( ! has_value() ), std::move( contained.error() );
    }

    constexpr unexpected_type get_unexpected() const
    {
        return make_unexpected( contained.error() );
    }

    template< typename Ex >
    bool has_exception() const
    {
        using ContainedEx = typename std::remove_reference< decltype( get_unexpected().value() ) >::type;
        return ! has_value() && std::is_base_of< Ex, ContainedEx>::value;
    }

    template< typename U
        nsel_REQUIRES_T(
            std::is_copy_constructible<T>::value &&
            std::is_convertible<U&&, T>::value
        )
    >
    value_type value_or( U && v ) const &
    {
        return has_value()
            ? contained.value()
            : static_cast<T>( std::forward<U>( v ) );
    }

    template< typename U
        nsel_REQUIRES_T(
            std::is_move_constructible<T>::value &&
            std::is_convertible<U&&, T>::value
        )
    >
    value_type value_or( U && v ) &&
    {
        return has_value()
            ? std::move( contained.value() )
            : static_cast<T>( std::forward<U>( v ) );
    }

    // unwrap()

//  template <class U, class E>
//  constexpr expected<U,E> expected<expected<U,E>,E>::unwrap() const&;

//  template <class T, class E>
//  constexpr expected<T,E> expected<T,E>::unwrap() const&;

//  template <class U, class E>
//  expected<U,E> expected<expected<U,E>, E>::unwrap() &&;

//  template <class T, class E>
//  template expected<T,E> expected<T,E>::unwrap() &&;

    // factories

//  template< typename Ex, typename F>
//  expected<T,E> catch_exception(F&& f);

//  template< typename F>
//  expected<decltype(func(declval<T>())),E> map(F&& func) ;

//  template< typename F>
//  'see below' bind(F&& func);

//  template< typename F>
//  expected<T,E> catch_error(F&& f);

//  template< typename F>
//  'see below' then(F&& func);

private:
    bool has_value_;
    detail::storage_t<T,E> contained;
};

/// class expected, void specialization

template< typename E >
class expected<void, E>
{
public:
    using value_type = void;
    using error_type = E;
    using unexpected_type = nonstd::unexpected_type<E>;

    // x.x.4.1 constructors

    constexpr expected() noexcept
    : has_value_( true )
    {
    }

    nsel_constexpr14 expected( expected const & other )
    : has_value_( other.has_value_ )
    {
        if ( ! has_value() ) contained.construct_error( other.contained.error() );
    }

    nsel_REQUIRES_0(
        std::is_move_constructible<E>::value
    )
    nsel_constexpr14 expected( expected && other ) noexcept
    (
        true    // TBD - see also non-void specialization
    )
    : has_value_( other.has_value_ )
    {
        if ( ! has_value() ) contained.construct_error( std::move( other.contained.error() ) );
    }

    constexpr explicit expected( nonstd_lite_in_place_t(void) )
    : has_value_( true )
    {
    }

    template< typename G = E >
    nsel_constexpr14 explicit expected( nonstd::unexpected_type<G> const & error
        nsel_REQUIRES_A(
            !std::is_convertible<const G&, E>::value /*=> explicit */
        )
    )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

    template< typename G = E >
    nsel_constexpr14 /*non-explicit*/ expected( nonstd::unexpected_type<G> const & error
        nsel_REQUIRES_A(
            std::is_convertible<const G&, E>::value /*=> non-explicit */
        )
    )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

    template< typename G = E >
    nsel_constexpr14 explicit expected( nonstd::unexpected_type<G> && error
        nsel_REQUIRES_A(
            !std::is_convertible<G&&, E>::value /*=> explicit */
        )
    )
    : has_value_( false )
    {
        contained.construct_error( std::move( error.value() ) );
    }

    template< typename G = E >
    nsel_constexpr14 /*non-explicit*/ expected( nonstd::unexpected_type<G> && error
        nsel_REQUIRES_A(
            std::is_convertible<G&&, E>::value /*=> non-explicit */
        )
    )
    : has_value_( false )
    {
        contained.construct_error( std::move( error.value() ) );
    }

    template< typename... Args
        nsel_REQUIRES_T(
            std::is_constructible<E, Args&&...>::value
        )
    >
    nsel_constexpr14 explicit expected( unexpect_t, Args&&... args )
    : has_value_( false )
    {
        contained.construct_error( std::forward<Args>( args )... );
    }

    template< typename U, typename... Args
        nsel_REQUIRES_T(
            std::is_constructible<U, std::initializer_list<U>, Args&&...>::value
        )
    >
    nsel_constexpr14 explicit expected( unexpect_t, std::initializer_list<U> il, Args&&... args )
    : has_value_( false )
    {
        contained.construct_error( il, std::forward<Args>( args )... );
    }


    // destructor

    ~expected()
    {
        if ( ! has_value() ) contained.destruct_error();
    }

    // x.x.4.3 assignment

//    nsel_REQUIRES_A(
//        std::is_copy_constructible<E>::value &&
//        std::is_copy_assignable<E>::value )

    expected & operator=( expected const & other )
    {
        expected( other ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES_A(
//        std::is_move_constructible<E>::value &&
//        std::is_move_assignable<E>::value )

    expected & operator=( expected && other ) noexcept
    (
        std::is_nothrow_move_assignable<E>::value &&
        std::is_nothrow_move_constructible<E>::value )
    {
        expected( std::move( other ) ).swap( *this );
        return *this;
    }

    void emplace()
    {}

    // x.x.4.4 swap

//    nsel_REQUIRES_A(
//        std::is_move_constructible<E>::value )

    void swap( expected & other ) noexcept
    (
#if nsel_CPP17_OR_GREATER
        std::is_nothrow_move_constructible<E>::value && std::is_nothrow_swappable<E&>::value
#else
        std::is_nothrow_move_constructible<E>::value && noexcept ( std::swap( std::declval<E&>(), std::declval<E&>() ) )
#endif
    )
    {
        using std::swap;

        if      ( ! bool(*this) && ! bool(other) ) { swap( contained.error(), other.contained.error() ); }
        else if (   bool(*this) && ! bool(other) ) { contained.construct_error( std::move( other.error() ) );
                                                   swap( has_value_, other.has_value_ ); }
        else if ( ! bool(*this) &&   bool(other) ) { other.swap( *this ); }
    }

    // x.x.4.5 observers

    constexpr explicit operator bool() const noexcept
    {
        return has_value();
    }

    constexpr bool has_value() const noexcept
    {
        return has_value_;
    }

    void value() const
    {}

    constexpr error_type const & error() const &
    {
        return assert( ! has_value() ), contained.error();
    }

    error_type & error() &
    {
        return assert( ! has_value() ), contained.error();
    }

    constexpr error_type const && error() const &&
    {
        return assert( ! has_value() ), std::move( contained.error() );
    }

    error_type && error() &&
    {
        return assert( ! has_value() ), std::move( contained.error() );
    }

    constexpr unexpected_type get_unexpected() const
    {
        return make_unexpected( contained.error() );
    }

    template< typename Ex >
    bool has_exception() const
    {
        return ! has_value() && std::is_base_of< Ex, decltype( get_unexpected().value() ) >::value;
    }

//  template constexpr 'see below' unwrap() const&;
//
//  template 'see below' unwrap() &&;

    // factories

//  template< typename Ex, typename F>
//  expected<void,E> catch_exception(F&& f);
//
//  template< typename F>
//  expected<decltype(func()), E> map(F&& func) ;
//
//  template< typename F>
//  'see below' bind(F&& func) ;
//
//  template< typename F>
//  expected<void,E> catch_error(F&& f);
//
//  template< typename F>
//  'see below' then(F&& func);

private:
    bool has_value_;
    detail::storage_t<void,E> contained;
};

// expected: relational operators

template< typename T, typename E >
constexpr bool operator==( expected<T,E> const & x, expected<T,E> const & y )
{
    return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
}

template< typename T, typename E >
constexpr bool operator!=( expected<T,E> const & x, expected<T,E> const & y )
{
    return !(x == y);
}

template< typename T, typename E >
constexpr bool operator<( expected<T,E> const & x, expected<T,E> const & y )
{
    return (!y) ? false : (!x) ? true : *x < *y;
}

template< typename T, typename E >
constexpr bool operator>( expected<T,E> const & x, expected<T,E> const & y )
{
    return (y < x);
}

template< typename T, typename E >
constexpr bool operator<=( expected<T,E> const & x, expected<T,E> const & y )
{
    return !(y < x);
}

template< typename T, typename E >
constexpr bool operator>=( expected<T,E> const & x, expected<T,E> const & y )
{
    return !(x < y);
}

// expected: comparison with unexpected_type

template< typename T, typename E >
constexpr bool operator==( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return (!x) ? x.get_unexpected() == u : false;
}

template< typename T, typename E >
constexpr bool operator==( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ( x == u );
}

template< typename T, typename E >
constexpr bool operator!=( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return ! ( x == u );
}

template< typename T, typename E >
constexpr bool operator!=( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ! ( x == u );
}

template< typename T, typename E >
constexpr bool operator<( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return (!x) ? ( x.get_unexpected() < u ) : false;
}

#if nsel_P0323R <= 2

template< typename T, typename E >
constexpr bool operator<( unexpected_type<E> const & u, expected<T,E> const & x )
{
  return (!x) ? ( u < x.get_unexpected() ) : true ;
}

template< typename T, typename E >
constexpr bool operator>( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return ( u < x );
}

template< typename T, typename E >
constexpr bool operator>( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ( x < u );
}

template< typename T, typename E >
constexpr bool operator<=( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return ! ( u < x );
}

template< typename T, typename E >
constexpr bool operator<=( unexpected_type<E> const & u, expected<T,E> const & x)
{
    return ! ( x < u );
}

template< typename T, typename E >
constexpr bool operator>=( expected<T,E> const & x, unexpected_type<E> const & u  )
{
    return ! ( u > x );
}

template< typename T, typename E >
constexpr bool operator>=( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ! ( x > u );
}

#endif // nsel_P0323R

// expected: comparison with T

template< typename T, typename E >
constexpr bool operator==( expected<T,E> const & x, T const & v )
{
    return bool(x) ? *x == v : false;
}

template< typename T, typename E >
constexpr bool operator==(T const & v, expected<T,E> const & x )
{
    return bool(x) ? v == *x : false;
}

template< typename T, typename E >
constexpr bool operator!=( expected<T,E> const & x, T const & v )
{
    return bool(x) ? *x != v : true;
}

template< typename T, typename E >
constexpr bool operator!=( T const & v, expected<T,E> const & x )
{
    return bool(x) ? v != *x : true;
}

template< typename T, typename E >
constexpr bool operator<( expected<T,E> const & x, T const & v )
{
    return bool(x) ? *x < v : true;
}

template< typename T, typename E >
constexpr bool operator<( T const & v, expected<T,E> const & x )
{
    return bool(x) ? v < *x : false;
}

template< typename T, typename E >
constexpr bool operator>( T const & v, expected<T,E> const & x )
{
    return bool(x) ? *x < v : false;
}

template< typename T, typename E >
constexpr bool operator>( expected<T,E> const & x, T const & v )
{
    return bool(x) ? v < *x : false;
}

template< typename T, typename E >
constexpr bool operator<=( T const & v, expected<T,E> const & x )
{
    return bool(x) ? ! ( *x < v ) : false;
}

template< typename T, typename E >
constexpr bool operator<=( expected<T,E> const & x, T const & v )
{
    return bool(x) ? ! ( v < *x ) : true;
}

template< typename T, typename E >
constexpr bool operator>=( expected<T,E> const & x, T const & v )
{
    return bool(x) ? ! ( *x < v ) : false;
}

template< typename T, typename E >
constexpr bool operator>=( T const & v, expected<T,E> const & x )
{
    return bool(x) ? ! ( v < *x ) : true;
}

/// x.x.x Specialized algorithms

template< typename T, typename E >
void swap( expected<T,E> & x, expected<T,E> & y ) noexcept ( noexcept ( x.swap(y) ) )
{
    x.swap( y );
}

#if nsel_P0323R <= 3

template< typename T >
constexpr auto make_expected( T && v ) -> expected< typename std::decay<T>::type >
{
    return expected< typename std::decay<T>::type >( std::forward<T>( v ) );
}

// expected<void> specialization:

auto inline make_expected() -> expected<void>
{
    return expected<void>( in_place );
}

template< typename T >
constexpr auto make_expected_from_current_exception() -> expected<T>
{
    return expected<T>( make_unexpected_from_current_exception() );
}

template< typename T >
auto make_expected_from_exception( std::exception_ptr v ) -> expected<T>
{
    return expected<T>( unexpected_type<std::exception_ptr>( std::forward<std::exception_ptr>( v ) ) );
}

template< typename T, typename E >
constexpr auto make_expected_from_error( E e ) -> expected<T, typename std::decay<E>::type>
{
    return expected<T, typename std::decay<E>::type>( make_unexpected( e ) );
}

template< typename F >
/*nsel_constexpr14*/
auto make_expected_from_call( F f,
    nsel_REQUIRES_A( ! std::is_same<typename std::result_of<F()>::type, void>::value )
) -> expected< typename std::result_of<F()>::type >
{
    try
    {
        return make_expected( f() );
    }
    catch (...)
    {
        return make_unexpected_from_current_exception();
    }
}

template< typename F >
/*nsel_constexpr14*/
auto make_expected_from_call( F f,
    nsel_REQUIRES_A( std::is_same<typename std::result_of<F()>::type, void>::value )
) -> expected<void>
{
    try
    {
        f();
        return make_expected();
    }
    catch (...)
    {
        return make_unexpected_from_current_exception();
    }
}

#endif // nsel_P0323R

} // namespace expected_lite

using namespace expected_lite;

// using expected_lite::expected;
// using ...

} // namespace nonstd

namespace std {

// expected: hash support

template< typename T, typename E >
struct hash< nonstd::expected<T,E> >
{
    using result_type = typename hash<T>::result_type;
    using argument_type = nonstd::expected<T,E>;

    constexpr result_type operator()(argument_type const & arg) const
    {
        return arg ? std::hash<T>{}(*arg) : result_type{};
    }
};

// TBD - ?? remove? see spec.
template< typename T, typename E >
struct hash< nonstd::expected<T&,E> >
{
    using result_type = typename hash<T>::result_type;
    using argument_type = nonstd::expected<T&,E>;

    constexpr result_type operator()(argument_type const & arg) const
    {
        return arg ? std::hash<T>{}(*arg) : result_type{};
    }
};

// TBD - implement
// bool(e), hash<expected<void,E>>()(e) shall evaluate to the hashing true;
// otherwise it evaluates to an unspecified value if E is exception_ptr or
// a combination of hashing false and hash<E>()(e.error()).

template< typename E >
struct hash< nonstd::expected<void,E> >
{
};

} // namespace std

namespace nonstd {

// void unexpected() is deprecated && removed in C++17

#if nsel_CPP17_OR_GREATER && nsel_COMPILER_MSVC_VERSION > 141
template< typename E >
using unexpected = unexpected_type<E>;
#endif

} // namespace nonstd

#undef nsel_REQUIRES
#undef nsel_REQUIRES_0
#undef nsel_REQUIRES_T

nsel_RESTORE_WARNINGS()

#endif // nsel_USES_STD_EXPECTED

#endif // NONSTD_EXPECTED_LITE_HPP
