#pragma once

#include <mbgl/util/optional.hpp>

#include <string>
#include <vector>

namespace mbgl {

class ProgramParameters {
public:
    ProgramParameters(float pixelRatio, bool overdraw, optional<std::string> cacheDir);

    const std::string& getDefines() const;
    optional<std::string> cachePath(const char* name) const;

    ProgramParameters withAdditionalDefines(const std::vector<std::string>& defines) const;

private:
    std::string defines;
    optional<std::string> cacheDir;
};

} // namespace mbgl
