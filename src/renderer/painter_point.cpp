#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/point_bucket.hpp>
#include <llmr/map/map.hpp>
#include <llmr/style/sprite.hpp>

using namespace llmr;

void Painter::renderPoint(PointBucket& bucket, const std::string& layer_name, const Tile::ID& /*id&*/) {
    // Abort early.
    if (!bucket.hasData()) return;
    if (pass == Opaque) return;

    auto point_properties = map.getStyle().computed.points;
    auto point_properties_it = point_properties.find(layer_name);
    if (point_properties_it == point_properties.end()) return;

    const PointProperties& properties = point_properties_it->second;
    if (!properties.enabled) return;

    translateLayer(properties.translate);

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

    if (!imagePos.size) {
        useProgram(dotShader->program);
        dotShader->setMatrix(matrix);
        dotShader->setColor(color);

        const float pointSize = (properties.radius ? properties.radius * 2 : 8) * map.getState().getPixelRatio();
#if defined(GL_ES_VERSION_2_0)
            dotShader->setSize(pointSize);
#else
            glPointSize(pointSize);
            glEnable(GL_POINT_SPRITE);
#endif
        dotShader->setBlur((properties.blur ? properties.blur : 1.5) / pointSize);

        glDepthRange(strata, 1.0f);
        bucket.drawPoints(*dotShader);
    } else {
        useProgram(pointShader->program);
        pointShader->setMatrix(matrix);
        pointShader->setColor(color);

        pointShader->setImage(0);
        pointShader->setPointTopLeft({{ imagePos.tl.x, imagePos.tl.y }});
        pointShader->setPointBottomRight({{ imagePos.br.x, imagePos.br.y }});

        sprite->raster.bind(map.getState().isChanging());

        const float pointSize = (properties.size ? properties.size : (imagePos.size.x / map.getState().getPixelRatio())) * 1.4142135623730951 * map.getState().getPixelRatio();
#if defined(GL_ES_VERSION_2_0)
            pointShader->setSize(pointSize);
#else
            glPointSize(pointSize);
            glEnable(GL_POINT_SPRITE);
#endif

        glDepthRange(strata, 1.0f);
        bucket.drawPoints(*pointShader);
    }

    translateLayer(properties.translate, true);
}
