#pragma once

#include <mbgl/platform/log.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <node.h>
#include <nan.h>
#pragma GCC diagnostic pop

namespace node_mbgl {

namespace util { template <typename T> class AsyncQueue; }

class NodeLogObserver : public mbgl::Log::Observer {
public:
    NodeLogObserver(v8::Handle<v8::Object> target);
    ~NodeLogObserver();

    // Log::Observer implementation
    virtual bool onRecord(mbgl::EventSeverity severity, mbgl::Event event, int64_t code, const std::string &msg) override;

private:
    v8::Persistent<v8::Object> module;

    struct LogMessage;
    using Queue = util::AsyncQueue<LogMessage>;
    Queue *queue = nullptr;
};

}
