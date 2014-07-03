#ifndef LLMR_STYLE_CLASS_DICTIONARY
#define LLMR_STYLE_CLASS_DICTIONARY

#include <cstdint>
#include <string>
#include <unordered_map>

namespace llmr {

enum class ClassID : uint32_t {
    Fallback = 0, // These values are from the fallback properties
    Default = 1, // These values are from the default style for a layer
    Named = 2 // These values (and all subsequent IDs) are from a named style from the layer
};

class ClassDictionary {
public:
    // Returns an ID for a class name. If the class name does not yet have an ID, one is
    // auto-generated and stored for future reference.
    static ClassID Lookup(const std::string &class_name);

    // Returns either Fallback, Default or Named, depending on the type of the class id.
    static ClassID Normalize(ClassID id);

private:
    static std::unordered_map<std::string, ClassID> store;
    static uint32_t offset;
};

}

#endif
