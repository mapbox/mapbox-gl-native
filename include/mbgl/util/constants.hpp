#ifndef MBGL_UTIL_CONSTANTS
#define MBGL_UTIL_CONSTANTS

#include <cmath>
#include <string>

namespace mbgl {

namespace util {

extern const float tileSize;

extern const double DEG2RAD;
extern const double RAD2DEG;
extern const double M2PI;
extern const double EARTH_RADIUS_M;
extern const double LATITUDE_MAX;

extern const std::string ANNOTATIONS_POINTS_LAYER_ID;

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
