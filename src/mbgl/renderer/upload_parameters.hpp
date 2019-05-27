#pragma once

#include <unordered_map>
#include <cstdint>

namespace mbgl {

class TransformState;
class LineAtlas;

class UploadParameters {
public:
    UploadParameters(const TransformState& state_,
                     LineAtlas& lineAtlas_)
        : state(state_),
          lineAtlas(lineAtlas_) {
    }

    const TransformState& state;
    LineAtlas& lineAtlas;
};

} // namespace mbgl
