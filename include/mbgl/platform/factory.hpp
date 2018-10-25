#pragma once

#include <mbgl/platform/platform_run_loop.hpp>

#include <memory>

namespace mbgl {

/**
 * @brief A factory class for platform dependent code.
 *
 * These symbols are weak and should be provided at
 * linking time by the platform linking with Mapbox GL
 * core library.
 */
class PlatformFactory {
public:
    /**
     * @brief Creates a new PlatformRunLoop object.
     *
     * A platform specific run loop that can sleep,
     * process tasks, wake-up in a thread safe-way.
     *
     * @return the platform run loop.
     */
    static std::unique_ptr<PlatformRunLoop> createRunLoop();

    PlatformFactory() = delete;
};

} // namespace mbgl
