#include <llmr/util/loop.hpp>

#include <uv.h>

using namespace uv;

loop::loop()
    : l(uv_loop_new()),
      a(new uv_async_t) {
    uv_async_init(l, a, async_cb);
    a->data = this;
}

loop::~loop() {
    uv_loop_delete(l);
    delete a;
}

uv_loop_t *loop::operator*() {
    return l;
}

void loop::run() {
    uv_run(l, UV_RUN_DEFAULT);
}

void loop::stop() {
    uv_stop(l);
}

struct task {
    callback work, after;
};

void loop::work(callback work, callback after) {
    uv_work_t *req = new uv_work_t;
    req->data = new task { work, after };
    uv_queue_work(l, req, work_cb, after_work_cb);
}

void loop::work_cb(uv_work_t* req) {
    task *t = static_cast<task *>(req->data);
    t->work();
}

void loop::after_work_cb(uv_work_t* req, int status) {
    task *t = static_cast<task *>(req->data);
    t->after();
    delete t;
    delete req;
}

void loop::schedule(callback cb) {
    {
        std::lock_guard<std::mutex> lock(async_mtx);
        async_fns.push(cb);
    }
    uv_async_send(a);
}

void loop::async_cb(uv_async_t* async, int status) {
    loop *l = static_cast<loop *>(async->data);
    for(;;) {
        callback cb;
        l->async_mtx.lock();
        if (l->async_fns.size()) {
            cb = l->async_fns.front();
            l->async_fns.pop();
        }
        l->async_mtx.unlock();
        if (cb) {
            cb();
        } else {
            return;
        }
    }
}


