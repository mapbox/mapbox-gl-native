#include <llmr/map/tile.hpp>

#include <stdint.h>
// #include <iostream>
#include <thread>

#include "pbf.hpp"

using namespace llmr;

tile::tile(int32_t z, int32_t x, int32_t y)
    : z(z),
      x(x),
      y(y),
      loaded(false),
      data(0),
      bytes(0) {
}

void tile::setData(uint8_t *data, uint32_t bytes) {
    this->data = (uint8_t *)malloc(bytes);
    this->bytes = bytes;
    memcpy(this->data, data, bytes);
}

bool tile::parse()
{
    fprintf(stderr, "[%8zx] parsing tile\n",
        std::hash<std::thread::id>()(std::this_thread::get_id())
    );

    // try {
        pbf tile(data, bytes);
        while (tile.next()) {
            if (tile.tag == 3) { // layer
                uint32_t bytes = (uint32_t)tile.varint();
                parseLayer(tile.data, bytes);
                tile.skipBytes(bytes);
            } else {
                tile.skip();
            }
        }
    // } catch (std::exception& ex) {
    //     std::cerr << ex.what();
    //     return false;
    // }

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
            uint64_t id = feature.varint();
        } else if (feature.tag == 2) {
            const uint8_t *tag_end = feature.data + feature.varint();
            while (feature.data < tag_end) {
                uint32_t key = (uint32_t)feature.varint();
                uint32_t value = (uint32_t)feature.varint();
            }
        } else if (feature.tag == 3) {
            uint32_t type = (uint32_t)feature.varint();
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
