#include <mbgl/util/uv-worker.h>
#include <mbgl/util/uv-messenger.h>

#include <stdio.h>
#include <assert.h>

typedef struct uv__worker_item_s uv__worker_item_t;
struct uv__worker_item_s {
    uv_worker_t *worker;
    void *data;
    uv_worker_cb work_cb;
    uv_worker_after_cb after_work_cb;
};

typedef struct uv__worker_thread_s uv__worker_thread_t;
struct uv__worker_thread_s {
    uv_worker_t *worker;
    uv_thread_t thread;
};

void uv__worker_free_messenger(uv_messenger_t *msgr) {
    free(msgr);
}

void uv__worker_thread_finished(uv__worker_thread_t *worker_thread) {
    uv_worker_t *worker = worker_thread->worker;

#ifdef DEBUG
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
        uv_messenger_stop(worker->msgr, uv__worker_free_messenger);
        if (worker->close_cb) {
            worker->close_cb(worker);
        }
    }
}

void uv__worker_after(void *ptr) {
    uv__worker_item_t *item = (uv__worker_item_t *)ptr;

    if (item->work_cb) {
        // We are finishing a regular work request.
        if (item->after_work_cb) {
            assert(item->after_work_cb);
            item->after_work_cb(item->data);
        }
        uv_worker_t *worker = item->worker;
        assert(worker->active_items > 0);
        if (--worker->active_items == 0) {
            uv_messenger_unref(worker->msgr);
        }
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
    while ((item = (uv__worker_item_t *)uv_chan_receive(&worker->chan)) != NULL) {
        assert(item->work_cb);
        item->work_cb(item->data);

        // Trigger the after callback in the main thread.
        uv_messenger_send(worker->msgr, item);
    }

    // Make sure to close all other workers too.
    uv_chan_send(&worker->chan, NULL);

    // Create a new worker item that acts as a terminate flag for this thread.
    item = (uv__worker_item_t *)malloc(sizeof(uv__worker_item_t));
    item->data = worker_thread;
    item->work_cb = NULL;
    item->after_work_cb = NULL;
    uv_messenger_send(worker->msgr, item);
}

int uv_worker_init(uv_worker_t *worker, uv_loop_t *loop, int count, const char *name) {
#ifdef DEBUG
    worker->thread_id = uv_thread_self();
#endif
    worker->loop = loop;
    worker->name = name;
    worker->count = 0;
    worker->close_cb = NULL;
    worker->active_items = 0;
    worker->msgr = (uv_messenger_t *)malloc(sizeof(uv_messenger_t));
    int ret = uv_messenger_init(loop, worker->msgr, uv__worker_after);
    if (ret < 0) {
        free(worker->msgr);
        return ret;
    }
    uv_messenger_unref(worker->msgr);
    ret = uv_chan_init(&worker->chan);
    if (ret < 0) return ret;

    // Initialize all worker threads.
    int i;
    for (i = 0; i < count; i++) {
        uv__worker_thread_t *worker_thread = (uv__worker_thread_t *)malloc(sizeof(uv__worker_thread_t));
        worker_thread->worker = worker;
        ret = uv_thread_create(&worker_thread->thread, uv__worker_thread_loop, worker_thread);
        if (ret < 0) return ret;
        worker->count++;
    }

    return 0;
}

void uv_worker_send(uv_worker_t *worker, void *data, uv_worker_cb work_cb,
                    uv_worker_after_cb after_work_cb) {
#ifdef DEBUG
    assert(uv_thread_self() == worker->thread_id);
#endif

    // It doesn't make sense to not provide a work callback. On the other hand, the after_work_cb
    // may be NULL. In that case, there will be no callback called in the current thread and the
    // worker item will instead be freed in the worker thread.
    assert(work_cb);

    uv__worker_item_t *item = (uv__worker_item_t *)malloc(sizeof(uv__worker_item_t));
    item->worker = worker;
    item->work_cb = work_cb;
    item->after_work_cb = after_work_cb;
    item->data = data;
    uv_chan_send(&worker->chan, item);
    if (worker->active_items++ == 0) {
        uv_messenger_ref(worker->msgr);
    }
}

void uv_worker_close(uv_worker_t *worker, uv_worker_close_cb close_cb) {
#ifdef DEBUG
    assert(uv_thread_self() == worker->thread_id);
#endif

    // Prevent double calling.
    assert(worker->close_cb == NULL);

    worker->close_cb = close_cb;
    uv_chan_send(&worker->chan, NULL);
    if (worker->active_items++ == 0) {
        uv_messenger_ref(worker->msgr);
    }
}
