#pragma once

#include <string>
#include <utility>

namespace mbgl {

class ProgramParameters {
public:
    ProgramParameters(float pixelRatio_ = 1.0,
                      bool overdraw_ = false,
                      std::string cacheDir_ = "")
        : pixelRatio(pixelRatio_), overdraw(overdraw_), cacheDir(std::move(cacheDir_)) {
    }

    const float pixelRatio;
    const bool overdraw;
    const std::string cacheDir;
};

} // namespace mbgl

