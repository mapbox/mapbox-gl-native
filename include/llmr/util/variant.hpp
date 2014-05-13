#ifndef UTIL_VARIANT_HPP
#define UTIL_VARIANT_HPP

#include <utility>
#include <typeinfo>
#include <type_traits>
#include <algorithm> // std::move/swap
#include <stdexcept> // runtime_error
#include <new> // operator new
#include <cstddef> // size_t
#include <iosfwd>

#ifdef NDEBUG
#define VARIANT_INLINE inline __attribute__((always_inline))
#else
#define VARIANT_INLINE __attribute__((noinline))
#endif

namespace llmr {

namespace util {

namespace detail {

static constexpr std::size_t invalid_value = std::size_t(-1);

template <typename T, typename...Types>
struct type_traits;

template <typename T, typename First, typename...Types>
struct type_traits<T, First, Types...>
{
    static constexpr std::size_t id = std::is_same<T, First>::value ? sizeof...(Types) : type_traits<T, Types...>::id;
};

template <typename T>
struct type_traits<T>
{
    static constexpr std::size_t id = invalid_value;
};

template <typename T, typename...Types>
struct is_valid_type;

template <typename T, typename First, typename... Types>
struct is_valid_type<T,First,Types...>
{
    static constexpr bool value = std::is_same<T,First>::value
        || is_valid_type<T,Types...>::value;
};

template <typename T>
struct is_valid_type<T> : std::false_type {};

}

template <std::size_t arg1, std::size_t ... others>
struct static_max;

template <std::size_t arg>
struct static_max<arg>
{
    static const std::size_t value = arg;
};

template <std::size_t arg1, std::size_t arg2, std::size_t ... others>
struct static_max<arg1, arg2, others...>
{
    static const std::size_t value = arg1 >= arg2 ? static_max<arg1, others...>::value :
        static_max<arg2, others...>::value;
};

template<typename... Types>
struct variant_helper;

template<typename T, typename... Types>
struct variant_helper<T, Types...>
{
    VARIANT_INLINE static void destroy(const std::size_t id, void * data)
    {
        if (id == sizeof...(Types))
        {
            reinterpret_cast<T*>(data)->~T();
        }
        else
        {
            variant_helper<Types...>::destroy(id, data);
        }
    }

    VARIANT_INLINE static void move(const std::size_t old_id, void * old_value, void * new_value)
    {
        if (old_id == sizeof...(Types))
        {
            new (new_value) T(std::move(*reinterpret_cast<T*>(old_value)));
            //std::memcpy(new_value, old_value, sizeof(T));
            // ^^  DANGER: this should only be considered for relocatable types e.g built-in types
            // Also, I don't see any measurable performance benefit just yet
        }
        else
        {
            variant_helper<Types...>::move(old_id, old_value, new_value);
        }
    }

    VARIANT_INLINE static void copy(const std::size_t old_id, const void * old_value, void * new_value)
    {
        if (old_id == sizeof...(Types))
        {
            new (new_value) T(*reinterpret_cast<const T*>(old_value));
        }
        else
        {
            variant_helper<Types...>::copy(old_id, old_value, new_value);
        }
    }
};

template<> struct variant_helper<>
{
    VARIANT_INLINE static void destroy(const std::size_t, void *) {}
    VARIANT_INLINE static void move(const std::size_t, void *, void *) {}
    VARIANT_INLINE static void copy(const std::size_t, const void *, void *) {}
};

namespace detail {

template <typename F, typename V, typename...Types>
struct dispatcher;

template <typename F, typename V, typename T, typename...Types>
struct dispatcher<F,V,T,Types...>
{
    typedef typename std::result_of<F(V const&)>::type result_type;

    VARIANT_INLINE static result_type apply(V const& v, F f)
    {
        if (v.get_type_id() == sizeof...(Types))
        {
            return f(v. template get<T>());
        }
        else
        {
            return dispatcher<F,V,Types...>::apply(v, f);
        }
    }
};

template<typename F,typename V>
struct dispatcher<F,V>
{
    typedef typename std::result_of<F(V const&)>::type result_type;

    VARIANT_INLINE static result_type apply(V const&, F)
    {
        throw std::runtime_error("unary dispatch: FAIL");
    }

};


template <typename F, typename V, typename T, typename...Types>
struct binary_dispatcher_rhs;

template <typename F, typename V, typename T0, typename T1, typename...Types>
struct binary_dispatcher_rhs<F,V,T0,T1,Types...>
{
    typedef typename std::result_of<F(V const&, V const&)>::type result_type;
    VARIANT_INLINE static result_type apply(V const& lhs, V const& rhs, F f)
    {
        if (rhs.get_type_id() == sizeof...(Types)) // call binary functor
        {
            return f(lhs. template get<T0>(), rhs. template get<T1>());
        }
        else
        {
            return binary_dispatcher_rhs<F,V,T0,Types...>::apply(lhs, rhs, f);
        }
    }
};

template<typename F,typename V, typename T>
struct binary_dispatcher_rhs<F,V,T>
{
    typedef typename std::result_of<F(V const&, V const&)>::type result_type;

    VARIANT_INLINE static result_type apply(V const&, V const&, F)
    {
        throw std::runtime_error("binary dispatch: FAIL");
    }
};


template <typename F, typename V, typename T, typename...Types>
struct binary_dispatcher_lhs;

template <typename F, typename V, typename T0, typename T1, typename...Types>
struct binary_dispatcher_lhs<F,V,T0,T1,Types...>
{
    typedef typename std::result_of<F(V const&, V const&)>::type binary_result_type;
    VARIANT_INLINE static binary_result_type apply(V const& lhs, V const& rhs, F f)
    {
        if (lhs.get_type_id() == sizeof...(Types)) // call binary functor
        {
            return f(lhs. template get<T1>(), rhs. template get<T0>());
        }
        else
        {
            return binary_dispatcher_lhs<F,V,T0,Types...>::apply(lhs, rhs, f);
        }
    }
};

template<typename F,typename V, typename T>
struct binary_dispatcher_lhs<F,V,T>
{
    typedef typename std::result_of<F(V const&, V const&)>::type result_type;

    VARIANT_INLINE static result_type apply(V const&, V const&, F)
    {
        throw std::runtime_error("binary dispatch: FAIL");
    }
};

template <typename F, typename V, typename...Types>
struct binary_dispatcher;

template <typename F, typename V, typename T, typename...Types>
struct binary_dispatcher<F,V,T,Types...>
{
    typedef typename std::result_of<F(V const&, V const&)>::type binary_result_type;

    VARIANT_INLINE static binary_result_type apply(V const& v0, V const& v1, F f)
    {
        if (v0.get_type_id() == sizeof...(Types))
        {
            if (v0.get_type_id() == v1.get_type_id())
            {
                return f(v0. template get<T>(), v1. template get<T>()); // call binary functor
            }
            else
            {
                return binary_dispatcher_rhs<F,V,T,Types...>::apply(v0, v1, f);
            }
        }
        else if (v1.get_type_id() == sizeof...(Types))
        {
            return binary_dispatcher_lhs<F,V,T,Types...>::apply(v0, v1, f);
        }
        return binary_dispatcher<F,V,Types...>::apply(v0, v1, f);
    }
};

template<typename F,typename V>
struct binary_dispatcher<F,V>
{
    typedef typename std::result_of<F(V const&, V const&)>::type binary_result_type;

    VARIANT_INLINE static binary_result_type apply(V const&, V const&, F)
    {
        throw std::runtime_error("binary dispatch: FAIL");
    }
};

// comparator functors
struct equal_comp
{
    template <typename T>
    bool operator()(T const& lhs, T const& rhs) const
    {
        return lhs == rhs;
    }
};

struct less_comp
{
    template <typename T>
    bool operator()(const T& lhs, const T& rhs) const
    {
        return lhs < rhs;
    }
};

template <typename Variant, typename Comp>
class comparer
{
public:
    explicit comparer(Variant const& lhs) noexcept
        : lhs_(lhs) {}
    comparer& operator=(const comparer&) = delete;
    // visitor
    template<typename T>
    bool operator()(T const& rhs_content) const
    {
        T const& lhs_content = lhs_.template get<T>();
        return Comp()(lhs_content, rhs_content);
    }
private:
    Variant const& lhs_;
};

// operator<< helper
template <typename Out>
class printer
{
public:
    explicit printer(Out & out)
        : out_(out) {}
    printer& operator=(printer const &) = delete;

// visitor
    template <typename T>
    void operator()( T const & operand) const
    {
        out_ << operand;
    }
private:
    Out & out_;
};

} // detail

template<typename... Types>
class variant
{
private:

    static const std::size_t data_size = static_max<sizeof(Types)...>::value;
    static const std::size_t data_align = static_max<alignof(Types)...>::value;

    using data_type = typename std::aligned_storage<data_size, data_align>::type;

    using helper_type = variant_helper<Types...>;

    std::size_t type_id;
    data_type data;

public:

    VARIANT_INLINE variant()
        : type_id(detail::invalid_value) {}

    template <typename T>
    VARIANT_INLINE explicit variant(T const& val) noexcept
        : type_id(detail::type_traits<T, Types...>::id)
    {
        static_assert(detail::is_valid_type<T,Types...>::value, "Not a valid type for this variant");
        new (&data) T(val);
    }

    template <typename T>
    VARIANT_INLINE variant(T && val) noexcept
        : type_id(detail::type_traits<T,Types...>::id)
    {
        static_assert(detail::is_valid_type<T,Types...>::value, "Not a valid type for this variant");
        new (&data) T(std::forward<T>(val)); // nothrow
    }

    VARIANT_INLINE variant(variant<Types...> const& old)
        : type_id(old.type_id)
    {
        helper_type::copy(old.type_id, &old.data, &data);
    }

    VARIANT_INLINE variant(variant<Types...>&& old) noexcept
        : type_id(old.type_id)
    {
        helper_type::move(old.type_id, &old.data, &data);
    }

    friend void swap(variant<Types...> & first, variant<Types...> & second)
    {
        using std::swap; //enable ADL
        swap(first.type_id, second.type_id);
        swap(first.data, second.data);
    }

    VARIANT_INLINE variant<Types...>& operator= (variant<Types...> other)
    {
        swap(*this, other);
        return *this;
    }

    template<typename T>
    VARIANT_INLINE bool is() const
    {
        return (type_id == detail::type_traits<T, Types...>::id);
    }

    VARIANT_INLINE bool valid() const
    {
        return (type_id != detail::invalid_value);
    }

    template<typename T, typename... Args>
    VARIANT_INLINE void set(Args&&... args)
    {
        helper_type::destroy(type_id, &data);
        new (&data) T(std::forward<Args>(args)...);
        type_id = detail::type_traits<T,Types...>::id;
    }

    template<typename T>
    VARIANT_INLINE T& get()
    {
        if (type_id == detail::type_traits<T,Types...>::id)
        {
            return *reinterpret_cast<T*>(&data);
        }
        else
        {
            throw std::runtime_error("in get()");
        }
    }

    template<typename T>
    VARIANT_INLINE T const& get() const
    {
        if (type_id == detail::type_traits<T,Types...>::id)
        {
            return *reinterpret_cast<T const*>(&data);
        }
        else
        {
            throw std::runtime_error("in get()");
        }
    }

    VARIANT_INLINE std::size_t get_type_id() const
    {
        return type_id;
    }

    // visitor
    // unary
    template <typename F, typename V>
    typename std::result_of<F(V const&)>::type
    VARIANT_INLINE static visit(V const& v, F f)
    {
        return detail::dispatcher<F, V, Types...>::apply(v, f);
    }
    // binary
    template <typename F, typename V>
    typename std::result_of<F(V const&, V const& )>::type
    VARIANT_INLINE static binary_visit(V const& v0, V const& v1, F f)
    {
        return detail::binary_dispatcher<F, V, Types...>::apply(v0, v1, f);
    }

    ~variant() noexcept
    {
        helper_type::destroy(type_id, &data);
    }

    // comparison operators
    // equality
    VARIANT_INLINE bool operator==(variant const& rhs) const
    {
        if (this->get_type_id() != rhs.get_type_id())
            return false;
        detail::comparer<variant, detail::equal_comp> visitor(*this);
        return visit(rhs, visitor);
    }
    // less than
    VARIANT_INLINE bool operator<(variant const& rhs) const
    {
        if (this->get_type_id() != rhs.get_type_id())
        {
            return this->get_type_id() < rhs.get_type_id();
            // ^^ borrowed from boost::variant
        }
        detail::comparer<variant, detail::less_comp> visitor(*this);
        return visit(rhs, visitor);
    }
};

// unary visitor interface
template <typename V, typename F>
typename std::result_of<F(V const&)>::type
VARIANT_INLINE static apply_visitor(V const& v, F f)
{
    return V::visit(v,f);
}

// binary visitor interface
template <typename V, typename F>
typename std::result_of<F(V const&, V const&)>::type
VARIANT_INLINE static apply_visitor(V const& v0, V const& v1, F f)
{
    return V::binary_visit(v0, v1, f);
}


// operator<<
template <typename charT, typename traits, typename Variant>
VARIANT_INLINE std::basic_ostream<charT,traits>&
operator<< (std::basic_ostream<charT,traits>& out, Variant const& rhs)
{
    detail::printer<std::basic_ostream<charT,traits> > visitor(out);
    apply_visitor(rhs, visitor);
    return out;
}

}
}
#endif  // UTIL_VARIANT_HPP
