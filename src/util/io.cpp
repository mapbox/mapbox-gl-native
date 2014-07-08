#include <llmr/util/io.hpp>

#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>

namespace llmr {
namespace util {

void write_file(const std::string &filename, const std::string &data) {
    FILE *fd = fopen(filename.c_str(), "wb");
    if (fd) {
        fwrite(data.data(), sizeof(std::string::value_type), data.size(), fd);
        fclose(fd);
    } else {
        fprintf(stderr, "Failed to open file\n");
    }
}

std::string read_file(const std::string &filename) {
    std::ifstream file(filename);
    if (file.good()) {
        std::stringstream data;
        data << file.rdbuf();
        return data.str();
    } else {
        throw std::runtime_error("Cannot read file " + filename);
    }
}

}
}