#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/map/map.hpp>

using namespace mbgl;

void Painter::renderRaster(RasterBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id) {
    if (pass == Translucent) return;
    
    const RasterProperties &properties = layer_desc->getProperties<RasterProperties>();
    
    if (layer_desc->layers) {

        bucket.texture.bindFramebuffer();
        
        preparePrerender(bucket);

        const RasterProperties modifiedProperties = [&]{
            RasterProperties modifiedProperties = properties;
            modifiedProperties.opacity = 1;   // figure out why this was here
            return modifiedProperties;
        }();

        const int buffer = bucket.properties.buffer * 4096.0f;
        
        const mat4 vtxMatrix = [&]{
            mat4 vtxMatrix;
            matrix::ortho(vtxMatrix, -buffer, 4096 + buffer, -4096 - buffer, buffer, 0, 1);
            matrix::translate(vtxMatrix, vtxMatrix, 0, -4096, 0);
            return vtxMatrix;
        }();
        
        map.renderLayers(layer_desc->layers);

        if (bucket.properties.blur > 0) {
            bucket.texture.blur(*this, bucket.properties.blur);
        }

        bucket.texture.unbindFramebuffer();
        finishPrerender(bucket);
        renderPrerenderedTexture(bucket);

    }

    depthMask(false);

    useProgram(rasterShader->program);
    rasterShader->setMatrix(matrix);
    rasterShader->setBuffer(0);
    rasterShader->setOpacity(1);
//    rasterShader->setOpacity(properties.opacity * tile_data->raster->opacity);        // TODO fix

    glDepthRange(strata + strata_epsilon, 1.0f);
    
    if (!bucket.hasData()) {
        bucket.drawRaster(*rasterShader, tileStencilBuffer, coveringRasterArray, bucket.texture.getTexture());
    } else {
        bucket.drawRaster(*rasterShader, tileStencilBuffer, coveringRasterArray);
    }

    depthMask(true);

}
