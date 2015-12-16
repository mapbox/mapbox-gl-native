#include <mbgl/platform/platform.hpp>
#include "jni.hpp"

#include <libgen.h>

namespace mbgl {
namespace platform {

// Returns the path to the root folder of the application.
const std::string &assetRoot() {
    return mbgl::android::apkPath;
}

}
}
