#pragma once

namespace mbgl {

class Backend;

class BackendScope {
public:
    BackendScope(Backend&);
    ~BackendScope();

private:
    Backend* priorBackend;
    Backend& backend;
};

} // namespace mbgl
