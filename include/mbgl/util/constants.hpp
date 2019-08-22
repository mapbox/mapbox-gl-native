#pragma once

#include <mbgl/util/chrono.hpp>
#include <mbgl/util/unitbezier.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace mbgl {

namespace util {

constexpr double tileSize = 512;

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
constexpr double PITCH_MIN = 0.0;
/*
 * Max pitch is limited to 90 deg - TransformState::fov / 2, which evaluates to 71.5 deg, when
 * perspective center's offset is 0.  When padding is used, and the perspective center moved from
 * center of the screen, max pitch is further capped by Transform::getMaxPitchForEdgeInsets.
 * We set the max to 70 to keep the limit constant with small padding.
 */
constexpr double PITCH_MAX = 70 * DEG2RAD;
constexpr double MIN_ZOOM = 0.0;
constexpr double MAX_ZOOM = 25.5;
constexpr float  MIN_ZOOM_F = MIN_ZOOM;
constexpr float  MAX_ZOOM_F = MAX_ZOOM;
constexpr uint8_t DEFAULT_MAX_ZOOM = 22;

// ONE_EM constant used to go between "em" units used in style spec and "points" used internally for layout.
constexpr float ONE_EM = 24.0f;

constexpr uint8_t DEFAULT_PREFETCH_ZOOM_DELTA = 4;

constexpr uint64_t DEFAULT_MAX_CACHE_SIZE = 50 * 1024 * 1024;

// Default ImageManager's cache size for images added via onStyleImageMissing API.
// Average sprite size with 1.0 pixel ratio is ~2kB, 8kB for pixel ratio of 2.0.
constexpr std::size_t DEFAULT_ON_DEMAND_IMAGES_CACHE_SIZE = 100 * 8192;

constexpr Duration DEFAULT_TRANSITION_DURATION = Milliseconds(300);
constexpr Seconds CLOCK_SKEW_RETRY_TIMEOUT { 30 };

constexpr UnitBezier DEFAULT_TRANSITION_EASE = { 0, 0, 0.25, 1 };

constexpr int DEFAULT_RATE_LIMIT_TIMEOUT = 5;

constexpr const char* API_BASE_URL = "https://api.mapbox.com";

constexpr uint8_t TERRAIN_RGB_MAXZOOM = 15;

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
