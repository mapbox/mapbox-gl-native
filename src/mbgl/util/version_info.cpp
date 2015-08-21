#include <mbgl/util/version.hpp>

namespace mbgl {
namespace version {

const int major = MBGL_VERSION_MAJOR;
const int minor = MBGL_VERSION_MINOR;
const int patch = MBGL_VERSION_PATCH;
const char *revision = MBGL_VERSION_REV;
const char *string = MBGL_VERSION_STRING;
const unsigned int number = MBGL_VERSION;

}
}