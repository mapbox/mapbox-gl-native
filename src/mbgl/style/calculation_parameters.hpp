#pragma once

#include <mbgl/map/zoom_history.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {
namespace style {

class CalculationParameters {
public:
    explicit CalculationParameters(float z_)
        : z(z_) {}

    CalculationParameters(float z_,
                          TimePoint now_,
                          ZoomHistory zoomHistory_,
                          Duration defaultFadeDuration_)
        : z(z_),
          now(std::move(now_)),
          zoomHistory(std::move(zoomHistory_)),
          defaultFadeDuration(std::move(defaultFadeDuration_)) {}

    float z;
    TimePoint now;
    ZoomHistory zoomHistory;
    Duration defaultFadeDuration;
};

} // namespace style
} // namespace mbgl
