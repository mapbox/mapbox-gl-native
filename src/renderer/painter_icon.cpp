#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/icon_bucket.hpp>
#include <llmr/map/map.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/util/math.hpp>

using namespace llmr;

void Painter::renderIcon(IconBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    // Abort early.
    if (!bucket.hasData()) return;
    if (pass == Opaque) return;

    const std::unordered_map<std::string, IconProperties> &icon_properties = map.getStyle().computed.icons;
    const std::unordered_map<std::string, IconProperties>::const_iterator icon_properties_it = icon_properties.find(layer_name);
    if (icon_properties_it == icon_properties.end()) return;

    const IconProperties& properties = icon_properties_it->second;
    if (!properties.enabled) return;

    Color color = properties.color;
    color[0] *= properties.opacity;
    color[1] *= properties.opacity;
    color[2] *= properties.opacity;
    color[3] *= properties.opacity;

    const mat4 &vtxMatrix = translatedMatrix(properties.translate, id, properties.translateAnchor);

    SpriteAtlas &spriteAtlas = map.getSpriteAtlas();

    useProgram(iconShader->program);
    iconShader->setMatrix(vtxMatrix);
    iconShader->setColor(color);
    iconShader->setImage(0);
    iconShader->setRatio(map.getState().getPixelRatio());
    iconShader->setDimension({{
        spriteAtlas.getTextureWidth(),
        spriteAtlas.getTextureHeight(),
    }});

    spriteAtlas.bind(map.getState().isChanging());

    const float iconSize = bucket.geometry.size * map.getState().getPixelRatio();
    iconShader->setSize(iconSize);
#ifndef GL_ES_VERSION_2_0
    glPointSize(iconSize);
    glEnable(GL_POINT_SPRITE);
#endif

    glDepthRange(strata, 1.0f);
    bucket.drawIcons(*iconShader);
}
