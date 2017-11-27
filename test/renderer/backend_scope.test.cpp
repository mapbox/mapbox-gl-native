#include <mbgl/test/util.hpp>

#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/renderer/backend_scope.hpp>

#include <functional>

using namespace mbgl;

class StubRendererBackend: public RendererBackend {
public:
    void bind() override {
    }

    mbgl::Size getFramebufferSize() const override {
        return mbgl::Size{};
    }

    void activate() override {
        if (activateFunction) activateFunction();
    }

    void deactivate() override {
        if (deactivateFunction) deactivateFunction();
    }

    void updateAssumedState() override {
        if (updateAssumedStateFunction) updateAssumedStateFunction();
    }

    gl::ProcAddress getExtensionFunctionPointer(const char*) override {
        return {};
    }

    std::function<void ()> activateFunction;
    std::function<void ()> deactivateFunction;
    std::function<void ()> updateAssumedStateFunction;
};

// A scope should activate on construction
// and deactivate on descruction (going out
// of scope)
TEST(BackendScope, SingleScope) {
    bool activated;
    bool deactivated;

    StubRendererBackend backend;
    backend.activateFunction = [&] { activated = true; };
    backend.deactivateFunction = [&] { deactivated = true; };

    {
        BackendScope test { backend };
    }

    ASSERT_TRUE(activated);
    ASSERT_TRUE(deactivated);
}

// With nested scopes, only the outer scope
// should activate/deactivate
TEST(BackendScope, NestedScopes) {
    int activated = 0;
    int deactivated = 0;

    StubRendererBackend backend;
    backend.activateFunction = [&] { activated++; };
    backend.deactivateFunction = [&] { deactivated++; };

    {
        BackendScope outer { backend };
        ASSERT_EQ(1, activated);
        {
            BackendScope inner { backend };
            ASSERT_EQ(1, activated);
        }
        ASSERT_EQ(0, deactivated);
    }

    ASSERT_EQ(1, deactivated);
}

// With chained scopes, where scopes have
// different backends, the scopes should each
// activate the scope on entering and de-activating
// on leaving the scope
TEST(BackendScope, ChainedScopes) {
    bool activatedA = false;
    bool activatedB = false;

    StubRendererBackend backendA;
    backendA.activateFunction = [&] { activatedA = true; };
    backendA.deactivateFunction = [&] { activatedA = false; };

    StubRendererBackend backendB;
    backendB.activateFunction = [&] { activatedB = true; };
    backendB.deactivateFunction = [&] { activatedB = false; };

    {
        BackendScope scopeA { backendA };
        ASSERT_TRUE(activatedA);
        {
            BackendScope scopeB { backendB };
            ASSERT_FALSE(activatedA);
            ASSERT_TRUE(activatedB);
        }
        ASSERT_FALSE(activatedB);
        ASSERT_TRUE(activatedA);
    }

    ASSERT_FALSE(activatedA);
    ASSERT_FALSE(activatedB);
}
