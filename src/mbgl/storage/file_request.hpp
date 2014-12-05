#ifndef MBGL_STORAGE_FILE_REQUEST
#define MBGL_STORAGE_FILE_REQUEST

#include <mbgl/storage/base_request.hpp>

namespace mbgl {

typedef struct uv_loop_s uv_loop_t;

struct FileRequestBaton;

class FileRequest : public BaseRequest {
public:
    FileRequest(const std::string &path, uv_loop_t *loop);
    ~FileRequest();

    void cancel();

private:
    FileRequestBaton *ptr = nullptr;

    friend struct FileRequestBaton;
};

}

#endif
