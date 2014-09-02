#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/request.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/platform/log.hpp>
#include <iostream>

const std::string base_directory = []{
    std::string fn = __FILE__;
    fn.erase(fn.find_last_of("/"));
    fn.erase(fn.find_last_of("/"));
    fn.erase(fn.find_last_of("/"));
    return fn + "/node_modules/mapbox-gl-test-suite/";
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

    std::string clean_url = util::percentDecode(url);
    if (clean_url.find("local://") == 0) {
        clean_url = base_directory + clean_url.substr(8);
    }

    std::shared_ptr<Request> req = std::make_shared<Request>(url, callback, loop);

    int err;

    std::string body;
    FILE *file = std::fopen(clean_url.c_str(),"rb");

    if (file != NULL)
    {
        std::fseek(file, 0, SEEK_END);
        std::size_t file_size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        body.resize(file_size);
        std::fread(&body[0], file_size, 1, file);
        std::fclose(file);
    }

    /*
    uv_fs_t open_req;
    err = uv_fs_open(l, &open_req, clean_url.c_str(), O_RDONLY, S_IRUSR, nullptr);
    uv_fs_req_cleanup(&open_req);
    if (err < 0) {
        req->res->code = err;
        req->res->error_message = uv_strerror(uv_last_error(l));
        Log::Warning(Event::HttpRequest, err, url + ": " + uv_strerror(uv_last_error(l)));
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
        req->res->error_message = uv_strerror(uv_last_error(l));
        Log::Warning(Event::HttpRequest, err, url + ": " + uv_strerror(uv_last_error(l)));
        req->complete();
        return req;
    }

    const uint64_t size = static_cast<const uv_statbuf_t*>(stat_req.ptr)->st_size;


    std::string body;
    body.resize(size);
    uv_buf_t uvbuf = uv_buf_init(const_cast<char *>(body.data()), body.size());

    uv_fs_t read_req;
    err = uv_fs_read(l, &read_req, fd, &uvbuf, 1, 0, nullptr);
    uv_fs_req_cleanup(&read_req);
    if (err < 0) {
        req->res->code = err;
        req->res->error_message = uv_strerror(uv_last_error(l));
        Log::Warning(Event::HttpRequest, err, url + ": " + uv_strerror(uv_last_error(l)));
        req->complete();
        return req;
    }


    uv_fs_t close_req;
    err = uv_fs_close(l, &close_req, fd, nullptr);
    uv_fs_req_cleanup(&close_req);
    if (err < 0) {
        req->res->code = err;
        req->res->error_message = uv_strerror(uv_last_error(l));
        Log::Warning(Event::HttpRequest, err, url + ": " + uv_strerror(uv_last_error(l)));
        req->complete();
        return req;
    }
    */

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
