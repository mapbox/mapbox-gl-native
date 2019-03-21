#include <mbgl/map/map_options.hpp>

namespace mbgl {

class MapOptions::Impl {
public:
    MapMode mapMode = MapMode::Continuous;
    ConstrainMode constrainMode = ConstrainMode::HeightOnly;
    ViewportMode viewportMode = ViewportMode::Default;
    bool crossSourceCollisions = true;
};

// These requires the complete type of Impl.
MapOptions::MapOptions() : impl_(std::make_unique<Impl>()) {}
MapOptions::~MapOptions() = default;
MapOptions::MapOptions(MapOptions&&) = default;
MapOptions::MapOptions(const MapOptions& other) : impl_(std::make_unique<Impl>(*other.impl_)) {}

MapOptions MapOptions::withMapMode(MapMode mode) {
    impl_->mapMode = mode;
    return std::move(*this);
}

MapMode MapOptions::mapMode() const {
    return impl_->mapMode;
}

MapOptions MapOptions::withConstrainMode(ConstrainMode mode) {
    impl_->constrainMode = mode;
    return std::move(*this);
}

ConstrainMode MapOptions::constrainMode() const {
    return impl_->constrainMode;
}

MapOptions MapOptions::withViewportMode(ViewportMode mode) {
    impl_->viewportMode = mode;
    return std::move(*this);
}

ViewportMode MapOptions::viewportMode() const {
    return impl_->viewportMode;
}

MapOptions MapOptions::withCrossSourceCollisions(bool enableCollisions) {
    impl_->crossSourceCollisions = enableCollisions;
    return std::move(*this);
}

bool MapOptions::crossSourceCollisions() const {
    return impl_->crossSourceCollisions;
}

}  // namespace mbgl
