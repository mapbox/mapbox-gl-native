#pragma once

#include <mbgl/map/view.hpp>
#include <mbgl/util/image.hpp>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

class OffscreenView : public View {
public:
    OffscreenView(gl::Context&, Size size = { 256, 256 });
    ~OffscreenView();

    void bind() override;

    PremultipliedImage readStillImage();

    const Size& getSize() const;

private:
    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace mbgl
