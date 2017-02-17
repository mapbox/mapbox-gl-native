#include "default_file_source.hpp"
#include <mbgl/util/logging.hpp>

#include <cassert>

namespace mbgl {
namespace android {

DefaultFileSource& defaultFileSource(const std::string& cachePath_, const std::string& assetRoot_) {
    static auto cachePath = cachePath_;
    assert(cachePath == cachePath_);
    static auto assetRoot = assetRoot_;
    assert(assetRoot == assetRoot_);

    static DefaultFileSource defaultFileSource{ cachePath, assetRoot };
    return defaultFileSource;
}

} // namespace android
} // namespace mbgl
