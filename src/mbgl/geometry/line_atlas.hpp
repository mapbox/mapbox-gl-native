#pragma once

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/object_store.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>
#include <unordered_map>

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
    LineAtlas(GLsizei width, GLsizei height);
    ~LineAtlas();

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(gl::ObjectStore&, gl::Context&, uint32_t unit);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::ObjectStore&, gl::Context&, uint32_t unit);

    LinePatternPos getDashPosition(const std::vector<float>&, LinePatternCap);
    LinePatternPos addDash(const std::vector<float>& dasharray, LinePatternCap);

    const GLsizei width;
    const GLsizei height;

private:
    const std::unique_ptr<GLbyte[]> data;
    bool dirty;
    mbgl::optional<gl::UniqueTexture> texture;
    int nextRow = 0;
    std::unordered_map<size_t, LinePatternPos> positions;
};

} // namespace mbgl
