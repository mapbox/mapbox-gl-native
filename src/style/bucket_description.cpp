#include <llmr/style/bucket_description.hpp>

#include <iostream>


std::ostream& llmr::operator<<(std::ostream& os, const BucketDescription& bucket) {
    os << "Bucket:" << std::endl;
    os << "  - type: " << (uint32_t)bucket.type << std::endl;
    os << "  - source_name: " << bucket.source_name << std::endl;
    os << "  - source_layer: " << bucket.source_layer << std::endl;
    os << "  - cap: " << (uint32_t)bucket.geometry.cap << std::endl;
    os << "  - join: " << (uint32_t)bucket.geometry.join << std::endl;
    os << "  - font: " << bucket.geometry.font << std::endl;
    os << "  - size: " << bucket.geometry.size << std::endl;
    return os;
}

std::ostream& llmr::operator<<(std::ostream& os, BucketType type) {
    switch (type) {
        case BucketType::None: os << "None"; return os;
        case BucketType::Fill: os << "Fill"; return os;
        case BucketType::Line: os << "Line"; return os;
        case BucketType::Icon: os << "Icon"; return os;
        case BucketType::Text: os << "Text"; return os;
        case BucketType::Raster: os << "Raster"; return os;
        default: os << "<unknown>"; return os;
    }
}