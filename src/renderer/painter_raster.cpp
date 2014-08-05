#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/map/map.hpp>

using namespace mbgl;

void Painter::renderRaster(RasterBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id) {
    if (pass == Translucent) return;
    
    const RasterProperties &properties = layer_desc->getProperties<RasterProperties>();
    
    if (layer_desc->rasterize) {
        auto rasterize = std::make_unique<RasterizeProperties>();
        
        if (pass == Translucent) {
            // Buffer value around the 0..4096 extent that will be drawn into the 256x256 pixel
            // texture. We later scale the texture so that the actual bounds will align with this
            // tile's bounds. The reason we do this is so that the
            const RasterizedProperties rasterize = layer_desc->rasterize->get(id.z);
            if (!bucket.prerendered) {
                
                bucket.prerendered = std::make_unique<PrerenderedTexture>(rasterize);
                bucket.prerendered->bindFramebuffer();
                
                preparePrerender(*bucket.prerendered);
                
                const RasterProperties modifiedProperties = [&]{
                    RasterProperties modifiedProperties = properties;
                    //                modifiedProperties.opacity = 1;   // figure out why this was here
                    return modifiedProperties;
                }();
                
                // When drawing the fill, we want to draw a buffer around too, so we
                // essentially downscale everyting, and then upscale it later when rendering.
                const int buffer = rasterize.buffer * 4096.0f;
                const mat4 vtxMatrix = [&]{
                    mat4 vtxMatrix;
                    matrix::ortho(vtxMatrix, -buffer, 4096 + buffer, -4096 - buffer, buffer, 0, 1);
                    matrix::translate(vtxMatrix, vtxMatrix, 0, -4096, 0);
                    return vtxMatrix;
                }();
                
                setOpaque();
                //                renderFill(bucket, modifiedProperties, id, vtxMatrix);
                
                
                
                //                resize();
                
                //                changeMatrix();
                
                //                map.updateRenderState();
                
                //                drawClippingMasks(getActiveSources());
                
                // Actually render the layers
                map.renderLayers(layer_desc->layers);
                
                // Finalize the rendering, e.g. by calling debug render calls per tile.
                // This guarantees that we have at least one function per tile called.
                // When only rendering layers via the stylesheet, it's possible that we don't
                // ever visit a tile during rendering.
                //                for (const std::shared_ptr<StyleSource> &source : getActiveSources()) {
                //                    source->source->finishRender(painter);
                //                }
                //
                //                // Schedule another rerender when we definitely need a next frame.
                //                if (transform.needsTransition() || style->hasTransitions()) {
                //                    update();
                //                }
                
                glFlush();

                if (rasterize.blur > 0) {
                    bucket.prerendered->blur(*this, rasterize.blur);
                }
                
                // RESET STATE
                bucket.prerendered->unbindFramebuffer();
                finishPrerender(*bucket.prerendered);
            }
            
            renderPrerenderedTexture(*bucket.prerendered, properties);

        }

    }

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
void Painter::renderRaster(GLuint texture, const RasterizeProperties &properties){
//    TODO do I need this?
//    map.renderLayers(layer_desc->layers);
}
