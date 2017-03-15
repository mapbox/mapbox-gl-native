#pragma once

#include <mbgl/util/optional.hpp>

#include <string>
#include <stdexcept>

namespace mbgl {
namespace util {

struct IOException : std::runtime_error {
    IOException(int err, const char* msg) : std::runtime_error(msg), code(err) {
    }
    const int code = 0;
};

void write_file(const std::string &filename, const std::string &data);
std::string read_file(const std::string &filename);

optional<std::string> readFile(const std::string &filename);
void deleteFile(const std::string& filename);

} // namespace util
} // namespace mbgl
