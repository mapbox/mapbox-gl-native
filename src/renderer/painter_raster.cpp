#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/raster_bucket.hpp>
#include <llmr/map/map.hpp>

using namespace llmr;

void Painter::renderRaster(RasterBucket& bucket, const std::string& layer_name, const Tile::ID& /*id*/) {
    if (pass == Translucent) return;

    const std::unordered_map<std::string, RasterProperties> &raster_properties = map.getStyle().computed.rasters;
    const std::unordered_map<std::string, RasterProperties>::const_iterator raster_properties_it = raster_properties.find(layer_name);

    const RasterProperties &properties = raster_properties_it != raster_properties.end()
                                             ? raster_properties_it->second
                                             : defaultRasterProperties;
    if (!properties.enabled) return;

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
