#include <llmr/style/bucket_description.hpp>

#include <iostream>

std::ostream& llmr::operator<<(std::ostream& os, const BucketDescription& bucket) {
    os << "Bucket:" << std::endl;
    os << "  - type: " << (uint32_t)bucket.type << std::endl;
    os << "  - source_name: " << bucket.source_name << std::endl;
    os << "  - source_layer: " << bucket.source_layer << std::endl;
    os << "  - source_field: " << bucket.source_field << std::endl;
    for (const Value& value : bucket.source_value) {
        os << "  - source_value: " << value << std::endl;
    }
    os << "  - cap: " << (uint32_t)bucket.cap << std::endl;
    os << "  - join: " << (uint32_t)bucket.join << std::endl;
    os << "  - font: " << bucket.font << std::endl;
    os << "  - font_size: " << bucket.font_size << std::endl;
    return os;
}
