#ifndef MBGL_UTIL_UV_CHANNEL
#define MBGL_UTIL_UV_CHANNEL

#include <uv.h>

#ifdef __cplusplus
extern "C" {
#endif

// Taken from http://navaneeth.github.io/blog/2013/08/02/channels-in-libuv/

typedef struct uv_chan_s uv_chan_t;

struct uv_chan_s {
    uv_mutex_t mutex;
    uv_cond_t cond;
    void *q[2];
};

int uv_chan_init(uv_chan_t *chan);
void uv_chan_send(uv_chan_t *chan, void *data);
void *uv_chan_receive(uv_chan_t *chan);
void uv_chan_destroy(uv_chan_t *chan);

#ifdef __cplusplus
}
#endif

#endif
