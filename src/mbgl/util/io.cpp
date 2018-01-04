#include <mbgl/util/io.hpp>

#include <cstdio>
#include <cerrno>
#include <iostream>
#include <sstream>
#include <fstream>

namespace mbgl {
namespace util {

void write_file(const std::string &filename, const std::string &data) {
    FILE *fd = fopen(filename.c_str(), "wb");
    if (fd) {
        fwrite(data.data(), sizeof(std::string::value_type), data.size(), fd);
        fclose(fd);
    } else {
        throw std::runtime_error(std::string("Failed to open file ") + filename);
    }
}

std::string read_file(const std::string &filename) {
    std::ifstream file(filename);
    if (file.good()) {
        std::stringstream data;
        data << file.rdbuf();
        return data.str();
    } else {
        throw std::runtime_error(std::string("Cannot read file ") + filename);
    }
}

optional<std::string> readFile(const std::string &filename) {
    std::ifstream file(filename);
    if (file.good()) {
        std::stringstream data;
        data << file.rdbuf();
        return data.str();
    }
    return {};
}

void deleteFile(const std::string& filename) {
    const int ret = std::remove(filename.c_str());
    if (ret != 0) {
        throw IOException(errno, "failed to unlink file");
    }
}

} // namespace util
} // namespace mbgl
