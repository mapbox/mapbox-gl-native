#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/size.hpp>

#include <memory>

namespace mbgl {

/**
 * @brief Holds values for Map options.
 */
class MapOptions final {
public:
    /**
     * @brief Constructs a MapOptions object with default values.
     */
    MapOptions();
    ~MapOptions();

    MapOptions(MapOptions&&) noexcept;

    /**
     * @brief Sets the map rendering mode. By default, it is set to Continuous
     * so the map will render as data arrives from the network and react
     * immediately to state changes.
     *
     * @param mode Map rendering mode.
     * @return MapOptions for chaining options together.
     */
    MapOptions& withMapMode(MapMode mode);

    /**
     * @brief Gets the previously set (or default) map mode.
     *
     * @return map mode.
     */
    MapMode mapMode() const;

    /**
     * @brief Sets the map constrain mode. This can be used to limit the map
     * to wrap around the globe horizontally. By default, it is set to
     * HeightOnly.
     *
     * @param mode Map constrain mode.
     * @return MapOptions for chaining options together.
     */
    MapOptions& withConstrainMode(ConstrainMode mode);

    /**
     * @brief Gets the previously set (or default) constrain mode.
     *
     * @return constrain mode.
     */
    ConstrainMode constrainMode() const;

    /**
     * @brief Sets the viewport mode. This can be used to flip the vertical
     * orientation of the map as some devices may use inverted orientation.
     *
     * @param mode Viewport mode.
     * @return MapOptions for chaining options together.
     */
    MapOptions& withViewportMode(ViewportMode mode);

    /**
     * @brief Gets the previously set (or default) viewport mode.
     *
     * @return viewport mode.
     */
    ViewportMode viewportMode() const;

    /**
     * @brief Specify whether to enable cross-source symbol collision detection
     * or not. By default, it is set to true.
     *
     * @param enableCollisions true to enable, false to disable
     * @return MapOptions for chaining options together.
     */
    MapOptions& withCrossSourceCollisions(bool enableCollisions);

    /**
     * @brief Gets the previously set (or default) crossSourceCollisions value.
     *
     * @return true if ecross-source symbol collision detection enabled,
     * false otherwise.
     */
    bool crossSourceCollisions() const;

    /**
     * @brief Sets the orientation of the Map. By default, it is set to
     * Upwards.
     *
     * @param orientation Orientation of the Map.
     * @return reference to MapOptions for chaining options together.
     */
    MapOptions& withNorthOrientation(NorthOrientation orientation);

    /**
     * @brief Gets the previously set (or default) orientation.
     *
     * @return Map orientation.
     */
    NorthOrientation northOrientation() const;

    /**
     * @brief Sets the size to resize the map object and renderer backend.
     *
     * @param size_ A size given in logical pixels.
     * @return reference to MapOptions for chaining options together.
     */
    MapOptions& withSize(Size size_);

    /**
     * @brief Gets the previously set size.
     *
     * @return Size.
     */
    Size size() const;

    /**
     * @brief Sets the custom pixel ratio. By default, it is set to 1.
     *
     * @param ratio Pixel ratio value.
     * @return reference to MapOptions for chaining options together.
     */
    MapOptions& withPixelRatio(float ratio);

    /**
     * @brief Gets the previously set (or default) pixel ratio value.
     *
     * @return pixel ratio value.
     */
    float pixelRatio() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace mbgl
