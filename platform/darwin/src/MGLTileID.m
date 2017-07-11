//
//  MGLTileID.m
//  ios
//
//  Created by Anna Johnson on 2/7/17.
//  Copyright © 2017 Mapbox. All rights reserved.
//

#import "MGLTileID.h"

bool MGLTileIDsEqual(MGLTileID one, MGLTileID two) {
  return (one.x == two.x) && (one.y == two.y) && (one.z == two.z);
}


MGLTileID MGLTileIDFromKey(uint64_t tilekey) {
    MGLTileID t;
    t.z = tilekey >> 56;
    t.x = tilekey >> 28 & 0xFFFFFFFLL;
    t.y = tilekey & 0xFFFFFFFLL;
    return t;
}

/// Returns a unique key of the tile for use in the SQLite cache
uint64_t MGLTileKey(MGLTileID tile) {
  uint64_t zoom = (uint64_t) tile.z & 0xFFLL; // 8bits, 256 levels
  uint64_t x = (uint64_t) tile.x & 0xFFFFFFFLL;  // 28 bits
  uint64_t y = (uint64_t) tile.y & 0xFFFFFFFLL;  // 28 bits
  
  uint64_t key = (zoom << 56) | (x << 28) | (y << 0);
  
  return key;
}


MGLTileID MGLTileIDMake(uint32_t x, uint32_t y, uint8_t z) {
  MGLTileID t;
  t.x = x;
  t.y = y;
  t.z = z;
  return t;
}
