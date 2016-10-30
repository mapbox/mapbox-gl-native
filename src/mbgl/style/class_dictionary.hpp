#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>

namespace mbgl {
namespace style {

enum class ClassID : uint32_t {
    Default = 1, // These values are from the default style for a layer
    Named = 2 // These values (and all subsequent IDs) are from a named style from the layer
};

class ClassDictionary {
private:
    ClassDictionary();

public:
    static ClassDictionary &Get();

    // Returns an ID for a class name. If the class name does not yet have an ID, one is
    // auto-generated and stored for future reference.
    ClassID lookup(const std::string &class_name);

    // Returns either Fallback, Default or Named, depending on the type of the class id.
    ClassID normalize(ClassID id);

private:
    std::unordered_map<std::string, ClassID> store = { { "", ClassID::Default } };
    uint32_t offset = 0;
};

} // namespace style
} // namespace mbgl

namespace std {

// Explicitly define std::hash<style::ClassID> because GCC doesn't automatically use std::hash<> of
// the underlying enum type.

template <>
struct hash<mbgl::style::ClassID> {
public:
    size_t operator()(const mbgl::style::ClassID id) const {
        using T = std::underlying_type_t<mbgl::style::ClassID>;
        return std::hash<T>()(static_cast<T>(id));
    }
};

} // namespace std
