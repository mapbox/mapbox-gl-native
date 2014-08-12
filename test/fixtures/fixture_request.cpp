#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/request.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/platform/log.hpp>

const std::string base_directory = []{
    std::string fn = __FILE__;
    fn.erase(fn.find_last_of("/"));
    return fn;
}();


namespace mbgl {

std::shared_ptr<platform::Request>
platform::request_http(const std::string &url,
                       std::function<void(Response *)> callback,
                       std::shared_ptr<uv::loop> loop) {
    uv_loop_t *l = nullptr;
    if (loop) {
        l = **loop;
    } else {
        l = uv_default_loop();
    }

    std::string clean_url = base_directory + "/" + url;
    auto pos = clean_url.find("://");
    if (pos != std::string::npos) {
        clean_url.replace(pos, 3, "/");
    }

    std::replace(clean_url.begin(), clean_url.end(), '+', ' ');

    std::shared_ptr<Request> req = std::make_shared<Request>(url, callback, loop);

    int err;

    uv_fs_t open_req;
    err = uv_fs_open(l, &open_req, clean_url.c_str(), O_RDONLY, S_IRUSR, nullptr);
    uv_fs_req_cleanup(&open_req);
    if (err < 0) {
        req->res->code = err;
        req->res->error_message = uv_strerror(err);
        Log::Warning(Event::HttpRequest, err, url + ": " + uv_strerror(err));
        req->complete();
        return req;
    }
    uv_file fd = err;

    uv_fs_t stat_req;
    uv_fs_fstat(l, &stat_req, fd, nullptr);
    uv_fs_req_cleanup(&stat_req);
    err = uv_fs_fstat(l, &stat_req, fd, nullptr);
    if (err < 0) {
        req->res->code = err;
        req->res->error_message = uv_strerror(err);
        Log::Warning(Event::HttpRequest, err, url + ": " + uv_strerror(err));
        req->complete();
        return req;
    }

    const uint64_t size = static_cast<const uv_stat_t*>(stat_req.ptr)->st_size;


    std::string body;
    body.resize(size);
    uv_buf_t uvbuf = uv_buf_init(const_cast<char *>(body.data()), body.size());

    uv_fs_t read_req;
    err = uv_fs_read(l, &read_req, fd, &uvbuf, 1, 0, nullptr);
    uv_fs_req_cleanup(&read_req);
    if (err < 0) {
        req->res->code = err;
        req->res->error_message = uv_strerror(err);
        Log::Warning(Event::HttpRequest, err, url + ": " + uv_strerror(err));
        req->complete();
        return req;
    }


    uv_fs_t close_req;
    err = uv_fs_close(l, &close_req, fd, nullptr);
    uv_fs_req_cleanup(&close_req);
    if (err < 0) {
        req->res->code = err;
        req->res->error_message = uv_strerror(err);
        Log::Warning(Event::HttpRequest, err, url + ": " + uv_strerror(err));
        req->complete();
        return req;
    }

    req->res->body.swap(body);
    req->res->code = 200;
    Log::Info(Event::HttpRequest, 200, url);
    req->complete();

    return req;
}

void platform::cancel_request_http(const std::shared_ptr<Request> &req) {
    if (req) {
        req->cancelled = true;
    }
}

}
