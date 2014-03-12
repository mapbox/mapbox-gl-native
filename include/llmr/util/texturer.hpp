#ifndef LLMR_UTIL_TEXTURER
#define LLMR_UTIL_TEXTURER

#include <llmr/util/noncopyable.hpp>
#include <llmr/platform/gl.hpp>

#include <set>
#include <mutex>

namespace llmr {

class Texturer : private util::noncopyable {

public:
    GLuint getTextureID();
    void removeTextureID(GLuint texture_id);
    void clearTextureIDs();

private:
    std::set<GLuint> texture_ids;
//    mutable std::mutex mtx;
};

}

#endif
