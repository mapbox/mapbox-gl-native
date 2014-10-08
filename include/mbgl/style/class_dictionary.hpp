#ifndef MBGL_STYLE_CLASS_DICTIONARY
#define MBGL_STYLE_CLASS_DICTIONARY

#include <cstdint>
#include <string>
#include <unordered_map>

namespace mbgl {

enum class ClassID : uint32_t {
    Fallback = 0, // These values are from the fallback properties
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

}

#endif
