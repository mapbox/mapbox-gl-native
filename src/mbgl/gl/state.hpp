#pragma once

namespace mbgl {
namespace gl {

// Helper struct that allows obtaining the default value of a Value class
template <typename T, typename = void>
struct DefaultValue {
    static typename T::Type Get() {
        return T::Get();
    }
};
template <typename T>
struct DefaultValue<T, decltype((void)T::Default, void())> {
    static typename T::Type Get() {
        return T::Default;
    }
};

// Wraps a piece of OpenGL state and remember its value to avoid redundant state calls.
// Wrapped types need to implement to the Value class interface:
//
// class Value {
//     using Type = ...;
//     static const constexpr Type Default = ...;
//     static void Set(const Type& value);
//     static Type Get();
// };
//
// The Get() function is optional, but if it is omitted, you must provide a Default.
// Default is also optional, but if it is omitted, you must provide a Get() function.
// If both are present, DefaultValue<T>::Get() will use the Default member.
template <typename T>
class State {
public:
    void operator=(const typename T::Type& value) {
        if (*this != value) {
            dirty = false;
            currentValue = value;
            T::Set(currentValue);
        }
    }

    bool operator==(const typename T::Type& value) const {
        return !(*this != value);
    }

    bool operator!=(const typename T::Type& value) const {
        return dirty || currentValue != value;
    }

    // Explicitly resets the piece of OpenGL state to its default value.
    void reset() {
        *this = defaultValue;
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

    void setDefaultValue(const typename T::Type& value) {
        defaultValue = value;
    }

private:
    typename T::Type defaultValue = DefaultValue<T>::Get();
    typename T::Type currentValue = defaultValue;
    bool dirty = false;
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
