#ifndef LLMR_MAP_TILE
#define LLMR_MAP_TILE

#include "../geometry/debug_font_buffer.hpp"
#include "../geometry/linevertexbuffer.hpp"
#include "../geometry/fill_buffer.hpp"

#include <stdint.h>
#include <forward_list>
#include <mutex>
#include <llmr/util/vec.hpp>
#include <string>

namespace llmr {

struct pbf;

class Tile {
public:
    struct exception : std::exception {};
    struct geometry_too_long_exception : exception {};

public:
    typedef std::shared_ptr<Tile> Ptr;

    typedef vec3<int32_t> ID;

    enum state {
        initial,
        loading,
        ready,
        obsolete
    };

    struct fill_index {
        struct group {
            uint32_t vertex_length;
            uint32_t elements_length;

            group() : vertex_length(0), elements_length(0) {}
            group(uint32_t vertex_length, uint32_t elements_length)
                : vertex_length(vertex_length),
                  elements_length(elements_length) {
            }
        };

        uint32_t vertex_start;
        uint32_t elements_start;
        uint32_t length;
        std::vector<group> groups;

        fill_index(uint32_t vertex_start, uint32_t elements_start)
            : vertex_start(vertex_start),
              elements_start(elements_start),
              length(0),
              groups(1) {
        }

        // debug
        std::string name;
    };


    // struct fill_index {
    //     uint32_t vertex_start;
    //     uint32_t vertex_length;
    //     uint32_t elements_start;
    //     uint32_t elements_length;
    //     std::string name;

    //     fill_index(uint32_t vertex_start, uint32_t elements_start)
    //         : vertex_start(vertex_start),
    //           vertex_length(0),
    //           elements_start(elements_start),
    //           elements_length(0) {
    //     }
    // };

public:
    Tile(ID id);
    ~Tile();

    // Make noncopyable
    Tile(const Tile &) = delete;
    Tile(const Tile &&) = delete;
    Tile &operator=(const Tile &) = delete;
    Tile &operator=(const Tile && ) = delete;

    // Other functions
    void setData(uint8_t *data, uint32_t bytes);
    bool parse();
    void parseLayer(const uint8_t *data, uint32_t bytes);
    void parseFeature(const uint8_t *data, uint32_t bytes);

    void addLineGeometry(pbf &geom);
    void addFillGeometry(pbf &geom);


    void cancel();

    const std::string toString() const;

    static ID parent(const ID &id, int32_t z);
    static std::forward_list<ID> children(const ID &id, int32_t z);

public:
    const ID id;
    state state;
    linevertexbuffer lineVertex;
    debug_font_buffer debugFontVertex;

    fill_buffer fillBuffer;
    std::vector<fill_index> fillIndices;

private:
    // Source data
    uint8_t *data;
    uint32_t bytes;

    std::mutex mtx;
};

}

#endif
