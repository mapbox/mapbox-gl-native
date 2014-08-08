#include <mbgl/renderer/painter.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_layer_group.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/transform.hpp>

using namespace mbgl;

void Painter::renderRaster(RasterBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix) {
    if (pass != Translucent) return;
    
    const RasterProperties &properties = layer_desc->getProperties<RasterProperties>();
    
    if (layer_desc->layers) {
        
        if (!bucket.texture.getTexture()) {

            bucket.texture.bindFramebuffer();
            
            preparePrerender(bucket);

//            const RasterProperties modifiedProperties = [&]{
//                RasterProperties modifiedProperties = properties;
//                modifiedProperties.opacity = 1;   // figure out why this was here
//                return modifiedProperties;
//            }();

            const int buffer = bucket.properties.buffer * 4096.0f;
            
            const mat4 oldMatrix = vtxMatrix;
            
            const mat4 preMatrix = [&]{
                mat4 vtxMatrix;
                matrix::ortho(vtxMatrix, -buffer, 4096 + buffer, -4096 - buffer, buffer, 0, 1);
                matrix::translate(vtxMatrix, vtxMatrix, 0, -4096, 0);
                return vtxMatrix;
            }();
            
//            *id.matrix = vtxMatrix;

            
            
            
//            glUniformMatrix4fv(matrix, 1, GL_FALSE, preMatrix.data());
            
            // call updateTiles to get parsed data for sublayers
            map.updateTiles();

//            map.state.matrixFor(matrix, id);
//            matrix::ortho(matrix, -buffer, 4096 + buffer, -4096 - buffer, buffer, 0, 1);
//            matrix::translate(matrix, matrix, 0, -4096, 0);

            
            int i = 0;
            for (auto it = layer_desc->layers->layers.begin(), end = layer_desc->layers->layers.end(); it != end; ++it, --i) {
                setOpaque();
                map.renderLayer(*it, Map::RenderPass::Opaque, &id, &preMatrix);
                setTranslucent();
                map.renderLayer(*it, Map::RenderPass::Translucent, &id, &preMatrix);
            }
            
//            TODO make a separate renderLayer overload that takes a prerendered + tileID

            if (bucket.properties.blur > 0) {
                bucket.texture.blur(*this, bucket.properties.blur);
            }

            bucket.texture.unbindFramebuffer();
            finishPrerender(bucket);

//            Tile(id).matrix = oldMatrix;
            
        }
        
        renderPrerenderedTexture(bucket, matrix);

    }

    depthMask(false);

    useProgram(rasterShader->program);
    rasterShader->setMatrix(matrix);
    rasterShader->setBuffer(0);
    rasterShader->setOpacity(1);
//    rasterShader->setOpacity(properties.opacity * tile_data->raster->opacity);        // TODO fix

    glDepthRange(strata + strata_epsilon, 1.0f);
    
    if (!bucket.hasData()) {
//        bucket.drawRaster(*rasterShader, tileStencilBuffer, coveringRasterArray, bucket.texture.getTexture());
    } else {
        bucket.drawRaster(*rasterShader, tileStencilBuffer, coveringRasterArray);
    }

    depthMask(true);

}
