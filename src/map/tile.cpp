#include <llmr/map/tile.hpp>

#include <stdint.h>
#include <cassert>
// #include <iostream>
#include <thread>

#include "pbf.hpp"
#include <llmr/util/vec2.hpp>
#include <cmath>

using namespace llmr;


// int64_t tile::toID(int32_t z, int32_t x, int32_t y, int32_t w) {
//     w *= 2;
//     if (w < 0) w = w * -1 -1;
//     int32_t dim = 1 << z;
//     return ((dim * dim * w + dim * y + x) * 32) + z;
// }

// vec4<int32_t> tile::fromID(int64_t id) {
//     vec4<int32_t> coord;
//     coord.z = id % 32;
//     int32_t dim = 1 << coord.z;
//     int64_t xy = ((id - coord.z) / 32);
//     coord.x = xy % dim;
//     coord.y = ((xy - coord.x) / dim) % dim;
//     coord.w = floor(xy / (dim * dim));
//     if (coord.w % 2 != 0) coord.w = coord.w * -1 -1;
//     coord.w /= 2;

//     fprintf(stderr, "tile: %d/%d/%d/%d\n", coord.z, coord.x, coord.y, coord.w);
//     return coord;
// };

tile::tile(int32_t z, int32_t x, int32_t y)
    : z(z),
      x(x),
      y(y),
      loaded(false),
      cancelled(false),
      data(0),
      bytes(0) {
}

tile::~tile() {
    fprintf(stderr, "[%p] deleting tile %d/%d/%d\n", this, z, x, y);
    if (this->data) {
        free(this->data);
    }
}

void tile::setData(uint8_t *data, uint32_t bytes) {
    this->data = (uint8_t *)malloc(bytes);
    this->bytes = bytes;
    memcpy(this->data, data, bytes);
}

void tile::cancel()
{
    std::lock_guard<std::mutex> lock(mutex);

    // TODO: thread safety
    if (!cancelled) {
        cancelled = true;
    } else {
        assert((!"logic error? multiple cancelleations"));
    }
}

bool tile::parse()
{
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (cancelled) {
            return false;
        }
    }

    // fprintf(stderr, "[%p] parsing tile [%d/%d/%d]...\n", this, z, x, y);

    pbf tile(data, bytes);

    int code = setjmp(tile.jump_buffer);
    if (code > 0) {
        fprintf(stderr, "[%p] parsing tile [%d/%d/%d]... failed: %s\n", this, z, x, y, tile.msg.c_str());
        cancel();
        return false;
    }

    while (tile.next()) {
        if (tile.tag == 3) { // layer
            uint32_t bytes = (uint32_t)tile.varint();
            parseLayer(tile.data, bytes);
            tile.skipBytes(bytes);
        } else {
            tile.skip();
        }
    }

    // fprintf(stderr, "[%p] parsing tile [%d/%d/%d]... done\n", this, z, x, y);

    {
        std::lock_guard<std::mutex> lock(mutex);
        if (cancelled) {
            return false;
        }
    }

    loaded = true;
    return true;
}

void tile::parseLayer(const uint8_t *data, uint32_t bytes) {
    pbf layer(data, bytes);
    std::string name;
    while (layer.next()) {
        if (layer.tag == 1) {
            name = layer.string();
        } else if (layer.tag == 2) {
            uint32_t bytes = (uint32_t)layer.varint();
            parseFeature(layer.data, bytes);
            layer.skipBytes(bytes);
        } else {
            layer.skip();
        }
    }
}

void tile::parseFeature(const uint8_t *data, uint32_t bytes) {
    pbf feature(data, bytes);
    while (feature.next()) {
        if (feature.tag == 1) {
            /*uint32_t id =*/ feature.varint();
        } else if (feature.tag == 2) {
            const uint8_t *tag_end = feature.data + feature.varint();
            while (feature.data < tag_end) {
                /*uint32_t key =*/ feature.varint();
                /*uint32_t value =*/ feature.varint();
            }
        } else if (feature.tag == 3) {
            /*uint32_t type =*/ feature.varint();
        } else if (feature.tag == 4) {
            uint32_t bytes = (uint32_t)feature.varint();
            loadGeometry(feature.data, bytes);
            feature.skipBytes(bytes);
        } else {
            feature.skip();
        }
    }
}

void tile::loadGeometry(const uint8_t *data, uint32_t bytes) {
    pbf geometry(data, bytes);

    uint32_t cmd = 1;
    uint32_t length = 0;
    int32_t x = 0, y = 0;

    // var lines = [];
    // var line = null;
    int32_t ox = 0, oy = 0;

    while (geometry.data < geometry.end) {
        if (!length) {
            uint32_t cmd_length = (uint32_t)geometry.varint();
            cmd = cmd_length & 0x7;
            length = cmd_length >> 3;
        }

        length--;

        if (cmd == 1 || cmd == 2) {
            x += geometry.svarint();
            y += geometry.svarint();

            if (cmd == 1) {
                // moveTo
                // fprintf(stderr, "[m %d/%d] ", x, y);
                // degenerate vertex
                lineVertex.addDegenerate();
                ox = x;
                oy = y;
            } else {
                // lineTo
                // fprintf(stderr, "[l %d/%d] ", x, y);
            }
            lineVertex.addCoordinate(x, y);
        } else if (cmd == 7) {
            // closePolygon
            // fprintf(stderr, "[c]\n");
            lineVertex.addCoordinate(ox, oy);
        } else {
            // throw new Error('unknown command ' + cmd);
            // throw std::runtime_error("unknown command");
            fprintf(stderr, "unknown command");
            exit(1);
        }
    }
}
