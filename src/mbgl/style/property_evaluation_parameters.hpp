#pragma once

#include <mbgl/map/zoom_history.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {
namespace style {

class PropertyEvaluationParameters {
public:
    explicit PropertyEvaluationParameters(float z_)
        : z(z_) {}

    PropertyEvaluationParameters(float z_,
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
