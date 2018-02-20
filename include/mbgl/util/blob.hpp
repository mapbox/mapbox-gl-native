#pragma once

#include <memory>
#include <string>

namespace mbgl {

class Blob {
public:
    Blob();
    Blob(std::shared_ptr<const std::string> bytes, bool gzip);
    Blob(std::string&& bytes, bool compressed);

    // Return uncompressed/compressed data.
    std::shared_ptr<const std::string> uncompressedData() const;
    std::shared_ptr<const std::string> compressedData() const;

    // Transform the blob to being uncompressed.
    void uncompress();

    bool isCompressed() const {
        return compressed;
    }

    explicit operator bool() const {
        return (bool)bytes;
    }

private:
    std::shared_ptr<const std::string> bytes;
    bool compressed;
};

} // namespace mbgl
