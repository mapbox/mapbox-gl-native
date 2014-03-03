#ifndef LLMR_UTIL_THREADPOOL
#define LLMR_UTIL_THREADPOOL

#include <pthread.h>
#include <forward_list>
#include <queue>

namespace llmr {
namespace util {

class Threadpool {
private:
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
    Threadpool(int max_workers = 4);
    typedef void (*Callback)(void *);
    void add(Callback callback, void *data);

private:
    typedef std::pair<Callback, void *> Task;
    const int max_workers;
    pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
    pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
    std::forward_list<Worker> workers;
    int worker_count = 0;
    std::queue<Task> tasks;
};

extern std::unique_ptr<Threadpool> threadpool;

}
}

#endif

