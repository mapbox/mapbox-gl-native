#pragma once

namespace mbgl {

class RendererBackend;

class BackendScope {
public:
    // There are two types of scopes: Creating an "Implicit" scope tells Mapbox GL that the
    // supporting windowing system has already activated the GL Backend and that no further actions
    // are required. Creating an "Explicit" scope actually enables the GL Backend, and disables it
    // when the BackendScope is destroyed.
    enum class ScopeType : bool {
        Implicit,
        Explicit,
    };

    BackendScope(RendererBackend&, ScopeType = ScopeType::Explicit);
    ~BackendScope();

    // Returns true when there is currently a BackendScope active in this thread.
    static bool exists();

private:
    BackendScope* priorScope;
    BackendScope* nextScope;
    RendererBackend& backend;
    const ScopeType scopeType;
};

} // namespace mbgl
