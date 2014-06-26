#include <llmr/style/bucket_description.hpp>

#include <iostream>

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