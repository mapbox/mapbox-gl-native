#include "request.hpp"

// #include <atomic>
// #include <cassert>

#include <uv.h>
#include <cstdlib>
// #include <llmr/util/threadpool.hpp>
#include <llmr/platform/platform.hpp>

namespace llmr {
namespace platform {

typedef struct curl_context_s {
  uv_poll_t poll_handle;
  curl_socket_t sockfd;
} curl_context_t;

}
}

using namespace llmr::platform;

size_t curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}


uv_once_t Request::init_thread = UV_ONCE_INIT;
uv::loop Request::loop;
uv_thread_t Request::thread;
uv_timer_t Request::timeout;
uv_async_t Request::async_add;
CURLM *Request::curl_handle = nullptr;
std::mutex Request::request_mutex;
std::queue<Request *> Request::requests;


Request::Request(std::string url, std::function<void(Response *)> fn)
:
      url(url),
      fn(fn)
{
    uv_once(&init_thread, init_thread_cb);

    {
        std::lock_guard<std::mutex> lock(request_mutex);
        requests.push(this);
    }

    uv_async_send(&async_add);
}

void Request::init_thread_cb() {
    uv_async_init(*loop, &async_add, async_add_cb);
    uv_thread_create(&thread, init_thread_run, nullptr);
}

curl_context_t* Request::create_curl_context(curl_socket_t sockfd)
{
  curl_context_t *context;

  context = (curl_context_t *) malloc(sizeof *context);

  context->sockfd = sockfd;

  uv_poll_init_socket(*loop, &context->poll_handle, sockfd);
  context->poll_handle.data = context;

  return context;
}


void curl_close_cb(uv_handle_t *handle)
{
  curl_context_t* context = (curl_context_t*) handle->data;
  free(context);
}


void Request::destroy_curl_context(curl_context_t *context)
{
  uv_close((uv_handle_t*) &context->poll_handle, curl_close_cb);
}

int Request::handle_socket(CURL *easy, curl_socket_t s, int action, void *userp,
                  void *socketp)
{
  curl_context_t *curl_context;
  if (action == CURL_POLL_IN || action == CURL_POLL_OUT) {
    if (socketp) {
      curl_context = (curl_context_t*) socketp;
    }
    else {
      curl_context = create_curl_context(s);
    }
    curl_multi_assign(curl_handle, s, (void *) curl_context);
  }

  switch (action) {
  case CURL_POLL_IN:
    uv_poll_start(&curl_context->poll_handle, UV_READABLE, curl_perform);
    break;
  case CURL_POLL_OUT:
    uv_poll_start(&curl_context->poll_handle, UV_WRITABLE, curl_perform);
    break;
  case CURL_POLL_REMOVE:
    if (socketp) {
      uv_poll_stop(&((curl_context_t*)socketp)->poll_handle);
      destroy_curl_context((curl_context_t*) socketp);
      curl_multi_assign(curl_handle, s, NULL);
    }
    break;
  default:
    abort();
  }

  return 0;
}


void Request::curl_perform(uv_poll_t *req, int status, int events)
{
  int running_handles;
  int flags = 0;
  curl_context_t *context;
  CURLMsg *message;
  int pending;

  uv_timer_stop(&timeout);

  if (events & UV_READABLE)
    flags |= CURL_CSELECT_IN;
  if (events & UV_WRITABLE)
    flags |= CURL_CSELECT_OUT;

  context = (curl_context_t*)req;

  curl_multi_socket_action(curl_handle, context->sockfd, flags,
                           &running_handles);

  while ((message = curl_multi_info_read(curl_handle, &pending))) {
    switch (message->msg) {
    case CURLMSG_DONE:
      {
        long code;
        Request *req = nullptr;
        curl_easy_getinfo(message->easy_handle, CURLINFO_RESPONSE_CODE, &code);
        curl_easy_getinfo(message->easy_handle, CURLINFO_PRIVATE, (char *)&req);

        req->res->code = code;
        req->fn(req->res);
      }

      curl_multi_remove_handle(curl_handle, message->easy_handle);
      curl_easy_cleanup(message->easy_handle);



      break;
    default:
      fprintf(stderr, "CURLMSG default\n");
      abort();
    }
  }
}

void Request::on_timeout(uv_timer_t *req, int status)
{
  int running_handles;
  curl_multi_socket_action(curl_handle, CURL_SOCKET_TIMEOUT, 0,
                           &running_handles);
}

void Request::start_timeout(CURLM *multi, long timeout_ms, void *userp)
{
  if (timeout_ms <= 0)
    timeout_ms = 1; /* 0 means directly call socket_action, but we'll do it in
                       a bit */
  uv_timer_start(&timeout, on_timeout, timeout_ms, 0);
}


void Request::init_thread_run(void *ptr) {
    uv_timer_init(*loop, &timeout);

    curl_handle = curl_multi_init();
    curl_multi_setopt(curl_handle, CURLMOPT_SOCKETFUNCTION, handle_socket);
    curl_multi_setopt(curl_handle, CURLMOPT_TIMERFUNCTION, start_timeout);

    loop.run();
    curl_multi_cleanup(curl_handle);
}

void Request::async_add_cb(uv_async_t *async, int status) {
    std::lock_guard<std::mutex> lock(request_mutex);

    while (requests.size()) {
        Request *req = requests.front();
        req->res = new Response();
        CURL *handle = curl_easy_init();
        curl_easy_setopt(handle, CURLOPT_PRIVATE, req);
        curl_easy_setopt(handle, CURLOPT_URL, req->url.c_str());
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_write_cb);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &req->res->body);
        curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "deflate");
        curl_multi_add_handle(curl_handle, handle);
        requests.pop();
    }
}
