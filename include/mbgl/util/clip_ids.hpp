#ifndef MBGL_UTIL_CLIP_IDS
#define MBGL_UTIL_CLIP_IDS

#include <mbgl/map/tile.hpp>
#include <list>
#include <set>
#include <map>

namespace mbgl {

static constexpr uint8_t clipMask[9] { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };

void updateClipIDs(const std::list<Tile *> &array);

std::map<Tile::ID, ClipID> computeClipIDs(std::forward_list<Tile::ID> array);

}

#endif
