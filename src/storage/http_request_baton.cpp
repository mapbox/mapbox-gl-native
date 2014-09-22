#include <mbgl/storage/http_request_baton.hpp>
#include <mbgl/storage/http_request.hpp>
#include <mbgl/storage/sqlite_store.hpp>
#include <mbgl/util/uv.hpp>

#include <uv.h>

#include <cassert>

namespace mbgl {

HTTPRequestBaton::HTTPRequestBaton() : thread_id(uv_thread_self()) {
}

HTTPRequestBaton::~HTTPRequestBaton() {
    assert(uv_thread_self() == thread_id);
    if (timer) {
        timer->data = nullptr;
    }
}

void HTTPRequestBaton::reset() {
    attempts++;
    response.reset();
    type = HTTPResponseType::Unknown;
}

void HTTPRequestBaton::retry(uint64_t delay) {
    assert(uv_thread_self() == thread_id);
    assert(!timer);
    timer = new uv_timer_t;
    timer->data = this;
    uv_timer_init(async->loop, timer);
    uv_timer_start(timer, [](uv_timer_t *timer) {
        std::unique_ptr<uv_timer_t, uv::deleter> timer_guard(timer);
        uv_timer_stop(timer);
        if (timer->data) {
            HTTPRequestBaton *baton = reinterpret_cast<HTTPRequestBaton *>(timer->data);
            assert(uv_thread_self() == baton->thread_id);
            baton->timer = nullptr;
            baton->start();
        }
    }, delay, 0);
}

void HTTPRequestBaton::stop() {
    if (timer) {
        uv_timer_stop(timer);
        uv::deleter()(timer);
        timer = nullptr;
    }

    cancel();
}


void HTTPRequestBaton::notify(uv_async_t *async_unguarded) {
    // Ensure deletion of pointers.
    std::unique_ptr<uv_async_t, uv::deleter> async(async_unguarded);
    std::unique_ptr<HTTPRequestBaton> baton((HTTPRequestBaton *)async->data);
    baton->cleanup();

    if (baton->request) {
        assert(uv_thread_self() == baton->request->thread_id);
        switch (baton->type) {
            case HTTPResponseType::TemporaryError:
            case HTTPResponseType::ConnectionError:
                // Exponential back-off:
                if (baton->attempts >= 4) {
                    // Fail immediately.
                    baton->request->response = std::move(baton->response);
                    baton->request->notify();
                } else {
                    baton->reset();
                    baton->retry(1000 * (1 << baton->attempts));
                    // Make sure we're not deleting the objects, since we're now reusing them.
                    baton.release();
                    async.release();
                }
                break;

            case HTTPResponseType::SingularError:
                // Retry immediately.
                if (baton->attempts >= 4) {
                    // Fail immediately.
                    baton->request->response = std::move(baton->response);
                    baton->request->notify();
                } else {
                    baton->reset();
                    baton->start();
                    // Make sure we're not deleting the objects, since we're now reusing them.
                    baton.release();
                    async.release();
                }
                break;

            case HTTPResponseType::Canceled:
                baton->request->response.reset();
                baton->request->notify();
                break;

            case HTTPResponseType::PermanentError:
            case HTTPResponseType::Successful:
                baton->request->store->put(baton->path, baton->request->type, *baton->response);
                baton->request->response = std::move(baton->response);
                baton->request->notify();
                break;

            case HTTPResponseType::NotModified:
                baton->request->store->updateExpiration(baton->path, baton->response->expires);
                baton->request->response = std::move(baton->response);
                baton->request->notify();
                break;

            default:
                // This should never happen.
                assert(false);
                break;
        }
        // Note: after calling notify(), the baton object may cease to exist.
    } else {
        // The HTTPRequest object has already been destructed. There is nobody we need to notify
        // anymore. All we need to do is to cleanup.
    }
}

}
