#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread_local.hpp>

#include <array>

#include <QThreadStorage>

namespace mbgl {
namespace util {

template class ThreadLocal<RunLoop>;
template class ThreadLocal<int>;
template class ThreadLocal<style::ClassDictionary>;

template <class T>
class ThreadLocal<T>::Impl {
public:
    QThreadStorage<std::array<T*, 1>> local;
};

template <class T>
ThreadLocal<T>::ThreadLocal() : impl(std::make_unique<Impl>()) {
    set(nullptr);
}

template <class T>
ThreadLocal<T>::~ThreadLocal() {
    delete get();
}

template <class T>
T* ThreadLocal<T>::get() {
    return impl->local.localData()[0];
}

template <class T>
void ThreadLocal<T>::set(T* ptr) {
   impl->local.localData()[0] = ptr;
}

} // namespace util
} // namespace mbgl
