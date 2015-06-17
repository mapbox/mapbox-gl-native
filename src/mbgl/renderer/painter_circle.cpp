#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/circle_bucket.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_layout.hpp>

#include <mbgl/map/sprite.hpp>
#include <mbgl/map/tile_id.hpp>

using namespace mbgl;

void Painter::renderCircle(CircleBucket& /* bucket */,
                           const StyleLayer& /* layer_desc */,
                           const TileID& /* id */,
                           const mat4& /* matrix */) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;

    // XXX Just a stub for now.
}
