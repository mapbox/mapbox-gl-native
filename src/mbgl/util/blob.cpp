#include <mbgl/util/blob.hpp>
#include <mbgl/util/compression.hpp>

namespace mbgl {

Blob::Blob() = default;

Blob::Blob(std::shared_ptr<const std::string> bytes_, bool compressed_)
    : bytes(std::move(bytes_)), compressed(compressed_) {
}

Blob::Blob(std::string&& bytes_, bool compressed_)
    : bytes(std::make_shared<const std::string>(std::move(bytes_))), compressed(compressed_) {
}

std::shared_ptr<const std::string> Blob::uncompressedData() const {
    if (!bytes) {
        throw std::runtime_error("invalid blob");
    }
    if (compressed) {
        return std::make_shared<const std::string>(util::decompress(*bytes));
    } else {
        return bytes;
    }
}

std::shared_ptr<const std::string> Blob::compressedData() const {
    if (!bytes) {
        throw std::runtime_error("invalid blob");
    }
    if (compressed) {
        return bytes;
    } else {
        return std::make_shared<const std::string>(util::compress(*bytes));
    }
}

void Blob::uncompress() {
    if (compressed) {
        bytes = uncompressedData();
        compressed = false;
    }
}


} // namespace mbgl
