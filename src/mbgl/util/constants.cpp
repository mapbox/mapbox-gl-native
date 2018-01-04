#include <mbgl/util/constants.hpp>

#include <limits>

namespace mbgl {

namespace debug {

#ifndef NDEBUG
const bool tileParseWarnings = false;
const bool styleParseWarnings = false;
const bool spriteWarnings = false;
const bool renderWarnings = false;
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
const bool labelTextMissingWarning = false;
const bool missingFontStackWarning = false;
const bool missingFontFaceWarning = false;
const bool glyphWarning = false;
const bool shapingWarning = false;
#endif

} // namespace debug

} // namespace mbgl
