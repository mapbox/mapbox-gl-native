#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>
#include <unordered_map>
#include <memory>

namespace mbgl {

namespace gl {
class Context;
} // namespace gl

typedef struct {
    float width;
    float height;
    float y;
} LinePatternPos;

enum class LinePatternCap : bool {
    Square = false,
    Round = true,
};

class LineAtlas {
public:
    LineAtlas(uint16_t width, uint16_t height);
    ~LineAtlas();

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(gl::Context&, gl::TextureUnit unit);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::Context&, gl::TextureUnit unit);

    LinePatternPos getDashPosition(const std::vector<float>&, LinePatternCap);
    LinePatternPos addDash(const std::vector<float>& dasharray, LinePatternCap);

    const uint16_t width;
    const uint16_t height;

private:
    const std::unique_ptr<char[]> data;
    bool dirty;
    mbgl::optional<gl::UniqueTexture> texture;
    int nextRow = 0;
    std::unordered_map<size_t, LinePatternPos> positions;
};

} // namespace mbgl
