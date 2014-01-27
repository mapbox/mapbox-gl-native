#ifndef LLMR_RENDERER_PAINTER
#define LLMR_RENDERER_PAINTER

#include "shader-fill.hpp"
#include "shader-line.hpp"
#include "shader-outline.hpp"

#include "../map/tile.hpp"

namespace llmr {

class Settings;
class Transform;
class Style;
class LayerDescription;

class FillBucket;
struct FillProperties;

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

    void renderBackground();
    void renderFill(FillBucket& bucket, const std::string& layer_name);


    void drawClippingMask();
    bool switchShader(Shader *shader);

private:
    void setupShaders();
    void changeMatrix(const std::shared_ptr<Tile>& tile);

public:

private:
    Transform& transform;
    Settings& settings;
    Style& style;
    float matrix[16];

    Shader *currentShader;
    FillShader *fillShader;
    LineShader *lineShader;
    OutlineShader *outlineShader;

    uint32_t tile_stencil_buffer;
    uint32_t tile_border_buffer;
};

}

#endif
