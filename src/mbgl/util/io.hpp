#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/blob.hpp>

#include <string>
#include <stdexcept>

namespace mbgl {
namespace util {

struct IOException : std::runtime_error {
    IOException(int err, const char* msg) : std::runtime_error(msg), code(err) {
    }
    const int code = 0;
};

void writeFile(const std::string &filename, Blob blob);
Blob readFile(const std::string &filename);
void deleteFile(const std::string& filename);

} // namespace util
} // namespace mbgl
