#ifndef MBGL_UTIL_UV
#define MBGL_UTIL_UV

#include <string>

typedef struct uv_handle_s uv_handle_t;
typedef struct uv_async_s uv_async_t;
typedef struct uv_timer_s uv_timer_t;
typedef struct uv_loop_s uv_loop_t;
typedef struct uv_fs_s uv_fs_t;

namespace uv {

std::string cwd();

class rwlock;
class loop;
class async;
class worker;
class mutex;
class cond;

const char *getFileRequestError(uv_fs_t *req);

template <typename T>
void close(T *specific) {
    uv_close(reinterpret_cast<uv_handle_t *>(specific), [](uv_handle_t *generic) {
        delete reinterpret_cast<T *>(generic);
    });
}

}

#endif
