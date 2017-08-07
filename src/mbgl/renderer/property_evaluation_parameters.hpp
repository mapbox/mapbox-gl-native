#pragma once

#include <mbgl/map/zoom_history.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

class PropertyEvaluationParameters {
public:
    explicit PropertyEvaluationParameters(float z_)
        : z(z_),
          now(Clock::time_point::max()),
          zoomHistory(),
          defaultFadeDuration(0),
          useIntegerZoom(false) {}

    PropertyEvaluationParameters(ZoomHistory zoomHistory_,
                          TimePoint now_,
                          Duration defaultFadeDuration_,
                          bool useIntegerZoom_ = false)
        : z(zoomHistory_.lastZoom),
          now(std::move(now_)),
          zoomHistory(std::move(zoomHistory_)),
          defaultFadeDuration(std::move(defaultFadeDuration_)),
          useIntegerZoom(useIntegerZoom_) {}

    float z;
    TimePoint now;
    ZoomHistory zoomHistory;
    Duration defaultFadeDuration;
    bool useIntegerZoom;
};

} // namespace mbgl
