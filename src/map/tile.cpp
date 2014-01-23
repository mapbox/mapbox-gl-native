#include <llmr/map/tile.hpp>

#include <stdint.h>
#include <cassert>
// #include <iostream>
#include <thread>

#include <llmr/util/pbf.hpp>
#include <llmr/util/string.hpp>
#include <llmr/geometry/geometry.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <cmath>

using namespace llmr;

Tile::ID Tile::parent(const ID& id, int32_t z) {
    assert(z < id.z);
    ID pos(id);
    while (pos.z > z) {
        pos.z--;
        pos.x = floor(pos.x / 2);
        pos.y = floor(pos.y / 2);
    }
    return pos;
}


std::forward_list<Tile::ID> Tile::children(const ID& id, int32_t z) {
    assert(z > id.z);
    int32_t factor = pow(2, z - id.z);

    std::forward_list<ID> children;
    for (int32_t y = id.y * factor, y_max = (id.y + 1) * factor; y < y_max; y++) {
        for (int32_t x = id.x * factor, x_max = (id.x + 1) * factor; x < x_max; x++) {
            children.emplace_front(x, y, z);
        }
    }
    return children;
}


Tile::Tile(ID id)
    : id(id),
      state(initial),
      fillBuffer(),
      data(0),
      bytes(0) {

    // Initialize tile debug coordinates
    char coord[32];
    snprintf(coord, sizeof(coord), "%d/%d/%d", id.z, id.x, id.y);
    debugFontVertex.addText(coord, 50, 200, 5);
}

Tile::~Tile() {
    std::lock_guard<std::mutex> lock(mtx);

    // fprintf(stderr, "[%p] deleting tile %d/%d/%d\n", this, id.z, id.x, id.y);
    if (this->data) {
        free(this->data);
    }
}

const std::string Tile::toString() const {
    return util::sprintf("[tile %d/%d/%d]", id.z, id.x, id.y);
}


void Tile::setData(uint8_t *data, uint32_t bytes) {
    this->data = (uint8_t *)malloc(bytes);
    this->bytes = bytes;
    memcpy(this->data, data, bytes);
}

void Tile::cancel() {
    // TODO: thread safety
    if (state != obsolete) {
        state = obsolete;
    } else {
        assert((!"logic error? multiple cancelleations"));
    }
}

bool Tile::parse() {
    std::lock_guard<std::mutex> lock(mtx);

    if (state == obsolete) {
        return false;
    }

    // fprintf(stderr, "[%p] parsing tile [%d/%d/%d]...\n", this, z, x, y);

    pbf tile(data, bytes);
    try {
        while (tile.next()) {
            if (tile.tag == 3) { // layer
                parseLayer(tile.message());
            } else {
                tile.skip();
            }
        }
    } catch (const std::exception& ex) {
        fprintf(stderr, "[%p] exception [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return false;
    }

    if (state == obsolete) {
        return false;
    } else {
        state = ready;
    }

    return true;
}

void Tile::parseLayer(const pbf data) {
    pbf layer(data);
    while (layer.next()) {
        if (layer.tag == 1) {
            std::string name = layer.string();
            if (name == "water") {
                layers.emplace_front(name, createFillBucket(data));
            }
        } else {
            layer.skip();
        }
    }
}

enum geom_type {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3
};

std::shared_ptr<Bucket> Tile::createFillBucket(const pbf data) {
    pbf layer(data);
    std::shared_ptr<FillBucket> bucket = std::make_shared<FillBucket>(fillBuffer);

    while (layer.next()) {
        if (layer.tag == 2) { // feature
            pbf feature = layer.message();
            pbf geometry;
            geom_type type = Unknown;

            while (feature.next()) {
                if (feature.tag == 3) { // geometry type
                    type = (geom_type)feature.varint();
                } else if (feature.tag == 4) { // geometry
                    geometry = feature.message();
                } else {
                    feature.skip();
                }
            }

            if (type == Polygon && geometry) {
                bucket->addGeometry(geometry);
            }
        } else {
            layer.skip();
        }
    }

    return bucket;
}

// void Tile::addLineGeometry(pbf& geom) {
//     Geometry geometry(geom);

//     Geometry::command cmd;
//     int32_t x, y;
//     while ((cmd = geometry.next(x, y)) != Geometry::end) {
//         if (cmd == Geometry::move_to) {
//             lineVertex.addDegenerate();
//         }
//         lineVertex.addCoordinate(x, y);
//     }
// }
