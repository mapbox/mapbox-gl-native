#ifndef MBGL_GL_GL_HELPER
#define MBGL_GL_GL_HELPER

namespace mbgl {
namespace gl {

template <typename T>
class Preserve {
public:
    inline Preserve() : data(T::Get()) {
    }
    inline ~Preserve() {
        T::Set(data);
    }

private:
    const typename T::Type data;
};

} // namespace gl
} // namespace mbgl

#endif
