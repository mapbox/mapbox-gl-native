#include <llmr/util/texturer.hpp>

#include <vector>

#define TextureMax 32

using namespace llmr;

GLuint Texturer::getTextureID() {
//    std::lock_guard<std::mutex> lock(mtx);

    if (texture_ids.empty())
    {
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

void Texturer::removeTextureID(GLuint texture_id) {
    //    std::lock_guard<std::mutex> lock(mtx);

    bool needs_clear = false;

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    texture_ids.insert(texture_id);

    if (texture_ids.size() > TextureMax)
        needs_clear = true;

    if (needs_clear)
        clearTextureIDs();
}

void Texturer::clearTextureIDs() {
//    std::lock_guard<std::mutex> lock(mtx);

    std::vector<GLuint> ids_to_remove;
    ids_to_remove.reserve(texture_ids.size());

    for (std::set<GLuint>::iterator id_iterator = texture_ids.begin(); id_iterator != texture_ids.end(); ++id_iterator)
        ids_to_remove.push_back(*id_iterator);

    if (!ids_to_remove.empty())
        glDeleteTextures((GLsizei)ids_to_remove.size(), &ids_to_remove[0]);

    texture_ids.clear();
}
