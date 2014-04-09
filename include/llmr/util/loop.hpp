#ifndef LLMR_UTIL_LOOP
#define LLMR_UTIL_LOOP

#include <functional>

#include <uv.h>
#include <queue>
#include <mutex>

namespace uv {

typedef std::function<void()> callback;

class loop {
public:
    loop();
    ~loop();

    uv_loop_t *operator*();
    void run();
    void stop();

    // Schedules a worker function to be called in a threadpool thread, then
    // the after callback will be called on the loop thread.
    void work(callback work, callback after);

    // Schedules a function to be called in the loop thread.
    void schedule(callback cb);

private:
    static void async_cb(uv_async_t* async);
    static void work_cb(uv_work_t* req);
    static void after_work_cb(uv_work_t* req, int status);

private:
    uv_loop_t *l;
    uv_async_t *a;
    std::mutex async_mtx;
    std::queue<callback> async_fns;
};

}

#endif
