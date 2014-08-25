#include <mbgl/util/compression.hpp>

#include <zlib.h>

#include <cstring>
#include <stdexcept>

namespace mbgl {
namespace util {

std::string compress(const std::string &raw) {
    z_stream deflate_stream;
    memset(&deflate_stream, 0, sizeof(deflate_stream));

    // TODO: reuse z_streams
    if (deflateInit(&deflate_stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
        throw std::runtime_error("failed to initialize deflate");
    }

    deflate_stream.next_in = (Bytef *)raw.data();
    deflate_stream.avail_in = raw.size();

    std::string result;
    char out[16384];

    int code;
    do {
        deflate_stream.next_out = reinterpret_cast<Bytef *>(out);
        deflate_stream.avail_out = sizeof(out);
        code = deflate(&deflate_stream, Z_FINISH);
        if (result.size() < deflate_stream.total_out) {
            // append the block to the output string
            result.append(out, deflate_stream.total_out - result.size());
        }
    } while (code == Z_OK);

    deflateEnd(&deflate_stream);

    if (code != Z_STREAM_END) {
        throw std::runtime_error(deflate_stream.msg);
    }

    return result;
}

std::string decompress(const std::string &raw) {
    z_stream inflate_stream;
    memset(&inflate_stream, 0, sizeof(inflate_stream));

    // TODO: reuse z_streams
    if (inflateInit(&inflate_stream) != Z_OK) {
        throw std::runtime_error("failed to initialize inflate");
    }

    inflate_stream.next_in = (Bytef *)raw.data();
    inflate_stream.avail_in = raw.size();

    std::string result;
    char out[15384];

    int code;
    do {
        inflate_stream.next_out = reinterpret_cast<Bytef *>(out);
        inflate_stream.avail_out = sizeof(out);
        code = inflate(&inflate_stream, 0);
        // result.append(out, sizeof(out) - inflate_stream.avail_out);
        if (result.size() < inflate_stream.total_out) {
            result.append(out, inflate_stream.total_out - result.size());
        }
    } while (code == Z_OK);

    inflateEnd(&inflate_stream);

    if (code != Z_STREAM_END) {
        throw std::runtime_error(inflate_stream.msg);
    }

    return result;
}
}
}
