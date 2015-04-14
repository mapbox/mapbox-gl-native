#include "node_log.hpp"

namespace node_mbgl {

struct NodeLogObserver::LogMessage {
    mbgl::EventSeverity severity;
    mbgl::Event event;
    int64_t code;
    std::string text;
};

NodeLogObserver::NodeLogObserver(v8::Handle<v8::Object> target)
    : queue(new Queue(uv_default_loop(), [this](LogMessage &message) {
          NanScope();

          auto msg = v8::Object::New();
          msg->Set(NanNew("class"), NanNew(mbgl::EventClass(message.event).c_str()));
          msg->Set(NanNew("severity"), NanNew(mbgl::EventSeverityClass(message.severity).c_str()));
          if (message.code != -1) {
              msg->Set(NanNew("code"), NanNew(double(message.code)));
          }
          if (!message.text.empty()) {
              msg->Set(NanNew("text"), NanNew(message.text));
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

NodeLogObserver::~NodeLogObserver() {
    queue->stop();
}

bool NodeLogObserver::onRecord(mbgl::EventSeverity severity, mbgl::Event event, int64_t code, const std::string &text) {
    queue->send({ severity, event, code, text });
    return true;
}

}
