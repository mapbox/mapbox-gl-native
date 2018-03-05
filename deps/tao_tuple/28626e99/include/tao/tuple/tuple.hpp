// The Art of C++ / Tuple
// Copyright (c) 2015-2016 Daniel Frey

#ifndef TAOCPP_INCLUDE_TUPLE_TUPLE_HPP
#define TAOCPP_INCLUDE_TUPLE_TUPLE_HPP

#include "../seq/config.hpp"
#include "../seq/integer_sequence.hpp"
#include "../seq/is_all.hpp"
#include "../seq/type_by_index.hpp"
#include "../seq/sum.hpp"
#include "../seq/make_integer_sequence.hpp"
#include "../seq/inclusive_scan.hpp"
#include "../seq/minus.hpp"
#include "../seq/map.hpp"
#include "../seq/exclusive_scan.hpp"

#include <type_traits>
#include <utility>
#include <memory>

#if (__cplusplus >= 201402L)
#define TAOCPP_TUPLE_CONSTEXPR constexpr
#else
#define TAOCPP_TUPLE_CONSTEXPR
#endif

namespace tao
{
  template< typename... Ts >
  struct tuple;
}

namespace std
{
  // 20.4.2.8 Tuple traits [tuple.traits]

  template< typename... Ts, typename A >
  struct uses_allocator< tao::tuple< Ts... >, A > : true_type {};
}

namespace tao
{
  template< std::size_t I, typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  const seq::type_by_index_t< I, Ts... >& get( const tuple< Ts... >& ) noexcept;

  template< std::size_t I, typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  seq::type_by_index_t< I, Ts... >& get( tuple< Ts... >& ) noexcept;

  template< std::size_t I, typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  seq::type_by_index_t< I, Ts... >&& get( const tuple< Ts... >&& ) noexcept;

  namespace impl
  {
    // TODO: std::pair support
    // TODO: allocator support

    using swallow = bool[];

    template< typename T, typename >
    struct dependent_type : T {};

    template< bool B, typename T = void >
    using enable_if_t = typename std::enable_if< B, T >::type;

    // TODO: using std::swap?
    template< typename T >
    using is_nothrow_swappable = std::integral_constant< bool, noexcept( swap( std::declval< T& >(), std::declval< T& >() ) ) >;

#if __cplusplus >= 201402L
    template< typename T >
    using is_final = std::is_final< T >;
#else
    template< typename T >
    using is_final = std::integral_constant< bool, __is_final( T ) >;
#endif

    template< bool, bool >
    struct uses_alloc_ctor;

    template< typename T, typename A, typename... As >
    using uses_alloc_ctor_t = uses_alloc_ctor< std::uses_allocator< T, A >::value, std::is_constructible< T, std::allocator_arg_t, A, As... >::value >*;

    template< std::size_t I, typename T, bool = std::is_empty< T >::value && !is_final< T >::value >
    struct tuple_value
    {
      T value;

      constexpr tuple_value()
        noexcept( std::is_nothrow_default_constructible< T >::value )
        : value()
      {
        static_assert( !std::is_reference< T >::value, "attempted to default construct a reference element in a tuple" );
      }

      template< bool B, typename A >
      tuple_value( uses_alloc_ctor< false, B >*, const A& )
        : value()
      {
        static_assert( !std::is_reference< T >::value, "attempted to default construct a reference element in a tuple" );
      }

      template< typename A >
      tuple_value( uses_alloc_ctor< true, true >*, const A& a )
        : value( std::allocator_arg_t(), a )
      {
        static_assert( !std::is_reference< T >::value, "attempted to default construct a reference element in a tuple" );
      }

      template< typename A >
      tuple_value( uses_alloc_ctor< true, false >*, const A& a )
        : value( a )
      {
        static_assert( !std::is_reference< T >::value, "attempted to default construct a reference element in a tuple" );
      }

      template< typename U,
                typename = impl::enable_if_t< !std::is_same< typename std::decay< U >::type, tuple_value >::value >,
                typename = impl::enable_if_t< std::is_constructible< T, U >::value > >
      TAOCPP_TUPLE_CONSTEXPR
      explicit tuple_value( U&& v )
        noexcept( std::is_nothrow_constructible< T, U >::value )
        : value( std::forward< U >( v ) )
      {}

      template< bool B, typename A, typename U >
      tuple_value( uses_alloc_ctor< false, B >*, const A&, U&& v )
        : value( std::forward< U >( v ) )
      {
        // TODO: Add check for rvalue to lvalue reference
      }

      template< typename A, typename U >
      tuple_value( uses_alloc_ctor< true, true >*, const A& a, U&& v )
        : value( std::allocator_arg_t(), a, std::forward< U >( v ) )
      {
        // TODO: Add check for rvalue to lvalue reference
      }

      template< typename A, typename U >
      tuple_value( uses_alloc_ctor< true, false >*, const A& a, U&& v )
        : value( std::forward< U >( v ), a )
      {
        // TODO: Add check for rvalue to lvalue reference
      }

      tuple_value( const tuple_value& ) = default;
      tuple_value( tuple_value&& ) = default;

      template< typename U >
      tuple_value& operator=( U&& v )
        noexcept( std::is_nothrow_assignable< T&, U >::value )
      {
        value = std::forward< U >( v );
        return *this;
      }

      void swap( tuple_value& v )
        noexcept( is_nothrow_swappable< T >::value )
      {
        using std::swap;
        swap( value, v.value );
      }

      TAOCPP_TUPLE_CONSTEXPR
      T& get() noexcept
      {
        return value;
      }

      TAOCPP_TUPLE_CONSTEXPR
      const T& get() const noexcept
      {
        return value;
      }
    };

    template< std::size_t I, typename T >
    struct tuple_value< I, T, true >
      : private T
    {
      constexpr tuple_value()
        noexcept( std::is_nothrow_default_constructible< T >::value )
        : T()
      {}

      template< bool B, typename A >
      tuple_value( uses_alloc_ctor< false, B >*, const A& )
        : T()
      {}

      template< typename A >
      tuple_value( uses_alloc_ctor< true, true >*, const A& a )
        : T( std::allocator_arg_t(), a )
      {}

      template< typename A >
      tuple_value( uses_alloc_ctor< true, false >*, const A& a )
        : T( a )
      {}

      template< typename U,
                typename = impl::enable_if_t< !std::is_same< typename std::decay< U >::type, tuple_value >::value >,
                typename = impl::enable_if_t< std::is_constructible< T, U >::value > >
      TAOCPP_TUPLE_CONSTEXPR
      explicit tuple_value( U&& v )
        noexcept( std::is_nothrow_constructible< T, U >::value )
        : T( std::forward< U >( v ) )
      {}

      template< bool B, typename A, typename U >
      tuple_value( uses_alloc_ctor< false, B >*, const A&, U&& v )
        : T( std::forward< U >( v ) )
      {}

      template< typename A, typename U >
      tuple_value( uses_alloc_ctor< true, true >*, const A& a, U&& v )
        : T( std::allocator_arg_t(), a, std::forward< U >( v ) )
      {}

      template< typename A, typename U >
      tuple_value( uses_alloc_ctor< true, false >*, const A& a, U&& v )
        : T( std::forward< U >( v ), a )
      {}

      tuple_value( const tuple_value& ) = default;
      tuple_value( tuple_value&& ) = default;

      template< typename U >
      tuple_value& operator=( U&& v )
        noexcept( std::is_nothrow_assignable< T&, U >::value )
      {
        T::operator=( std::forward< U >( v ) );
        return *this;
      }

      void swap( tuple_value& v )
        noexcept( is_nothrow_swappable< T >::value )
      {
        using std::swap;
        swap( *this, v );
      }

      TAOCPP_TUPLE_CONSTEXPR
      T& get() noexcept
      {
        return static_cast< T& >( *this );
      }

      TAOCPP_TUPLE_CONSTEXPR
      const T& get() const noexcept
      {
        return static_cast< const T& >( *this );
      }
    };

    template< typename, typename... >
    struct tuple_base;

    template< std::size_t... Is, typename... Ts >
    struct tuple_base< seq::index_sequence< Is... >, Ts... >
      : tuple_value< Is, Ts >...
    {
      constexpr tuple_base() = default;

      template< typename... Us >
      TAOCPP_TUPLE_CONSTEXPR
      explicit tuple_base( Us&&... us )
        : tuple_value< Is, Ts >( std::forward< Us >( us ) )...
      {}

      template< typename A, typename... Us >
      TAOCPP_TUPLE_CONSTEXPR
      tuple_base( std::allocator_arg_t, const A& a, Us&&... us )
        : tuple_value< Is, Ts >( uses_alloc_ctor_t< Ts, A, Us >(), a, std::forward< Us >( us ) )...
      {}

      tuple_base( const tuple_base& ) = default;
      tuple_base( tuple_base&& ) = default;

      tuple_base& operator=( const tuple_base& v )
        noexcept( seq::is_all< std::is_nothrow_copy_assignable< Ts >::value... >::value )
      {
#ifdef TAOCPP_FOLD_EXPRESSIONS
        ( tuple_value< Is, Ts >::operator=( static_cast< tuple_value< Is, Ts >& >( v ).get() ), ... );
#else
        (void)swallow{ ( tuple_value< Is, Ts >::operator=( static_cast< tuple_value< Is, Ts >& >( v ).get() ), true )..., true };
#endif
        return *this;
      }

      tuple_base& operator=( tuple_base&& v )
        noexcept( seq::is_all< std::is_nothrow_move_assignable< Ts >::value... >::value )
      {
#ifdef TAOCPP_FOLD_EXPRESSIONS
        ( tuple_value< Is, Ts >::operator=( std::forward< Ts >( static_cast< tuple_value< Is, Ts >& >( v ).get() ) ), ... );
#else
        (void)swallow{ ( tuple_value< Is, Ts >::operator=( static_cast< tuple_value< Is, Ts >& >( v ) ), true )..., true };
#endif
        return *this;
      }

      template< typename... Us >
      tuple_base& operator=( const tuple< Us... >& v )
        noexcept( seq::is_all< std::is_nothrow_assignable< Ts&, const Us& >::value... >::value )
      {
#ifdef TAOCPP_FOLD_EXPRESSIONS
        ( tuple_value< Is, Ts >::operator=( get< Is >( v ) ), ... );
#else
        (void)swallow{ ( tuple_value< Is, Ts >::operator=( get< Is >( v ) ), true )..., true };
#endif
        return *this;
      }

      template< typename... Us >
      tuple_base& operator=( tuple< Us... >&& v )
        noexcept( seq::is_all< std::is_nothrow_assignable< Ts&, Us&& >::value... >::value )
      {
#ifdef TAOCPP_FOLD_EXPRESSIONS
        ( tuple_value< Is, Ts >::operator=( get< Is >( std::move( v ) ) ), ... );
#else
        (void)swallow{ ( tuple_value< Is, Ts >::operator=( get< Is >( std::move( v ) ) ), true )..., true };
#endif
        return *this;
      }

      void swap( tuple_base& v )
        noexcept( seq::is_all< impl::is_nothrow_swappable< Ts >::value... >::value )
      {
#ifdef TAOCPP_FOLD_EXPRESSIONS
        ( static_cast< tuple_value< Is, Ts >& >( *this ).swap( static_cast< tuple_value< Is, Ts >& >( v ) ), ... );
#else
        (void)swallow{ ( static_cast< tuple_value< Is, Ts >& >( *this ).swap( static_cast< tuple_value< Is, Ts >& >( v ) ), true )..., true };
#endif
      }
    };
  }

  // 20.4.2 Class template tuple [tuple.tuple]

  // tuple
  template< typename... Ts >
  struct tuple
  {
  private:
    using base_t = impl::tuple_base< seq::index_sequence_for< Ts... >, Ts... >;
    base_t base;

    template< std::size_t I, typename... Us >
    friend TAOCPP_TUPLE_CONSTEXPR
    const seq::type_by_index_t< I, Us... >& get( const tuple< Us... >& ) noexcept;

    template< std::size_t I, typename... Us >
    friend TAOCPP_TUPLE_CONSTEXPR
    seq::type_by_index_t< I, Us... >& get( tuple< Us... >& ) noexcept;

    template< std::size_t I, typename... Us >
    friend TAOCPP_TUPLE_CONSTEXPR
    seq::type_by_index_t< I, Us... >&& get( tuple< Us... >&& ) noexcept;

  public:
    // 20.4.2.1 Construction [tuple.cnstr]

    // TODO: Move this templated condition to base?
    template< typename dummy = void,
              typename = impl::enable_if_t< seq::is_all< impl::dependent_type< std::is_default_constructible< Ts >, dummy >::value... >::value > >
    constexpr tuple()
      noexcept( seq::is_all< std::is_nothrow_default_constructible< Ts >::value... >::value )
      : base()
    {}

    template< typename dummy = void,
              typename = impl::enable_if_t< seq::is_all< impl::dependent_type< std::is_copy_constructible< Ts >, dummy >::value... >::value > >
    TAOCPP_TUPLE_CONSTEXPR
    explicit tuple( const Ts&... ts )
      noexcept( seq::is_all< std::is_nothrow_copy_constructible< Ts >::value... >::value )
      : base( ts... )
    {}

    template< typename... Us,
              typename = impl::enable_if_t< sizeof...( Us ) == sizeof...( Ts ) >,
              typename = impl::enable_if_t< seq::is_all< std::is_constructible< Ts, Us&& >::value... >::value > >
    TAOCPP_TUPLE_CONSTEXPR
    explicit tuple( Us&&... us )
      noexcept( seq::is_all< std::is_nothrow_constructible< Ts, Us&& >::value... >::value )
      : base( std::forward< Us >( us )... )
    {}

    tuple( const tuple& ) = default;
    tuple( tuple&& ) = default;

    template< typename... Us,
              typename = impl::enable_if_t< sizeof...( Us ) == sizeof...( Ts ) >,
              typename = impl::enable_if_t< seq::is_all< std::is_constructible< Ts, const Us& >::value... >::value > >
    TAOCPP_TUPLE_CONSTEXPR
    explicit tuple( const tuple< Us... >& v )
      noexcept( seq::is_all< std::is_nothrow_constructible< Ts, const Us& >::value... >::value )
      : base( v )
    {}

    template< typename... Us,
              typename = impl::enable_if_t< sizeof...( Us ) == sizeof...( Ts ) >,
              typename = impl::enable_if_t< seq::is_all< std::is_constructible< Ts, Us&& >::value... >::value > >
    TAOCPP_TUPLE_CONSTEXPR
    explicit tuple( tuple< Us... >&& v )
      noexcept( seq::is_all< std::is_nothrow_constructible< Ts, Us&& >::value... >::value )
      : base( std::move( v ) )
    {}

    template< typename A,
              typename dummy = void,
              typename = impl::enable_if_t< seq::is_all< impl::dependent_type< std::is_default_constructible< Ts >, dummy >::value... >::value > >
    tuple( std::allocator_arg_t, const A& a )
      : base( std::allocator_arg_t(), a )
    {}

    template< typename A,
              typename dummy = void,
              typename = impl::enable_if_t< seq::is_all< impl::dependent_type< std::is_copy_constructible< Ts >, dummy >::value... >::value > >
    tuple( std::allocator_arg_t, const A& a, const Ts&... ts )
      : base( std::allocator_arg_t(), a, ts... )
    {}

    template< typename A,
              typename... Us,
              typename = impl::enable_if_t< sizeof...( Us ) == sizeof...( Ts ) >,
              typename = impl::enable_if_t< seq::is_all< std::is_constructible< Ts, Us&& >::value... >::value > >
    tuple( std::allocator_arg_t, const A& a, Us&&... us )
      : base( std::allocator_arg_t(), a, std::forward< Us >( us )... )
    {}

    template< typename A >
    tuple( std::allocator_arg_t, const A& a, const tuple& v )
      : base( std::allocator_arg_t(), a, v )
    {}

    template< typename A >
    tuple( std::allocator_arg_t, const A& a, tuple&& v )
      : base( std::allocator_arg_t(), a, std::move( v ) )
    {}

    template< typename A,
              typename... Us,
              typename = impl::enable_if_t< sizeof...( Us ) == sizeof...( Ts ) >,
              typename = impl::enable_if_t< seq::is_all< std::is_constructible< Ts, const Us& >::value... >::value > >
    tuple( std::allocator_arg_t, const A& a, const tuple< Us... >& v )
      : base( std::allocator_arg_t(), a, v )
    {}

    template< typename A,
              typename... Us,
              typename = impl::enable_if_t< sizeof...( Us ) == sizeof...( Ts ) >,
              typename = impl::enable_if_t< seq::is_all< std::is_constructible< Ts, Us&& >::value... >::value > >
    tuple( std::allocator_arg_t, const A& a, tuple< Us... >&& v )
      : base( std::allocator_arg_t(), a, std::move( v ) )
    {}

    // 20.4.2.2 Assignment [tuple.assign]

    template< typename T,
              typename = impl::enable_if_t< std::is_assignable< base_t&, T >::value > >
    tuple& operator=( T&& v )
      noexcept( std::is_nothrow_assignable< base_t&, T >::value )
    {
      base = std::forward< T >( v );
      return *this;
    }

    // 20.4.2.3 swap [tuple.swap]

    void swap( tuple& v )
      noexcept( noexcept( base.swap( v.base ) ) )
    {
      base.swap( v.base );
    }
  };

  template<>
  struct tuple<>
  {
    constexpr tuple() noexcept {}
    template< typename A > tuple( std::allocator_arg_t, const A& ) noexcept {}
    template< typename A > tuple( std::allocator_arg_t, const A&, const tuple& ) noexcept {}
    void swap( tuple& ) noexcept {}
  };

  // 20.4.2.4 Tuple creation functions [tuple.creation]

  // ignore helper
  namespace impl
  {
    struct ignore_t
    {
      template< typename U >
      ignore_t& operator=( U&& )
      {
        return *this;
      }
    };
  }

  // ignore
  const impl::ignore_t ignore{};

  // make_tuple helper
  namespace impl
  {
    template< typename T >
    struct make_tuple_return
    {
      using type = T;
    };

    template< typename T >
    struct make_tuple_return< std::reference_wrapper< T > >
    {
      using type = T&;
    };

    template< typename T >
    using make_tuple_return_t = typename make_tuple_return< T >::type;
  }

  // make_tuple
  template< typename... Ts, typename R = tuple< impl::make_tuple_return_t< typename std::decay< Ts >::type >... > >
  TAOCPP_TUPLE_CONSTEXPR
  R make_tuple( Ts&&... ts )
  {
    return R( std::forward< Ts >( ts )... );
  }

  // forward_as_tuple
  template< typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  tuple< Ts&&... > forward_as_tuple( Ts&&... ts ) noexcept
  {
    return tuple< Ts&&... >( std::forward< Ts >( ts )... );
  }

  // tie
  template< typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  tuple< Ts&... > tie( Ts&... ts ) noexcept
  {
    return tuple< Ts&... >( ts... );
  }

  // tuple_cat is found at the end, as it requires access to tuple_element_t and get<I>

  // 20.4.2.5 Tuple helper classes [tuple.helper]

  // tuple_size
  template< typename T > struct tuple_size;
  template< typename T > struct tuple_size< const T > : tuple_size< T > {};
  template< typename T > struct tuple_size< volatile T > : tuple_size< T > {};
  template< typename T > struct tuple_size< const volatile T > : tuple_size< T > {};

  template< typename... Ts >
  struct tuple_size< tuple< Ts... > >
    : std::integral_constant< std::size_t, sizeof...( Ts ) >
  {};

  // tuple_element
  template< std::size_t I, typename T > struct tuple_element;
  template< std::size_t I, typename T > struct tuple_element< I, const T > : tuple_element< I, T > {};
  template< std::size_t I, typename T > struct tuple_element< I, volatile T > : tuple_element< I, T > {};
  template< std::size_t I, typename T > struct tuple_element< I, const volatile T > : tuple_element< I, T > {};

  template< std::size_t I, typename... Ts >
  struct tuple_element< I, tuple< Ts... > >
    : seq::type_by_index< I, Ts... >
  {};

#if __cplusplus >= 201402L
  template< std::size_t I, typename T >
  using tuple_element_t = typename tuple_element< I, T >::type;
#endif

  // 20.4.2.6 Element access [tuple.elem]

  // get<I>
  template< std::size_t I, typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  const seq::type_by_index_t< I, Ts... >& get( const tuple< Ts... >& v ) noexcept
  {
    return static_cast< const impl::tuple_value< I, seq::type_by_index_t< I, Ts... > >& >( v.base ).get();
  }

  template< std::size_t I, typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  seq::type_by_index_t< I, Ts... >& get( tuple< Ts... >& v ) noexcept
  {
    return static_cast< impl::tuple_value< I, seq::type_by_index_t< I, Ts... > >& >( v.base ).get();
  }

  template< std::size_t I, typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  seq::type_by_index_t< I, Ts... >&& get( tuple< Ts... >&& v ) noexcept
  {
    using type = seq::type_by_index_t< I, Ts... >;
    return static_cast< type&& >( static_cast< impl::tuple_value< I, type >& >( v.base ).get() );
  }

  // get<T> helper
  namespace impl
  {
    template< typename T, typename... Ts >
    using count_of = seq::sum< std::size_t, ( std::is_same< T, Ts >::value ? 1 : 0 )... >;

    template< typename, typename, typename... >
    struct index_of_impl;

    template< std::size_t... Is, typename T, typename... Ts >
    struct index_of_impl< seq::index_sequence< Is... >, T, Ts... >
      : seq::sum< std::size_t, ( std::is_same< T, Ts >::value ? Is : 0 )... >
    {
      static_assert( count_of< T, Ts... >::value > 0, "T not found within Ts..." );
      static_assert( count_of< T, Ts... >::value < 2, "T must be unique within Ts..." );
    };

    template< typename T, typename... Ts >
    using index_of = index_of_impl< seq::index_sequence_for< Ts... >, T, Ts... >;
  }

  // get<T>
  template< typename T, typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  const T& get( const tuple< Ts... >& v ) noexcept
  {
    return get< impl::index_of< T, Ts... >::value >( v );
  }

  template< typename T, typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  T& get( tuple< Ts... >& v ) noexcept
  {
    return get< impl::index_of< T, Ts... >::value >( v );
  }

  template< typename T, typename... Ts >
  TAOCPP_TUPLE_CONSTEXPR
  T&& get( tuple< Ts... >&& v ) noexcept
  {
    return get< impl::index_of< T, Ts... >::value >( std::move( v ) );
  }

  // 20.4.2.7 Relational operators [tuple.rel]

  // operators helper
  namespace impl
  {
    template< typename >
    struct tuple_equal;

    template< std::size_t... Is >
    struct tuple_equal< seq::index_sequence< Is... > >
    {
      template< typename T, typename U >
      TAOCPP_TUPLE_CONSTEXPR
      bool operator()( const T& lhs, const U& rhs ) const
      {
#ifdef TAOCPP_FOLD_EXPRESSIONS
        return ( static_cast< bool >( get< Is >( lhs ) == get< Is >( rhs ) ) && ... );
#else
        bool result = true;
        (void)swallow{ ( result = result && static_cast< bool >( get< Is >( lhs ) == get< Is >( rhs ) ) )..., true };
        return result;
#endif
      }
    };

    template< typename >
    struct tuple_less;

    template< std::size_t... Is >
    struct tuple_less< seq::index_sequence< Is... > >
    {
      template< typename T, typename U >
      TAOCPP_TUPLE_CONSTEXPR
      bool operator()( const T& lhs, const U& rhs ) const
      {
        bool result = false;
#ifdef TAOCPP_DUMMY // TAOCPP_FOLD_EXPRESSIONS
        // TODO: This fold expression does not work as expected. Why?
        (void)( ( ( result = static_cast< bool >( get< Is >( lhs ) < get< Is >( rhs ) ) ) || static_cast< bool >( get< Is >( rhs ) < get< Is >( lhs ) ) ) || ... );
#else
        bool no_result_yet = false;
        (void)swallow{ ( no_result_yet = no_result_yet || ( result = static_cast< bool >( get< Is >( lhs ) < get< Is >( rhs ) ) ) || static_cast< bool >( get< Is >( rhs ) < get< Is >( lhs ) ) )..., true };
        (void)no_result_yet;
#endif
        return result;
      }
    };
  }

  // operators
  template< typename... Ts, typename... Us, typename = impl::enable_if_t< sizeof...( Ts ) == sizeof...( Us ) > >
  TAOCPP_TUPLE_CONSTEXPR
  bool operator==( const tuple< Ts... >& lhs, const tuple< Us... >& rhs )
  {
    return impl::tuple_equal< seq::index_sequence_for< Ts... > >()( lhs, rhs );
  }

  template< typename... Ts, typename... Us >
  TAOCPP_TUPLE_CONSTEXPR
  bool operator!=( const tuple< Ts... >& lhs, const tuple< Us... >& rhs )
  {
    return !( lhs == rhs );
  }

  template< typename... Ts, typename... Us, typename = impl::enable_if_t< sizeof...( Ts ) == sizeof...( Us ) > >
  TAOCPP_TUPLE_CONSTEXPR
  bool operator<( const tuple< Ts... >& lhs, const tuple< Us... >& rhs )
  {
    return impl::tuple_less< seq::index_sequence_for< Ts... > >()( lhs, rhs );
  }

  template< typename... Ts, typename... Us >
  TAOCPP_TUPLE_CONSTEXPR
  bool operator>=( const tuple< Ts... >& lhs, const tuple< Us... >& rhs )
  {
    return !( lhs < rhs );
  }

  template< typename... Ts, typename... Us >
  TAOCPP_TUPLE_CONSTEXPR
  bool operator>( const tuple< Ts... >& lhs, const tuple< Us... >& rhs )
  {
    return rhs < lhs;
  }

  template< typename... Ts, typename... Us >
  TAOCPP_TUPLE_CONSTEXPR
  bool operator<=( const tuple< Ts... >& lhs, const tuple< Us... >& rhs )
  {
    return !( rhs < lhs );
  }

  // 20.4.2.9 Tuple specialized algorithms [tuple.special]

  // swap
  template< typename... Ts >
  void swap( tuple< Ts... >& lhs, tuple< Ts... >& rhs )
    noexcept( noexcept( lhs.swap( rhs ) ) )
  {
    lhs.swap( rhs );
  }

  // (continued:) 20.4.2.4 Tuple creation functions [tuple.creation]

  // tuple_cat helper
  namespace impl
  {
    template< std::size_t M, std::size_t... Ns >
    struct count_less_or_equal
      : seq::sum< std::size_t, ( ( Ns <= M ) ? 1 : 0 )... >
    {};

    template< typename, typename >
    struct expand;

    template< std::size_t... Is, std::size_t... Ns >
    struct expand< seq::index_sequence< Is... >, seq::index_sequence< Ns... > >
    {
      using type = seq::index_sequence< count_less_or_equal< Is, Ns... >::value... >;
    };

    template< typename I, typename S >
    using expand_t = typename expand< I, S >::type;

    template< typename... >
    struct tuple_cat_result;

    template< std::size_t... Os, std::size_t... Is, typename... Ts >
    struct tuple_cat_result< seq::index_sequence< Os... >, seq::index_sequence< Is... >, Ts... >
    {
      using type = tuple< typename tuple_element< Is, seq::type_by_index_t< Os, Ts... > >::type... >;
    };

    template< typename... Ts >
    using tuple_cat_result_t = typename tuple_cat_result< Ts... >::type;

    template< typename... Ts >
    struct tuple_cat_helper
    {
      using tuple_size_sequence = seq::index_sequence< tuple_size< Ts >::value... >;
      using result_index_sequence = seq::make_index_sequence< seq::sum< tuple_size_sequence >::value >;

      using outer_index_sequence = expand_t< result_index_sequence, seq::inclusive_scan_t< tuple_size_sequence > >;
      using inner_index_sequence = seq::minus_t< result_index_sequence, seq::map_t< outer_index_sequence, seq::exclusive_scan_t< tuple_size_sequence > > >;

      using result_type = tuple_cat_result_t< outer_index_sequence, inner_index_sequence, Ts... >;
    };

    template< typename R, std::size_t... Os, std::size_t... Is, typename T >
    TAOCPP_TUPLE_CONSTEXPR
    R tuple_cat( seq::index_sequence< Os... >, seq::index_sequence< Is... >, T v )
    {
      return R( get< Is >( get< Os >( v ) )... );
    }
  }

  // tuple_cat
  template< typename... Ts, typename H = impl::tuple_cat_helper< typename std::remove_reference< Ts >::type... >, typename R = typename H::result_type >
  TAOCPP_TUPLE_CONSTEXPR
  R tuple_cat( Ts&&... ts )
  {
    return impl::tuple_cat< R >( typename H::outer_index_sequence(), typename H::inner_index_sequence(), tao::forward_as_tuple( std::forward< Ts >( ts )... ) );
  }
}

#undef TAOCPP_TUPLE_CONSTEXPR

#endif // TAOCPP_INCLUDE_TUPLE_TUPLE_HPP
