#pragma once

namespace mbgl {
namespace gfx {

template <typename T>
class DebugGroup {
public:
    DebugGroup(T& scope_, const char* name) : scope(&scope_) {
        scope->pushDebugGroup(name);
    }

    DebugGroup(DebugGroup&& rhs) : scope(rhs.scope) {
        rhs.scope = nullptr;
    }

    ~DebugGroup() {
        if (scope) {
            scope->popDebugGroup();
        }
    }

private:
    T* scope;
};

} // namespace gfx
} // namespace mbgl
