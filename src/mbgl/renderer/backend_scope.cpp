#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/thread_local.hpp>

#include <cassert>

namespace {

mbgl::util::ThreadLocal<mbgl::gfx::BackendScope>& currentScope() {
    static mbgl::util::ThreadLocal<mbgl::gfx::BackendScope> backendScope;

    return backendScope;
}

} // namespace

namespace mbgl {
namespace gfx {

BackendScope::BackendScope(RendererBackend& backend_, ScopeType scopeType_)
    : priorScope(currentScope().get()),
      nextScope(nullptr),
      backend(backend_),
      scopeType(scopeType_) {
    if (priorScope) {
        assert(priorScope->nextScope == nullptr);
        priorScope->nextScope = this;
        priorScope->deactivate();
    }

    activate();

    currentScope().set(this);
}

BackendScope::~BackendScope() {
    assert(nextScope == nullptr);
    deactivate();

    if (priorScope) {
        priorScope->activate();
        currentScope().set(priorScope);
        assert(priorScope->nextScope == this);
        priorScope->nextScope = nullptr;
    } else {
        currentScope().set(nullptr);
    }
}

void BackendScope::activate() {
    if (scopeType == ScopeType::Explicit &&
            !(priorScope && this->backend == priorScope->backend) &&
            !(nextScope && this->backend == nextScope->backend)) {
        // Only activate when set to Explicit and
        // only once per RenderBackend
        backend.activate();
        activated = true;
    }
}

void BackendScope::deactivate() {
    if (activated &&
        !(nextScope && this->backend == nextScope->backend)) {
        // Only deactivate when set to Explicit and
        // only once per RenderBackend
        backend.deactivate();
        activated = false;
    }
}

bool BackendScope::exists() {
    return currentScope().get();
}

} // namespace gfx
} // namespace mbgl
