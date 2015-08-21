#ifndef TEST_FIXTURES_MOCK_VIEW
#define TEST_FIXTURES_MOCK_VIEW

#include <mbgl/map/view.hpp>

#include <array>

namespace mbgl {

class MockView : public View {
public:
    MockView() = default;

    // View implementation.
    float getPixelRatio() const override { return 1; }
    std::array<uint16_t, 2> getSize() const override { return {{ 0, 0 }}; }
    std::array<uint16_t, 2> getFramebufferSize() const override { return {{ 0, 0 }}; }

    void activate() override {};
    void deactivate() override {};
    void notify() override {};
    void invalidate() override {}
    void swap() override {}
};

}

#endif
