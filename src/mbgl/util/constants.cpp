#include <mbgl/util/constants.hpp>

const float mbgl::util::tileSize = 512.0f;
const char *mbgl::util::userAgent = "MapboxGL/1.0 (+https://mapbox.com/mapbox-gl/)";

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
