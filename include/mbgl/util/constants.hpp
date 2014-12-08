#ifndef MBGL_UTIL_CONSTANTS
#define MBGL_UTIL_CONSTANTS

#include <cmath>

namespace mbgl {

namespace util {

extern const float tileSize;
extern const char *userAgent;

}

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

}

}

#endif
