#include <mbgl/style/class_dictionary.hpp>

#include <pthread.h>

namespace mbgl {
namespace style {

ClassDictionary::ClassDictionary() {}

ClassDictionary &ClassDictionary::Get() {
    static pthread_once_t store_once = PTHREAD_ONCE_INIT;
    static pthread_key_t store_key;

    // Create the key.
    pthread_once(&store_once, []() {
        pthread_key_create(&store_key, [](void *ptr) {
            delete reinterpret_cast<ClassDictionary *>(ptr);
        });
    });

    ClassDictionary *ptr = reinterpret_cast<ClassDictionary *>(pthread_getspecific(store_key));
    if (ptr == nullptr) {
        ptr = new ClassDictionary();
        pthread_setspecific(store_key, ptr);
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
