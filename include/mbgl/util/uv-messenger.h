#ifndef MBGL_UTIL_UV_MESSENGER
#define MBGL_UTIL_UV_MESSENGER

#ifdef __cplusplus
extern "C" {
#endif

#include <uv.h>

typedef struct uv_messenger_s uv_messenger_t;
typedef void (*uv_messenger_cb)(void *arg);
typedef void (*uv_messenger_stop_cb)(uv_messenger_t *msgr);

struct uv_messenger_s {
    uv_mutex_t mutex;
    uv_async_t async;
    uv_messenger_cb callback;
    void *data;
    void *queue[2];
};

int uv_messenger_init(uv_loop_t *loop, uv_messenger_t *msgr, uv_messenger_cb callback);
void uv_messenger_send(uv_messenger_t *msgr, void *arg);
void uv_messenger_stop(uv_messenger_t *msgr);
void uv_messenger_ref(uv_messenger_t *msgr);
void uv_messenger_unref(uv_messenger_t *msgr);

#ifdef __cplusplus
}
#endif

#endif
