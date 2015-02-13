#include "node_log.hpp"

#include <cstdarg>

namespace node_mbgl {

struct NodeLogBackend::Message {
    mbgl::EventSeverity severity;
    mbgl::Event event;
    int64_t code;
    std::string text;
};

NodeLogBackend::NodeLogBackend(v8::Handle<v8::Object> target)
    : queue(new Queue(uv_default_loop(), [this](Message &message) {
          NanScope();

          auto msg = v8::Object::New();
          msg->Set(NanNew("class"), NanNew(mbgl::EventClass(message.event).c_str()));
          msg->Set(NanNew("severity"), NanNew(mbgl::EventSeverityClass(message.severity).c_str()));
          if (!message.text.empty()) {
              msg->Set(NanNew("text"), NanNew(message.text));
          }
          if (message.code != 0) {
              msg->Set(NanNew("code"), NanNew(double(message.code)));
          }

          v8::Local<v8::Value> argv[] = { NanNew("message"), msg };
          auto emit = module->Get(NanNew("emit"))->ToObject();
          emit->CallAsFunction(module, 2, argv);
      })) {
    NanScope();
    module = v8::Persistent<v8::Object>::New(target);

    // Don't keep the event loop alive.
    queue->unref();
}

NodeLogBackend::~NodeLogBackend() {
    queue->stop();
}

void NodeLogBackend::record(mbgl::EventSeverity severity, mbgl::Event event, const std::string &msg) {
    queue->send({ severity, event, 0, msg });
}

void NodeLogBackend::record(mbgl::EventSeverity severity, mbgl::Event event, const char* format, ...) {
    char msg[512];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, 512, format, args);
    va_end(args);
    queue->send({ severity, event, 0, msg });
}

void NodeLogBackend::record(mbgl::EventSeverity severity, mbgl::Event event, int64_t code) {
    queue->send({ severity, event, code, "" });
}

void NodeLogBackend::record(mbgl::EventSeverity severity, mbgl::Event event, int64_t code, const std::string &msg) {
    queue->send({ severity, event, code, msg });
}

}
