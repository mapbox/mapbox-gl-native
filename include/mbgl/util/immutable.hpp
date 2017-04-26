#pragma once

#include <memory>

namespace mbgl {

/**
 * `Mutable<T>` is a non-nullable uniquely owning reference to a `T`. It can be efficiently converted
 * to `Immutable<T>`.
 * 
 * The lifecycle of `Mutable<T>` and `Immutable<T>` is as follows:
 * 
 *   1. Create a `Mutable<T>` using `makeMutable(...)`
 *   2. Mutate it freely
 *   3. When you're ready to freeze its state and enable safe cross-thread sharing, move assign or
 *      move construct it to `Immutable<T>`
 *
 * The reason that `Mutable<T>` exists, rather than simply using a `std::unique_ptr<T>`, is to take advantage
 * of the underlying single-allocation optimization provided by `std::make_shared`.
 */
template <class T>
class Mutable {
public:
    Mutable(Mutable&&) = default;
    Mutable& operator=(Mutable&&) = default;

    Mutable(const Mutable&) = delete;
    Mutable& operator=(const Mutable&) = delete;

    T* get() { return ptr.get(); }
    T* operator->() { return ptr.get(); }
    T& operator*() { return *ptr; }

private:
    Mutable(std::shared_ptr<T>&& s)
        : ptr(std::move(s)) {}

    std::shared_ptr<T> ptr;

    template <class S> friend class Immutable;
    template <class S, class... Args> friend Mutable<S> makeMutable(Args&&...);
};

template <class T, class... Args>
Mutable<T> makeMutable(Args&&... args) {
    return Mutable<T>(std::make_shared<T>(std::forward<Args>(args)...));
}

/**
 * `Immutable<T>` is a non-nullable shared reference to a `const T`. Construction requires
 * a transfer of unique ownership from a `Mutable<T>`; once constructed it has the same behavior
 * as `std::shared_ptr<const T>` but with better indication of intent.
 *
 * Normally one should not share state between threads because it's difficult to verify the
 * absence of read/write data races. `Immutable` provides a guarantee that no writes are
 * possible, and instances therefore can be freely transferred and shared between threads.
 */
template <class T>
class Immutable {
public:
    template <class S>
    Immutable(Mutable<S>&& s)
        : ptr(std::const_pointer_cast<const S>(std::move(s.ptr))) {}

    template <class S>
    Immutable(Immutable<S>&& s)
        : ptr(std::move(s.ptr)) {}

    template <class S>
    Immutable(const Immutable<S>& s)
        : ptr(s.ptr) {}

    template <class S>
    Immutable& operator=(Mutable<S>&& s) {
        ptr = std::const_pointer_cast<const S>(std::move(s.ptr));
        return *this;
    }

    template <class S>
    Immutable& operator=(Immutable<S>&& s) {
        ptr = std::move(s.ptr);
        return *this;
    }

    template <class S>
    Immutable& operator=(const Immutable<S>& s) {
        ptr = s.ptr;
        return *this;
    }

    const T* get() const { return ptr.get(); }
    const T* operator->() const { return ptr.get(); }
    const T& operator*() const { return *ptr; }

    friend bool operator==(const Immutable<T>& lhs, const Immutable<T>& rhs) {
        return lhs.ptr == rhs.ptr;
    }

    friend bool operator!=(const Immutable<T>& lhs, const Immutable<T>& rhs) {
        return lhs.ptr != rhs.ptr;
    }

private:
    Immutable(std::shared_ptr<const T>&& s)
        : ptr(std::move(s)) {}

    std::shared_ptr<const T> ptr;

    template <class S> friend class Immutable;
    template <class S> friend class EnableImmutableFromThis;
    template <class S, class U> friend Immutable<S> staticImmutableCast(const Immutable<U>&);
};

template <class T>
class EnableImmutableFromThis : public std::enable_shared_from_this<const T> {
public:
    Immutable<T> immutableFromThis() const {
        return Immutable<T>(this->shared_from_this());
    }
};

template <class S, class U>
Immutable<S> staticImmutableCast(const Immutable<U>& u) {
    return Immutable<S>(std::static_pointer_cast<const S>(u.ptr));
}

} // namespace mbgl
