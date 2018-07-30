#pragma once

#include <typeinfo>
#include <type_traits>
#include <stdexcept>
namespace mbgl {
namespace util {

class bad_any_cast : public std::bad_cast {
public:
    const char* what() const noexcept override {
        return "bad any_cast<>()";
    }
};
/**
 * A variant of `std::any` for non-copyable types.
 *
 * Use `unique_any` for non-copyable types (e.g. `std::unique_ptr<T>`)
 * or to ensure that no copies are made of copyable types that are
 * moved in.
 *
 * `uniqe_any` differs from `std::any` in that it does not support copy construction
 * or copy assignment. It also does not require the contained type to be copy
 * constructible.
 *
 * The `any_cast<T>()` methods work similar to `std::any_cast<T>()` except that
 * non-copyable types may only be cast to references.
 *
 * Example usage:
 *  unique_any u1(3);
 *  auto u2 = unique_any(std::move(u1)); // u1 is moved from
 *  int i = any_cast<int>(u2);
 *
 *  unique_any u2;
 *  u2 = std::unique_ptr<int>(new int);
 *  std::unique_ptr<int> iPtr = any_cast<std::unique_ptr<int>>(std::move(u2));
 *
 * Inspired by linb::any (https://github.com/thelink2012/any) and the
 * libc++ implementation (https://github.com/llvm-mirror/libcxx).
 */
class unique_any final
{
public:
    unique_any() = default;

    //Copy constructor (deleted)
    unique_any(const unique_any& rhs) = delete;

    unique_any(unique_any&& rhs)  : vtable(rhs.vtable) {
        if (vtable) {
            vtable->move(std::move(rhs.storage), storage);
        }
        rhs.vtable = nullptr;
    }

    // Constructs with a direct-initilizated object of type ValueType
    template <typename ValueType,
        typename _Vt = std::decay_t<ValueType>,
        typename = std::enable_if_t<!std::is_same<_Vt, unique_any>::value> >
    unique_any(ValueType&& value) {
        create(std::forward<ValueType>(value));
    }

    ~unique_any() {
        reset();
    }

    unique_any& operator=(unique_any&& rhs)  {
        unique_any(std::move(rhs)).swap(*this);
        return *this;
    }

    template <class ValueType,
        typename = std::enable_if_t<!std::is_same<std::decay_t<ValueType>, unique_any>::value> >
    unique_any& operator=(ValueType&& rhs) {
        unique_any(std::forward<ValueType>(rhs)).swap(*this);
        return *this;
    }

    void reset()  {
        if (vtable) {
            vtable->destroy(storage);
            vtable = nullptr;
        }
    }

    void swap(unique_any& rhs)  {
        if (this == &rhs) {
            return;
        } else {
            unique_any tmp(std::move(rhs));
            rhs.vtable = vtable;
            if (rhs.vtable) {
                rhs.vtable->move(std::move(storage), rhs.storage);
            }
            vtable = tmp.vtable;
            if (vtable) {
                vtable->move(std::move(tmp.storage), storage);
            }
        }
    }

    const std::type_info& type() const  {
      return !has_value()? typeid(void) : vtable->type();
    }

    bool has_value() const  {
        return vtable != nullptr;
    }

private:

    union Storage {
        using StackStorage = std::aligned_storage_t<3*sizeof(void*), std::alignment_of<void*>::value>;
        Storage() = default;

        void * dynamic { nullptr };
        StackStorage stack;
    };

    template<typename T>
    struct AllocateOnStack : std::integral_constant<bool,
        sizeof(T) <= sizeof(Storage::stack)
        && std::alignment_of<T>::value <= std::alignment_of<Storage::StackStorage>::value
        && std::is_nothrow_move_constructible<T>::value> {
    };

    struct VTable {
        virtual ~VTable() = default;
        virtual void move(Storage&& src, Storage& dest) = 0;
        virtual void destroy(Storage&) = 0;
        virtual const std::type_info& type() = 0;
    };
  
    template <typename ValueType>
    struct VTableHeap : public VTable {
        void move(Storage&& src, Storage& dest) override {
            dest.dynamic = src.dynamic;
            src.dynamic = nullptr;
        }

        void destroy(Storage& s) override {
            delete reinterpret_cast<ValueType*>(s.dynamic);
        }

        const std::type_info& type() override {
            return typeid(ValueType);
        }
    };

    template <typename ValueType>
    struct VTableStack : public VTable {
        void move(Storage&& src, Storage& dest) override {
            new (&dest.stack) ValueType(std::move(reinterpret_cast<ValueType&>(src.stack)));
            destroy(src);
        }

        void destroy(Storage& s) override {
            reinterpret_cast<ValueType&>(s.stack).~ValueType();
        }

        const std::type_info& type() override {
            return typeid(ValueType);
        }
    };

    template <typename ValueType>
    static VTable* vtableForType() {
        using VTableType = std::conditional_t<AllocateOnStack<ValueType>::value, VTableStack<ValueType>, VTableHeap<ValueType> >;
        static VTableType vtable;
        return &vtable;
    }

    template <typename ValueType, typename _Vt>
    std::enable_if_t<AllocateOnStack<_Vt>::value>
    createStorage(ValueType&& value) {
        new (&storage.stack) _Vt(std::forward<ValueType>(value));
    }

    template <typename ValueType, typename _Vt>
    std::enable_if_t<!AllocateOnStack<_Vt>::value>
    createStorage(ValueType&& value) {
        storage.dynamic = new _Vt(std::forward<ValueType>(value));
    }

    template <typename ValueType>
    void create(ValueType&& value) {
        using _Vt = std::decay_t<ValueType>;
        vtable = vtableForType<_Vt>();
        createStorage<ValueType, _Vt>(std::forward<ValueType>(value));
    }

    VTable* vtable { nullptr };
    Storage storage;

protected:
    template<class ValueType>
    friend const ValueType* any_cast(const unique_any* operand) ;

    template<class ValueType>
    friend ValueType* any_cast(unique_any* operand) ;

    template<typename ValueType, typename _Vt = std::decay_t<ValueType> >
    ValueType* cast()
    {
        return reinterpret_cast<ValueType *>(
            AllocateOnStack<_Vt>::value ? &storage.stack : storage.dynamic);
    }
};

template<typename ValueType>
inline const ValueType* any_cast(const unique_any* any)
{
    return any_cast<ValueType>(const_cast<unique_any *>(any));
}

template<typename ValueType>
inline ValueType* any_cast(unique_any* any)
{
    if(any == nullptr || any->type() != typeid(ValueType))
        return nullptr;
    else
        return any->cast<ValueType>();
}

template<typename ValueType, typename _Vt = std::decay_t<ValueType> >
inline ValueType any_cast(const unique_any& any)
{
    static_assert(std::is_constructible<ValueType, const _Vt&>::value,
        "any_cast type can't construct copy of contained object");
    auto temp = any_cast<_Vt>(&any);
    if (temp == nullptr) {
        throw bad_any_cast();
    }
    return static_cast<ValueType>(*temp);
}

template<typename ValueType, typename _Vt = std::decay_t<ValueType> >
inline ValueType any_cast(unique_any& any)
{
    static_assert(std::is_constructible<ValueType, const _Vt&>::value,
        "any_cast type can't construct copy of contained object");
    auto temp = any_cast<_Vt>(&any);
    if (temp == nullptr) {
        throw bad_any_cast();
    }
    return static_cast<ValueType>(*temp);
}

template<typename ValueType, typename _Vt = std::remove_cv_t<ValueType> >
inline ValueType any_cast(unique_any&& any)
{
    auto temp = any_cast<_Vt>(&any);
    if (temp == nullptr) {
        throw bad_any_cast();
    }
    auto retValue = static_cast<ValueType>(std::move(*temp));
    any.reset();
    return std::move(retValue);
}

} // namespace util
} // namespace mbgl

namespace std {

inline void swap(mbgl::util::unique_any& lhs, mbgl::util::unique_any& rhs)  {
    lhs.swap(rhs);
}

} // namespace std
