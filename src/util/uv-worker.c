#include <mbgl/util/uv-worker.h>
#include <mbgl/util/uv-messenger.h>
#include <mbgl/util/queue.h>

#include <stdio.h>

typedef struct uv__worker_item_s uv__worker_item_t;
struct uv__worker_item_s {
    void *data;
    uv_worker_cb work_cb;
    uv_worker_after_cb after_work_cb;
};

typedef struct uv__work_thread_s uv__work_thread_t;
struct uv__work_thread_s {
    uv_thread_t thread;
    void *queue[2];
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

    // Make sure to close all other workers too.
    uv_chan_send(&worker->chan, NULL);
}

int uv_worker_init(uv_worker_t *worker, uv_loop_t *loop, int count, const char *name) {
    worker->name = name;
    worker->msgr = malloc(sizeof(uv_messenger_t));
    int ret = uv_messenger_init(loop, worker->msgr, uv__worker_after);
    if (ret < 0) return ret;
    ret = uv_chan_init(&worker->chan);
    if (ret < 0) return ret;

    // Initialize worker threads.
    QUEUE_INIT(&worker->threads);
    for (int i = 0; i < count; i++) {
        uv__work_thread_t *work_thread = malloc(sizeof(uv__work_thread_t));
        ret = uv_thread_create(&work_thread->thread, uv__worker_thread_loop, worker);
        if (ret < 0) return ret;
        QUEUE_INSERT_TAIL(&worker->threads, &work_thread->queue);
    }

    return 0;
}

void uv_worker_send(uv_worker_t *worker, void *data, uv_worker_cb work_cb,
                    uv_worker_after_cb after_work_cb) {
    uv__worker_item_t *item = malloc(sizeof(uv__worker_item_t));
    item->work_cb = work_cb;
    item->after_work_cb = after_work_cb;
    item->data = data;
    uv_chan_send(&worker->chan, item);
}

void uv_worker_join_all(uv_worker_t *worker) {
    QUEUE *head = NULL;
    uv__work_thread_t *item = NULL;
    while (!QUEUE_EMPTY(&worker->threads)) {
        head = QUEUE_HEAD(&worker->threads);
        item = QUEUE_DATA(head, uv__work_thread_t, queue);
        QUEUE_REMOVE(head);
        uv_thread_join(&item->thread);
        free(item);
    }
}

void uv_worker_close(uv_worker_t *worker) {
    uv_chan_send(&worker->chan, NULL);
    uv_worker_join_all(worker);
    uv_chan_destroy(&worker->chan);
    uv_messenger_stop(worker->msgr);
}
