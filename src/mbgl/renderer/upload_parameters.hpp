#pragma once

#include <unordered_map>
#include <cstdint>

namespace mbgl {

class TransformState;
class LineAtlas;

class UploadParameters {
public:
    UploadParameters(const TransformState& state_,
                     ImageManager& imageManager_,
                     LineAtlas& lineAtlas_)
        : state(state_),
          imageManager(imageManager_),
          lineAtlas(lineAtlas_) {
    }

    const TransformState& state;
    ImageManager& imageManager;
    LineAtlas& lineAtlas;
};

} // namespace mbgl
