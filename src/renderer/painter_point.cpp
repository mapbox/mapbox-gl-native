#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/point_bucket.hpp>
#include <llmr/map/map.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/util/math.hpp>

using namespace llmr;

void Painter::renderPoint(PointBucket& bucket, const std::string& layer_name, const Tile::ID& id) {
    // Abort early.
    if (!bucket.hasData()) return;
    if (pass == Opaque) return;

    auto point_properties = map.getStyle().computed.points;
    auto point_properties_it = point_properties.find(layer_name);
    if (point_properties_it == point_properties.end()) return;

    const PointProperties& properties = point_properties_it->second;
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

        const float pointSize = (properties.radius ? properties.radius * 2 : 8) * map.getState().getPixelRatio();
        dotShader->setSize(pointSize);
#ifndef GL_ES_VERSION_2_0
            glPointSize(pointSize);
            glEnable(GL_POINT_SPRITE);
#endif
        dotShader->setBlur((properties.blur ? properties.blur : 1.5) / pointSize);

        glDepthRange(strata, 1.0f);
        bucket.drawPoints(*dotShader);
    } else {
        useProgram(pointShader->program);
        pointShader->setMatrix(vtxMatrix);
        pointShader->setColor(color);
        pointShader->setImage(0);
        pointShader->setPosition({{
            0.5f * (imagePos.tl.x + imagePos.br.x),
            0.5f * (imagePos.tl.y + imagePos.br.y),
        }});

        pointShader->setDimension({{
            static_cast<float>(sprite->raster.width),
            static_cast<float>(sprite->raster.height)
        }});

        sprite->raster.bind(map.getState().isChanging());

        const float pointSize = util::max(imagePos.size.x, imagePos.size.y) + 2;
        pointShader->setSize(pointSize);
#ifndef GL_ES_VERSION_2_0
        glPointSize(pointSize);
        glEnable(GL_POINT_SPRITE);
#endif

        glDepthRange(strata, 1.0f);
        bucket.drawPoints(*pointShader);
    }
}
