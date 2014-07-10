#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/raster_bucket.hpp>
#include <llmr/style/style_layer.hpp>
#include <llmr/map/map.hpp>

using namespace llmr;

void Painter::renderRaster(RasterBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& /*id*/) {
    if (pass == Translucent) return;

    const RasterProperties &properties = layer_desc->getProperties<RasterProperties>();

    depthMask(false);

    useProgram(rasterShader->program);
    rasterShader->setMatrix(matrix);
    rasterShader->setBuffer(0);
    rasterShader->setOpacity(1);
    // rasterShader->setOpacity(properties.opacity * tile_data->raster->opacity);

    glDepthRange(strata + strata_epsilon, 1.0f);
    bucket.drawRaster(*rasterShader, tileStencilBuffer, coveringRasterArray);

    depthMask(true);
}
