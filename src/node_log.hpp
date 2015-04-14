#pragma once

#include <mbgl/platform/log.hpp>
#include <mbgl/util/async_queue.hpp>

#include <node.h>
#include <nan.h>

namespace mbgl { namespace util { template <typename T> class AsyncQueue; } }

namespace node_mbgl {

class NodeLogObserver : public mbgl::Log::Observer {
public:
    NodeLogObserver(v8::Handle<v8::Object> target);
    ~NodeLogObserver();

    // Log::Observer implementation
    virtual bool onRecord(mbgl::EventSeverity severity, mbgl::Event event, int64_t code, const std::string &msg) override;

private:
    v8::Persistent<v8::Object> module;

    struct LogMessage;
    using Queue = mbgl::util::AsyncQueue<LogMessage>;
    Queue *queue = nullptr;
};

}
