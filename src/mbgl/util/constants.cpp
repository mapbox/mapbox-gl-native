#include <mbgl/util/constants.hpp>

#include <limits>

namespace mbgl {

namespace util {

const float tileSize = 512.0f;

/*
 * The maximum extent of a feature that can be safely stored in the buffer.
 * In practice, all features are converted to this extent before being added.
 *
 * Positions are stored as signed 16bit integers.
 * One bit is lost for signedness to support featuers extending past the left edge of the tile.
 * One bit is lost because the line vertex buffer packs 1 bit of other data into the int.
 * One bit is lost to support features extending past the extent on the right edge of the tile.
 * This leaves us with 2^13 = 8192
 */
const int32_t EXTENT = 8192;

const double DEG2RAD = M_PI / 180.0;
const double RAD2DEG = 180.0 / M_PI;
const double M2PI = 2 * M_PI;
const double EARTH_RADIUS_M = 6378137;
const double LATITUDE_MAX = 85.051128779806604;
const double LONGITUDE_MAX = 180;
const double DEGREES_MAX = 360;
const double PITCH_MAX = M_PI / 3;
const double MIN_ZOOM = 0.0;
const double MAX_ZOOM = 25.5;

const uint64_t DEFAULT_MAX_CACHE_SIZE = 50 * 1024 * 1024;

const SystemDuration CLOCK_SKEW_RETRY_TIMEOUT = Seconds(30);

} // namespace util

namespace debug {

#if defined(DEBUG)
const bool tileParseWarnings = false;
const bool styleParseWarnings = false;
const bool spriteWarnings = false;
const bool renderWarnings = false;
const bool renderTree = false;
const bool labelTextMissingWarning = true;
const bool missingFontStackWarning = true;
const bool missingFontFaceWarning = true;
const bool glyphWarning = true;
const bool shapingWarning = true;
#else
const bool tileParseWarnings = false;
const bool styleParseWarnings = false;
const bool spriteWarnings = false;
const bool renderWarnings = false;
const bool renderTree = false;
const bool labelTextMissingWarning = false;
const bool missingFontStackWarning = false;
const bool missingFontFaceWarning = false;
const bool glyphWarning = false;
const bool shapingWarning = false;
#endif

} // namespace debug

} // namespace mbgl
