#include "node_log.hpp"
#include "util/async_queue.hpp"

#include <mbgl/map/environment.hpp>

namespace node_mbgl {

struct NodeLogObserver::LogMessage {
    mbgl::EventSeverity severity;
    mbgl::Event event;
    int64_t code;
    std::string text;
    int environment = -1;
    std::string threadName = "";

    LogMessage(mbgl::EventSeverity severity_, mbgl::Event event_, int64_t code_, std::string text_, int environment_, std::string threadName_)
        : severity(severity_),
        event(event_),
        code(code_),
        text(text_),
        environment(environment_),
        threadName(threadName_) {};

    LogMessage(mbgl::EventSeverity severity_, mbgl::Event event_, int64_t code_, std::string text_)
        : severity(severity_),
        event(event_),
        code(code_),
        text(text_) {}
};

NodeLogObserver::NodeLogObserver(v8::Handle<v8::Object> target)
    : queue(new Queue(uv_default_loop(), [this](LogMessage &message) {
          NanScope();

          auto msg = v8::Object::New();
          msg->Set(NanNew("class"), NanNew(mbgl::EventClass(message.event).c_str()));
          msg->Set(NanNew("severity"), NanNew(mbgl::EventSeverityClass(message.severity).c_str()));
          if (message.code != -1) {
              msg->Set(NanNew("code"), NanNew<v8::Number>(message.code));
          }
          if (!message.text.empty()) {
              msg->Set(NanNew("text"), NanNew(message.text));
          }
          if (message.environment != -1) {
              msg->Set(NanNew("environment"), NanNew<v8::Number>(message.environment));
          }
          if (!message.threadName.empty()) {
              msg->Set(NanNew("threadName"), NanNew(message.threadName));
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
    if (mbgl::Environment::inScope()) {
        int env = static_cast<int>(mbgl::Environment::Get().getID());
        std::string threadName = mbgl::Environment::threadName();
        queue->send({ severity, event, code, text, env, threadName });
    } else {
        queue->send({ severity, event, code, text });
    }
    return true;
}

}
