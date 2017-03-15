#pragma once

#include <string>

namespace mbgl {

class ProgramParameters {
public:
    ProgramParameters(float pixelRatio_ = 1.0,
                      bool overdraw_ = false,
                      const std::string& cacheDir_ = "")
        : pixelRatio(pixelRatio_), overdraw(overdraw_), cacheDir(cacheDir_) {
    }

    const float pixelRatio;
    const bool overdraw;
    const std::string cacheDir;
};

} // namespace mbgl

