#include <mbgl/map/map_options.hpp>

namespace mbgl {

class MapOptions::Impl {
public:
    MapMode mapMode = MapMode::Continuous;
    ConstrainMode constrainMode = ConstrainMode::HeightOnly;
    ViewportMode viewportMode = ViewportMode::Default;
    NorthOrientation orientation = NorthOrientation::Upwards;
    bool crossSourceCollisions = true;
};

// These requires the complete type of Impl.
MapOptions::MapOptions() : impl_(std::make_unique<Impl>()) {}
MapOptions::~MapOptions() = default;
MapOptions::MapOptions(MapOptions&&) noexcept = default;

MapOptions& MapOptions::withMapMode(MapMode mode) {
    impl_->mapMode = mode;
    return *this;
}

MapMode MapOptions::mapMode() const {
    return impl_->mapMode;
}

MapOptions& MapOptions::withConstrainMode(ConstrainMode mode) {
    impl_->constrainMode = mode;
    return *this;
}

ConstrainMode MapOptions::constrainMode() const {
    return impl_->constrainMode;
}

MapOptions& MapOptions::withViewportMode(ViewportMode mode) {
    impl_->viewportMode = mode;
    return *this;
}

ViewportMode MapOptions::viewportMode() const {
    return impl_->viewportMode;
}

MapOptions& MapOptions::withCrossSourceCollisions(bool enableCollisions) {
    impl_->crossSourceCollisions = enableCollisions;
    return *this;
}

bool MapOptions::crossSourceCollisions() const {
    return impl_->crossSourceCollisions;
}

MapOptions& MapOptions::withNorthOrientation(NorthOrientation orientation) {
    impl_->orientation = orientation;
    return *this;
}

NorthOrientation MapOptions::northOrientation() const {
    return impl_->orientation;
}

}  // namespace mbgl
