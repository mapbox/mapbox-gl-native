#include <llmr/platform/request.hpp>
#include <llmr/platform/platform.hpp>
#include <uv.h>

using namespace llmr::platform;

Request::Request(const std::string &url,
                 std::function<void(Response *)> background_function,
                 std::function<void()> foreground_callback,
                 uv_loop_t *loop)
    : url(url),
      background_function(background_function),
      foreground_callback(foreground_callback),
      loop(loop) {

    // Add a check handle without a callback to keep the default loop running.
    // We don't have a real handler attached to the default loop right from the
    // beginning, because we're using asynchronous messaging to perform the actual
    // request in the request thread. Only after the request is complete, we
    // create an actual work request that is attached to the default loop.
    check = new uv_check_t();
    uv_check_init(loop, check);
    uv_check_start(check, [](uv_check_t *) {});
}

Request::~Request() {
    // We need to remove our no-op handle again to allow the main event loop to exit.
    uv_check_stop(check);
}

void Request::work_callback(uv_work_t *work) {
    std::shared_ptr<Request> *req = static_cast<std::shared_ptr<Request> *>(work->data);
    (*req)->background_function((*req)->res.get());
}

// This callback is executed in the main loop.
void Request::after_work_callback(uv_work_t *work, int /*status*/) {
    std::shared_ptr<Request> *req = static_cast<std::shared_ptr<Request> *>(work->data);

    (*req)->foreground_callback();

    // This finally deletes the *pointer* to the shared pointer we've been holding on since we
    // pushed it on the add_queue on request creation.
    delete req;
    delete work;
}
