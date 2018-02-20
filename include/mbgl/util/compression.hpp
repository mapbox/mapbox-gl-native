#pragma once

#include <string>

namespace mbgl {
namespace util {

// Compresses data with the deflate algorithm.
std::string compress(const std::string& raw);

// Decompresses data that is in deflate format, optionally wrapped in a gzip container.
std::string decompress(const std::string& raw);

// Returns true when there's a good chance that the string can be compressed.
// In particular, it returns false when the data is an already compressed image format.
bool isCompressible(const std::string& raw);

} // namespace util
} // namespace mbgl
