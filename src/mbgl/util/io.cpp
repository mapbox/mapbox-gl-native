#include <mbgl/util/io.hpp>

#include <cerrno>
#include <iostream>
#include <fstream>

namespace mbgl {
namespace util {

void writeFile(const std::string &filename, Blob blob) {
    std::ofstream file(filename, std::ios::binary);
    if (file.good()) {
        file << *blob.uncompressedData();
    } else {
        throw IOException(errno, "failed to write file");
    }
}

Blob readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (file.good()) {
        return { { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() },
                 false };
    } else {
        return {};
    }
}

void deleteFile(const std::string& filename) {
    const int ret = std::remove(filename.c_str());
    if (ret != 0) {
        throw IOException(errno, "failed to unlink file");
    }
}

} // namespace util
} // namespace mbgl
