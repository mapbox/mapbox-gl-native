#pragma once

#include <mbgl/style/bucket_parameters.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <memory>
#include <string>

namespace mbgl {
namespace style {

class RasterBucketParameters {
public:
    RasterBucketParameters(const OverscaledTileID& tileID_,
                           std::shared_ptr<const std::string> data_)
        : tileID(tileID_), data(data_) {
    }

    const OverscaledTileID& tileID;
    std::shared_ptr<const std::string> data;
};

} // namespace style
} // namespace mbgl
