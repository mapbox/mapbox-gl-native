#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/util/thread_local.hpp>

#include <cassert>

namespace mbgl {

static util::ThreadLocal<BackendScope> currentScope;

BackendScope::BackendScope(RendererBackend& backend_, ScopeType scopeType_)
    : priorScope(currentScope.get()),
      nextScope(nullptr),
      backend(backend_),
      scopeType(scopeType_) {
    if (priorScope) {
        assert(priorScope->nextScope == nullptr);
        priorScope->nextScope = this;
    }
    if (scopeType == ScopeType::Explicit) {
        backend.activate();
    }

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
        if (scopeType == ScopeType::Explicit) {
            backend.deactivate();
        }

        currentScope.set(nullptr);
    }
}

bool BackendScope::exists() {
    return currentScope.get();
}

} // namespace mbgl
