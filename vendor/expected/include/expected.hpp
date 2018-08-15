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

#include <cassert>
#include <exception>
#include <functional>
#include <initializer_list>
#include <new>
#include <stdexcept>
#include <system_error>
#include <type_traits>
#include <utility>

#define  expected_lite_VERSION "0.1.0"

// expected-lite configuration
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

// Compiler detection (C++20 is speculative):
// Note: MSVC supports C++14 since it supports C++17.

#ifdef _MSVC_LANG
# define type_MSVC_LANG  _MSVC_LANG
#else
# define type_MSVC_LANG  0
#endif

#define type_CPP11             (__cplusplus == 201103L )
#define nsel_CPP11_OR_GREATER  (__cplusplus >= 201103L || type_MSVC_LANG >= 201103L )
#define nsel_CPP14_OR_GREATER  (__cplusplus >= 201402L || type_MSVC_LANG >= 201703L )
#define nsel_CPP17_OR_GREATER  (__cplusplus >= 201703L || type_MSVC_LANG >= 201703L )
#define nsel_CPP20_OR_GREATER  (__cplusplus >= 202000L || type_MSVC_LANG >= 202000L )

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

#define nsel_REQUIRES(...) \
    typename std::enable_if<__VA_ARGS__, void*>::type = 0

#define nsel_REQUIRES_0(...) \
    template< bool B = (__VA_ARGS__), typename std::enable_if<B, int>::type = 0 >

#define nsel_REQUIRES_T(...) \
    typename = typename std::enable_if< (__VA_ARGS__), nonstd::expected_detail::enabler >::type

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

namespace nonstd {

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

namespace expected_detail {

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

} // namespace expected_detail

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

    template< typename E2, nsel_REQUIRES_T(
        std::is_constructible<E,E2&&>::value )
    >
    constexpr explicit unexpected_type( E2 && error )
    : m_error( std::forward<E2>( error ) )
    {}

    template< typename E2 >
    constexpr explicit unexpected_type( unexpected_type<E2> const & error, nsel_REQUIRES(
        std::is_constructible<E,E2 const &>::value
        && !std::is_convertible<E2 const &, E>::value /*=> explicit */ )
    )
    : m_error( error )
    {}

    template< typename E2 >
    constexpr /*non-explicit*/ unexpected_type( unexpected_type<E2> const & error, nsel_REQUIRES(
        std::is_constructible<E,E2 const &>::value
        && std::is_convertible<E2 const &, E>::value /*=> non-explicit */ )
    )
    : m_error( error )
    {}

    template< typename E2 >
    constexpr explicit unexpected_type( unexpected_type<E2> && error, nsel_REQUIRES(
        std::is_constructible<E,E2&&>::value
        && !std::is_convertible<E2&&, E>::value /*=> explicit */ )
    )
    : m_error( error )
    {}

    template< typename E2 >
    constexpr /*non-explicit*/ unexpected_type( unexpected_type<E2> && error, nsel_REQUIRES(
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

//      , nsel_REQUIRES(
//        std::is_move_constructible<E>::value
//        && std::is_swappable<E>::value )

    void swap( unexpected_type & rhs ) noexcept (
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
        swap( m_error, rhs.m_error );
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

template <typename E>
struct is_unexpected : std::false_type {};

template <typename E>
struct is_unexpected< unexpected_type<E> > : std::true_type {};

#endif // nsel_P0323R

// unexpected: factory

// keep make_unexpected() removed in p0323r2 for pre-C++17:

template< typename E>
nsel_constexpr14 auto
make_unexpected( E && v) -> unexpected_type< typename std::decay<E>::type >
{
    return unexpected_type< typename std::decay<E>::type >( v );
}

#if nsel_P0323R <= 3

/*nsel_constexpr14*/ auto inline
make_unexpected_from_current_exception() -> unexpected_type< std::exception_ptr >
{
    return unexpected_type< std::exception_ptr >( std::current_exception() );
}

#endif // nsel_P0323R

/// in-place tag: construct a value in-place (should come from std::experimental::optional)

struct in_place_t{};

nsel_inline17 constexpr in_place_t in_place{};

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

    nsel_constexpr14 expected( expected const & rhs
//    , nsel_REQUIRES(
//        std::is_copy_constructible<T>::value
//        && std::is_copy_constructible<E>::value )
    )
    : has_value_( rhs.has_value_ )
    {
        if ( has_value() ) contained.construct_value( rhs.contained.value() );
        else               contained.construct_error( rhs.contained.error() );
    }

    nsel_constexpr14 expected( expected && rhs
//    , nsel_REQUIRES(
//        std::is_move_constructible<T>::value
//        && std::is_move_constructible<E>::value )
    ) noexcept (
        std::is_nothrow_move_constructible<T>::value
        && std::is_nothrow_move_constructible<E>::value
    )
    : has_value_( rhs.has_value_ )
    {
        if ( has_value() ) contained.construct_value( std::move( rhs.contained.value() ) );
        else               contained.construct_error( std::move( rhs.contained.error() ) );
    }

    template< typename U, typename G >
    nsel_constexpr14 explicit expected( expected<U, G> const & rhs, nsel_REQUIRES(
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
    : has_value_( rhs.has_value_ )
    {
        if ( has_value() ) contained.construct_value( rhs.contained.value() );
        else               contained.construct_error( rhs.contained.error() );
    }

    template< typename U, typename G >
    nsel_constexpr14 /*non-explicit*/ expected( expected<U, G> const & rhs, nsel_REQUIRES(
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
    : has_value_( rhs.has_value_ )
    {
        if ( has_value() ) contained.construct_value( rhs.contained.value() );
        else               contained.construct_error( rhs.contained.error() );
    }

    template< typename U, typename G >
    nsel_constexpr14 explicit expected( expected<U, G> && rhs, nsel_REQUIRES(
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
    : has_value_( rhs.has_value_ )
    {
        if ( has_value() ) contained.construct_value( std::move( rhs.contained.value() ) );
        else               contained.construct_error( std::move( rhs.contained.error() ) );
    }

    template< typename U, typename G >
    nsel_constexpr14 /*non-explicit*/ expected( expected<U, G> && rhs, nsel_REQUIRES(
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
    : has_value_( rhs.has_value_ )
    {
        if ( has_value() ) contained.construct_value( std::move( rhs.contained.value() ) );
        else               contained.construct_error( std::move( rhs.contained.error() ) );
    }

    nsel_constexpr14 expected( value_type const & v
//    , nsel_REQUIRES(
//        std::is_copy_constructible<T>::value )
    )
    : has_value_( true )
    {
        contained.construct_value( v );
    }

    template< typename U = T >
    nsel_constexpr14 explicit expected( U && v, nsel_REQUIRES(
        std::is_constructible<T,U&&>::value
        && !std::is_same<typename std20::remove_cvref<U>::type, in_place_t>::value
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
        contained.construct_value( std::forward<U>( v ) );
    }

    template< typename U = T >
    nsel_constexpr14 expected( U && v, nsel_REQUIRES(
        std::is_constructible<T,U&&>::value
        && !std::is_same<typename std20::remove_cvref<U>::type, in_place_t>::value
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
        contained.construct_value( std::forward<U>( v ) );
    }

    template <typename... Args, nsel_REQUIRES_T(
        std::is_constructible<T, Args&&...>::value ) >

    nsel_constexpr14 explicit expected( in_place_t, Args&&... args )
    : has_value_( true )
    {
        contained.construct_value( std::forward<Args>( args )... );
    }

    template< typename U, typename... Args, nsel_REQUIRES_T(
        std::is_constructible<T, std::initializer_list<U>, Args&&...>::value ) >

    nsel_constexpr14 explicit expected( in_place_t, std::initializer_list<U> il, Args&&... args )
    : has_value_( true )
    {
        contained.construct_value( il, std::forward<Args>( args )... );
    }

    template< typename G = E >
    nsel_constexpr14 explicit expected( nonstd::unexpected_type<G> const & error, nsel_REQUIRES(
        !std::is_convertible<const G&, E>::value /*=> explicit */ )
    )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

    template< typename G = E >
    nsel_constexpr14 /*non-explicit*/ expected( nonstd::unexpected_type<G> const & error, nsel_REQUIRES(
        std::is_convertible<const G&, E>::value /*=> non-explicit */ )
    )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

    template< typename G = E >
    nsel_constexpr14 explicit expected( nonstd::unexpected_type<G> && error, nsel_REQUIRES(
        !std::is_convertible<G&&, E>::value /*=> explicit */ )
    )
    : has_value_( false )
    {
        contained.construct_error( std::move( error.value() ) );
    }

    template< typename G = E >
    nsel_constexpr14 /*non-explicit*/ expected( nonstd::unexpected_type<G> && error, nsel_REQUIRES(
        std::is_convertible<G&&, E>::value /*=> non-explicit */ )
    )
    : has_value_( false )
    {
        contained.construct_error( std::move( error.value() ) );
    }

    template< typename... Args, nsel_REQUIRES_T(
        std::is_constructible<E, Args&&...>::value )
    >
    nsel_constexpr14 explicit expected( unexpect_t, Args&&... args )
    : has_value_( false )
    {
        contained.construct_error( std::forward<Args>( args )... );
    }

    template< typename U, typename... Args, nsel_REQUIRES_T(
        std::is_constructible<T, std::initializer_list<U>, Args&&...>::value )
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

//    nsel_REQUIRES(
//        std::is_copy_constructible<T>::value &&
//        std::is_copy_assignable<T>::value &&
//        std::is_copy_constructible<E>::value &&
//        std::is_copy_assignable<E>::value )

    expected operator=( expected const & rhs )
    {
        expected( rhs ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES(
//        std::is_move_constructible<T>::value &&
//        std::is_move_assignable<T>::value &&
//        std::is_move_constructible<E>::value &&
//        std::is_move_assignable<E>::value )

    expected & operator=( expected && rhs ) noexcept
    (
        std::is_nothrow_move_assignable<T>::value &&
        std::is_nothrow_move_constructible<T>::value&&
        std::is_nothrow_move_assignable<E>::value &&
        std::is_nothrow_move_constructible<E>::value )
    {
        expected( std::move( rhs ) ).swap( *this );
        return *this;
    }

    template< typename U, nsel_REQUIRES_T(
        std::is_constructible<T,U>::value &&
        std::is_assignable<T&, U>::value ) >

    expected & operator=( U && v )
    {
        expected( std::forward<U>( v ) ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES(
//        std::is_copy_constructible<E>::value &&
//        std::is_assignable<E&, E>::value )

    expected & operator=( unexpected_type const & u )
    {
        expected( std::move( u ) ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES(
//        std::is_copy_constructible<E>::value &&
//        std::is_assignable<E&, E>::value )

    expected & operator=( unexpected_type && u )
    {
        expected( std::move( u ) ).swap( *this );
        return *this;
    }

    template< typename... Args, nsel_REQUIRES_T(
        std::is_constructible<T, Args&&...>::value ) >

    void emplace( Args &&... args )
    {
        expected( in_place, std::forward<Args>(args)... ).swap( *this );
    }

    template< typename U, typename... Args, nsel_REQUIRES_T(
        std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value ) >

    void emplace( std::initializer_list<U> il, Args &&... args )
    {
        expected( in_place, il, std::forward<Args>(args)... ).swap( *this );
    }

    // x.x.4.4 swap

//    nsel_REQUIRES(
//        std::is_move_constructible<T>::value &&
//        std::is_move_constructible<E>::value )

    void swap( expected & rhs ) noexcept
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

        if      (   bool(*this) &&   bool(rhs) ) { swap( contained.value(), rhs.contained.value() ); }
        else if ( ! bool(*this) && ! bool(rhs) ) { swap( contained.error(), rhs.contained.error() ); }
        else if (   bool(*this) && ! bool(rhs) ) { error_type t( std::move( rhs.error() ) );
                                                   rhs.contained.destruct_error();
                                                   rhs.contained.construct_value( std::move( contained.value() ) );
                                                   contained.destruct_value();
                                                   contained.construct_error( std::move( t ) );
                                                   swap( has_value_, rhs.has_value_ ); }
        else if ( ! bool(*this) &&   bool(rhs) ) { rhs.swap( *this ); }
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

    template< typename U, nsel_REQUIRES_T(
        std::is_copy_constructible<T>::value &&
        std::is_convertible<U&&, T>::value ) >

    value_type value_or( U && v ) const &
    {
        return has_value()
            ? contained.value()
            : static_cast<T>( std::forward<U>( v ) );
    }

    template< typename U, nsel_REQUIRES_T(
        std::is_move_constructible<T>::value &&
        std::is_convertible<U&&, T>::value ) >

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

//  template <typename Ex, typename F>
//  expected<T,E> catch_exception(F&& f);

//  template <typename F>
//  expected<decltype(func(declval<T>())),E> map(F&& func) ;

//  template <typename F>
//  'see below' bind(F&& func);

//  template <typename F>
//  expected<T,E> catch_error(F&& f);

//  template <typename F>
//  'see below' then(F&& func);

private:
    bool has_value_;
    expected_detail::storage_t<T,E> contained;
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

    nsel_constexpr14 expected( expected const & rhs )
    : has_value_( rhs.has_value_ )
    {
        if ( ! has_value() ) contained.construct_error( rhs.contained.error() );
    }

    nsel_REQUIRES_0(
        std::is_move_constructible<E>::value )

    nsel_constexpr14 expected( expected && rhs ) noexcept
    (
        true    // TBD - see also non-void specialization
    )
    : has_value_( rhs.has_value_ )
    {
        if ( ! has_value() ) contained.construct_error( std::move( rhs.contained.error() ) );
    }

    constexpr explicit expected( in_place_t )
    : has_value_( true )
    {
    }

    template< typename G = E >
    nsel_constexpr14 explicit expected( nonstd::unexpected_type<G> const & error, nsel_REQUIRES(
        !std::is_convertible<const G&, E>::value /*=> explicit */ )
    )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

    template< typename G = E >
    nsel_constexpr14 /*non-explicit*/ expected( nonstd::unexpected_type<G> const & error, nsel_REQUIRES(
        std::is_convertible<const G&, E>::value /*=> non-explicit */ )
    )
    : has_value_( false )
    {
        contained.construct_error( error.value() );
    }

    template< typename G = E >
    nsel_constexpr14 explicit expected( nonstd::unexpected_type<G> && error, nsel_REQUIRES(
        !std::is_convertible<G&&, E>::value /*=> explicit */ )
    )
    : has_value_( false )
    {
        contained.construct_error( std::move( error.value() ) );
    }

    template< typename G = E >
    nsel_constexpr14 /*non-explicit*/ expected( nonstd::unexpected_type<G> && error, nsel_REQUIRES(
        std::is_convertible<G&&, E>::value /*=> non-explicit */ )
    )
    : has_value_( false )
    {
        contained.construct_error( std::move( error.value() ) );
    }

    template< typename... Args, nsel_REQUIRES_T(
        std::is_constructible<E, Args&&...>::value )
    >
    nsel_constexpr14 explicit expected( unexpect_t, Args&&... args )
    : has_value_( false )
    {
        contained.construct_error( std::forward<Args>( args )... );
    }

    template< typename U, typename... Args, nsel_REQUIRES_T(
        std::is_constructible<U, std::initializer_list<U>, Args&&...>::value )
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

//    nsel_REQUIRES(
//        std::is_copy_constructible<E>::value &&
//        std::is_copy_assignable<E>::value )

    expected & operator=( expected const & rhs )
    {
        expected( rhs ).swap( *this );
        return *this;
    }

//    nsel_REQUIRES(
//        std::is_move_constructible<E>::value &&
//        std::is_move_assignable<E>::value )

    expected & operator=( expected && rhs ) noexcept
    (
        std::is_nothrow_move_assignable<E>::value &&
        std::is_nothrow_move_constructible<E>::value )
    {
        expected( std::move( rhs ) ).swap( *this );
        return *this;
    }

    void emplace()
    {}

    // x.x.4.4 swap

//    nsel_REQUIRES(
//        std::is_move_constructible<E>::value )

    void swap( expected & rhs ) noexcept
    (
#if nsel_CPP17_OR_GREATER
        std::is_nothrow_move_constructible<E>::value && std::is_nothrow_swappable<E&>::value
#else
        std::is_nothrow_move_constructible<E>::value && noexcept ( std::swap( std::declval<E&>(), std::declval<E&>() ) )
#endif
    )
    {
        using std::swap;

        if      ( ! bool(*this) && ! bool(rhs) ) { swap( contained.error(), rhs.contained.error() ); }
        else if (   bool(*this) && ! bool(rhs) ) { contained.construct_error( std::move( rhs.error() ) );
                                                   swap( has_value_, rhs.has_value_ ); }
        else if ( ! bool(*this) &&   bool(rhs) ) { rhs.swap( *this ); }
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

    template <typename Ex>
    bool has_exception() const
    {
        return ! has_value() && std::is_base_of< Ex, decltype( get_unexpected().value() ) >::value;
    }

//  template constexpr 'see below' unwrap() const&;
//
//  template 'see below' unwrap() &&;

    // factories

//  template <typename Ex, typename F>
//  expected<void,E> catch_exception(F&& f);
//
//  template <typename F>
//  expected<decltype(func()), E> map(F&& func) ;
//
//  template <typename F>
//  'see below' bind(F&& func) ;
//
//  template <typename F>
//  expected<void,E> catch_error(F&& f);
//
//  template <typename F>
//  'see below' then(F&& func);

private:
    bool has_value_;
    expected_detail::storage_t<void,E> contained;
};

// expected: relational operators

template <typename T, typename E>
constexpr bool operator==( expected<T,E> const & x, expected<T,E> const & y )
{
    return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
}

template <typename T, typename E>
constexpr bool operator!=( expected<T,E> const & x, expected<T,E> const & y )
{
    return !(x == y);
}

template <typename T, typename E>
constexpr bool operator<( expected<T,E> const & x, expected<T,E> const & y )
{
    return (!y) ? false : (!x) ? true : *x < *y;
}

template <typename T, typename E>
constexpr bool operator>( expected<T,E> const & x, expected<T,E> const & y )
{
    return (y < x);
}

template <typename T, typename E>
constexpr bool operator<=( expected<T,E> const & x, expected<T,E> const & y )
{
    return !(y < x);
}

template <typename T, typename E>
constexpr bool operator>=( expected<T,E> const & x, expected<T,E> const & y )
{
    return !(x < y);
}

// expected: comparison with unexpected_type

template <typename T, typename E>
constexpr bool operator==( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return (!x) ? x.get_unexpected() == u : false;
}

template <typename T, typename E>
constexpr bool operator==( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ( x == u );
}

template <typename T, typename E>
constexpr bool operator!=( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return ! ( x == u );
}

template <typename T, typename E>
constexpr bool operator!=( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ! ( x == u );
}

template <typename T, typename E>
constexpr bool operator<( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return (!x) ? ( x.get_unexpected() < u ) : false;
}

#if nsel_P0323R <= 2

template <typename T, typename E>
constexpr bool operator<( unexpected_type<E> const & u, expected<T,E> const & x )
{
  return (!x) ? ( u < x.get_unexpected() ) : true ;
}

template <typename T, typename E>
constexpr bool operator>( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return ( u < x );
}

template <typename T, typename E>
constexpr bool operator>( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ( x < u );
}

template <typename T, typename E>
constexpr bool operator<=( expected<T,E> const & x, unexpected_type<E> const & u )
{
    return ! ( u < x );
}

template <typename T, typename E>
constexpr bool operator<=( unexpected_type<E> const & u, expected<T,E> const & x)
{
    return ! ( x < u );
}

template <typename T, typename E>
constexpr bool operator>=( expected<T,E> const & x, unexpected_type<E> const & u  )
{
    return ! ( u > x );
}

template <typename T, typename E>
constexpr bool operator>=( unexpected_type<E> const & u, expected<T,E> const & x )
{
    return ! ( x > u );
}

#endif // nsel_P0323R

// expected: comparison with T

template <typename T, typename E>
constexpr bool operator==( expected<T,E> const & x, T const & v )
{
    return bool(x) ? *x == v : false;
}

template <typename T, typename E>
constexpr bool operator==(T const & v, expected<T,E> const & x )
{
    return bool(x) ? v == *x : false;
}

template <typename T, typename E>
constexpr bool operator!=( expected<T,E> const & x, T const & v )
{
    return bool(x) ? *x != v : true;
}

template <typename T, typename E>
constexpr bool operator!=( T const & v, expected<T,E> const & x )
{
    return bool(x) ? v != *x : true;
}

template <typename T, typename E>
constexpr bool operator<( expected<T,E> const & x, T const & v )
{
    return bool(x) ? *x < v : true;
}

template <typename T, typename E>
constexpr bool operator<( T const & v, expected<T,E> const & x )
{
    return bool(x) ? v < *x : false;
}

template <typename T, typename E>
constexpr bool operator>( T const & v, expected<T,E> const & x )
{
    return bool(x) ? *x < v : false;
}

template <typename T, typename E>
constexpr bool operator>( expected<T,E> const & x, T const & v )
{
    return bool(x) ? v < *x : false;
}

template <typename T, typename E>
constexpr bool operator<=( T const & v, expected<T,E> const & x )
{
    return bool(x) ? ! ( *x < v ) : false;
}

template <typename T, typename E>
constexpr bool operator<=( expected<T,E> const & x, T const & v )
{
    return bool(x) ? ! ( v < *x ) : true;
}

template <typename T, typename E>
constexpr bool operator>=( expected<T,E> const & x, T const & v )
{
    return bool(x) ? ! ( *x < v ) : false;
}

template <typename T, typename E>
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

template< typename T>
constexpr auto make_expected( T && v ) -> expected< typename std::decay<T>::type >
{
    return expected< typename std::decay<T>::type >( std::forward<T>( v ) );
}

// expected<void> specialization:

auto inline make_expected() -> expected<void>
{
    return expected<void>( in_place );
}

template< typename T>
constexpr auto make_expected_from_current_exception() -> expected<T>
{
    return expected<T>( make_unexpected_from_current_exception() );
}

template< typename T>
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
    nsel_REQUIRES( ! std::is_same<typename std::result_of<F()>::type, void>::value )
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
    nsel_REQUIRES( std::is_same<typename std::result_of<F()>::type, void>::value )
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

#endif // NONSTD_EXPECTED_LITE_HPP
