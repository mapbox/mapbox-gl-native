#ifndef LLMR_RENDERER_PAINTER
#define LLMR_RENDERER_PAINTER

#include "shader-fill.hpp"
#include "shader-plain.hpp"
#include "shader-outline.hpp"
#include "shader-line.hpp"

#include "../map/tile.hpp"
#include "../util/mat4.hpp"

namespace llmr {

class Settings;
class Transform;
class Style;
class LayerDescription;

class FillBucket;
class LineBucket;

class Painter {
public:
    Painter(Transform& transform, Settings& settings, Style& style);

    // Make noncopyable
    Painter(const Painter&) = delete;
    Painter(const Painter&&) = delete;
    Painter &operator=(const Painter&) = delete;
    Painter &operator=(const Painter&&) = delete;


    void setup();
    void teardown();

    void clear();
    void render(const std::shared_ptr<Tile>& tile);
    void renderLayers(const std::shared_ptr<Tile>& tile, const std::vector<LayerDescription>& layers);
    void renderDebug(const std::shared_ptr<Tile>& tile);

    void renderMatte();
    void renderBackground();
    void renderFill(FillBucket& bucket, const std::string& layer_name);
    void renderLine(LineBucket& bucket, const std::string& layer_name);


    void drawClippingMask();
    bool switchShader(std::shared_ptr<Shader> shader);

private:
    void setupShaders();
    void changeMatrix(const std::shared_ptr<Tile>& tile);

public:

private:
    Transform& transform;
    Settings& settings;
    Style& style;

    mat4 nativeMatrix;
    mat4 matrix;
    mat4 exMatrix;

    std::shared_ptr<Shader> currentShader;
    std::shared_ptr<FillShader> fillShader;
    std::shared_ptr<PlainShader> plainShader;
    std::shared_ptr<OutlineShader> outlineShader;
    std::shared_ptr<LineShader> lineShader;

    uint32_t tile_stencil_buffer;
    uint32_t tile_border_buffer;
    uint32_t matte_buffer;
};

}

#endif
