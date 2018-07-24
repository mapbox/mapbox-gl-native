// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/shaders/hillshade.hpp>
#include <mbgl/util/compression.hpp>

#include <cstdint>

namespace mbgl {
namespace shaders {

const char* hillshade::name = "hillshade";
const char* hillshade::vertexSource = [] () {
    static const uint8_t compressed[] = {
        0x78, 0xda, 0x65, 0x8d, 0xc1, 0x0a, 0xc2, 0x30,
        0x10, 0x44, 0xef, 0xfb, 0x15, 0x73, 0x6c, 0xa5,
        0x68, 0x5b, 0x7a, 0x50, 0x82, 0xff, 0xe0, 0x1f,
        0x94, 0xa8, 0xb1, 0x2c, 0xd8, 0x44, 0xd2, 0x4d,
        0xa8, 0x88, 0xff, 0x6e, 0x4c, 0x51, 0x10, 0xf7,
        0xb2, 0xcb, 0xcc, 0xec, 0xbc, 0x60, 0xf9, 0xe2,
        0xfc, 0x88, 0x51, 0x4b, 0x87, 0xd0, 0xa7, 0xe5,
        0x79, 0x56, 0x44, 0x5a, 0xd2, 0x71, 0x0c, 0x62,
        0x10, 0xcd, 0xa9, 0x85, 0xee, 0x6f, 0x6e, 0x52,
        0xff, 0xaa, 0x98, 0x59, 0x82, 0x37, 0x8b, 0x4b,
        0x51, 0xfb, 0x3b, 0xdb, 0x61, 0x31, 0xe3, 0x47,
        0x74, 0x7c, 0x4e, 0xf5, 0x6c, 0x8b, 0x12, 0x0f,
        0x42, 0x9a, 0xe1, 0xda, 0x1f, 0xdc, 0xc4, 0xc2,
        0xce, 0x62, 0xff, 0x85, 0x62, 0xf5, 0xfe, 0xeb,
        0x8a, 0x8c, 0xaa, 0x50, 0x57, 0x68, 0x4a, 0x95,
        0xf3, 0xb9, 0x29, 0x25, 0x7f, 0x70, 0xd8, 0x60,
        0xdb, 0xec, 0xda, 0x75, 0xad, 0xe8, 0x49, 0x2f,
        0x97, 0x4d, 0x40, 0xaf
    };
    static std::string decompressed = util::decompress(std::string(reinterpret_cast<const char*>(compressed), sizeof(compressed)));
    return decompressed.c_str();
}();
const char* hillshade::fragmentSource = [] () {
    static const uint8_t compressed[] = {
        0x78, 0xda, 0x8d, 0x56, 0xdb, 0x6e, 0xdb, 0x38,
        0x10, 0x7d, 0xf7, 0x57, 0x4c, 0x91, 0x87, 0x95,
        0x1b, 0x5b, 0x8a, 0x95, 0x78, 0x9d, 0xa4, 0x9b,
        0x16, 0x8b, 0xa6, 0x05, 0x02, 0x14, 0x6d, 0x90,
        0x16, 0xed, 0xc3, 0xa2, 0x30, 0x68, 0x89, 0x96,
        0x98, 0xa5, 0x44, 0x95, 0xa4, 0x7c, 0xe9, 0x62,
        0xff, 0xbd, 0x33, 0x24, 0x65, 0xcb, 0x4e, 0xb0,
        0xd8, 0xbc, 0x24, 0x21, 0x67, 0xce, 0x5c, 0xce,
        0x99, 0xa1, 0xda, 0x5a, 0x2c, 0x95, 0xae, 0xc0,
        0xb0, 0xaa, 0x91, 0x5c, 0xa7, 0xb7, 0xd0, 0xce,
        0x45, 0xc5, 0x0a, 0xfe, 0x6a, 0xb0, 0x62, 0x7a,
        0x2b, 0xea, 0x02, 0x56, 0x3c, 0x4b, 0x61, 0x35,
        0x6f, 0x94, 0x79, 0x35, 0x18, 0xb4, 0xc1, 0xde,
        0x1d, 0xb6, 0x73, 0xc9, 0xac, 0x66, 0x35, 0x59,
        0x1f, 0x5f, 0x88, 0xa2, 0xb4, 0x07, 0xa7, 0x17,
        0x78, 0x6a, 0x4a, 0x96, 0xab, 0xf5, 0x93, 0xe3,
        0x12, 0x8d, 0x9f, 0x77, 0x60, 0x59, 0xc6, 0x6b,
        0x3c, 0x1e, 0x9c, 0xe4, 0x7c, 0x29, 0x6a, 0x0e,
        0xf7, 0x77, 0x70, 0x1e, 0x4f, 0x2e, 0x26, 0xd3,
        0xab, 0xf4, 0xf7, 0xe9, 0xf9, 0xf4, 0xf2, 0x6a,
        0x76, 0x75, 0x3e, 0x18, 0xac, 0x94, 0xc8, 0xa1,
        0x62, 0xa2, 0x8e, 0x86, 0xf0, 0xcf, 0x00, 0xf0,
        0xc7, 0xf9, 0x37, 0x62, 0xc3, 0x25, 0xdc, 0x80,
        0xe5, 0x1b, 0xdb, 0x6a, 0x9e, 0xde, 0x46, 0xa1,
        0xb8, 0x91, 0xaf, 0x67, 0x88, 0xc0, 0xc1, 0x38,
        0x85, 0x9c, 0x6b, 0xb1, 0x42, 0xe3, 0x28, 0x72,
        0x6e, 0xb1, 0x2e, 0xe0, 0x25, 0xa4, 0xf1, 0xd9,
        0x10, 0xc6, 0x30, 0xc1, 0x5f, 0xc1, 0x36, 0x49,
        0xe0, 0x1b, 0x87, 0x5c, 0xac, 0x44, 0xce, 0xc1,
        0x96, 0x1c, 0x8c, 0x54, 0x0d, 0x87, 0xc5, 0x16,
        0x18, 0x98, 0x8c, 0x49, 0x0e, 0x4b, 0x96, 0x59,
        0xa5, 0x61, 0xc1, 0x0c, 0xcf, 0x41, 0xd5, 0xce,
        0x28, 0x53, 0x46, 0xd4, 0xa0, 0x96, 0xee, 0x1f,
        0x87, 0xff, 0x9b, 0x01, 0xd6, 0x34, 0x5a, 0x6d,
        0x30, 0x21, 0xcb, 0x01, 0x1b, 0x29, 0x6c, 0x9b,
        0xf3, 0x2e, 0x86, 0x55, 0x80, 0xb5, 0xab, 0xb6,
        0xb6, 0x80, 0xfd, 0x80, 0x8a, 0xeb, 0x8c, 0x11,
        0x2a, 0x7a, 0x3c, 0xf2, 0xcc, 0x0a, 0xc4, 0xcd,
        0x85, 0xc1, 0x13, 0xfa, 0x33, 0x06, 0xc3, 0x39,
        0x9c, 0x5c, 0x5c, 0x9e, 0xcd, 0x9c, 0x75, 0xce,
        0x2d, 0x13, 0xd2, 0x38, 0xac, 0xa5, 0x54, 0xcc,
        0xfa, 0xcc, 0xde, 0xfb, 0xc4, 0x6e, 0x28, 0x9b,
        0x48, 0xb3, 0x5c, 0xb0, 0xda, 0x44, 0xd1, 0x9e,
        0xc4, 0xbf, 0xce, 0xbe, 0x63, 0xad, 0xbd, 0xff,
        0x27, 0xdf, 0x87, 0xd8, 0x83, 0x08, 0xab, 0xc7,
        0x73, 0xd7, 0xb1, 0x78, 0x3b, 0x84, 0xd3, 0x23,
        0x13, 0xec, 0xcc, 0xbe, 0x31, 0x4c, 0x1a, 0x05,
        0x55, 0x2b, 0xad, 0x68, 0xe4, 0xf6, 0xa8, 0x3f,
        0x35, 0x30, 0xbd, 0x10, 0xe8, 0xa8, 0xb7, 0xf0,
        0x73, 0x1c, 0xda, 0x84, 0x3d, 0x99, 0xc4, 0xe9,
        0xb4, 0x9f, 0xab, 0x73, 0xb8, 0x01, 0x66, 0x59,
        0x1d, 0xd1, 0x1d, 0xe6, 0x20, 0x79, 0x5d, 0xd8,
        0x32, 0x72, 0x1c, 0x0d, 0x21, 0xe9, 0xd7, 0x13,
        0xc2, 0x7b, 0x5f, 0x66, 0x1a, 0xec, 0x0e, 0x3a,
        0x3b, 0xcb, 0x78, 0x03, 0x2f, 0x6e, 0xe0, 0x0c,
        0xd3, 0x7f, 0xe3, 0xd1, 0xfc, 0xe9, 0x76, 0x04,
        0xe3, 0x70, 0x3f, 0x84, 0x6b, 0x92, 0x54, 0x42,
        0x44, 0x53, 0xa9, 0xc1, 0x00, 0x5e, 0x07, 0x2f,
        0x2a, 0xfd, 0x1a, 0xc6, 0x3d, 0xfe, 0x7d, 0x1c,
        0x51, 0x5b, 0x5e, 0x1b, 0x61, 0xb7, 0x18, 0x2a,
        0x88, 0x3d, 0xde, 0x1c, 0xf4, 0x21, 0xcf, 0x09,
        0x18, 0x69, 0xac, 0xd8, 0xdf, 0x24, 0x14, 0x61,
        0x88, 0xbb, 0x86, 0x6b, 0xf4, 0x41, 0xc6, 0xb3,
        0xd2, 0x35, 0xa7, 0x90, 0x6a, 0xc1, 0x24, 0x38,
        0x00, 0x50, 0x0b, 0xa2, 0x76, 0x04, 0xeb, 0x52,
        0xe0, 0x35, 0x22, 0x18, 0x84, 0x48, 0x52, 0x02,
        0x21, 0x5b, 0x67, 0x44, 0xb2, 0xf9, 0x29, 0xaa,
        0xd6, 0x96, 0x4c, 0x76, 0xe1, 0x90, 0x17, 0xe1,
        0x14, 0xb1, 0x0b, 0x70, 0xa4, 0x9e, 0xb3, 0x9c,
        0x17, 0x48, 0xba, 0xd6, 0xdc, 0x34, 0xaa, 0xce,
        0x69, 0xa8, 0xd1, 0xa2, 0x46, 0xf1, 0x94, 0x09,
        0x21, 0x5b, 0xd5, 0x74, 0xda, 0x5c, 0x09, 0xbe,
        0x6e, 0xf0, 0x02, 0x2b, 0xf4, 0xec, 0xd9, 0x2d,
        0x4a, 0x9a, 0xba, 0xda, 0x45, 0x63, 0x75, 0xee,
        0x6e, 0x94, 0x16, 0x85, 0xa8, 0x31, 0x79, 0x1a,
        0x6a, 0xae, 0x61, 0xcd, 0x0c, 0xac, 0xb5, 0xb0,
        0xd8, 0x98, 0x10, 0x9f, 0x37, 0x16, 0xa2, 0xb1,
        0x90, 0xb2, 0xad, 0xd0, 0x90, 0x32, 0xbc, 0x15,
        0x3a, 0x88, 0x77, 0x0c, 0x57, 0x38, 0x59, 0xe8,
        0x42, 0x50, 0xbb, 0x8a, 0xe2, 0x3e, 0x95, 0xfe,
        0xb0, 0xd7, 0xe0, 0x2d, 0x4a, 0xef, 0xfe, 0xee,
        0x60, 0x0e, 0xfd, 0xc4, 0xed, 0x65, 0xc6, 0x37,
        0x58, 0x20, 0x6e, 0x0c, 0xc1, 0x24, 0xca, 0x6f,
        0x37, 0x82, 0x3b, 0xba, 0x46, 0xd0, 0x1a, 0x2a,
        0x9f, 0x01, 0x3a, 0x66, 0xad, 0x74, 0x59, 0x81,
        0x11, 0x95, 0x90, 0x4c, 0x63, 0xda, 0xbb, 0xf1,
        0x2b, 0x0f, 0xb0, 0x1c, 0x80, 0x6e, 0x54, 0xb0,
        0x5f, 0xb6, 0xb5, 0xaf, 0xe2, 0x49, 0x93, 0xae,
        0x3b, 0x80, 0xd2, 0xda, 0xc6, 0x5c, 0x27, 0x49,
        0x21, 0x6c, 0xd9, 0x2e, 0xe2, 0x4c, 0x55, 0x49,
        0xc5, 0x9a, 0x85, 0xda, 0x84, 0x5f, 0xe3, 0x42,
        0x8e, 0x1f, 0x4d, 0xb2, 0x40, 0xfe, 0xf1, 0xc4,
        0x20, 0x7a, 0x62, 0x74, 0x96, 0x38, 0xa4, 0x31,
        0x21, 0x25, 0x18, 0x1e, 0xf9, 0x32, 0x18, 0x26,
        0x71, 0xbb, 0xcf, 0x51, 0x6c, 0x92, 0x7d, 0x26,
        0x3c, 0x7e, 0x34, 0x27, 0x1f, 0xd2, 0xc9, 0x6c,
        0xfc, 0x21, 0x4d, 0x2f, 0xbb, 0xc0, 0x86, 0xc4,
        0x82, 0xed, 0xa3, 0x9d, 0x8a, 0xac, 0xec, 0x85,
        0xba, 0x62, 0xb2, 0xe5, 0x06, 0x32, 0xcd, 0x69,
        0xdf, 0x54, 0x4a, 0x23, 0x85, 0x0d, 0xfb, 0xd1,
        0x72, 0x34, 0x95, 0x92, 0x48, 0xc4, 0xc6, 0xf4,
        0x09, 0xa0, 0xee, 0x61, 0xf7, 0x27, 0xf1, 0xe5,
        0x6c, 0x8a, 0x7c, 0xed, 0x91, 0x5e, 0xe2, 0xd9,
        0x6c, 0xda, 0x1f, 0xbb, 0x8a, 0x6d, 0xbe, 0x12,
        0x3c, 0xd0, 0xb4, 0xd1, 0xbc, 0x12, 0x4d, 0x47,
        0xdb, 0x27, 0xff, 0x1c, 0xe6, 0x7a, 0x0f, 0xf4,
        0xc2, 0x9b, 0xbf, 0xa1, 0x9d, 0xab, 0xd6, 0x11,
        0x05, 0x1c, 0x79, 0x22, 0xbb, 0xa5, 0x8b, 0x93,
        0xd9, 0xbb, 0xea, 0xc2, 0x74, 0xb7, 0xb4, 0x9d,
        0x76, 0xa1, 0xaf, 0xbd, 0xe7, 0x5e, 0x1e, 0x5f,
        0x48, 0x59, 0xee, 0xfd, 0x40, 0xed, 0x4b, 0x1c,
        0x03, 0x1c, 0xc1, 0x8e, 0x75, 0x94, 0xc5, 0x1a,
        0x99, 0xd9, 0xef, 0x66, 0xde, 0x0d, 0x80, 0xd7,
        0x11, 0x8e, 0x60, 0xa7, 0x2b, 0x52, 0xf7, 0x7f,
        0x03, 0x90, 0xfb, 0x71, 0xfb, 0xed, 0x51, 0x70,
        0x9c, 0x5b, 0x4d, 0x7d, 0xc7, 0x28, 0x59, 0x49,
        0x9b, 0x13, 0x38, 0x96, 0x64, 0x48, 0x41, 0xcf,
        0xc6, 0x42, 0x7b, 0x6f, 0xa0, 0x5a, 0x7b, 0x30,
        0x15, 0x1e, 0xd3, 0xef, 0xf0, 0x5e, 0x5f, 0x0f,
        0x96, 0xf0, 0x6e, 0xff, 0x2e, 0x54, 0x48, 0x31,
        0xb8, 0xd1, 0xec, 0xf6, 0x0b, 0x72, 0xaf, 0x56,
        0x26, 0xf1, 0xed, 0xc7, 0x7a, 0x8e, 0xa4, 0x72,
        0x5c, 0x51, 0x77, 0x2d, 0x30, 0xa9, 0xd7, 0x9e,
        0xb8, 0x9c, 0x96, 0x88, 0xa5, 0x4d, 0xe5, 0xc4,
        0xe9, 0x66, 0x8e, 0x2d, 0x97, 0xb4, 0x81, 0x7d,
        0x63, 0x29, 0x86, 0xd7, 0x5d, 0x87, 0xe6, 0x8b,
        0x7d, 0xa2, 0xca, 0x3f, 0x1c, 0x5e, 0x58, 0x95,
        0xd8, 0xa5, 0x15, 0xd7, 0x88, 0x16, 0x10, 0x9c,
        0x56, 0xe9, 0xbd, 0x31, 0x0d, 0xd3, 0x58, 0x46,
        0xbc, 0x7f, 0xe1, 0x7d, 0x5d, 0x73, 0x6f, 0x77,
        0xd3, 0x3d, 0x55, 0xfe, 0x94, 0xd4, 0xd1, 0x7d,
        0x41, 0xe0, 0x9f, 0xae, 0xcc, 0xa8, 0xaf, 0x62,
        0xdc, 0xf9, 0x23, 0xda, 0xf3, 0xa3, 0xf0, 0xbe,
        0xf7, 0x04, 0xeb, 0x7a, 0x84, 0x4f, 0xd0, 0xc2,
        0x44, 0x95, 0xca, 0xa3, 0x28, 0xbc, 0x2b, 0xa7,
        0xdd, 0x56, 0x22, 0x61, 0xe2, 0x7a, 0x3f, 0xa5,
        0xb4, 0x47, 0x87, 0x5f, 0x09, 0xbb, 0xdc, 0x1c,
        0xc8, 0x2e, 0xb5, 0x4a, 0x6c, 0xa2, 0xee, 0x03,
        0x68, 0xd4, 0xff, 0xe6, 0x19, 0x79, 0x43, 0xca,
        0x16, 0x85, 0x74, 0xc0, 0xe9, 0xff, 0xce, 0xba,
        0x90, 0xf3, 0xf7, 0x9a, 0x15, 0x6f, 0x43, 0xac,
        0x83, 0xae, 0xec, 0x1e, 0xf0, 0x5e, 0x3e, 0x31,
        0xa3, 0x67, 0xbc, 0x77, 0x40, 0x5f, 0x58, 0x62,
        0x89, 0x7b, 0x06, 0x3e, 0x7d, 0x7d, 0xf7, 0x70,
        0xfb, 0xf0, 0xe7, 0xb7, 0xf9, 0xdd, 0xc7, 0xcf,
        0xf7, 0xef, 0xde, 0x7e, 0xf9, 0xf4, 0xf0, 0x5c,
        0x04, 0x2a, 0x30, 0xf2, 0xf1, 0x4f, 0x38, 0xbe,
        0x28, 0xcb, 0xc1, 0xbf, 0x83, 0x5f, 0x25, 0x18,
        0x54, 0xec
    };
    static std::string decompressed = util::decompress(std::string(reinterpret_cast<const char*>(compressed), sizeof(compressed)));
    return decompressed.c_str();
}();

} // namespace shaders
} // namespace mbgl
