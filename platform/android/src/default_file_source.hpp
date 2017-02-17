#pragma once

#include <mbgl/storage/default_file_source.hpp>

namespace mbgl {
namespace android {

DefaultFileSource& defaultFileSource(const std::string& cachePath = ":memory:",
                                     const std::string& assetRoot = ".");

} // namespace android
} // namespace mbgl
