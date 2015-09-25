#include <mbgl/util/texture_pool.hpp>

#include <mbgl/util/gl_object_store.hpp>
#include <mbgl/util/thread_context.hpp>

#include <vector>

const GLsizei TextureMax = 64;

using namespace mbgl;

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
    bool needs_clear = false;

    texture_ids.insert(texture_id);

    if (texture_ids.size() > TextureMax) {
        needs_clear = true;
    }

    if (needs_clear) {
    // TODO: We need to find a better way to deal with texture pool cleanup
//        clearTextureIDs();
    }
}

void TexturePool::clearTextureIDs() {
    auto getGLObjectStore = util::ThreadContext::getGLObjectStore();
    for (auto texture : texture_ids) {
        getGLObjectStore->abandonTexture(texture);
    }
    texture_ids.clear();
}
