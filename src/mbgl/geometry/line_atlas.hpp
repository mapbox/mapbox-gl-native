#ifndef MBGL_GEOMETRY_LINE_ATLAS
#define MBGL_GEOMETRY_LINE_ATLAS

#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/gl_object_store.hpp>

#include <vector>
#include <map>

namespace mbgl {

typedef struct {
    float width;
    float height;
    float y;
} LinePatternPos;

class LineAtlas {
public:
    LineAtlas(GLsizei width, GLsizei height);
    ~LineAtlas();

    // Binds the atlas texture to the GPU, and uploads data if it is out of date.
    void bind(gl::GLObjectStore&);

    // Uploads the texture to the GPU to be available when we need it. This is a lazy operation;
    // the texture is only bound when the data is out of date (=dirty).
    void upload(gl::GLObjectStore&);

    LinePatternPos getDashPosition(const std::vector<float>&, bool, gl::GLObjectStore&);
    LinePatternPos addDash(const std::vector<float> &dasharray, bool round, gl::GLObjectStore&);

    const GLsizei width;
    const GLsizei height;

private:
    const std::unique_ptr<GLbyte[]> data;
    bool dirty;
    gl::TextureHolder texture;
    int nextRow = 0;
    std::map<size_t, LinePatternPos> positions;
};

} // namespace mbgl

#endif
