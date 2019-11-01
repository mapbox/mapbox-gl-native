#pragma once

#include <mbgl/gfx/texture.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>

#include <map>
#include <memory>
#include <vector>

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

class DashPatternTexture {
public:
    DashPatternTexture(const std::vector<float>& from, const std::vector<float>& to, LinePatternCap);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is uploaded for the first time.
    void upload(gfx::UploadPass&);

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    gfx::TextureBinding textureBinding() const;

    // Returns the size of the texture image.
    Size getSize() const;

    const LinePatternPos& getFrom() const { return from; }
    const LinePatternPos& getTo() const { return to; }

private:
    LinePatternPos from, to;
    variant<AlphaImage, gfx::Texture> texture;
};

class LineAtlas {
public:
    LineAtlas();
    ~LineAtlas();

    // Obtains or creates a texture that has both line patterns in it
    DashPatternTexture& getDashPatternTexture(const std::vector<float>& from,
                                              const std::vector<float>& to,
                                              LinePatternCap);

    // Uploads the textures to the GPU to be available when we need it.
    void upload(gfx::UploadPass&);

private:
    std::map<size_t, DashPatternTexture> textures;

    // Stores a list of hashes of texture objcts that need uploading.
    std::vector<size_t> needsUpload;
};

} // namespace mbgl
