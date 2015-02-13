#ifndef MBGL_UTIL_UV_WORKER
#define MBGL_UTIL_UV_WORKER

#include <stdlib.h>

#include <mbgl/util/uv-messenger.h>
#include <mbgl/util/uv-channel.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uv_worker_s uv_worker_t;

typedef void (*uv_worker_cb)(void *data);
typedef void (*uv_worker_after_cb)(void *data);
typedef void (*uv_worker_close_cb)(uv_worker_t *worker);

struct uv_worker_s {
#ifdef DEBUG
    unsigned long thread_id;
#endif
    uv_loop_t *loop;
    uv_messenger_t *msgr;
    uv_chan_t chan;
    const char *name;
    int count;
    uv_worker_close_cb close_cb;
    unsigned int active_items;
};

int uv_worker_init(uv_worker_t *worker, uv_loop_t *loop, int count, const char *name);
void uv_worker_send(uv_worker_t *worker, void *data, uv_worker_cb work_cb,
                    uv_worker_after_cb after_work_cb);
void uv_worker_close(uv_worker_t *worker, uv_worker_close_cb close_cb);

#ifdef __cplusplus
}
#endif

#endif
