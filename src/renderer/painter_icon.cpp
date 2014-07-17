#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/icon_bucket.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/sprite.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/util/math.hpp>

using namespace mbgl;

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
    iconShader->setColor({{ 1, 1, 1, 1 }});
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
