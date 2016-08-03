#include <mbgl/style/class_dictionary.hpp>

#include <mbgl/util/thread_local.hpp>

namespace mbgl {
namespace style {

ClassDictionary::ClassDictionary() {}

ClassDictionary &ClassDictionary::Get() {
    static util::ThreadLocal<ClassDictionary> dictionary;

    ClassDictionary *ptr = dictionary.get();
    if (ptr == nullptr) {
        ptr = new ClassDictionary();
        dictionary.set(ptr);
    }

    return *ptr;
}

ClassID ClassDictionary::lookup(const std::string &class_name) {
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

ClassID ClassDictionary::normalize(ClassID id) {
    if (id >= ClassID::Named) {
        return ClassID::Named;
    } else {
        return id;
    }
}

} // namespace style
} // namespace mbgl
