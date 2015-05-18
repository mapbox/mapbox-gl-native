#ifndef TEST_RESOURCES_MOCK_VIEW
#define TEST_RESOURCES_MOCK_VIEW

#include <mbgl/map/view.hpp>

namespace mbgl {

class MockView : public View {
public:
    MockView() = default;

    // View implementation.
    void activate() override {};
    void deactivate() override {};
    void notify() override {};
    void invalidate(std::function<void()>) override {};
};

}

#endif
