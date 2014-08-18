#ifndef MBGL_GEOMETRY_GEOMETRY
#define MBGL_GEOMETRY_GEOMETRY

#include <mbgl/util/pbf.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <cstdlib>

namespace mbgl {

class Geometry : private util::noncopyable {

public:
    inline explicit Geometry(pbf& data);

    enum command : uint8_t {
        end = 0,
        move_to = 1,
        line_to = 2,
        close = 7
    };

    inline command next(int32_t &rx, int32_t &ry);

private:
    pbf& data;
    uint8_t cmd;
    uint32_t length;
    int32_t x, y;
    int32_t ox, oy;
};

Geometry::Geometry(pbf& data)
    : data(data),
      cmd(1),
      length(0),
      x(0), y(0),
      ox(0), oy(0) {}

Geometry::command Geometry::next(int32_t &rx, int32_t &ry) {
    if (data.data < data.end) {
        if (length == 0) {
            uint32_t cmd_length = static_cast<uint32_t>(data.varint());
            cmd = cmd_length & 0x7;
            length = cmd_length >> 3;
        }

        --length;

        if (cmd == move_to || cmd == line_to) {
            rx = (x += data.svarint());
            ry = (y += data.svarint());

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
