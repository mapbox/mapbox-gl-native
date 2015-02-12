#pragma once

#include <mbgl/platform/log.hpp>
#include <mbgl/util/async_queue.hpp>

#include <node.h>
#include <nan.h>

namespace mbgl { namespace util { template <typename T> class AsyncQueue; } }

namespace node_mbgl {

class NodeLogBackend : public mbgl::LogBackend {
public:
    NodeLogBackend(v8::Handle<v8::Object> target);
    ~NodeLogBackend();

    void record(mbgl::EventSeverity severity, mbgl::Event event, const std::string &msg);
    void record(mbgl::EventSeverity severity, mbgl::Event event, const char* format, ...);
    void record(mbgl::EventSeverity severity, mbgl::Event event, int64_t code);
    void record(mbgl::EventSeverity severity, mbgl::Event event, int64_t code, const std::string &msg);

private:
    v8::Persistent<v8::Object> module;

    struct Message;
    using Queue = mbgl::util::AsyncQueue<Message>;
    Queue *queue = nullptr;
};


}
