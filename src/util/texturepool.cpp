#include <mbgl/util/texturepool.hpp>

#include <vector>

const int TextureMax = 64;

using namespace mbgl;

GLuint Texturepool::getTextureID() {
    if (texture_ids.empty()) {
        GLuint new_texture_ids[TextureMax];
        glGenTextures(TextureMax, new_texture_ids);
        for (uint32_t id = 0; id < TextureMax; id++) {
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

void Texturepool::removeTextureID(GLuint texture_id) {
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

void Texturepool::clearTextureIDs() {
    std::vector<GLuint> ids_to_remove;
    ids_to_remove.reserve(texture_ids.size());

    for (std::set<GLuint>::iterator id_iterator = texture_ids.begin();
         id_iterator != texture_ids.end(); ++id_iterator) {
        ids_to_remove.push_back(*id_iterator);
    }

    if (!ids_to_remove.empty()) {
        glDeleteTextures((GLsizei)ids_to_remove.size(), &ids_to_remove[0]);
    }

    texture_ids.clear();
}
