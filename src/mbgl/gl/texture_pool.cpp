#include <mbgl/gl/texture_pool.hpp>

#include <mbgl/gl/gl_object_store.hpp>
#include <mbgl/util/thread_context.hpp>

#include <vector>

const GLsizei TextureMax = 64;

namespace mbgl {
namespace gl {

GLuint TexturePool::getTextureID() {
    if (texture_ids.empty()) {
        GLuint new_texture_ids[TextureMax];
        MBGL_CHECK_ERROR(glGenTextures(TextureMax, new_texture_ids));
        for (GLuint id = 0; id < TextureMax; id++) {
            texture_ids.insert(new_texture_ids[id]);
        }
    }

    GLuint id = 0;

    if (!texture_ids.empty()) {
        std::set<GLuint>::iterator id_iterator = texture_ids.begin();
        id = *id_iterator;
        texture_ids.erase(id_iterator);
    }

    return id;
}

void TexturePool::removeTextureID(GLuint texture_id) {
    texture_ids.insert(texture_id);
    // TODO: We need to find a better way to deal with texture pool cleanup
}

} // namespace gl
} // namespace mbgl
