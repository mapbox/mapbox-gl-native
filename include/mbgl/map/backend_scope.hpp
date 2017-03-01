#pragma once

namespace mbgl {

class Backend;

class BackendScope {
public:
    BackendScope(Backend&);
    ~BackendScope();

private:
    BackendScope* priorScope;
    BackendScope* nextScope;
    Backend& backend;
};

} // namespace mbgl
