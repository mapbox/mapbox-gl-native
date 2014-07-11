#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/icon_bucket.hpp>
#include <llmr/map/map.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/style/style_layer.hpp>
#include <llmr/geometry/sprite_atlas.hpp>
#include <llmr/util/math.hpp>

using namespace llmr;

void Painter::renderIcon(IconBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id) {
    // Abort early.
    if (!bucket.hasData()) return;
    if (pass == Opaque) return;

    const IconProperties &properties = layer_desc->getProperties<IconProperties>();

// TODO: when translating icon, are we doing this in the bucket already?
//    const mat4 &vtxMatrix = translatedMatrix(properties.translate, id, properties.translateAnchor);

    SpriteAtlas &spriteAtlas = *map.getSpriteAtlas();

    useProgram(iconShader->program);
    iconShader->setMatrix(matrix);
// TODO: update
//    iconShader->setColor(color);
    iconShader->setImage(0);
    iconShader->setRatio(map.getState().getPixelRatio());
    iconShader->setDimension({{
        spriteAtlas.getTextureWidth(),
        spriteAtlas.getTextureHeight(),
    }});

    spriteAtlas.bind(map.getState().isChanging());

    const float iconSize = bucket.properties.size * map.getState().getPixelRatio();
    iconShader->setSize(iconSize);
#ifndef GL_ES_VERSION_2_0
    glPointSize(iconSize);
    glEnable(GL_POINT_SPRITE);
#endif

    glDepthRange(strata, 1.0f);
    bucket.drawIcons(*iconShader);
}
