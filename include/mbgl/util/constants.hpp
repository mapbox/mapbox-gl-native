#ifndef MBGL_UTIL_CONSTANTS
#define MBGL_UTIL_CONSTANTS

#include <cmath>
#include <string>
#include <vector>

namespace mbgl {

namespace util {

extern const float tileSize;
extern const int32_t EXTENT;

extern const double DEG2RAD;
extern const double RAD2DEG;
extern const double M2PI;
extern const double EARTH_RADIUS_M;
extern const double LATITUDE_MAX;
extern const double PITCH_MAX;
extern const double MIN_ZOOM;
extern const double MAX_ZOOM;

extern const uint64_t DEFAULT_MAX_CACHE_SIZE;
extern const uint64_t DEFAULT_MAX_CACHE_ENTRY_SIZE;

} // namespace util

namespace debug {

extern const bool tileParseWarnings;
extern const bool styleParseWarnings;
extern const bool spriteWarnings;
extern const bool renderWarnings;
extern const bool renderTree;
extern const bool labelTextMissingWarning;
extern const bool missingFontStackWarning;
extern const bool missingFontFaceWarning;
extern const bool glyphWarning;
extern const bool shapingWarning;

} // namespace debug

} // namespace mbgl

#endif
