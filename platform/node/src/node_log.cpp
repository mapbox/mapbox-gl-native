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

NodeLogObserver::NodeLogObserver(v8::Local<v8::Object> target)
    : queue(new Queue(uv_default_loop(), [this](LogMessage &message) {
          Nan::HandleScope scope;

          auto msg = Nan::New<v8::Object>();

          Nan::Set(msg, Nan::New("class").ToLocalChecked(),
              Nan::New(mbgl::EventClass(message.event).c_str()).ToLocalChecked());

          Nan::Set(msg, Nan::New("severity").ToLocalChecked(),
              Nan::New(mbgl::EventSeverityClass(message.severity).c_str()).ToLocalChecked());

          if (message.code != -1) {
              Nan::Set(msg, Nan::New("code").ToLocalChecked(),
                  Nan::New<v8::Number>(message.code));
          }

          if (!message.text.empty()) {
              Nan::Set(msg, Nan::New("text").ToLocalChecked(),
                  Nan::New(message.text).ToLocalChecked());
          }

          v8::Local<v8::Value> argv[] = { Nan::New("message").ToLocalChecked(), msg };
          auto handle = Nan::New<v8::Object>(module);
          auto emit = Nan::Get(handle, Nan::New("emit").ToLocalChecked()).ToLocalChecked()->ToObject();
          Nan::CallAsFunction(emit, handle, 2, argv);
      })) {
    Nan::HandleScope scope;
    module.Reset(target);

    // Don't keep the event loop alive.
    queue->unref();
}

NodeLogObserver::~NodeLogObserver() {
    queue->stop();
    module.Reset();
}

bool NodeLogObserver::onRecord(mbgl::EventSeverity severity, mbgl::Event event, int64_t code, const std::string &text) {
    queue->send({ severity, event, code, text });
    return true;
}

}
