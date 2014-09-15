#include <mbgl/util/uv-worker.h>
#include <mbgl/util/uv-messenger.h>

#include <stdio.h>

typedef struct uv__worker_item_s uv__worker_item_t;
struct uv__worker_item_s {
    void *data;
    uv_worker_cb work_cb;
    uv_worker_after_cb after_work_cb;
};

void uv__worker_after(void *ptr) {
    uv__worker_item_t *item = (uv__worker_item_t *)ptr;
    item->after_work_cb(item->data);
    free(item);
}

void uv__worker_thread_loop(void *ptr) {
    uv_worker_t *worker = (uv_worker_t *)ptr;

#ifdef __APPLE__
    if (worker->name) {
        pthread_setname_np(worker->name);
    }
#endif

    uv__worker_item_t *item = NULL;
    while ((item = uv_chan_receive(&worker->chan)) != NULL) {
        item->work_cb(item->data);
        uv_messenger_send(worker->msgr, item);
    }
}

int uv_worker_init(uv_worker_t *worker, uv_loop_t *loop) {
    return uv_worker_init_named(worker, loop, NULL);
}

int uv_worker_init_named(uv_worker_t *worker, uv_loop_t *loop, const char *name) {
    worker->name = name;
    worker->msgr = malloc(sizeof(uv_messenger_t));
    int ret = uv_messenger_init(loop, worker->msgr, uv__worker_after);
    if (ret < 0) return ret;
    ret = uv_chan_init(&worker->chan);
    if (ret < 0) return ret;
    return uv_thread_create(&worker->thread, uv__worker_thread_loop, worker);
}

void uv_worker_send(uv_worker_t *worker, void *data, uv_worker_cb work_cb,
                    uv_worker_after_cb after_work_cb) {
    uv__worker_item_t *item = malloc(sizeof(uv__worker_item_t));
    item->work_cb = work_cb;
    item->after_work_cb = after_work_cb;
    item->data = data;
    uv_chan_send(&worker->chan, item);
}

void uv_worker_close(uv_worker_t *worker) {
    uv_chan_send(&worker->chan, NULL);
    uv_thread_join(&worker->thread);
    uv_messenger_stop(worker->msgr);
}
