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
                          const TimePoint& now_,
                          const ZoomHistory& zoomHistory_,
                          const Duration& defaultFadeDuration_)
        : z(z_),
          now(now_),
          zoomHistory(zoomHistory_),
          defaultFadeDuration(defaultFadeDuration_) {}

    float z;
    TimePoint now;
    ZoomHistory zoomHistory;
    Duration defaultFadeDuration;
};

} // namespace style
} // namespace mbgl
