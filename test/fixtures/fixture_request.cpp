#include <mbgl/storage/http_request_baton.hpp>
#include <mbgl/storage/file_request_baton.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/platform/log.hpp>
#include <iostream>

#include <uv.h>

#include <cassert>


const std::string base_directory = []{
    std::string fn = __FILE__;
    fn.erase(fn.find_last_of("/"));
    fn.erase(fn.find_last_of("/"));
    fn.erase(fn.find_last_of("/"));
    return fn + "/node_modules/mapbox-gl-test-suite/";
}();

namespace mbgl {

void HTTPRequestBaton::start(const util::ptr<HTTPRequestBaton> &baton) {
    assert(uv_thread_self() == baton->thread_id);

    std::string clean_url = util::percentDecode(baton->path);
    if (clean_url.find("local://") == 0) {
        clean_url = base_directory + clean_url.substr(8);
    }

    baton->response = std::make_unique<Response>();
    FILE *file = fopen(clean_url.c_str(),"rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        const size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);
        baton->response->data.resize(size);
        fread(&baton->response->data[0], size, 1, file);
        fclose(file);

        baton->response->code = 200;
        baton->type = HTTPResponseType::Successful;
    } else {
        baton->type = HTTPResponseType::PermanentError;
        baton->response->code = 404;
    }

    uv_async_send(baton->async);
}

void HTTPRequestBaton::stop(const util::ptr<HTTPRequestBaton> &baton) {
    fprintf(stderr, "HTTP request cannot be canceled because it is answered immediately");
    abort();
}

namespace platform {

std::string defaultCacheDatabase() {
    // Disables the cache.
    return "";
}

}

}
