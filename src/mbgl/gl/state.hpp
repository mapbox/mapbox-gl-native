#pragma once

#include <tuple>

namespace mbgl {
namespace gl {

// Wraps a piece of OpenGL state and remember its value to avoid redundant state calls.
// Wrapped types need to implement to the Value class interface:
//
// class Value {
//     using Type = ...;
//     static const constexpr Type Default = ...;
//     static void Set(const Type& value);
//     static Type Get();
// };
template <typename T, typename... Args>
class State {
public:
    State(Args&&... args) : params(std::forward_as_tuple(::std::forward<Args>(args)...)) {
    }

    void operator=(const typename T::Type& value) {
        if (*this != value) {
            setCurrentValue(value);
            set(std::index_sequence_for<Args...>{});
        }
    }

    bool operator==(const typename T::Type& value) const {
        return !(*this != value);
    }

    bool operator!=(const typename T::Type& value) const {
        return dirty || currentValue != value;
    }

    void setCurrentValue(const typename T::Type& value) {
        dirty = false;
        currentValue = value;
    }

    // Mark the state as dirty. This means that the next time we are assigning a value to this
    // piece of OpenGL state will always result in an actual OpenGL call.
    void setDirty() {
        dirty = true;
    }

    typename T::Type getCurrentValue() const {
        return currentValue;
    }

    bool isDirty() const {
        return dirty;
    }

private:
    template <std::size_t... I>
    void set(std::index_sequence<I...>) {
        T::Set(currentValue, std::get<I>(params)...);
    }

private:
    typename T::Type currentValue = T::Default;
    bool dirty = true;
    const std::tuple<Args...> params;
};

} // namespace gl
} // namespace mbgl
