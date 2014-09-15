#include <mbgl/util/uv-channel.h>
#include <mbgl/util/queue.h>

#include <stdlib.h>

// Taken from http://navaneeth.github.io/blog/2013/08/02/channels-in-libuv/

typedef struct {
    void *data;
    void *active_queue[2];
} uv__chan_item_t;

int uv_chan_init(uv_chan_t *chan) {
    int r = uv_mutex_init(&chan->mutex);
    if (r == -1)
        return r;

    QUEUE_INIT(&chan->q);

    return uv_cond_init(&chan->cond);
}

void uv_chan_send(uv_chan_t *chan, void *data) {
    uv__chan_item_t *item = malloc(sizeof(uv__chan_item_t));
    item->data = data;

    uv_mutex_lock(&chan->mutex);
    QUEUE_INSERT_TAIL(&chan->q, &item->active_queue);
    uv_cond_signal(&chan->cond);
    uv_mutex_unlock(&chan->mutex);
}

void *uv_chan_receive(uv_chan_t *chan) {
    uv__chan_item_t *item;
    QUEUE *head;
    void *data = NULL;

    uv_mutex_lock(&chan->mutex);
    while (QUEUE_EMPTY(&chan->q)) {
        uv_cond_wait(&chan->cond, &chan->mutex);
    }

    head = QUEUE_HEAD(&chan->q);
    item = QUEUE_DATA(head, uv__chan_item_t, active_queue);
    data = item->data;
    QUEUE_REMOVE(head);
    free(item);
    uv_mutex_unlock(&chan->mutex);
    return data;
}

void uv_chan_destroy(uv_chan_t *chan) {
    uv_cond_destroy(&chan->cond);
    uv_mutex_destroy(&chan->mutex);
}
