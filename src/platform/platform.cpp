#include <llmr/platform/platform.hpp>

const char *llmr::kTileVectorURL = "http://localhost:3333/v3/mapbox.mapbox-streets-v4/%d/%d/%d.gl.pbf";
const char *llmr::kTileRasterURL = "https://a.tiles.mapbox.com/v3/justin.map-pgygbwdm/%d/%d/%d%s.png256";
const char *llmr::kSpriteURL = "http://localhost:3333/images/sprite";

const int32_t llmr::kTileVectorMaxZoom = 14;
const int32_t llmr::kTileRasterMaxZoom = 21;
