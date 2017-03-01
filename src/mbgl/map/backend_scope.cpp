#include <mbgl/map/backend_scope.hpp>
#include <mbgl/map/backend.hpp>
#include <mbgl/util/thread_local.hpp>

#include <cassert>

namespace mbgl {

static util::ThreadLocal<BackendScope> currentScope;

BackendScope::BackendScope(Backend& backend_)
    : priorScope(currentScope.get()),
      nextScope(nullptr),
      backend(backend_) {
    if (priorScope) {
        assert(priorScope->nextScope == nullptr);
        priorScope->nextScope = this;
    }
    backend.activate();
    currentScope.set(this);
}

BackendScope::~BackendScope() {
    assert(nextScope == nullptr);
    if (priorScope) {
        priorScope->backend.activate();
        currentScope.set(priorScope);
        assert(priorScope->nextScope == this);
        priorScope->nextScope = nullptr;
    } else {
        backend.deactivate();
        currentScope.set(nullptr);
    }
}

} // namespace mbgl
