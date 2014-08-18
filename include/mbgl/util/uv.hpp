#ifndef MBGL_UTIL_UV
#define MBGL_UTIL_UV

#include <string>

typedef struct uv_async_s uv_async_t;
typedef struct uv_handle_s uv_handle_t;
typedef struct uv_loop_s uv_loop_t;

namespace uv {

std::string cwd();

class thread;
class rwlock;
class loop;

}

#endif
