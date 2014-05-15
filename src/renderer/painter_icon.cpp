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

    auto icon_properties = map.getStyle().computed.icons;
    auto icon_properties_it = icon_properties.find(layer_name);
    if (icon_properties_it == icon_properties.end()) return;

    const IconProperties& properties = icon_properties_it->second;
    if (!properties.enabled) return;

    Color color = properties.color;
    color[0] *= properties.opacity;
    color[1] *= properties.opacity;
    color[2] *= properties.opacity;
    color[3] *= properties.opacity;

    auto &sprite = map.getStyle().sprite;
    ImagePosition imagePos;

    if (properties.image.length() && sprite && sprite->isLoaded()) {
        std::string sized_image = properties.image;
        if (properties.size) {
            sized_image.append("-");
            sized_image.append(std::to_string(static_cast<int>(std::round(properties.size))));
        }
        imagePos = sprite->getPosition(sized_image, false);
    }

    const mat4 &vtxMatrix = translatedMatrix(properties.translate, id, properties.translateAnchor);

    if (!imagePos.size) {
        useProgram(dotShader->program);
        dotShader->setMatrix(vtxMatrix);
        dotShader->setColor(color);

        const float iconSize = (properties.radius ? properties.radius * 2 : 8) * map.getState().getPixelRatio();
        dotShader->setSize(iconSize);
#ifndef GL_ES_VERSION_2_0
            glPointSize(iconSize);
            glEnable(GL_POINT_SPRITE);
#endif
        dotShader->setBlur((properties.blur ? properties.blur : 1.5) / iconSize);

        glDepthRange(strata, 1.0f);
        bucket.drawIcons(*dotShader);
    } else {
        useProgram(iconShader->program);
        iconShader->setMatrix(vtxMatrix);
        iconShader->setColor(color);
        iconShader->setImage(0);
        iconShader->setPosition({{
            0.5f * (imagePos.tl.x + imagePos.br.x),
            0.5f * (imagePos.tl.y + imagePos.br.y),
        }});

        iconShader->setDimension({{
            static_cast<float>(sprite->raster.width),
            static_cast<float>(sprite->raster.height)
        }});

        sprite->raster.bind(map.getState().isChanging());

        const float iconSize = util::max(imagePos.size.x, imagePos.size.y) + 2;
        iconShader->setSize(iconSize);
#ifndef GL_ES_VERSION_2_0
        glPointSize(iconSize);
        glEnable(GL_POINT_SPRITE);
#endif

        glDepthRange(strata, 1.0f);
        bucket.drawIcons(*iconShader);
    }
}
