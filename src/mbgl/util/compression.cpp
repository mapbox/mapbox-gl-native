#include <mbgl/util/compression.hpp>

#if defined(__QT__) && defined(_WINDOWS)
#include <QtZlib/zlib.h>
#else
#include <zlib.h>
#endif

#include <cstdio>
#include <cstring>
#include <stdexcept>

// Check zlib library version.
const static bool zlibVersionCheck __attribute__((unused)) = []() {
    const char *const version = zlibVersion();
    if (version[0] != ZLIB_VERSION[0]) {
        char message[96];
        snprintf(message, 96, "zlib version mismatch: headers report %s, but library reports %s",
                 ZLIB_VERSION, version);
        throw std::runtime_error(message);
    }

    return true;
}();

namespace mbgl {
namespace util {

// Needed when using a zlib compiled with -DZ_PREFIX
// because it will mess with this function name and
// cause a link error.
#undef compress

std::string compress(const std::string &raw) {
    z_stream deflate_stream;
    memset(&deflate_stream, 0, sizeof(deflate_stream));

    // TODO: reuse z_streams
    if (deflateInit(&deflate_stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
        throw std::runtime_error("failed to initialize deflate");
    }

    deflate_stream.next_in = (Bytef *)raw.data();
    deflate_stream.avail_in = uInt(raw.size());

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
    // MAX_WBITS + allows decoding gzip in addition to zlib
    if (inflateInit2(&inflate_stream, MAX_WBITS + 32) != Z_OK) {
        throw std::runtime_error("failed to initialize inflate");
    }

    inflate_stream.next_in = (Bytef *)raw.data();
    inflate_stream.avail_in = uInt(raw.size());

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
        throw std::runtime_error(inflate_stream.msg ? inflate_stream.msg : "decompression error");
    }

    return result;
}

bool isCompressible(const std::string& raw) {
    // WebP
    if (raw.size() >= 12 && static_cast<uint8_t>(raw[0]) == 'R' &&
        static_cast<uint8_t>(raw[1]) == 'I' && static_cast<uint8_t>(raw[2]) == 'F' &&
        static_cast<uint8_t>(raw[3]) == 'F' && static_cast<uint8_t>(raw[8]) == 'W' &&
        static_cast<uint8_t>(raw[9]) == 'E' && static_cast<uint8_t>(raw[10]) == 'B' &&
        static_cast<uint8_t>(raw[11]) == 'P') {
        // Note: the WebP container format allows uncompressed data as well, but we just assume that
        // all WebP files are already compressed.
        return false;
    }

    // PNG
    if (raw.size() >= 8 && static_cast<uint8_t>(raw[0]) == 0x89 &&
        static_cast<uint8_t>(raw[1]) == 'P' && static_cast<uint8_t>(raw[2]) == 'N' &&
        static_cast<uint8_t>(raw[3]) == 'G' && static_cast<uint8_t>(raw[4]) == '\r' &&
        static_cast<uint8_t>(raw[5]) == '\n' && static_cast<uint8_t>(raw[6]) == 0x1a &&
        static_cast<uint8_t>(raw[7]) == '\n') {
        // Note: this assumes the PNG file itself is compressed. However, it is possible to create
        // PNG files with uncompressed data in it (zlib compression 0), but they are exceedingly
        // rare, so we don't care about them.
        return false;
    }

    // JPEG
    if (raw.size() >= 3 && static_cast<uint8_t>(raw[0]) == 0xff &&
        static_cast<uint8_t>(raw[1]) == 0xd8 && static_cast<uint8_t>(raw[2]) == 0xff) {
        return false;
    }

    return true;
}

} // namespace util
} // namespace mbgl
