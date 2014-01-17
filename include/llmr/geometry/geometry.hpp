#ifndef LLMR_GEOMETRY_GEOMETRY
#define LLMR_GEOMETRY_GEOMETRY

#include <llmr/util/pbf.hpp>

#include <cstdlib>

namespace llmr {

class geometry {
public:
    inline geometry(const uint8_t *data, uint32_t bytes);

    enum command {
        end = 0,
        move_to = 1,
        line_to = 2,
        close = 7
    };

    inline command next(int32_t &rx, int32_t &ry);

private:
    pbf pbf;
    uint32_t cmd;
    uint32_t length;
    int32_t x, y;
    int32_t ox, oy;
};

geometry::geometry(const uint8_t *data, uint32_t bytes)
    : pbf(data, bytes),
      cmd(1),
      length(0),
      x(0), y(0),
      ox(0), oy(0) {}

geometry::command geometry::next(int32_t &rx, int32_t &ry) {
    if (pbf.data < pbf.end) {
        if (!length) {
            uint32_t cmd_length = (uint32_t)pbf.varint();
            cmd = cmd_length & 0x7;
            length = cmd_length >> 3;
        }

        length--;

        if (cmd == move_to || cmd == line_to) {
            rx = (x += pbf.svarint());
            ry = (y += pbf.svarint());

            if (cmd == move_to) {
                ox = x;
                oy = y;
                return move_to;
            } else {
                return line_to;
            }
        } else if (cmd == close) {
            rx = ox;
            ry = oy;
            return close;
        } else {
            fprintf(stderr, "unknown command: %d\n", cmd);
            // TODO: gracefully handle geometry parse failures
            return end;
        }
    } else {
        return end;
    }
}

}

#endif
