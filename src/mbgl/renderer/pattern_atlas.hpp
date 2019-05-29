#pragma once

#include <mapbox/shelf-pack.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/optional.hpp>

#include <unordered_map>
#include <string>

namespace mbgl {

template <class T>
class Actor;

namespace gfx {
class UploadPass;
} // namespace gfx

class PatternAtlas {
public:
    PatternAtlas();
    PatternAtlas(const PatternAtlas&) = delete;
    PatternAtlas& operator=(const PatternAtlas&) = delete;
    ~PatternAtlas();

    optional<ImagePosition> getPattern(const std::string&) const;
    optional<ImagePosition> addPattern(const style::Image::Impl&);
    void removePattern(const std::string&);

    gfx::TextureBinding textureBinding() const;

    void upload(gfx::UploadPass&);
    Size getPixelSize() const;

    const PremultipliedImage& getAtlasImageForTests() const {
        return atlasImage;
    }

private:
    struct Pattern {
        mapbox::Bin* bin;
        ImagePosition position;
    };
    mapbox::ShelfPack shelfPack;
    std::unordered_map<std::string, Pattern> patterns;
    PremultipliedImage atlasImage;
    mbgl::optional<gfx::Texture> atlasTexture;
    bool dirty = true;
};
 
} // namespace mbgl
