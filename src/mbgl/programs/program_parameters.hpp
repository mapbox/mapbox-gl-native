#pragma once

namespace mbgl {

class ProgramParameters {
public:
    ProgramParameters(float pixelRatio_ = 1.0, bool overdraw_ = false)
      : pixelRatio(pixelRatio_),
        overdraw(overdraw_) {}

    float pixelRatio;
    bool overdraw;
};

} // namespace mbgl

