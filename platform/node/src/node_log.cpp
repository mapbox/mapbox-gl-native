#include "node_log.hpp"
#include "util/async_queue.hpp"

namespace node_mbgl {

struct NodeLogObserver::LogMessage {
    mbgl::EventSeverity severity;
    mbgl::Event event;
    int64_t code;
    std::string text;

    LogMessage(mbgl::EventSeverity severity_, mbgl::Event event_, int64_t code_, std::string text_)
        : severity(severity_),
        event(event_),
        code(code_),
        text(text_) {}
};

NodeLogObserver::NodeLogObserver(v8::Handle<v8::Object> target)
    : queue(new Queue(uv_default_loop(), [this](LogMessage &message) {
          NanScope();

          auto msg = NanNew<v8::Object>();
          msg->Set(NanNew("class"), NanNew(mbgl::EventClass(message.event).c_str()));
          msg->Set(NanNew("severity"), NanNew(mbgl::EventSeverityClass(message.severity).c_str()));
          if (message.code != -1) {
              msg->Set(NanNew("code"), NanNew<v8::Number>(message.code));
          }
          if (!message.text.empty()) {
              msg->Set(NanNew("text"), NanNew(message.text));
          }

          v8::Local<v8::Value> argv[] = { NanNew("message"), msg };
          auto handle = NanNew<v8::Object>(module);
          auto emit = handle->Get(NanNew("emit"))->ToObject();
          emit->CallAsFunction(handle, 2, argv);
      })) {
    NanScope();
    NanAssignPersistent(module, target);

    // Don't keep the event loop alive.
    queue->unref();
}

NodeLogObserver::~NodeLogObserver() {
    queue->stop();
}

bool NodeLogObserver::onRecord(mbgl::EventSeverity severity, mbgl::Event event, int64_t code, const std::string &text) {
    queue->send({ severity, event, code, text });
    return true;
}

}
