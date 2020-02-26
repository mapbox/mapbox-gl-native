#pragma once

#include <memory>

namespace mbgl {

// Using this type as a return type enforces the client to retain the returned object.
template <typename T>
class Pass {
public:
    Pass(T obj_) : obj(std::move(obj_)) {}
    Pass(Pass&&) noexcept = default;
    Pass(const Pass&) = delete;
    Pass& operator=(const Pass&) = delete;
    operator T() && { return std::move(obj); }

private:
    T obj;
};

template <typename T>
using PassRefPtr = Pass<std::shared_ptr<T>>;

} // namespace mbgl