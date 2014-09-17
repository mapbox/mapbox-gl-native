#include <mbgl/util/uv-worker.h>
#include <mbgl/util/uv-messenger.h>

#include <stdio.h>
#include <assert.h>

typedef struct uv__worker_item_s uv__worker_item_t;
struct uv__worker_item_s {
    void *data;
    uv_worker_cb work_cb;
    uv_worker_after_cb after_work_cb;
};

typedef struct uv__worker_thread_s uv__worker_thread_t;
struct uv__worker_thread_s {
    uv_worker_t *worker;
    uv_thread_t thread;
};

void uv__worker_thread_finished(uv__worker_thread_t *worker_thread) {
    uv_worker_t *worker = worker_thread->worker;

#ifndef NDEBUG
    assert(uv_thread_self() == worker->thread_id);
#endif

    // This should at most block very briefly. We are sending the termination
    // notification as the last thing in the worker thread, so by now the thread
    // has probably terminated already. If not, the waiting time should be
    // extremely short.
    uv_thread_join(&worker_thread->thread);

    assert(worker->count > 0);
    worker->count--;
    if (worker->count == 0) {
        uv_chan_destroy(&worker->chan);
        uv_messenger_stop(worker->msgr);
        if (worker->close_cb) {
            worker->close_cb(worker);
        }
    }
}

void uv__worker_after(void *ptr) {
    uv__worker_item_t *item = (uv__worker_item_t *)ptr;

    if (item->work_cb) {
        // We are finishing a regular work request.
        assert(item->after_work_cb);
        item->after_work_cb(item->data);
    } else {
        // This is a worker thread termination.
        uv__worker_thread_t *worker_thread = (uv__worker_thread_t *)item->data;
        uv__worker_thread_finished(worker_thread);
        free(worker_thread);
    }

    free(item);
}

void uv__worker_thread_loop(void *ptr) {
    uv__worker_thread_t *worker_thread = (uv__worker_thread_t *)ptr;
    uv_worker_t *worker = worker_thread->worker;

#ifdef __APPLE__
    if (worker->name) {
        pthread_setname_np(worker->name);
    }
#endif

    uv__worker_item_t *item = NULL;
    while ((item = uv_chan_receive(&worker->chan)) != NULL) {
        assert(item->work_cb);
        item->work_cb(item->data);

        if (item->after_work_cb) {
            // Trigger the after callback in the main thread.
            uv_messenger_send(worker->msgr, item);
        } else {
            // There is no after work callback, so it wouldn't do anything anyway.
            free(item);
        }
    }

    // Make sure to close all other workers too.
    uv_chan_send(&worker->chan, NULL);

    // Create a new worker item that acts as a terminate flag for this thread.
    item = malloc(sizeof(uv__worker_item_t));
    item->data = worker_thread;
    item->work_cb = NULL;
    item->after_work_cb = NULL;
    uv_messenger_send(worker->msgr, item);
}

int uv_worker_init(uv_worker_t *worker, uv_loop_t *loop, int count, const char *name) {
#ifndef NDEBUG
    worker->thread_id = uv_thread_self();
#endif
    worker->name = name;
    worker->count = 0;
    worker->close_cb = NULL;
    worker->msgr = malloc(sizeof(uv_messenger_t));
    int ret = uv_messenger_init(loop, worker->msgr, uv__worker_after);
    if (ret < 0) {
        free(worker->msgr);
        return ret;
    }
    ret = uv_chan_init(&worker->chan);
    if (ret < 0) return ret;

    // Initialize all worker threads.
    for (int i = 0; i < count; i++) {
        uv__worker_thread_t *worker_thread = malloc(sizeof(uv__worker_thread_t));
        worker_thread->worker = worker;
        ret = uv_thread_create(&worker_thread->thread, uv__worker_thread_loop, worker_thread);
        if (ret < 0) return ret;
        worker->count++;
    }

    return 0;
}

void uv_worker_send(uv_worker_t *worker, void *data, uv_worker_cb work_cb,
                    uv_worker_after_cb after_work_cb) {
#ifndef NDEBUG
    assert(uv_thread_self() == worker->thread_id);
#endif

    // It doesn't make sense to not provide a work callback. On the other hand, the after_work_cb
    // may be NULL. In that case, there will be no callback called in the current thread and the
    // worker item will instead be freed in the worker thread.
    assert(work_cb);

    uv__worker_item_t *item = malloc(sizeof(uv__worker_item_t));
    item->work_cb = work_cb;
    item->after_work_cb = after_work_cb;
    item->data = data;
    uv_chan_send(&worker->chan, item);
}

void uv_worker_close(uv_worker_t *worker, uv_worker_close_cb close_cb) {
#ifndef NDEBUG
    assert(uv_thread_self() == worker->thread_id);
#endif

    // Prevent double calling.
    assert(worker->close_cb == NULL);

    worker->close_cb = close_cb;
    uv_chan_send(&worker->chan, NULL);
}
