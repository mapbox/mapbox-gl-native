#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/unitbezier.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace mbgl {

namespace util {

constexpr float tileSize = 512;

/*
 * The maximum extent of a feature that can be safely stored in the buffer.
 * In practice, all features are converted to this extent before being added.
 *
 * Positions are stored as signed 16bit integers.
 * One bit is lost for signedness to support features extending past the left edge of the tile.
 * One bit is lost because the line vertex buffer used to pack 1 bit of other data into the int.
 * This is no longer the case but we're reserving this bit anyway.
 * One bit is lost to support features extending past the extent on the right edge of the tile.
 * This leaves us with 2^13 = 8192
 */
constexpr int32_t EXTENT = 8192;

constexpr double DEG2RAD = M_PI / 180.0;
constexpr double RAD2DEG = 180.0 / M_PI;
constexpr double M2PI = M_PI * 2;
constexpr double EARTH_RADIUS_M = 6378137;
constexpr double LATITUDE_MAX = 85.051128779806604;
constexpr double LONGITUDE_MAX = 180;
constexpr double DEGREES_MAX = 360;
constexpr double PITCH_MAX = M_PI / 3;
constexpr double MIN_ZOOM = 0.0;
constexpr double MAX_ZOOM = 25.5;
constexpr float  MIN_ZOOM_F = MIN_ZOOM;
constexpr float  MAX_ZOOM_F = MAX_ZOOM;
constexpr uint8_t DEFAULT_MAX_ZOOM = 22;

constexpr uint8_t DEFAULT_PREFETCH_ZOOM_DELTA = 4;

constexpr uint64_t DEFAULT_MAX_CACHE_SIZE = 50 * 1024 * 1024;

constexpr Duration DEFAULT_TRANSITION_DURATION = Milliseconds(300);
constexpr Seconds CLOCK_SKEW_RETRY_TIMEOUT { 30 };

constexpr UnitBezier DEFAULT_TRANSITION_EASE = { 0, 0, 0.25, 1 };

constexpr int DEFAULT_RATE_LIMIT_TIMEOUT = 5;

constexpr const char* API_BASE_URL = "https://api.mapbox.com";

} // namespace util

namespace debug {

extern const bool tileParseWarnings;
extern const bool styleParseWarnings;
extern const bool spriteWarnings;
extern const bool renderWarnings;
extern const bool labelTextMissingWarning;
extern const bool missingFontStackWarning;
extern const bool missingFontFaceWarning;
extern const bool glyphWarning;
extern const bool shapingWarning;

} // namespace debug

} // namespace mbgl
