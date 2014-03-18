#ifndef LLMR_UTIL_TEXTUREPOOL
#define LLMR_UTIL_TEXTUREPOOL

#include <llmr/util/noncopyable.hpp>
#include <llmr/platform/gl.hpp>

#include <set>
#include <mutex>

namespace llmr {

class Texturepool : private util::noncopyable {

public:
    GLuint getTextureID();
    void removeTextureID(GLuint texture_id);
    void clearTextureIDs();

private:
    std::set<GLuint> texture_ids;
};

}

#endif
