#ifndef LLMR_UTIL_LOOP
#define LLMR_UTIL_LOOP

#include <functional>

typedef struct uv_loop_s uv_loop_t;
typedef struct uv_work_s uv_work_t;

namespace uv {

typedef std::function<void()> callback;

class loop {
public:
    loop();
    ~loop();

    uv_loop_t *operator*();
    void run();
    void stop();
    void work(callback work, callback after);

private:
    static void work_cb(uv_work_t* req);
    static void after_work_cb(uv_work_t* req, int status);

private:
    uv_loop_t *l;
};

}

#endif
