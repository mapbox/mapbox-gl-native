#pragma once

#include <type_traits>
#include <utility>

namespace mbgl {
namespace util {

class peer {
public:
    peer() = default;
    peer(const peer&) = delete;

    peer(peer&& other)
        : vtable(other.vtable)
    {
        if (vtable) {
            vtable->move(other.storage, storage);
        }
        other.vtable = nullptr;
    }

    template <class T>
    peer(T&& value) {
        using _Vt = std::decay_t<T>;
        vtable = get_vtable<_Vt>();
        new (&storage) _Vt(std::forward<T>(value));
    }

    ~peer() {
        reset();
    }

    peer& operator=(peer&& rhs) {
        peer(std::move(rhs)).swap(*this);
        return *this;
    }

    void reset()  {
        if (vtable) {
            vtable->destroy(storage);
            vtable = nullptr;
        }
    }

    void swap(peer& rhs)  {
        if (this == &rhs) {
            return;
        } else {
            peer tmp(std::move(rhs));
            rhs.vtable = vtable;
            if (rhs.vtable) {
                rhs.vtable->move(storage, rhs.storage);
            }
            vtable = tmp.vtable;
            if (vtable) {
                vtable->move(tmp.storage, storage);
            }
        }
    }

    bool has_value() const {
        return vtable != nullptr;
    }

    template <class T>
    T& get() {
        return reinterpret_cast<T&>(storage);
    }

    template <class T>
    T&& take() {
        reset();
        return std::move(get<T>());
    }

private:
    using storage_t = std::aligned_storage_t<2*sizeof(void*), alignof(void*)>;

    struct vtable {
        virtual ~vtable() = default;
        virtual void move(storage_t&, storage_t&) = 0;
        virtual void destroy(storage_t&) = 0;
    };

    template <class T>
    struct vtable_impl : public vtable {
        static_assert(sizeof(T) <= sizeof(storage_t), "peer object is too big");

        void move(storage_t& src, storage_t& dst) override {
            new (&dst) T(std::move(reinterpret_cast<T&>(src)));
            destroy(src);
        }

        void destroy(storage_t& s) override {
            reinterpret_cast<T&>(s).~T();
        }
    };

    template <class T>
    static vtable* get_vtable() {
        static vtable_impl<T> vtable;
        return &vtable;
    }

    vtable* vtable = nullptr;
    storage_t storage;
};

} // namespace util
} // namespace mbgl
