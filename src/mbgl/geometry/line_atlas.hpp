#pragma once

#include <mbgl/gfx/texture.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>
#include <unordered_map>
#include <memory>

namespace mbgl {

namespace gfx {
class UploadPass;
} // namespace gfx

class LinePatternPos {
public:
    float width;
    float height;
    float y;
};

enum class LinePatternCap : bool {
    Square = false,
    Round = true,
};

class LineAtlas {
public:
    LineAtlas(Size);
    ~LineAtlas();

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    gfx::TextureBinding textureBinding();

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gfx::UploadPass&);

    LinePatternPos getDashPosition(const std::vector<float>&, LinePatternCap);
    LinePatternPos addDash(const std::vector<float>& dasharray, LinePatternCap);

    void clear();
    Size getSize() const;

private:
    const AlphaImage image;
    bool dirty;
    optional<gfx::Texture> texture;
    uint32_t nextRow = 0;
    std::unordered_map<size_t, LinePatternPos> positions;
};

} // namespace mbgl
