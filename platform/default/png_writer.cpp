#include <mbgl/util/compression.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/premultiply.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/crc.hpp>
#pragma GCC diagnostic pop

#include <cassert>
#include <cstring>

#define NETWORK_BYTE_UINT32(value)                                                                 \
    char(value >> 24), char(value >> 16), char(value >> 8), char(value >> 0)

namespace {

void addChunk(std::string& png, const char* type, const char* data = "", const uint32_t size = 0) {
    assert(strlen(type) == 4);

    // Checksum encompasses type + data
    boost::crc_32_type checksum;
    checksum.process_bytes(type, 4);
    checksum.process_bytes(data, size);

    const char length[4] = { NETWORK_BYTE_UINT32(size) };
    const char crc[4] = { NETWORK_BYTE_UINT32(checksum.checksum()) };

    png.reserve(png.size() + 4 /* length */ + 4 /* type */ + size + 4 /* CRC */);
    png.append(length, 4);
    png.append(type, 4);
    png.append(data, size);
    png.append(crc, 4);
}

} // namespace

namespace mbgl {

// Encode PNGs without libpng.
std::string encodePNG(const PremultipliedImage& pre) {
    // Make copy of the image so that we can unpremultiply it.
    const auto src = util::unpremultiply(pre.clone());

    // PNG magic bytes
    const char preamble[8] = { char(0x89), 'P', 'N', 'G', '\r', '\n', 0x1a, '\n' };

    // IHDR chunk for our RGBA image.
    const char ihdr[13] = {
        NETWORK_BYTE_UINT32(src.size.width),  // width
        NETWORK_BYTE_UINT32(src.size.height), // height
        8,                                    // bit depth == 8 bits
        6,                                    // color type == RGBA
        0,                                    // compression method == deflate
        0,                                    // filter method == default
        0,                                    // interlace method == none
    };

    // Prepare the (compressed) data chunk.
    const auto stride = src.stride();
    std::string idat;
    for (uint32_t y = 0; y < src.size.height; y++) {
        // Every scanline needs to be prefixed with one byte that indicates the filter type.
        idat.append(1, 0); // filter type 0
        idat.append((const char*)(src.data.get() + y * stride), stride);
    }
    idat = util::compress(idat);

    // Assemble the PNG.
    std::string png;
    png.reserve((8 /* preamble */) + (12 + 13 /* IHDR */) +
                (12 + idat.size() /* IDAT */) + (12 /* IEND */));
    png.append(preamble, 8);
    addChunk(png, "IHDR", ihdr, 13);
    addChunk(png, "IDAT", idat.data(), static_cast<uint32_t>(idat.size()));
    addChunk(png, "IEND");
    return png;
}

} // namespace mbgl
