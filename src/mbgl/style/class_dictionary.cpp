#include <mbgl/style/class_dictionary.hpp>

#include <uv.h>

namespace mbgl {

ClassDictionary::ClassDictionary() {}

ClassDictionary &ClassDictionary::Get() {
    // Note: We should eventually switch to uv_key_* functions, but libuv 0.10 doesn't have these
    // yet. Instead, we're using the pthread functions directly for now.
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

}
