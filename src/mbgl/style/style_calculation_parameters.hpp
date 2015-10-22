#ifndef STYLE_CALCULATION_PARAMETERS
#define STYLE_CALCULATION_PARAMETERS

#include <mbgl/style/zoom_history.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

class StyleCalculationParameters {
public:
    StyleCalculationParameters(float z_)
        : z(z_) {}

    StyleCalculationParameters(float z_,
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

}

#endif
