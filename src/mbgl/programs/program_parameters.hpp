#pragma once

#include <string>
#include <utility>

namespace mbgl {

class ProgramParameters {
public:
    ProgramParameters(float pixelRatio, bool overdraw, std::string cacheDir);

    const std::string defines;

    std::string cachePath(const char* name) const;

private:
    const std::size_t hash;
    const std::string cacheDir;
};

} // namespace mbgl
