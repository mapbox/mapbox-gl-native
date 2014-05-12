#include <llmr/style/layer_description.hpp>

#include <iostream>

using namespace llmr;

size_t LayerDescription::size() const {
    size_t count = 1;
    for (const LayerDescription &layer : child_layer) {
        count += layer.size();
    }
    return count;
}

std::ostream& llmr::operator<<(std::ostream& os, const LayerDescription& layer) {
    os << "Structure: " << layer.name << std::endl;
    os << "  - bucket_name: " << layer.bucket_name << std::endl;
    for (const LayerDescription& value : layer.child_layer) {
        os << "  - child_layer: " << value << std::endl;
    }
    return os;
}
