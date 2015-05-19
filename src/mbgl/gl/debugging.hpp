#ifndef MBGL_GL_DEBUG_MESSAGES
#define MBGL_GL_DEBUG_MESSAGES

#include <string>

namespace mbgl {
namespace gl {
namespace debugging {

void enable();

struct group {
    group(const std::string&);
    ~group();
};

}
}
}

#endif
