#pragma once

#include <unordered_map>
#include <cstdint>

namespace mbgl {

class TransformState;
class LineAtlas;
class VariableOffset;
using VariableOffsets = std::unordered_map<uint32_t, VariableOffset>;

class UploadParameters {
public:
    UploadParameters(const TransformState& state_,
                     const VariableOffsets& variableOffsets_,
                     ImageManager& imageManager_,
                     LineAtlas& lineAtlas_)
        : state(state_),
          variableOffsets(variableOffsets_),
          imageManager(imageManager_),
          lineAtlas(lineAtlas_) {
    }

    const TransformState& state;
    const VariableOffsets& variableOffsets;
    ImageManager& imageManager;
    LineAtlas& lineAtlas;
};

} // namespace mbgl
