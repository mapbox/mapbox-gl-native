#include <mbgl/util/uv.hpp>

#include <uv.h>

namespace uv {

std::string cwd() {
    size_t max = 0;
    std::string dir;
    do {
        max += 256;
        dir.resize(max);
        uv_cwd(const_cast<char *>(dir.data()), max);
    } while (max == dir.size());
    dir.resize(max - 1);
    return dir;
}

void deleter::operator()(uv_async_t *async) {
    uv_close((uv_handle_t *)async, [](uv_handle_t *handle) {
        delete (uv_async_t *)handle;
    });
}

void deleter::operator()(uv_timer_t *timer) {
    uv_close((uv_handle_t *)timer, [](uv_handle_t *handle) {
        delete (uv_timer_t *)handle;
    });
}

}
