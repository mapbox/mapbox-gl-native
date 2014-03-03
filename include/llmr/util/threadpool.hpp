#ifndef LLMR_UTIL_THREADPOOL
#define LLMR_UTIL_THREADPOOL

#include <pthread.h>
#include <forward_list>
#include <queue>

namespace llmr {
namespace util {

class Threadpool {
    class Worker {
    public:
        Worker(Threadpool& pool);
        ~Worker();
        static void *loop(void *ptr);

    private:
        Threadpool& pool;
        pthread_t thread;
    };

public:
    typedef void (*Callback)(void *);
    Threadpool(Callback callback, int max_workers = 4);

    void add(void *data);

private:
    const int max_workers;
    pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
    pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
    std::forward_list<Worker> workers;
    int worker_count = 0;
    Callback callback;
    std::queue<void *> tasks;
};

}
}

#endif

