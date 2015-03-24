#include <mbgl/util/constants.hpp>

const float mbgl::util::tileSize = 512.0f;

const double mbgl::util::DEG2RAD = M_PI / 180.0;
const double mbgl::util::RAD2DEG = 180.0 / M_PI;
const double mbgl::util::M2PI = 2 * M_PI;
const double mbgl::util::EARTH_RADIUS_M = 6378137;
const double mbgl::util::LATITUDE_MAX = 85.05112878;

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
