#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace mbgl {
namespace util {

class peer {
public:
    peer() noexcept : storage(nullptr, noop_deleter) {}

    template <class T>
    peer(T&& value) noexcept : storage(new std::decay_t<T>(std::forward<T>(value)), cast_deleter<std::decay_t<T>>) {
        static_assert(!std::is_same<peer, std::decay_t<T>>::value, "Peer must not wrap itself.");
    }

    bool has_value() const noexcept { return static_cast<bool>(storage); }

    template <class T>
    T& get() noexcept { return *reinterpret_cast<T*>(storage.get()); }

private:
    template <typename T>
    static void cast_deleter(void* ptr) noexcept { delete reinterpret_cast<T*>(ptr); }
    static void noop_deleter(void*) noexcept {}

    using storage_t = std::unique_ptr<void, void(*)(void*)>;
    storage_t storage;
};

} // namespace util
} // namespace mbgl
