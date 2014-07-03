#include <llmr/style/class_dictionary.hpp>

namespace llmr {

ClassID ClassDictionary::Lookup(const std::string &class_name) {
    auto it = store.find(class_name);
    if (it == store.end()) {
        // Insert the class name into the store.
        ClassID id = ClassID(uint32_t(ClassID::Named) + offset++);
        store.emplace(class_name, id);
        return id;
    } else {
        return it->second;
    }
}

ClassID ClassDictionary::Normalize(ClassID id) {
    if (id >= ClassID::Named) {
        return ClassID::Named;
    } else {
        return id;
    }
}


std::unordered_map<std::string, ClassID> ClassDictionary::store = { { "", ClassID::Default } };
uint32_t ClassDictionary::offset = 0;

}
