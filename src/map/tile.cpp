#include <llmr/map/tile.hpp>

#include <stdint.h>
#include <cassert>
// #include <iostream>
#include <thread>

#include <llmr/util/pbf.hpp>
#include <llmr/util/string.hpp>
#include <llmr/geometry/geometry.hpp>
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
                uint32_t bytes = (uint32_t)tile.varint();
                parseLayer(tile.data, bytes);
                tile.skipBytes(bytes);
            } else {
                tile.skip();
            }
        }
    } catch (const pbf::exception& ex) {
        fprintf(stderr, "[%p] parsing tile [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return false;
    } catch (const Tile::exception& ex) {
        fprintf(stderr, "[%p] parsing tile [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return false;
    } catch (const std::exception& ex) {
        fprintf(stderr, "[%p] general exception [%d/%d/%d]... failed: %s\n", this, id.z, id.x, id.y, ex.what());
        cancel();
        return false;
    }

    if (state == obsolete) {
        return false;
    } else {
        state = ready;
    }

    // int i = 0;
    // for (const fill_index& index : fillIndices) {
    //     fprintf(stderr, "[%p] %d: vertex   % 8d / % 8d\n", this, i, index.vertex_start, index.vertex_length);
    //     fprintf(stderr, "[%p] %d: elements % 8d / % 8d\n", this, i, index.elements_start, index.elements_length);
    //     i++;
    // }

    return true;
}

void Tile::parseLayer(const uint8_t *data, uint32_t bytes) {
    pbf layer(data, bytes);
    std::string name;

    uint32_t vertex_start = fillBuffer.vertex_length();
    uint32_t elements_start = fillBuffer.elements_length();
    fillIndices.emplace_back(vertex_start, elements_start);

    while (layer.next()) {
        if (layer.tag == 1) {
            name = layer.string();
            fillIndices.back().name = name;
        } else if (layer.tag == 2) {
            uint32_t bytes = (uint32_t)layer.varint();
            parseFeature(layer.data, bytes);
            layer.skipBytes(bytes);
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


void Tile::parseFeature(const uint8_t *data, uint32_t bytes) {
    pbf feature(data, bytes);

    geom_type type = Unknown;
    pbf geom;

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
            type = (geom_type)feature.varint();
        } else if (feature.tag == 4) {
            geom = feature.message();
        } else {
            feature.skip();
        }
    }

    if (geom) {
        if (type == LineString) {
            addLineGeometry(geom);
        } else if (type == Polygon) {
            addFillGeometry(geom);
        }
    }
}

void Tile::addFillGeometry(pbf& geom) {

    std::vector<std::vector<std::pair<int16_t, int16_t>>> lines;

    {
        std::vector<std::pair<int16_t, int16_t>> line;
        Geometry::command cmd;
        int32_t x, y;
        Geometry geometry(geom);
        while ((cmd = geometry.next(x, y)) != Geometry::end) {
            if (cmd == Geometry::move_to) {
                if (line.size()) {
                    lines.push_back(line);
                    line.clear();
                }
            }
            line.emplace_back(x, y);
        }
        if (line.size()) {
            lines.push_back(line);
        }
    }

    for (const std::vector<std::pair<int16_t, int16_t>>& line : lines) {
        uint32_t vertex_start = fillBuffer.vertex_length();

        fillBuffer.addDegenerate();
        for (const std::pair<int16_t, int16_t>& coord : line) {
            fillBuffer.addCoordinate(coord.first, coord.second);
        }

        uint32_t vertex_end = fillBuffer.vertex_length();

        if (vertex_end - vertex_start > 65535) {
            throw geometry_too_long_exception();
        }

        if (!fillIndices.size()) {
            // Create a new index because there is none yet.
            throw std::runtime_error("no index yet");
        }

        fill_index& index = fillIndices.back();
        if (!index.groups.size()) {
            throw std::runtime_error("no group yet");
        }

        uint32_t vertex_count = vertex_end - vertex_start;
        index.length += vertex_count;

        if (index.groups.back().vertex_length + vertex_count > 65535) {
            // Move to a new group because the old one can't hold the geometry.
            index.groups.emplace_back();
        }

        fill_index::group& group = index.groups.back();

        // We're generating triangle fans, so we always start with the first
        // coordinate in this polygon.
        // The first valid index that is not a degenerate.
        uint16_t firstIndex = group.vertex_length + 1;

        assert(firstIndex + vertex_count - 1 < 65536);

        uint32_t elements_start = fillBuffer.elements_length();

        for (uint32_t i = 2; i < vertex_count; i++) {
            fillBuffer.addElements(firstIndex, firstIndex + i - 1, firstIndex + i);
        }

        uint32_t elements_end = fillBuffer.elements_length();
        uint32_t elements_count = elements_end - elements_start;
        group.vertex_length += vertex_count;
        group.elements_length += elements_count;
    }
}

void Tile::addLineGeometry(pbf& geom) {
    Geometry geometry(geom);

    Geometry::command cmd;
    int32_t x, y;
    while ((cmd = geometry.next(x, y)) != Geometry::end) {
        if (cmd == Geometry::move_to) {
            lineVertex.addDegenerate();
        }
        lineVertex.addCoordinate(x, y);
    }
}
