#include <mbgl/util/constants.hpp>

const float mbgl::util::tileSize = 512.0f;

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
const int32_t mbgl::util::EXTENT = 8192;

const double mbgl::util::DEG2RAD = M_PI / 180.0;
const double mbgl::util::RAD2DEG = 180.0 / M_PI;
const double mbgl::util::M2PI = 2 * M_PI;
const double mbgl::util::EARTH_RADIUS_M = 6378137;
const double mbgl::util::LATITUDE_MAX = 85.05112878;
const double mbgl::util::PITCH_MAX = M_PI / 3;

#if defined(DEBUG)
const bool mbgl::debug::tileParseWarnings = false;
const bool mbgl::debug::styleParseWarnings = false;
const bool mbgl::debug::spriteWarnings = false;
const bool mbgl::debug::renderWarnings = false;
const bool mbgl::debug::renderTree = false;
const bool mbgl::debug::labelTextMissingWarning = true;
const bool mbgl::debug::missingFontStackWarning = true;
const bool mbgl::debug::missingFontFaceWarning = true;
const bool mbgl::debug::glyphWarning = true;
const bool mbgl::debug::shapingWarning = true;
#else
const bool mbgl::debug::tileParseWarnings = false;
const bool mbgl::debug::styleParseWarnings = false;
const bool mbgl::debug::spriteWarnings = false;
const bool mbgl::debug::renderWarnings = false;
const bool mbgl::debug::renderTree = false;
const bool mbgl::debug::labelTextMissingWarning = false;
const bool mbgl::debug::missingFontStackWarning = false;
const bool mbgl::debug::missingFontFaceWarning = false;
const bool mbgl::debug::glyphWarning = false;
const bool mbgl::debug::shapingWarning = false;
#endif
