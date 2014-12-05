#ifndef MBGL_STORAGE_FILE_SOURCE
#define MBGL_STORAGE_FILE_SOURCE

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/storage/resource_type.hpp>
#include <mbgl/storage/request.hpp>

#include <string>
#include <functional>

typedef struct uv_loop_s uv_loop_t;


namespace mbgl {

class FileSource : public util::noncopyable {
public:
    virtual ~FileSource() = default;

    virtual void setLoop(uv_loop_t*) = 0;
    virtual bool hasLoop() = 0;
    virtual void clearLoop() = 0;

    virtual void setBase(std::string) = 0;
    virtual std::unique_ptr<Request> request(ResourceType type, const std::string &url) = 0;
    virtual void prepare(std::function<void()> fn) = 0;
};

}

#endif
