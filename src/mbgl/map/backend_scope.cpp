#include <mbgl/map/backend_scope.hpp>
#include <mbgl/map/backend.hpp>
#include <mbgl/util/thread_local.hpp>

namespace mbgl {

static util::ThreadLocal<Backend> currentBackend;

BackendScope::BackendScope(Backend& backend_)
    : priorBackend(currentBackend.get()),
      backend(backend_) {
    backend.activate();
    currentBackend.set(&backend);
}

BackendScope::~BackendScope() {
    if (priorBackend) {
        priorBackend->activate();
        currentBackend.set(priorBackend);
    } else {
        backend.deactivate();
        currentBackend.set(nullptr);
    }
}

} // namespace mbgl
