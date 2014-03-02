#include "request.hpp"

#include <queue>
#include <forward_list>
#include <pthread.h>

#include <llmr/platform/platform.hpp>

using namespace llmr::platform;

class Threadpool;

class Worker {
public:
    Worker(Threadpool& pool);

    static void *loop(void *ptr) {
        pthread_setname_np("worker");
        static_cast<Worker *>(ptr)->loop();
        pthread_exit(nullptr);
    }

    void loop();

private:
    Threadpool& pool;
    pthread_t thread;
};


class Threadpool {
    friend class Worker;

public:
    typedef void (*Callback)(void *);
    Threadpool(Callback callback);

    void add(void *data);

private:
    pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
    pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
    std::forward_list<Worker> workers;
    Callback callback;
    std::queue<void *> tasks;
};


Threadpool::Threadpool(Callback callback)
    : callback(callback) {
    const int worker_count = 4;
    fprintf(stderr, "workers: %d\n", worker_count);
    for (int i = 0; i < worker_count; i++) {
        workers.emplace_front(*this);
    }
}

void Threadpool::add(void *data) {
    pthread_mutex_lock(&mutex);
    tasks.push(data);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condition);
}

Worker::Worker(Threadpool& pool)
    : pool(pool) {
    pthread_create(&thread, nullptr, loop, (void *)this);
}

void Worker::loop() {
    pthread_mutex_lock(&pool.mutex);
    while (true) {
        if (pool.tasks.size()) {
            void *task = pool.tasks.front();
            pool.tasks.pop();
            pthread_mutex_unlock(&pool.mutex);
            pool.callback(task);
            pthread_mutex_lock(&pool.mutex);
        } else {
            pthread_cond_wait(&pool.condition, &pool.mutex);
        }
    }
}

Threadpool *request_pool = nullptr;
CURLSH *curl_share = nullptr;
pthread_mutex_t curl_share_mutex = PTHREAD_MUTEX_INITIALIZER;


void curl_share_lock(CURL *, curl_lock_data, curl_lock_access, void *) {
    pthread_mutex_lock(&curl_share_mutex);
}

void curl_share_unlock(CURL *, curl_lock_data, void *) {
    pthread_mutex_unlock(&curl_share_mutex);
}


Request::Request(std::string url, std::function<void(platform::Response&)> bg, std::function<void()> fg)
    : done(false),
      cancelled(false),
      url(url),
      background_function(bg),
      foreground_callback(fg) {
    if (!request_pool) {
        request_pool = new Threadpool(request);
    }
    if (!curl_share) {
        // curl init
        curl_global_init(CURL_GLOBAL_ALL);
        curl_share = curl_share_init();
        curl_share_setopt(curl_share, CURLSHOPT_LOCKFUNC, curl_share_lock);
        curl_share_setopt(curl_share, CURLSHOPT_UNLOCKFUNC, curl_share_unlock);
    }
    request_pool->add(this);
}


size_t Request::write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}


int Request::progress_callback(void *ptr, double dltotal, double dlnow, double ultotal, double ulnow) {
    Request *req = static_cast<Request *>(ptr);
    bool cancel = req->cancelled;
    if (cancel) {
        fprintf(stderr, "cancel download %s\n", req->url.c_str());
    }
    return cancel;
}

void Request::request(void *ptr) {
    Request *req = static_cast<Request *>(ptr);
    Response res;

    fprintf(stderr, "[%p] request %s\n", pthread_self(), req->url.c_str());

    // TODO: use curl multi to be able to cancel, or to 

    CURL *curl = curl_easy_init();
    CURLcode code;

    curl_easy_setopt(curl, CURLOPT_URL, req->url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res.body);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, req);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
    curl_easy_setopt(curl, CURLOPT_SHARE, curl_share);
    code = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res.code);
    curl_easy_cleanup(curl);

    if (code != CURLE_ABORTED_BY_CALLBACK) {
        req->background_function(res);
    }

    req->done = true;
}

void Request::cancel() {
    cancelled = true;
}
