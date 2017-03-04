#pragma once

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
template <typename T>
class State {
public:
    void operator=(const typename T::Type& value) {
        if (*this != value) {
            setCurrentValue(value);
            T::Set(currentValue);
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
    typename T::Type currentValue = T::Default;
    bool dirty = true;
};

// Helper struct that stores the current state and restores it upon destruction. You should not use
// this code normally, except for debugging purposes.
template <typename T>
class PreserveState {
public:
    PreserveState() : value(T::Get()) {
    }
    ~PreserveState() {
        T::Set(value);
    }

private:
    const typename T::Type value;
};

} // namespace gl
} // namespace mbgl
