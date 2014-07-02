#include <llmr/util/constants.hpp>

const char *llmr::kGlyphURL = "http://mapbox.s3.amazonaws.com/gl-glyphs-256/%s/%d-%d.pbf";

const float llmr::util::tileSize = 512.0f;

#if defined(DEBUG)
const bool llmr::debug::tileParseWarnings = false;
const bool llmr::debug::styleParseWarnings = false;
const bool llmr::debug::spriteWarnings = false;
const bool llmr::debug::renderWarnings = false;
const bool llmr::debug::renderTree = false;
const bool llmr::debug::labelTextMissingWarning = true;
const bool llmr::debug::missingFontStackWarning = true;
const bool llmr::debug::missingFontFaceWarning = true;
const bool llmr::debug::glyphWarning = true;
const bool llmr::debug::shapingWarning = true;
#else
const bool llmr::debug::tileParseWarnings = false;
const bool llmr::debug::styleParseWarnings = false;
const bool llmr::debug::spriteWarnings = false;
const bool llmr::debug::renderWarnings = false;
const bool llmr::debug::renderTree = false;
const bool llmr::debug::labelTextMissingWarning = false;
const bool llmr::debug::missingFontStackWarning = false;
const bool llmr::debug::missingFontFaceWarning = false;
const bool llmr::debug::glyphWarning = false;
const bool llmr::debug::shapingWarning = false;
#endif
